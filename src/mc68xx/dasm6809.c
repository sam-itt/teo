/*
 *  Module d'�mulation des micro-circuits Motorola MC68xx:
 *    - microprocesseur MC6809E
 *    - PIA MC6846
 *    - PIA MC6821
 *
 *  Copyright (C) 1996 Sylvain Huet, 1999 Eric Botcazou.
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
 *  Module     : mc68xx/dasm6809.c
 *  Version    : 1.4
 *  Cr�� par   : Sylvain Huet 1996
 *  Modifi� par: Eric Botcazou 23/11/2000
 *               Fran�ois Mouret 23/09/2013 10/05/2014
 *
 *  D�sassembleur du Motorola MC6809E
 *
 *  version 1.0: d�sassembleur fonctionnel
 *  version 1.1: bugfixes, interface
 *  version 1.2: correction d'un bug d'�criture de 0x10 et 0x11
 *  version 1.3: correction d'un bug des offsets 5-bit n�gatifs
 *               inclusion des d�clarations des tables externes
 *               nouvel identifiant du module: dasm
 *  version 1.4: le buffer d'entr�e est en unsigned char
 *               correction d'un bug de TFR et EXG
 *               corrections mineures (MacOS port)
 *  version 1.5: �limination des tabulations dans le texte
 *               clarification du code
 *               indentation avec des espaces
 *               structure en entr�e, taille en sortie
 *               r�cup�ration de caract�ristiques du code machine
 *               simplification du code pour PSHS, PSHU, PULS, PULU
 *               simplification du code pour index� 15 bits
 *               optimisation du code pour index�
 *               ajustement de l'offset pour index� avec PCR
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <string.h>
#endif

#include "mc68xx/dasm6809.h"
#include "mc68xx/mc6809.h"


/* tables des instructions */

int dasm6809_size[]=
{2,2,1,2,2,1,2,2,2,2,2,1,2,2,2,2
,0,0,1,1,1,1,3,3,1,1,2,1,2,1,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,1,1,1,1,2,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,2,1,1,2,2,1,2,2,2,2,2,1,2,2,2,2
,3,1,1,3,3,1,3,3,3,3,3,1,3,3,3,3
,2,2,2,3,2,2,2,1,2,2,2,2,3,2,3,1
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,2,2,2,3,2,2,2,1,2,2,2,2,3,1,3,1
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3

,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,4,1,1,1,1,1,1,1,1,4,1,4,1
,1,1,1,3,1,1,1,1,1,1,1,1,3,1,3,3
,1,1,1,3,1,1,1,1,1,1,1,1,3,1,3,3
,1,1,1,4,1,1,1,1,1,1,1,1,4,1,4,4
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4

,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,4,1,1,1,1,1,1,1,1,4,1,1,1
,1,1,1,3,1,1,1,1,1,1,1,1,3,1,1,1
,1,1,1,3,1,1,1,1,1,1,1,1,3,1,1,1
,1,1,1,4,1,1,1,1,1,1,1,1,4,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};


int dasm6809_addr[]=
{0,1,1,0,0,1,0,0,0,0,0,1,0,0,0,0
,6,6,1,1,1,1,2,2,1,1,3,1,3,1,1,1
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,4,4,4,4,3,3,3,3,1,1,1,1,3,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,1
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
,3,3,3,3,3,3,3,1,3,3,3,3,3,1,3,1
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5

,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,3,1,1,1,1,1,1,1,1,3,1,3,1
,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,0
,1,1,1,4,1,1,1,1,1,1,1,1,4,1,4,4
,1,1,1,5,1,1,1,1,1,1,1,1,5,1,5,5
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5

,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,3,1,1,1,1,1,1,1,1,3,1,1,1
,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1
,1,1,1,4,1,1,1,1,1,1,1,1,4,1,1,1
,1,1,1,5,1,1,1,1,1,1,1,1,5,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};


static const int cpu_cycles[]=
{6,2,0,6,6,0,6,6,6,6,6,0,6,6,3,6
,0,0,2,4,0,0,5,9,0,2,3,0,3,2,8,6
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,4,4,4,4,5,5,5,5,0,5,3,6,20,11,0,19
,2,0,0,2,2,0,2,2,2,2,2,0,2,2,0,2
,2,0,0,2,2,0,2,2,2,2,2,0,2,2,0,2
,6,0,0,6,6,0,6,6,6,6,6,0,6,6,3,6
,7,0,0,7,7,0,7,7,7,7,7,0,7,7,4,7
,2,2,2,4,2,2,2,0,2,2,2,2,4,7,3,0
,4,4,4,6,4,4,4,4,4,4,4,4,6,7,5,5
,4,4,4,6,4,4,4,4,4,4,4,4,6,7,5,5
,5,5,5,7,5,5,5,5,5,5,5,5,7,8,6,6
,2,2,2,4,2,2,2,0,2,2,2,2,3,0,3,0
,4,4,4,6,4,4,4,4,4,4,4,4,5,5,5,5
,4,4,4,6,4,4,4,4,4,4,4,4,5,5,5,5
,5,5,5,7,5,5,5,5,5,5,5,5,6,6,6,6

,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,5,0,0,0,0,0,0,0,0,5,0,4,0
,0,0,0,7,0,0,0,0,0,0,0,0,7,0,6,6
,0,0,0,7,0,0,0,0,0,0,0,0,7,0,6,6
,0,0,0,8,0,0,0,0,0,0,0,0,8,0,7,7
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7

,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,5,0,0,0,0,0,0,0,0,5,0,0,0
,0,0,0,7,0,0,0,0,0,0,0,0,7,0,0,0
,0,0,0,7,0,0,0,0,0,0,0,0,7,0,0,0
,0,0,0,8,0,0,0,0,0,0,0,0,8,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


static const char mne[]="\
NEG ??  ??  COM LSR ??  ROR ASR ASL ROL DEC ??  INC TST JMP CLR \
--  --  NOP SYNC??  ??  LBRALBSR??  DAA ORCC??  ANDCSEX EXG TFR \
BRA BRN BHI BLS BCC BLO BNE BEQ BVC BVS BPL BMI BGE BLT BGT BLE \
LEAXLEAYLEASLEAUPSHSPULSPSHUPULU??  RTS ABX RTI CWAIMUL ??  SWI \
NEGA??  ??  COMALSRA??  RORAASRAASLAROLADECA??  INCATSTA??  CLRA\
NEGB??  ??  COMBLSRB??  RORBASRBLSLBROLBDECB??  INCBTSTB??  CLRB\
NEG ??  ??  COM LSR ??  ROR ASR ASL ROL DEC ??  INC TST JMP CLR \
NEG ??  ??  COM LSR ??  ROR ASR ASL ROL DEC ??  INC TST JMP CLR \
SUBACMPASBCASUBDANDABITALDA ??  EORAADCAORA ADDACMPXBSR LDX ??  \
SUBACMPASBCASUBDANDABITALDA STA EORAADCAORA ADDACMPXJSR LDX STX \
SUBACMPASBCASUBDANDABITALDA STA EORAADCAORA ADDACMPXJSR LDX STX \
SUBACMPASBCASUBDANDABITALDA STA EORAADCAORA ADDACMPXJSR LDX STX \
SUBBCMPBSBCBADDDANDBBITBLDB ??  EORBADCBORB ADDBLDD ??  LDU ??  \
SUBBCMPBSBCBADDDANDBBITBLDB STB EORBADCBORB ADDBLDD STD LDU STU \
SUBBCMPBSBCBADDDANDBBITBLDB STB EORBADCBORB ADDBLDD STD LDU STU \
SUBBCMPBSBCBADDDANDBBITBLDB STB EORBADCBORB ADDBLDD STD LDU STU \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  LBRNLBHILBLSLBCCLBLOLBNELBEQLBVCLBVSLBPLLBMILBGELBLTLBGTLBLE\
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  SWI2\
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY ??  \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY STY \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY STY \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY STY \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS STS \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS STS \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS STS \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  SWI3\
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
";

static const char reg[16][3] = {"D" ,"X" ,"Y" ,"U" ,
                                "S" ,"PC","?" ,"?" ,
                                "A" ,"B" ,"CC","DP",
                                "?" ,"?" ,"?" ,"?" };
static const int  stack_reg[2][8]={ {5,3,2,1,11,9,8,10},
                                    {5,4,2,1,11,9,8,10} };



/* dasm6809_Disassemble:
 *  D�sassemble une instruction 6809 et retourne sa taille.
 */
int dasm6809_Disassemble(struct MC6809_DASM *mc6809_dasm)
{
    register int i;
    int size, offset, opcode=mc6809_dasm->fetch[0];
    int cycle1 = -1;
    int cycle2 = -1;
    int cycle3 = -1;
    char asm_buffer[MC6809_DASM_BUFFER_SIZE/2], *p=asm_buffer;
    char *out_str = mc6809_dasm->str;
    unsigned char *in_str =  mc6809_dasm->fetch;

    /* codes 0x10 et 0x11 d'instruction longue */
    if (opcode == 0x10)
        opcode = 256 + (in_str++[1]);
    else
    if (opcode == 0x11)
        opcode = 512 + (in_str++[1]);

    /* affichage du mn�monique */
    strncpy (p, mne+opcode*4, 4);
    p += 4;
    if (opcode == 0x1c)  /* ANDCC */
        p += sprintf (p, "C");
    else
        p += sprintf (p, " ");
    p += sprintf (p, "  ");

    size = dasm6809_size[opcode];
    cycle1 = cpu_cycles[opcode];

    switch (dasm6809_addr[opcode]) {
    case 0:  /* direct */
        p += sprintf(p,"<$%02X", in_str[1]);
        break;

    case 1:  /* inh�rent */
        if ((opcode == 0x1E) || (opcode == 0x1F)) /* TFR et EXG */
            p += sprintf (p, "%s,%s", 
                             reg[(in_str[1]>>4)&0xF],
                             reg[in_str[1]&0xF]);
        if (opcode == 0x3B) /* RTI */
            cycle3 = 15;
        break;

    case 2: /* relatif */
        offset = (size==2 ? (signed char)in_str[1]
                          : (signed char)in_str[1]*256+in_str[2] );
        p += sprintf (p, "$%04X", mc6809_dasm->addr + size + offset);
        if ((opcode>=0x121) && (opcode<=0x12f))
            cycle3 = 6;
        break;

    case 3: /* imm�diat */
        if ((opcode&0xFC) == 0x34)  /* piles S et U */
        {
            cycle2 = 0;
            if (opcode&1)
            {        /* on d�pile */
                for (i=7; i>=0; i--)
                {
                    if ((0x80>>i)&in_str[1])
                    {
                        p += sprintf(p,((cycle2))?",":"");
                        p += sprintf (p,"%s", reg[stack_reg[(opcode&2)>>1][i]]);
                        cycle2 += (i > 3) ? 2 : 1;
                    }
                }
            }
            else
            {        /* on empile */
                for (i=0; i<8; i++)
                {
                    if ((0x80>>i)&in_str[1])
                    {
                        p += sprintf(p,((cycle2))?",":"");
                        p += sprintf(p, "%s", reg[stack_reg[(opcode&2)>>1][i]]);
                        cycle2 += (i < 4) ? 1 : 2;
                    }
                }
            }
        }
        else if (size == 2)
            p += sprintf (p, "#$%02X", in_str[1]);
        else
            p += sprintf (p, "#$%04X", in_str[1]*256+in_str[2]);
        break;

	case 4: /* indexed */
        switch (in_str[1]&0x8F) {
        case 0x89 :
        case 0x8d :
        case 0x8f :
            p += sprintf(p,">");
            break;

        case 0x88 :
        case 0x8c :
             p += sprintf(p,"<");
             break;
        }

        if ((in_str[1]&0x90) == 0x90)
            p += sprintf(p,"[");

        switch (in_str[1]&0x8f) {
        case 0x80:  /* ,r+ */
            p += sprintf (p, ",%s+", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 2;
            break;

        case 0x81:  /* ,r++ */
            p += sprintf (p, ",%s++", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 3;
            break;

        case 0x82:  /* ,-r */
            p += sprintf (p, ",-%s", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 2;
            break;

        case 0x83:  /* ,--r */
            p += sprintf (p, ",--%s", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 3;
            break;

        case 0x84:  /* ,r */
            p += sprintf (p, ",%s", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 0;
            break;

        case 0x85:  /* B,r */
            p += sprintf (p, "B,%s", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 1;
            break;

        case 0x86:  /* A,r */
            p += sprintf (p, "A,%s", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 1;
            break;

        case 0x87:  /* error */
            break;

        case 0x88:  /* $00,r */
            offset = (signed char)in_str[2];
            if (offset < 0)
            {
                p += sprintf(p,"-");
                offset = -offset;
            }
            p += sprintf (p, "$%02X,%s", offset, reg[((in_str[1]&0x60)>>5)+1]);
            size++;
            cycle2 = 1;
            break;

        case 0x89:  /* $0000,r */
            offset = (signed char)in_str[2]*256+in_str[3];
            if (offset < 0)
            {
                p += sprintf (p, "-");
                offset = -offset;
            }
            p += sprintf (p, "$%04X,%s", offset, reg[((in_str[1]&0x60)>>5)+1]);
            size += 2;
            cycle2 = 4;
            break;

        case 0x8a:  /* error */
            break;

        case 0x8b:  /* D,r */
            p += sprintf (p, "D,%s", reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 4;
            break;

        case 0x8c:  /* $00,PCR */
            offset = (signed char)in_str[2];
            size++;
            offset += size + mc6809_dasm->addr;
            p+=sprintf(p,"$%04X,PCR",offset&0xffff);
            cycle2 = 1;
            break;

        case 0x8d:  /* $0000,PCR */
            offset = (signed char)in_str[2]*256+in_str[3];
            size += 2;
            offset += size + mc6809_dasm->addr;
            p += sprintf (p, "$%04X,PCR", offset&0xffff);
            cycle2 = 5;
            break;

        case 0x8e:  /* error */
            break;

        case 0x8f:  /* $0000 */
            p += sprintf (p, "$%04X", in_str[2]*256+in_str[3]);
            size += 2;
            cycle2 = 2;
            break;

        default:
            /* 0,r 5 bits */
            offset = ((int)in_str[1]&0xf)-((int)in_str[1]&0x10);
            p += sprintf (p, "%d,%s", offset, reg[((in_str[1]&0x60)>>5)+1]);
            cycle2 = 1;
            break;
        }  /* end of switch (in_str[1]&0x8f) */

        if ((in_str[1]&0x90) == 0x90)
        {
            p += sprintf (p, "]");
            cycle2 += 3;
        }
        break;

	case 5: /* �tendu */
        p += sprintf (p, ">$%04X", (in_str[1]*256+in_str[2])&0xffff);
        break;

    default: /* erreur */
        p += sprintf (p, "???");
        break;
    }

    out_str += sprintf (out_str, "%04X  ", mc6809_dasm->addr);

    if (mc6809_dasm->mode == MC6809_DASM_BINASM_MODE)
    {
        if (opcode > 255)  /* instruction longue */
            in_str--;

        for (i=0; i<size; i++)
            out_str += sprintf (out_str, "%02X ", in_str[i]);

        for (i=size; i<MC6809_DASM_FETCH_SIZE; i++)
            out_str += sprintf (out_str, "   ");

        out_str += sprintf (out_str, " ");
    }

    mc6809_dasm->cycle1 = cycle1;
    mc6809_dasm->cycle2 = cycle2;
    mc6809_dasm->cycle3 = cycle3;

    strcat (mc6809_dasm->str, asm_buffer);

    return size;
}
