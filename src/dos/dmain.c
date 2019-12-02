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
 *  Copyright (C) 1997-2018 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
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
 *  Module     : dos/main.c
 *  Version    : 1.8.5
 *  Cr�� par   : Gilles F�tis 1998
 *  Modifi� par: Eric Botcazou 04/11/2003
 *               Samuel Devulder 08/2011
 *               Fran�ois Mouret 08/2011 25/04/2012 01/11/2012
 *                               19/09/2013 13/04/2014 31/07/2016
 *                               25/10/2018
 *
 *  Boucle principale de l'�mulateur.
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <allegro.h>
#endif

#include "defs.h"
#include "teo.h"
#include "option.h"
#include "ini.h"
#include "image.h"
#include "main.h"
#include "errors.h"
#include "media/disk.h"
#include "media/cass.h"
#include "media/memo.h"
#include "media/printer.h"
#include "mc68xx/mc6809.h"
#include "alleg/gfxdrv.h"
#include "alleg/gui.h"
#include "alleg/joyint.h"
#include "alleg/mouse.h"
#include "alleg/sound.h"
#include "dos/keybint.h"
#include "dos/floppy.h"
#include "dos/debug.h"


/* pour limiter la taille de l'�x�cutable */
BEGIN_COLOR_DEPTH_LIST
    COLOR_DEPTH_8
    COLOR_DEPTH_15
    COLOR_DEPTH_16
END_COLOR_DEPTH_LIST

BEGIN_GFX_DRIVER_LIST
    GFX_DRIVER_VGA
    GFX_DRIVER_VBEAF
    GFX_DRIVER_VESA3
    GFX_DRIVER_VESA2L
    GFX_DRIVER_VESA2B
    GFX_DRIVER_VESA1
END_GFX_DRIVER_LIST

BEGIN_DIGI_DRIVER_LIST
    DIGI_DRIVER_SB
    DIGI_DRIVER_SOUNDSCAPE
    DIGI_DRIVER_AUDIODRIVE
    DIGI_DRIVER_WINSOUNDSYS
END_DIGI_DRIVER_LIST 

BEGIN_MIDI_DRIVER_LIST
END_MIDI_DRIVER_LIST

struct EMUTEO teo;

static int reset = FALSE;
static int gfx_mode = NO_GFX;
struct STRING_LIST *remain_name = NULL;

int frame;                 /* compteur de frame vid�o */
int direct_write_support = TRUE;
static volatile int tick;  /* compteur du timer */


static void Timer(void)
{
    tick++;
}
END_OF_FUNCTION(Timer)



/* RunTO8:
 *  Boucle principale de l'�mulateur.
 */
static void RunTO8(void)
{
    amouse_Install (TEO_STATUS_MOUSE); /* la souris est le p�riph�rique de pointage par d�faut */
    RetraceScreen(0, 0, SCREEN_W, SCREEN_H);

    do  /* boucle principale de l'�mulateur */
    {
        teo.command=TEO_COMMAND_NONE;

        /* installation des handlers clavier, souris et son */ 
        dkeybint_Install();
        amouse_Install(LAST_POINTER);

        if (teo.setting.exact_speed)
        {
            if (teo.setting.sound_enabled)
                asound_Start();
            else
            {
                install_int_ex(Timer, BPS_TO_TIMER(TEO_FRAME_FREQ));
                frame=1;
                tick=frame;
            }
        }

        do  /* boucle d'�mulation */
        {
            (void)teo_DoFrame();

            /* rafra�chissement de la palette */ 
            if (need_palette_refresh)
                RefreshPalette();

            /* rafra�chissement de l'�cran */
            RefreshScreen();

            /* mise � jour de la position des joysticks */
            ajoyint_Update();

            /* synchronisation sur fr�quence r�elle */
            if (teo.setting.exact_speed)
            {
                if (teo.setting.sound_enabled)
                    asound_Play ();
                else
                    while (frame==tick)
                        ;
            }
            disk_WriteTimeout();
            frame++;
        }
        while (teo.command==TEO_COMMAND_NONE);  /* fin de la boucle d'�mulation */

        /* d�sinstallation des handlers clavier, souris et son */
        if (teo.setting.exact_speed)
        {
            if (teo.setting.sound_enabled)
                asound_Stop();
            else
                remove_int(Timer);
        }

        amouse_ShutDown();
        dkeybint_ShutDown();

        /* �x�cution des commandes */
        if (teo.command==TEO_COMMAND_PANEL)
            agui_Panel();

        if (teo.command==TEO_COMMAND_SCREENSHOT)
            agfxdrv_Screenshot();

        if (teo.command==TEO_COMMAND_DEBUGGER)
        {
            remove_keyboard();
            SetGraphicMode(SHUTDOWN);
            ddebug_Run();
            SetGraphicMode(RESTORE);
            install_keyboard();
        }

        if (teo.command==TEO_COMMAND_RESET)
            teo_Reset();

        if (teo.command==TEO_COMMAND_COLD_RESET)
        {
            teo_ColdReset();
            amouse_Install(TEO_STATUS_MOUSE);
        }

        if (teo.command==TEO_COMMAND_FULL_RESET)
        {
            teo_FullReset();
            amouse_Install(TEO_STATUS_MOUSE);
        }
    }
    while (teo.command != TEO_COMMAND_QUIT);  /* fin de la boucle principale */

    /* Finit d'ex�cuter l'instruction et/ou l'interruption courante */
    mc6809_FlushExec();
}



/* main_ErrorMessage:
 *  Affiche un message d'erreur et sort du programme.
 */
static void main_ErrorMessage(const char msg[])
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}



/* ReadCommandLine:
 *  Lit la ligne de commande
 */
static void ReadCommandLine(int argc, char *argv[])
{
    char *message;
    int mode40=0, mode80=0, truecolor=0;

    struct OPTION_ENTRY entries[] = {
        { "reset", 'r', OPTION_ARG_BOOL, &reset,
           is_fr?"Reset � froid de l'�mulateur"
                :"Cold-reset emulator", NULL },
        { "disk0", '0', OPTION_ARG_FILENAME, &teo.disk[0].file,
           is_fr?"Charge un disque virtuel (lecteur 0)"
                :"Load virtual disk (drive 0)",
           is_fr?"FICHIER":"FILE" },
        { "disk1", '1', OPTION_ARG_FILENAME, &teo.disk[1].file,
           is_fr?"Charge un disque virtuel (lecteur 1)"
                :"Load virtual disk (drive 1)",
           is_fr?"FICHIER":"FILE" },
        { "disk2", '2', OPTION_ARG_FILENAME, &teo.disk[2].file,
           is_fr?"Charge un disque virtuel (lecteur 2)"
                :"Load virtual disk (drive 2)",
           is_fr?"FICHIER":"FILE" },
        { "disk3", '3', OPTION_ARG_FILENAME, &teo.disk[3].file,
           is_fr?"Charge un disque virtuel (lecteur 3)"
                :"Load virtual disk (drive 3)",
           is_fr?"FICHIER":"FILE" },
        { "cass", '\0', OPTION_ARG_FILENAME, &teo.cass.file,
           is_fr?"Charge une cassette":"Load a tape",
           is_fr?"FICHIER":"FILE" },
        { "memo", '\0', OPTION_ARG_FILENAME, &teo.memo.file,
           is_fr?"Charge une cartouche":"Load a cartridge",
           is_fr?"FICHIER":"FILE" },
        { "mode40", '\0', OPTION_ARG_BOOL, &mode40,
           is_fr?"Affichage en 40 colonnes":"40 columns display", NULL},
        { "mode80", '\0', OPTION_ARG_BOOL, &mode80,
           is_fr?"Affichage en 80 colonnes":"80 columns display", NULL},
        { "truecolor", '\0', OPTION_ARG_BOOL, &truecolor,
           is_fr?"Affichage en vraies couleurs":"Truecolor display", NULL},
        { NULL, 0, 0, NULL, NULL, NULL }
    };
    message = option_Parse (argc, argv, "teo", entries, &remain_name);
    if (message != NULL)
        main_ErrorMessage(message);
        
    if (mode40)    gfx_mode = GFX_MODE40   ; else
    if (mode80)    gfx_mode = GFX_MODE80   ; else
    if (truecolor) gfx_mode = GFX_TRUECOLOR;
}



/* thomson_take char:
 *  Convert Thomson ASCII char into ISO-8859-1.
 */
static int thomson_take_char (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case '\x60':   /* middle line */
            strcat (pc_text, "\xad");
            i++;
            break;

        case '\x7e':   /* overline */
            strcat (pc_text, "\xaf");
            i++;
            break;

        case '\x7f':   /* full block */
            strcat (pc_text, " ");
            i++;
            break;

        default:       /* ASCII char */
            if (thomson_text[i] != '\0')
            {
                if (thomson_text[i] >= ' ')
                {
                    strncat (pc_text, thomson_text+i, 1);
                    i++;
                }
                else
                {
                    strcat (pc_text, "?");
                    i++;
                }
            }
            break;
    }
    return i;
}



/* thomson_accent_grave:
 *  Convert Thomson grave accents into ISO-8859-1.
 */
static int thomson_accent_grave (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case 'a': strcat (pc_text, "\xe0"); i++; break;
        case 'e': strcat (pc_text, "\xe8"); i++; break;
        case 'i': strcat (pc_text, "\xec"); i++; break;
        case 'o': strcat (pc_text, "\xf2"); i++; break;
        case 'u': strcat (pc_text, "\xf9"); i++; break;
        case 'A': strcat (pc_text, "\xc0"); i++; break;
        case 'E': strcat (pc_text, "\xc8"); i++; break;
        case 'I': strcat (pc_text, "\xcc"); i++; break;
        case 'O': strcat (pc_text, "\xd2"); i++; break;
        case 'U': strcat (pc_text, "\xd9"); i++; break;
        default : i = thomson_take_char (thomson_text, i, pc_text); break;
    }
    return i;
}



/* thomson_accent_acute:
 *  Convert Thomson acute accents into ISO-8859-1.
 */
static int thomson_accent_acute (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case 'a': strcat (pc_text, "\xe1"); i++; break;
        case 'e': strcat (pc_text, "\xe9"); i++; break;
        case 'i': strcat (pc_text, "\xed"); i++; break;
        case 'o': strcat (pc_text, "\xf3"); i++; break;
        case 'u': strcat (pc_text, "\xfa"); i++; break;
        case 'A': strcat (pc_text, "\xc1"); i++; break;
        case 'E': strcat (pc_text, "\xc9"); i++; break;
        case 'I': strcat (pc_text, "\xcd"); i++; break;
        case 'O': strcat (pc_text, "\xd3"); i++; break;
        case 'U': strcat (pc_text, "\xda"); i++; break;
        default : i = thomson_take_char (thomson_text, i, pc_text); break;
    }
    return i;
}



/* thomson_accent_circ:
 *  Convert Thomson circumflex accents into ISO-8859-1.
 */
static int thomson_accent_circ (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case 'a': strcat (pc_text, "\xe2"); i++; break;
        case 'e': strcat (pc_text, "\xea"); i++; break;
        case 'i': strcat (pc_text, "\xee"); i++; break;
        case 'o': strcat (pc_text, "\xf4"); i++; break;
        case 'u': strcat (pc_text, "\xfb"); i++; break;
        case 'A': strcat (pc_text, "\xc2"); i++; break;
        case 'E': strcat (pc_text, "\xca"); i++; break;
        case 'I': strcat (pc_text, "\xce"); i++; break;
        case 'O': strcat (pc_text, "\xd4"); i++; break;
        case 'U': strcat (pc_text, "\xdb"); i++; break;
        default : i = thomson_take_char (thomson_text, i, pc_text); break;
    }
    return i;
}



/* thomson_accent_uml:
 *  Convert Thomson diaeresis accents into ISO-8859-1.
 */
static int thomson_accent_uml (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case 'a': strcat (pc_text, "\xe4"); i++; break;
        case 'e': strcat (pc_text, "\xeb"); i++; break;
        case 'i': strcat (pc_text, "\xef"); i++; break;
        case 'o': strcat (pc_text, "\xf6"); i++; break;
        case 'u': strcat (pc_text, "\xfc"); i++; break;
        case 'A': strcat (pc_text, "\xc4"); i++; break;
        case 'E': strcat (pc_text, "\xcb"); i++; break;
        case 'I': strcat (pc_text, "\xcf"); i++; break;
        case 'O': strcat (pc_text, "\xd6"); i++; break;
        case 'U': strcat (pc_text, "\xdc"); i++; break;
        default : i = thomson_take_char (thomson_text, i, pc_text); break;
    }
    return i;
}



/* thomson_accent_cedil:
 *  Convert Thomson cedilla into ISO-8859-1.
 */
static int thomson_accent_cedil (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case 'c': strcat (pc_text, "\xe7"); i++; break;
        case 'C': strcat (pc_text, "\xc7"); i++; break;
        default : i = thomson_take_char (thomson_text, i, pc_text); break;
    }
    return i;
}



/* thomson_accent:
 *  Convert Thomson accent into ISO-8859-1.
 */
static int thomson_accent (char *thomson_text, int i, char *pc_text)
{
    switch (thomson_text[i])
    {
        case '\0': break;
        case 'A': i = thomson_accent_grave (thomson_text, i+1, pc_text); break;
        case 'B': i = thomson_accent_acute (thomson_text, i+1, pc_text); break;
        case 'C': i = thomson_accent_circ  (thomson_text, i+1, pc_text); break;
        case 'H': i = thomson_accent_uml   (thomson_text, i+1, pc_text); break;
        case 'K': i = thomson_accent_cedil (thomson_text, i+1, pc_text); break;
        case '#': strcat (pc_text, "\xa3"); i++; break;  /* pound */
        case '$': strcat (pc_text, "$"); i++; break;  /* dollar */
        case '&': strcat (pc_text, "#"); i++; break;  /* diesis */
        case ',': strcat (pc_text, "<-"); i++; break;  /* arrow left */
        case '-': strcat (pc_text, "^"); i++; break;  /* arrow up */
        case '.': strcat (pc_text, "->"); i++; break;  /* arrow right */
        case '/': strcat (pc_text, "V"); i++; break;  /* arrow down */
        case '0': strcat (pc_text, "\xb0"); i++; break; /* degree */
        case '1': strcat (pc_text, "\xb1"); i++; break; /*plus minus */
        case '8': strcat (pc_text, "\xf7"); i++; break; /* */
        case '<': strcat (pc_text, "\xbc"); i++; break; /* 1/4 */
        case '=': strcat (pc_text, "\xbd"); i++; break; /* 1/2 */
        case '>': strcat (pc_text, "\xbe"); i++; break; /* 3/4 */
        case 'j': strcat (pc_text, "OE"); i++; break;   /* */
        case 'z': strcat (pc_text, "oe"); i++; break;   /* */
        case '{': strcat (pc_text, "\xdf"); i++; break; /* sharp S */
        case '\'':strcat (pc_text, "\xa7"); i++; break; /* */
        default : i++; break;
    }
    return i;
}



/* thomson_escape:
 *  Skip Thomson escape sequence.
 */
static int thomson_escape (char *thomson_text, int i)
{
    /* skip full screen codes */
    while ((thomson_text[i] == '\x20')
        || (thomson_text[i] == '\x23'))
    {
        i++;
    }

    /* skip escape code */
    if (thomson_text[i] != '\0')
    {
        i++;
    }
    return i;
}



/* thomson_cursor:
 *  Skip Thomson cursor sequence.
 */
static int thomson_cursor (char *thomson_text, int i)
{
    /* skip first code */
    if (thomson_text[i] != '\0')
    {
        i++;
        /* skip second code */
        if (thomson_text[i] != '\0')
        {
            i++;
        }
    }
    return i;
}


/* ------------------------------------------------------------------------- */
 
 
/* main_ThomsonToPcText:
 *  Convert Thomson string into ISO-8859-1.
 */
char *main_ThomsonToPcText (char *thomson_text)
{
    int i = 0;
    static char pc_text[306];

    pc_text[0] = 0;

    while (thomson_text[i] != '\0')
    {
        switch (thomson_text[i])
        {
            case '\x16':   /* accent sequence */
                i = thomson_accent (thomson_text, i+1, pc_text);
                break;

            case '\x1b':   /* escape sequence */
                i = thomson_escape (thomson_text, i+1);
                break;

            case '\x1f':   /* cursor sequence */
                i = thomson_cursor (thomson_text, i+1);
                break;

            default:
                i = thomson_take_char (thomson_text, i, pc_text);
                break;
        }
     }

     return pc_text;
}



/* main_DisplayMessage:
 *  Affiche un message de sortie et sort du programme.
 */
void main_DisplayMessage(const char msg[])
{
    agui_PopupMessage (msg);
}


/* main_ExitMessage:
 *  Affiche un message de sortie et sort du programme.
 */
void main_ExitMessage(const char msg[])
{
    main_DisplayMessage(msg);
    exit(EXIT_FAILURE);
}



#define IS_3_INCHES(drive) ((drive_type[drive]>2) && (drive_type[drive]<7))

/* main:
 *  Point d'entr�e du programme appel� par MS-DOS.
 */
int main(int argc, char *argv[])
{
    char version_name[]="Teo "TEO_VERSION_STR" (MSDOS/DPMI)";
#ifdef FRENCH_LANGUAGE
    char *mode_desc[3]= {
        " 1. Mode 40 colonnes 16 couleurs\n    (affichage rapide, adapt� aux jeux et � la plupart des applications)",
        " 2. Mode 80 colonnes 16 couleurs\n    (pour les applications fonctionnant en 80 colonnes)",
        " 3. Mode 80 colonnes 4096 couleurs\n    (affichage lent mais support des changements dynamiques de palette)" };
#else
    char *mode_desc[3]= {
        " 1. 40 columns mode 16 colors\n    (fast diplay, adapted to games and most applications)",
        " 2. 80 columns mode 16 colors\n    (for applications which needs 80 columns)",
        " 3. 80 columns mode 4096 colors\n    (slow display but allow dynamic changes of palette)" };
#endif
    int direct_support = 0;
    int drive_type[4];
    int njoy = 0;  /* njoy=-1 si joystick non support�s */
    int scancode, i;
    struct STRING_LIST *str_list = NULL;
    char *cfg_file;

#ifdef FRENCH_LANGUAGE
    is_fr = 1;
#else
    is_fr = 0;
#endif

    /* traitement des param�tres */
    ini_Load();                   /* Charge les param�tres par d�faut */
    ReadCommandLine (argc, argv); /* R�cup�ration des options */

    /* initialisation de la librairie Allegro */
    set_uformat(U_ASCII);  /* pour les accents fran�ais */
    allegro_init();
    /*
    if(allegro_init() != 0){
        printf("Couldn't initialize Allegro, bailing out !\n");
        exit(EXIT_FAILURE);
    }*/

    cfg_file = std_GetFirstExistingConfigFile(ALLEGRO_CONFIG_FILE);
    if(cfg_file){
        set_config_file(cfg_file);
        std_free(cfg_file);
    }else{
        printf("Config file %s not found, using default values\n",ALLEGRO_CONFIG_FILE);
    }

    cfg_file = std_GetFirstExistingConfigFile("akeymap.ini");
    if(cfg_file){
        override_config_file(cfg_file);
        std_free(cfg_file);
    }else{
        printf("Keymap %s not found !\n","akeymap.ini");
        exit(-1);
    }

    install_keyboard();
    install_timer();
    if (njoy >= 0)
        install_joystick(JOY_TYPE_AUTODETECT);
    LOCK_VARIABLE(teo);
    LOCK_VARIABLE(tick);
    LOCK_FUNCTION(Timer);

    /* message d'ent�te */
    if (is_fr) {
    printf("Voici %s l'�mulateur Thomson TO8.\n", version_name);
    printf("Copyright 1997-2012 Gilles F�tis, Eric Botcazou, Alex Pukall,J�r�mie Guillaume, Fran�ois Mouret, Samuel Devulder\n\n");
    printf("Touches: [ESC] Panneau de contr�le\n");
    printf("         [F11] Capture d'�cran\n");
    printf("         [F12] D�bogueur\n\n");
    } else {
    printf("Here is %s the Thomson TO8 emulator.\n", version_name);
    printf("Copyright 1997-2012 Gilles F�tis, Eric Botcazou, Alex Pukall,J�r�mie Guillaume, Fran�ois Mouret, Samuel Devulder\n\n");
    printf("Keys: [ESC] Control panel\n");
    printf("      [F11] Screen capture\n");
    printf("      [F12] Debugger\n\n");
    }

    /* d�tection de la pr�sence de joystick(s) */
    njoy = MIN(TEO_NJOYSTICKS, num_joysticks);

    /* initialisation de l'�mulateur */
    printf(is_fr?"Initialisation de l'�mulateur...":"Emulator initialization...");

    if (teo_Init(TEO_NJOYSTICKS-njoy) < 0)
        main_ErrorMessage(teo_error_msg);

    printf("ok\n");

    /* initialisation de l'interface clavier */
    dkeybint_Init();

    /* initialisation de l'interface d'acc�s direct */
    dfloppy_Init (drive_type, direct_write_support);

    /* D�tection des lecteurs support�s (3"5 seulement) */
    for (i=0; i<4; i++)
    {
        if (IS_3_INCHES(i))
            direct_support |= (1<<i);
    }

    /* initialisation du son */
    asound_Init(25600);  /* 25600 Hz */

    /* initialisation des joysticks */
    ajoyint_Init(njoy);

    /* s�lection du mode graphique */ 
    printf(is_fr?"\nS�lection du mode graphique:\n\n":"\nSelect graphic mode:\n\n");

    if (gfx_mode == NO_GFX)
    {
        for (i=0; i<3; i++)
            printf("%s\n\n", mode_desc[i]);
            
        printf(is_fr?"Votre choix: [1 par d�faut] ":"Your choice: [1 by default] ");

        do
        {
            scancode = readkey()>>8;

            if (key_shifts&KB_CTRL_FLAG)
               reset = 0;

            switch (scancode_to_ascii(scancode))
            {
                case '1':
                case 13:
                    gfx_mode=GFX_MODE40;
                    break;

                case '2':
                    gfx_mode=GFX_MODE80;
                    break;

                case '3':
                    gfx_mode=GFX_TRUECOLOR;
                    break;
            }
        }
        while (gfx_mode == NO_GFX);
    }
    else
    {
        printf("%s\n\n", mode_desc[gfx_mode-1]);
    }

    /* initialisation du mode graphique */
    switch (gfx_mode)
    {
        case GFX_MODE40:
            if (agfxdrv_Init(GFX_MODE40, 8, GFX_VGA, FALSE))
                goto driver_found;
            break;

        case GFX_MODE80:
            for (i=0; i<3; i++)
                if (agfxdrv_Init(GFX_MODE80, 8, GFX_AUTODETECT, FALSE))
                    goto driver_found;
            break;
                
        case GFX_TRUECOLOR:
            for (i=0; i<3; i++)
                if (agfxdrv_Init(GFX_TRUECOLOR, 15, GFX_AUTODETECT, FALSE) || 
                           agfxdrv_Init(GFX_TRUECOLOR, 16, GFX_AUTODETECT, FALSE))
                    goto driver_found;
           break;
    }

    main_ErrorMessage(is_fr?"\nErreur: mode graphique non support�.":"\nError: unsupported graphic mode.");

  driver_found:

    disk_FirstLoad ();  /* chargement des disquettes �ventuelles */
    cass_FirstLoad ();  /* chargement de la cassette �ventuelle */
    if (memo_FirstLoad () < 0) /* Chargement de la cartouche �ventuelle */
        reset = 1;

    /* chargement des options non d�finies */
    for (str_list=remain_name; str_list!=NULL; str_list=str_list->next)
        if (option_Undefined (str_list->str) == 1)
            reset = 1;
    std_StringListFree (remain_name);

    /* Restitue l'�tat sauvegard� de l'�mulateur */
    teo_FullReset();
    if (reset == 0)
        if (image_Load ("autosave.img") != 0)
            teo_FullReset();

    /* initialisation de l'interface utilisateur */
    agui_Init(version_name, gfx_mode, direct_support);
    
    /* et c'est parti !!! */
    RunTO8();

    /* Sauvegarde de l'�tat de l'�mulateur */
    ini_Save();
    image_Save ("autosave.img");

    /* lib�re la m�moire de la GUI */
    agui_Free();

    /* sortie du mode graphique */
    SetGraphicMode(SHUTDOWN);

    /* mise au repos de l'interface d'acc�s direct */
    dfloppy_Exit();

    /* sortie de l'�mulateur */
    printf(is_fr?"A bient�t !\n":"Goodbye !\n");

    /* sortie de l'�mulateur */
    exit(EXIT_SUCCESS);
}

