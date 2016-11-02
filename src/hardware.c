/*
 *    TTTTTTTTTTTTTT  EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *    TTTTTTTTTTTTTT  EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EEEEEEEEEE      OO          OO
 *          TT        EEEEEEEEEE      OO          OO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *          TT        EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *
 *                  L'�mulateur Thomson TO8
 *
 *  Copyright (C) 1997-2016 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
 *                          J�r�mie Guillaume, Fran�ois Mouret
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 *  Module     : hardware.c
 *  Version    : 1.8.4
 *  Cr�� par   : Gilles F�tis
 *  Modifi� par: Eric Botcazou 24/10/2003
 *               Fran�ois Mouret 18/09/2006 02/02/2012 29/09/2012
 *                               18/09/2013 09/04/2014 18/08/2015
 *                               31/07/2016
 *
 *  Emulation de l'environnement mat�riel du MC6809E:
 *	- carte m�moire
 *	- circuits d'entr�e/sortie du syst�me
 *	- circuits d'entr�e/sortie des p�riph�riques
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <string.h>
   #include <stdlib.h>
   #include <time.h>
#endif

#include "defs.h"
#include "teo.h"
#include "errors.h"
#include "hardware.h"
#include "mc68xx/dasm6809.h"
#include "mc68xx/mc6809.h"
#include "mc68xx/mc6821.h"
#include "mc68xx/mc6846.h"
#include "media/disk.h"
#include "media/keyboard.h"
#include "media/cass.h"
#include "media/mouse.h"
#include "media/printer.h"


/* les composants m�t�riels de l'�mulateur */
struct MC6846_PIA mc6846;      /* PIA 6846 syst�me         */
struct MC6821_PIA pia_int;     /* PIA 6821 syst�me         */
struct MC6821_PIA pia_ext;     /* PIA 6821 musique et jeux */
struct GATE_ARRAY mode_page;   /* Gate Array mode page     */
struct EF9369 pal_chip;        /* circuit de palette vid�o */
struct MEMORY_PAGER mempager;  /* carte m�moire logique    */
struct MEMORY mem;             /* carte m�moire physique   */
struct MOTHERBOARD mb;         /* (pseudo) carte m�re      */

mc6809_clock_t screen_clock;


/* Fonctions de commutation de l'espace m�moire:
 */
static void update_cart(void)
{
    static uint8 cart_garbage[32];

    if (mode_page.cart&0x20)
    {   /* l'espace cartouche est mapp� sur la RAM */
        mempager.segment[0x0]=mem.ram.bank[mempager.cart.ram_page];
        mempager.segment[0x1]=mem.ram.bank[mempager.cart.ram_page]+0x1000;
        mempager.segment[0x2]=mem.ram.bank[mempager.cart.ram_page]+0x2000;
        mempager.segment[0x3]=mem.ram.bank[mempager.cart.ram_page]+0x3000;

        /* n�cessaire pour Bob Morane SF !! */
        if (mode_page.system1&8)
            mc6846.prc|=4;
    }
    else if (mc6846.prc&4)
    {   /* l'espace cartouche est mapp� sur la ROM interne */
        mempager.segment[0x0]=mem.rom.bank[mempager.cart.rom_page];
        mempager.segment[0x1]=mem.rom.bank[mempager.cart.rom_page]+0x1000;
        mempager.segment[0x2]=mem.rom.bank[mempager.cart.rom_page]+0x2000;
        mempager.segment[0x3]=mem.rom.bank[mempager.cart.rom_page]+0x3000;
    }
    else
    {   /* l'espace cartouche est mapp� sur la cartouche M�mo7 */
        if (mem.cart.nbank==0)
            mempager.segment[0x0]=cart_garbage;
        else
        {
            mempager.segment[0x0]=mem.cart.bank[mempager.cart.page];
            mempager.segment[0x1]=mem.cart.bank[mempager.cart.page]+0x1000;
            mempager.segment[0x2]=mem.cart.bank[mempager.cart.page]+0x2000;
            mempager.segment[0x3]=mem.cart.bank[mempager.cart.page]+0x3000;
        }
    }
}


static void update_screen(void)
{
    mempager.segment[0x4]=mem.ram.bank[0]+(1-mempager.screen.page)*0x2000;
    mempager.segment[0x5]=mem.ram.bank[0]+(1-mempager.screen.page)*0x2000+0x1000;
}


static void update_system(void)
{
    mempager.segment[0x6]=mem.ram.bank[mempager.system.page];
    mempager.segment[0x7]=mem.ram.bank[mempager.system.page]+0x1000;
    mempager.segment[0x8]=mem.ram.bank[mempager.system.page]+0x2000;
    mempager.segment[0x9]=mem.ram.bank[mempager.system.page]+0x3000;
}


static void update_data(void)
{
    if (mode_page.system1&0x10)
        mempager.data.page = mempager.data.reg_page;
    else
        mempager.data.page = mempager.data.pia_page;

    mempager.segment[0xA]=mem.ram.bank[mempager.data.page]+0x2000;
    mempager.segment[0xB]=mem.ram.bank[mempager.data.page]+0x3000;
    mempager.segment[0xC]=mem.ram.bank[mempager.data.page];
    mempager.segment[0xD]=mem.ram.bank[mempager.data.page]+0x1000;
}


static void update_mon(void)
{
    mempager.segment[0xE]=mem.mon.bank[mempager.mon.page];
    mempager.segment[0xF]=mem.mon.bank[mempager.mon.page]+0x1000;
}



/* update_color:
 *  Met � jour la couleur correspond � l'index sp�cifi�.
 */
static void update_color(int index)
{
    static int gamma[16] = {
        0  , 100, 127, 147,
        163, 179, 191, 203,
        215, 223, 231, 239,
        243, 247, 251, 255
    };

    teo_SetColor(index, gamma[ pal_chip.color[index].gr&0xF],
                        gamma[(pal_chip.color[index].gr&0xF0)>>4],
                        gamma[ pal_chip.color[index].b&0xF]);
}



/* SetDeviceRegister:
 *  D�pose un octet dans le registre du p�riph�rique et
 *  modifie en cons�quence son �tat.
 */
static void SetDeviceRegister(int addr, int val)
{
    int index;
    int data;

    switch (addr)
    {
        /* PIA 6846 syst�me */
        case 0xE7C1:
            data = mc6846.crc;
            mc6846_WriteCommand(&mc6846, val);

            /* $E7C1 generates sound only if b3 state changes */
            if (((mc6846.crc&0x30) == 0x30)
             && ((mc6846.crc&8) != (data&8)))
                teo_PutSoundByte(mc6809_clock(),
                                 mc6846.crc&8
                                   ? 0x00
                                   : (mc6821_ReadPort(&pia_ext.portb)&0x3F)<<2);
            break;

        case 0xE7C2:
            mc6846.ddrc=val;
            break;

        case 0xE7C3:
            mc6846_WriteData(&mc6846, val);

            /* bit 0: s�lection demi-page VRAM */
            mempager.screen.page = (mc6846.prc&1 ? 1 : 0);
            mempager.screen.update();

            /* bit 2: commutation page cartouche */
            mempager.cart.update();

            /* bit 4: s�lection page moniteur */
            mempager.mon.page = (mc6846.prc&0x10 ? 1 : 0);
            mempager.mon.update();

            /* bit 5: ACK clavier */
            keyboard_SetACK(mc6846.prc&0x20);
            break;

        case 0xE7C5:
            mc6846_SetTcr(&mc6846, val);
            break;

        case 0xE7C6:
            mc6846_SetTmsb(&mc6846, val);
            break;

        case 0xE7C7:
            mc6846_SetTlsb(&mc6846, val);
            break;

        /* PIA 6821 syst�me */
        case 0xE7C8:
            mc6821_WriteData(&pia_int.porta, val);

            /* �criture sur le port donn�e de l'imprimante */
            printer_WriteData(0xFE, mc6821_ReadPort(&pia_int.porta));
            break;

        case 0xE7C9:
            mc6821_WriteData(&pia_int.portb, val);

            /* �criture sur le port donn�e de l'imprimante */
            printer_WriteData(0x01, mc6821_ReadPort(&pia_int.portb));

            /* �criture sur le STROBE de l'imprimante */
            printer_SetStrobe(mc6821_ReadPort(&pia_int.portb)&0x02);

            switch (mc6821_ReadPort(&pia_int.portb)&0xF8)
            {
                case 0xF0:  /* DDRB 0x0F */
                    mempager.data.pia_page=2;
                    break;

                case 0xE8:  /* DDRB 0x17 */
                    mempager.data.pia_page=3;
                    break;

                case 0x18:  /* DDRB 0xE7 */
                    mempager.data.pia_page=4;
                    break;

                case 0x58:  /* DDRB 0xA7 */
                    mempager.data.pia_page=5;
                    break;

                case 0x98:  /* DDRB 0x67 */
                    mempager.data.pia_page=6;
                    break;

                case 0xD8:  /* DDRB 0x27 */
                    mempager.data.pia_page=7;
                    break;

/* le TO7-70 et le TO9 ont une correspondance subtilement diff�rente: 
   les cas 0x58 et 0x98 sont interchang�s !!!
   (voir Chip: le Cr�puscule du Naja routines 8029 et 8085 et le manuel
    technique TO8/9/9+ page 45) */
            }

            mempager.data.update();
            break;

        case 0xE7CA:
            mc6821_WriteCommand(&pia_int.porta, val);
#ifdef LEP_Motor
            if ((val&0x30) == 0x30)
                LEP_Motor(pia_int.porta.cr&8 ? OFF : ON);
#endif
            break;

        case 0xE7CB:
            mc6821_WriteCommand(&pia_int.portb, val);
            break;

        /* PIA 6821 musique et jeux */
        case 0xE7CC:
            mc6821_WriteData(&pia_ext.porta, val);
            break;

        case 0xE7CD:
            data = mc6821_ReadData(&pia_ext.portb);
            mc6821_WriteData(&pia_ext.portb, val);

            if ((mc6846.crc&8) == 0)  /* MUTE son inactif */
            {
                /* When the bit 2 of $e7cf is set to 0, the value put
                   into $e7cd is for the PIA direction register.
                   It is always possible to generate sound that way,
                   but only if b2-b3-b4-b5 state changes. It gives
                   then the opportunity to set some special codes
                   noiselessly (like $fc for the joystick) even
                   if the sound line is full open. */
                if (((mc6821_ReadCommand(&pia_ext.portb)&0x04) != 0)
                 || ((data&0x3c) != (mc6821_ReadData(&pia_ext.portb)&0x3c)))
                {
                    teo_PutSoundByte(
                        mc6809_clock(),
                        (mc6821_ReadPort(&pia_ext.portb)&0x3F)<<2);
                }
            }
            break;

        case 0xE7CE:
            mc6821_WriteCommand(&pia_ext.porta, val);
            break;

        case 0xE7CF:
            mc6821_WriteCommand(&pia_ext.portb, val);
            break;

        /* Gate Array lecteur de disquettes */
        case 0xE7D0:
            disk[dkcurr].dkc->SetReg0(val);
            break;

        case 0xE7D1:
            disk[dkcurr].dkc->SetReg1(val);
            break;

        case 0xE7D2:
            disk[dkcurr].dkc->SetReg2(val);
            break;

        case 0xE7D3:
            disk[dkcurr].dkc->SetReg3(val);
            break;

        case 0xE7D4:
            disk[dkcurr].dkc->SetReg4(val);
            break;

        case 0xE7D5:
            disk[dkcurr].dkc->SetReg5(val);
            break;

        case 0xE7D6:
            disk[dkcurr].dkc->SetReg6(val);
            break;

        case 0xE7D7:
            disk[dkcurr].dkc->SetReg7(val);
            break;

        case 0xE7D8:
            disk[dkcurr].dkc->SetReg8(val);
            break;

        case 0xE7D9:
            disk[dkcurr].dkc->SetReg9(val);
            break;

        /* Gate Array mode page */
        case 0xE7DA:
            index = (mode_page.p_addr>>1);

            if (mode_page.p_addr&1)
            {
                val = 0xE0|(val&0xF);
                pal_chip.color[index].b = val;
                pal_chip.update(index);
                teo_new_video_params = TRUE;
            }
            else 
            {
                pal_chip.color[index].gr = val;
                pal_chip.update(index);
                teo_new_video_params = TRUE;
            }

            mode_page.p_addr=(mode_page.p_addr+1)&0x1F;
            break;

        case 0xE7DB:
            mode_page.p_addr=val&0x1F;
            break;

        case 0xE7DC:
            if (val != mode_page.lgamod)
            {
                mode_page.lgamod=val;
                teo_new_video_params=TRUE;
            }
            break;

        case 0xE7DD:
            if (mempager.screen.vram_page != (val>>6)) /* commutation de la VRAM */
            {
               mempager.screen.vram_page=val>>6;
               teo_new_video_params=TRUE;
            }
            if ( ((mode_page.system2&0xF) != (val&0xF)) && teo_SetBorderColor)
                teo_SetBorderColor(mode_page.lgamod, val&0xF);

            mode_page.system2=val;
            break;

        case 0xE7E5:
            mode_page.ram_data=val;
 
            /* bit 0-4: s�lection page de l'espace donn�es */
            mempager.data.reg_page=val&0x1F;
            mempager.data.update();
            break;

        case 0xE7E6:  /* commutation de l'espace cartouche */
            mode_page.cart=val&0x7f;  /* b7 is a "dead" bit always set to 0 */
            mempager.cart.ram_page=val&0x1F;
            mempager.cart.update();
            break;

        case 0xE7E7:
            mode_page.system1=(val&0x5F)+(mode_page.system1&0xA0);

            /* bit 4: mode de commutation de l'espace donn�es */
            mempager.data.update();
            break;

    } /* end of switch */
}



/* GetRealValue_lp4:
 *  R�cup�re la valeur ajust�e de mode_page.lp4
 */
static inline int GetRealValue_lp4(void)
{
    mc6809_clock_t spot_pos, spot_point;
    int lp4;

    /* Positionne d'office b7 et b5 � 1 */
    lp4=mode_page.lp4|0xA0;

    /* Position du spot dans l'�cran */
    spot_pos=mc6809_clock()-screen_clock;

    /* Ajoute 1 cycle si ligne impaire:
        Pour assurer la "f�brilit�" du spot ?
        Chinese Stack ne fonctionne bien qu'avec un ajout
        de 1 cycle pour les lignes impaires, et le raster de
        couleurs de HCL en est parfaitement centr� */
    if ((spot_pos/FULL_LINE_CYCLES)&1)
        spot_pos+=1;
    
    /* Positionne b7 � 0 si le spot est au dessus
       ou au dessous de l'�cran affichable */
    spot_point=spot_pos/FULL_LINE_CYCLES;    /* Num�ro de ligne */
    if ((spot_point<TOP_SHADOW_LINES+TOP_BORDER_LINES)
        || (spot_point>=TOP_SHADOW_LINES+TOP_BORDER_LINES+WINDOW_LINES))
        lp4&=0x7f;

    /* Positionne b5 � 0 si le spot est � droite
       ou � gauche de l'�cran affichable */
    spot_point=spot_pos%FULL_LINE_CYCLES;    /* Num�ro de colonne */
    if ((spot_point<LEFT_SHADOW_CYCLES+LEFT_BORDER_CYCLES)
        || (spot_point>=LEFT_SHADOW_CYCLES+LEFT_BORDER_CYCLES+WINDOW_LINE_CYCLES))
        lp4&=0xDF;

    return lp4;
}


/* StoreWord:
 *  Ecrit deux octets en m�moire.
 */
static void StoreWord(int addr, int val)
{
    hardware_StoreByte(addr, (val>>8));
    hardware_StoreByte(addr+1, val&0xFF);
}



/* GetDeviceRegister:
 *  Lit un octet en m�moire.
 */
static int GetDeviceRegister(int addr)
{
    int index;

    if ((0xE7C0<=addr) && (addr<=0xE7FF))
        switch (addr)
        {
            /* PIA 6846 syst�me */
            case 0xE7C0:
            case 0xE7C4:
                return mc6846.csr;

            case 0xE7C1:
                return mc6846.crc;

            case 0xE7C2:
                return mc6846.ddrc;

            case 0xE7C3:
                return mc6846.prc;

            case 0xE7C5:
                return mc6846_tcr(&mc6846);

            case 0xE7C6:
                return mc6846_tmsb(&mc6846);

            case 0xE7C7:
                return mc6846_tlsb(&mc6846);

            /* PIA 6821 syst�me */
            case 0xE7C8:
                return mc6821_ReadData(&pia_int.porta);

            case 0xE7C9:
                return mc6821_ReadData(&pia_int.portb);

            case 0xE7CA:
                return mc6821_ReadCommand(&pia_int.porta);

            case 0xE7CB:
                return mc6821_ReadCommand(&pia_int.portb);

            /* PIA 6821 musique et jeux */
            case 0xE7CC:
                return mc6821_ReadData(&pia_ext.porta);

            case 0xE7CD:
                return mc6821_ReadData(&pia_ext.portb);

            case 0xE7CE:
                return mc6821_ReadCommand(&pia_ext.porta);

            case 0xE7CF:
                return mc6821_ReadCommand(&pia_ext.portb);

            /* Gate Array lecteur de disquettes */
            case 0xE7D0:
                return disk[dkcurr].dkc->GetReg0();

            case 0xE7D1:
                return disk[dkcurr].dkc->GetReg1();

            case 0xE7D2:
                return disk[dkcurr].dkc->GetReg2();

            case 0xE7D3:
                return disk[dkcurr].dkc->GetReg3();

            case 0xE7D4:
                return disk[dkcurr].dkc->GetReg4();

            case 0xE7D5:
                return disk[dkcurr].dkc->GetReg5();

            case 0xE7D6:
                return disk[dkcurr].dkc->GetReg6();

            case 0xE7D7:
                return disk[dkcurr].dkc->GetReg7();

            case 0xE7D8:
                return disk[dkcurr].dkc->GetReg8();

            case 0xE7D9:
                return disk[dkcurr].dkc->GetReg9();

            /* Gate Array mode page */
            case 0xE7DA:
                index = (mode_page.p_addr>>1);

                if (mode_page.p_addr&1)
                    mode_page.p_data = pal_chip.color[index].b;
                else
                    mode_page.p_data = pal_chip.color[index].gr;

                mode_page.p_addr = (mode_page.p_addr+1)&0x1F;
                return mode_page.p_data;

            case 0xE7DB:
                return mode_page.p_addr;

            case 0xE7DC:
            case 0xE7DD:
            case 0xE7DE:
            case 0xE7DF:
            case 0xE7E0:
            case 0xE7E1:
            case 0xE7E2:
            case 0xE7E3: 
                return 0xCC;

            case 0xE7E4:
                return (mode_page.commut&1 ? mode_page.lp1
                                           : mode_page.system2&0xC0);

            case 0xE7E5:
                /* Retourne le num�ro de banque courante m�me en mode PIA */
                return (mode_page.commut&1 ? mode_page.lp2
                                           : mempager.data.page&0x1F);

            case 0xE7E6:
                return (mode_page.commut&1 ? mode_page.lp3 : mode_page.cart);

            case 0xE7E7:
                return (/*mode_page.lp4*/ GetRealValue_lp4()&0xFE)
                                        | (mode_page.commut&1);
        }

    return LOAD_BYTE(addr);
}



/* undefined_ram_byte:
 *  Return a byte from the extra memory space without extension.
 */
static int undefined_ram_byte (void)
{
    static int count = 0;
    static int byte = 0;

    if (count-- <= 0)
    {
        byte = rand() % 100;
        count = rand() % 8;

        if (byte < 45)
             byte = 0xd8;
        else
        if (byte < 90)
             byte = 0xf0;
        else
        {
            count = 1;
            if (byte < 93) 
                byte = 0xe3;
            else
            if (byte < 95)
                byte = 0xc1;
            else
            if (byte < 97)
                byte = 0xff;
            else
                byte = 0x00;
        }
    }
    return byte;
}



/* LoadByte:
 *  Lit un octet en m�moire.
 */
int LoadByte (int addr)
{
    int msq=addr>>12;

    switch (msq)
    {
        case 0x0: /* espace cartouche */
        case 0x1:
        case 0x2:
        case 0x3:
            if (mode_page.cart&0x20)  /* l'espace est mapp� sur la RAM */
                if (mempager.cart.ram_page >= teo.setting.bank_range)
                    return undefined_ram_byte ();
            break;

        case 0xA: /* espace donn�es */
        case 0xB:
        case 0xC:
        case 0xD:
            if (mempager.data.page >= teo.setting.bank_range)
                return undefined_ram_byte ();
            break;

        case 0xE:
        case 0xF:
            if ((0xE7C0<=addr) && (addr<=0xE7FF))
                return GetDeviceRegister(addr);
            break;
    }
    return LOAD_BYTE (addr);
}



/* LoadWord:
 *  Lit deux octets en m�moire.
 */
static int LoadWord(int addr)
{
    return (LoadByte(addr)<<8)+LoadByte(addr+1);
}



/* FetchInstr:
 *  Remplit le buffer de fetch du CPU.
 */
static void FetchInstr(int addr, unsigned char fetch_buffer[])
{
    register int i;

    for (i=0; i<MC6809_DASM_FETCH_SIZE; i++)
        fetch_buffer[i]=LOAD_BYTE(addr+i);
}



/* BiosCall:
 *  Appel aux routines du BIOS de gestion des p�riph�riques.
 */
static int BiosCall(struct MC6809_REGS *regs)
{
    time_t x;
    struct tm *t;

    switch (regs->pc)
    {
        case 0x25D4:  /* routine d'affichage de la date */
            time(&x);
            t = gmtime(&x);
            STORE_BYTE(0x607C,t->tm_mday);
            STORE_BYTE(0x607D,t->tm_mon+1);
            STORE_BYTE(0x607E,t->tm_year%100);
            return 0x10; /* LDY imm�diat */

        case 0x315A:  /* routine de s�lection souris/crayon optique */
            teo_SetPointer( LOAD_BYTE(0x6074)&0x80 ? TEO_STATUS_MOUSE
                                                   : TEO_STATUS_LIGHTPEN);
            mouse_Reset();
            return 0x8E; /* LDX imm�diat */

        case 0x357A:  /* page de modification palette: fin */
            mode_page.lgamod=TEO_COL40;
            return 0x7D; /* TST �tendu */

        case 0x3686:  /* page de modification palette: d�but */
            mode_page.lgamod=TEO_PALETTE;
            return 0x86; /* LDA imm�diat */

        case 0x337E:  /* routine GETL crayon optique   */
        case 0x3F97:  /* routine GETL crayon optique 2 */
	    mouse_GetLightpen(&regs->xr, &regs->yr, &regs->cc);
            break;

        case 0xFA5A:  /* routine CASS */
            cass_Event(&regs->br,&regs->cc);
            break;

 	/* Imprimante */
        case 0xFB66:
            printer_Close();
            return 0x8a; /* ORA imm�diat */

    } /* end of switch */
    return 0x12;  /* NOP */
}


/* ------------------------------------------------------------------------- */


#ifdef DEBUG  /* la fonction n'est pas inlin�e */

void DrawGPL(int addr)
{ 
    int pt,col;

    if (addr>=0x1F40)
        return;

    pt =mem.ram.bank[mempager.screen.vram_page][addr];
    col=mem.ram.bank[mempager.screen.vram_page][addr+0x2000];

    teo_DrawGPL(mode_page.lgamod, addr, pt, col);
}

#endif



/* hardware_StoreByte:
 *  Ecrit un octet en m�moire.
 */
void hardware_StoreByte(int addr, int val)
{
    /* masque de commutation des pages de la cartouche */
    static const int page_mask[]={0, 0x0, 0x1, 0x2, 0x2};
    
    int msq=addr>>12;

    switch (msq)
    {
        case 0x0: /* espace cartouche */
        case 0x1:
        case 0x2:
        case 0x3:
            if (mode_page.cart&0x20)  /* l'espace est mapp� sur la RAM */
            {
                if (mempager.cart.ram_page < teo.setting.bank_range)
                {
                    if (mode_page.cart&0x40) /* l'�criture est autoris�e */
                    {
                        STORE_BYTE(addr, val);

                        if ((mempager.screen.vram_page==mempager.cart.ram_page)
                         && mb.direct_screen_mode)
                            DrawGPL(addr&0x1FFF);
                    }
                }
            }
            else if (addr<=0x1FFF)  /* commutation par latchage */
            {
                if (mc6846.prc&4)
                    mempager.cart.rom_page = addr&3;
                else
                    mempager.cart.page = addr&page_mask[mem.cart.nbank];
                    
                mempager.cart.update();
            }
            break;

        case 0x4: /* espace �cran */
        case 0x5:
            STORE_BYTE(addr, val);

            if ((mempager.screen.vram_page==0) && mb.direct_screen_mode)
                DrawGPL(addr&0x1FFF);

            break;

        case 0x6: /* espace syst�me non commutable */
        case 0x7:
        case 0x8:
        case 0x9:
            STORE_BYTE(addr, val);

            if ((mempager.screen.vram_page==1) && mb.direct_screen_mode)
                DrawGPL(addr&0x1FFF);

            break;

        case 0xA: /* espace donn�es */
        case 0xB:
        case 0xC:
        case 0xD:
            if (mempager.data.page < teo.setting.bank_range)
            {
                STORE_BYTE(addr, val);

                if ((mempager.screen.vram_page==mempager.data.page)
                 && mb.direct_screen_mode)
                    DrawGPL(addr&0x1FFF);
            }
            break;

        case 0xE:
        case 0xF:
            if ((0xE7C0<=addr) && (addr<=0xE7FF))
                SetDeviceRegister(addr,val&0xff);

            break;
    }
}



/* hardware_Init:
 *  Initialise la carte m�moire de l'�mulateur.
 */
void hardware_Init(void)
{
    register int i;

    time_t t;

    srand((unsigned) time(&t));

    mc6809_interface.FetchInstr   = FetchInstr;
    mc6809_interface.LoadByte     = LoadByte;
    mc6809_interface.LoadWord     = LoadWord;
    mc6809_interface.StoreByte    = hardware_StoreByte;
    mc6809_interface.StoreWord    = StoreWord;
    mc6809_interface.TrapCallback = BiosCall;
    mc6809_Init();

    /* circuit de palette vid�o */
    pal_chip.update = update_color;

    /* d�finition de la carte m�moire physique */
    /* pas de cartouche */
    mem.cart.nbank = 0;
    mem.cart.size = 0x4000;

    /* 64ko de ROM */
    mem.rom.nbank = 4;
    mem.rom.size = 0x4000;
    for (i=0; i<mem.rom.nbank; i++)
        mem.rom.bank[i] = NULL;
        

#ifdef DEBIAN_BUILD
    strcpy(mem.rom.filename[0], "/usr/share/teo/system/rom/basic512.rom");
    strcpy(mem.rom.filename[1], "/usr/share/teo/system/rom/extramon.rom");
    strcpy(mem.rom.filename[2], "/usr/share/teo/system/rom/basic1.rom");
    strcpy(mem.rom.filename[3], "/usr/share/teo/system/rom/expl.rom");
#else
    strcpy(mem.rom.filename[0], "system/rom/basic512.rom");
    strcpy(mem.rom.filename[1], "system/rom/extramon.rom");
    strcpy(mem.rom.filename[2], "system/rom/basic1.rom");
    strcpy(mem.rom.filename[3], "system/rom/expl.rom");
#endif

    /* 512ko de RAM */
    mem.ram.nbank = 32;            
    mem.ram.size = 0x4000;
    teo.setting.bank_range = 32;
    for (i=0; i<mem.ram.nbank; i++)
        mem.ram.bank[i] = NULL;

    /* 16ko de ROM moniteur */
    mem.mon.nbank = 2;
    mem.mon.size = 0x2000;
    for (i=0; i<mem.mon.nbank; i++)
        mem.mon.bank[i] = NULL;

#ifdef DEBIAN_BUILD
    strcpy(mem.mon.filename[0], "/usr/share/teo/system/rom/monitor1.rom");
    strcpy(mem.mon.filename[1], "/usr/share/teo/system/rom/monitor2.rom");
#else
    strcpy(mem.mon.filename[0], "system/rom/monitor1.rom");
    strcpy(mem.mon.filename[1], "system/rom/monitor2.rom");
#endif

    /* d�finition de la carte m�moire logique */
    mempager.cart.update = update_cart;
    mempager.screen.update = update_screen;
    mempager.system.update = update_system;
    mempager.data.update = update_data;
    mempager.mon.update = update_mon;

    /* carte m�re */
    mb.exact_clock = 0;
    mb.direct_screen_mode = TRUE;

    LOCK_VARIABLE(mc6809_irq);
    LOCK_VARIABLE(mc6846);
    LOCK_VARIABLE(pia_int);
    LOCK_VARIABLE(pia_ext);
    LOCK_VARIABLE(mode_page);
    LOCK_VARIABLE(pal_chip);
    LOCK_VARIABLE(mempager);
    LOCK_VARIABLE(mem);
    LOCK_VARIABLE(screen_clock);
}

