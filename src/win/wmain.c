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
 *  Copyright (C) 1997-2013 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
 *                          J�r�mie Guillaume, Samuel Devulder
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
 *  Module     : win/main.c
 *  Version    : 1.8.2
 *  Cr�� par   : Eric Botcazou septembre 2000
 *  Modifi� par: Eric Botcazou 24/10/2003
 *               Samuel Devulder 30/07/2011
 *               Fran�ois Mouret 19/10/2012 24/10/2012
 *
 *  Boucle principale de l'�mulateur.
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <allegro.h>
   #include <winalleg.h>
   #include <sys/stat.h>
   #include <ctype.h>
#endif

#include "defs.h"
#include "teo.h"
#include "option.h"
#include "std.h"
#include "ini.h"
#include "image.h"
#include "main.h"
#include "errors.h"
#include "media/disk/controlr.h"
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
#include "win/keybint.h"
#include "win/gui.h"


struct EMUTEO teo;

static int reset = FALSE;
static int gfx_mode = GFX_WINDOW;
static int windowed_mode = TRUE;
struct STRING_LIST *remain_name = NULL;

int frame;                  /* compteur de frame vid�o */
static volatile int tick;   /* compteur du timer       */

static void Timer(void)
{
    tick++;
}



/* RetraceCallback:
 *  Fonction callback de retra�age de l'�cran apr�s
 *  restauration de l'application.
 */
static void RetraceCallback(void)
{
    acquire_screen();
    RetraceScreen(0, 0, SCREEN_W, SCREEN_H);
    release_screen();
}



/* RunTO8:
 *  Boucle principale de l'�mulateur.
 */
static void RunTO8(void)
{
    amouse_Install(TEO_STATUS_MOUSE); /* la souris est le p�riph�rique de pointage par d�faut */
    RetraceScreen(0, 0, SCREEN_W, SCREEN_H);

    do  /* boucle principale de l'�mulateur */
    {
        teo.command=TEO_COMMAND_NONE;

        /* installation des handlers clavier, souris et son */ 
        wkeybint_Install();
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
            dkc->ClearWriteFlag();
            teo_DoFrame(FALSE);

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
                    asound_Play();
                else
                    while (frame==tick)
                   Sleep(0);
            }

            dkc->WriteUpdateTimeout();
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
        wkeybint_ShutDown();

        /* �x�cution des commandes */
        if (teo.command==TEO_COMMAND_PANEL)
        {
            if (windowed_mode)
                wgui_Panel();
            else
                agui_Panel();
        }

        if (teo.command==TEO_COMMAND_SCREENSHOT)
            agfxdrv_Screenshot();

        if (teo.command==TEO_COMMAND_RESET)
            teo_Reset();

        if (teo.command==TEO_COMMAND_COLD_RESET)
        {
            teo_ColdReset();
            amouse_Install(TEO_STATUS_MOUSE);
        }
    }
    while (teo.command != TEO_COMMAND_QUIT);  /* fin de la boucle principale */

    /* Finit de sauver les donn�es disquettes */
    dkc->WriteUpdateTrack();

    /* Finit d'ex�cuter l'instruction et/ou l'interruption courante */
    mc6809_FlushExec();
}



/* read_command_line:
 *  Lit la ligne de commande
 */
static void read_command_line(int argc, char *argv[])
{
    char *message;
    int mode40=0, mode80=0, truecolor=0, windowd=0;

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
        { "window", '\0', OPTION_ARG_BOOL, &windowd,
           is_fr?"Mode fen�tr�":"Windowed display", NULL},
        { NULL, 0, 0, NULL, NULL, NULL }
    };
    message = option_Parse (argc, argv, "teow", entries, &remain_name);
    if (message != NULL)
        main_ExitMessage(message);
        
    if (mode40)    gfx_mode = GFX_MODE40   ; else
    if (mode80)    gfx_mode = GFX_MODE80   ; else
    if (truecolor) gfx_mode = GFX_TRUECOLOR; else
    if (windowd)   gfx_mode = GFX_WINDOW;
}


/* ------------------------------------------------------------------------- */


/* DisplayMessage:
 *  Affiche un message.
 */
void main_DisplayMessage(const char msg[])
{
    if (windowed_mode)
    {
        MessageBox(NULL, (const char*)msg, is_fr?"Teo - Erreur":"Teo - Error",
                    MB_OK | MB_ICONERROR);
    }
    else
    {
        agui_PopupMessage (msg);
    }
}



/* ExitMessage:
 *  Affiche un message de sortie et sort du programme.
 */
void main_ExitMessage(const char msg[])
{
    allegro_exit(); /* pour �viter une fen�tre DirectX zombie */
    main_DisplayMessage(msg);
    exit(EXIT_FAILURE);
}



/* close_procedure:
 *  Proc�dure de fermeture de la fen�tre par le bouton close.
 */
static void close_procedure (void)
{
    teo.command = TEO_COMMAND_QUIT;
}
 


/* WinMain:
 *  Point d'entr�e du programme appel� par l'API Win32.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
    char version_name[]="Teo "TEO_VERSION_STR" (Windows/DirectX)";
    int alleg_depth, argc=0;
#ifndef __MINGW32__
    char *argv[16];
#else
    char **argv;
#endif
    int njoy = 0;
    struct STRING_LIST *str_list;

#ifdef FRENCH_LANGUAGE
    is_fr = 1;
#else
    is_fr = 0;
#endif

    /* conversion de la ligne de commande Windows */
    prog_inst = hInst;
    prog_icon = LoadIcon(hInst, "thomson_ico");

#ifndef __MINGW32__	
    if (*lpCmdLine)
    {
        argv[argc++]=lpCmdLine++;

        while (*lpCmdLine)
            if (*lpCmdLine == ' ')
            {
                *lpCmdLine++ = '\0';
                argv[argc++]=lpCmdLine++;
            }
            else
                lpCmdLine++;
    }
#else
	/* Windows fourni des argc/argv d�j� pars�s qui tient 
	   compte des guillemets et des blancs. */
	argc = __argc;
	argv = (void*)__argv;
#endif

    ini_Load();                   /* Charge les param�tres par d�faut */
    read_command_line (argc, argv); /* R�cup�ration des options */

    /* l'initialisation de l'interface clavier, qui utilise un appel GDI, doit avoir lieu
       avant celle du module clavier d'Allegro, bas� sur DirectInput */
    wkeybint_Init();

    /* initialisation de la librairie Allegro */
    set_uformat(U_ASCII);  /* pour les accents Latin-1 */
    allegro_init();
    set_config_file(ALLEGRO_CONFIG_FILE);
    install_keyboard();
    install_timer();
    if (njoy >= 0)
        install_joystick(JOY_TYPE_AUTODETECT);

    /* d�coration de la fen�tre */
    set_window_title(is_fr?"Teo - l'�mulateur TO8 (menu:ESC)":"Teo - the TO8 emulator (menu:ESC)");
    prog_win = win_get_window();
    SetClassLong(prog_win, GCL_HICON,   (LONG) prog_icon);
    SetClassLong(prog_win, GCL_HICONSM, (LONG) prog_icon);

    /* d�tection de la pr�sence de joystick(s) */
    njoy = MIN(TEO_NJOYSTICKS, num_joysticks);

    /* initialisation de l'�mulateur */
    printf(is_fr?"Initialisation de l'�mulateur...":"Emulator initialization...");
    if (teo_Init(TEO_NJOYSTICKS-njoy) < 0)
        main_ExitMessage(teo_error_msg);
    printf("ok\n");

    /* initialisation du son */
    asound_Init(51200);  /* 51200 Hz car 25600 Hz provoque des irr�gularit�s du timer */

    /* initialisation des joysticks */
    ajoyint_Init(njoy);

    /* initialisation du mode graphique */
    switch(gfx_mode)
    {
        case GFX_MODE40:
            if (!agfxdrv_Init(GFX_MODE40, 8, GFX_AUTODETECT_FULLSCREEN, FALSE))
                main_ExitMessage(is_fr?"Mode graphique non support�."
                                      :"Unsupported graphic mode");
            windowed_mode = FALSE;
            break;

        case GFX_MODE80:
            if (!agfxdrv_Init(GFX_MODE80, 8, GFX_AUTODETECT_FULLSCREEN, FALSE))
                main_ExitMessage(is_fr?"Mode graphique non support�."
                                      :"Unsupported graphic mode");
            windowed_mode = FALSE;
            break;

        case GFX_TRUECOLOR:
            if (!agfxdrv_Init(GFX_TRUECOLOR, 15, GFX_AUTODETECT_FULLSCREEN, FALSE))
                if (!agfxdrv_Init(GFX_TRUECOLOR, 16, GFX_AUTODETECT_FULLSCREEN, FALSE))
                    if (!agfxdrv_Init(GFX_TRUECOLOR, 24, GFX_AUTODETECT_FULLSCREEN, FALSE))
                        if (!agfxdrv_Init(GFX_TRUECOLOR, 32, GFX_AUTODETECT_FULLSCREEN, FALSE))
                            main_ExitMessage(is_fr?"Mode graphique non support�."
                                                  :"Unsupported graphic mode");
            windowed_mode = FALSE;
            break;

        case GFX_WINDOW:
            alleg_depth = desktop_color_depth();

            switch (alleg_depth)
            {
                case 8:  /* 8bpp */
                default:
                    main_ExitMessage(is_fr?"Mode graphique non support�."
                                          :"Unsupported graphic mode");
                    break;

                case 16: /* 15 ou 16bpp */
                    if ( !agfxdrv_Init(GFX_TRUECOLOR, 15, GFX_AUTODETECT_WINDOWED, TRUE) && 
                         !agfxdrv_Init(GFX_TRUECOLOR, 16, GFX_AUTODETECT_WINDOWED, TRUE) )
                            main_ExitMessage(is_fr?"Mode graphique non support�."
                                                  :"Unsupported graphic mode");
                    gfx_mode = GFX_TRUECOLOR;
                    break;
 
                case 24: /* 24bpp */
                case 32: /* 32bpp */
                    if (!agfxdrv_Init(GFX_TRUECOLOR, alleg_depth, GFX_AUTODETECT_WINDOWED, TRUE))
                        main_ExitMessage(is_fr?"Mode graphique non support�."
                                              :"Unsupported graphic mode");
                    gfx_mode = GFX_TRUECOLOR;
                    break;
            }
            windowed_mode = TRUE;
            break;
    }

    /* installation de la fonction callback de retra�age de l'�cran n�cessaire
       pour les modes fullscreen */
    set_display_switch_callback(SWITCH_IN, RetraceCallback);

    /* on continue de tourner m�me sans focus car sinon la gui se bloque,
     * et le buffer son tourne sur lui m�me sans mise � jour et c'est moche. */
    set_display_switch_mode(SWITCH_BACKGROUND); 

    disk_FirstLoad ();  /* Chargement des disquettes �ventuelles */
    cass_FirstLoad ();  /* Chargement de la cassette �ventuelle */
    if (memo_FirstLoad () < 0) /* Chargement de la cartouche �ventuelle */
        reset = 1;

    /* Chargement des options non d�finies */
    for (str_list=remain_name; str_list!=NULL; str_list=str_list->next)
        option_Undefined (str_list->str);
    std_StringListFree (remain_name);

    /* reset �ventuel de l'�mulateur */
    teo_ColdReset();
    if (reset == 0)  
        if (access("autosave.img", F_OK) >= 0)
            image_Load("autosave.img");

    /* initialisation de l'interface utilisateur Allegro */
    if (!windowed_mode)
    {
       agui_Init(version_name, gfx_mode, FALSE);
    }
    else
    {
       set_window_close_hook(close_procedure);
    }

    /* et c'est parti !!! */
    RunTO8();

    /* Sauvegarde de l'�tat de l'�mulateur */
    ini_Save();
    image_Save ("autosave.img");

    /* d�sinstallation du callback *avant* la sortie du mode graphique */
    remove_display_switch_callback(RetraceCallback);

    /* lib�ration de la m�moire si mode fen�tr� */
    if (windowed_mode)
       wgui_Free();
    else
       agui_Free();

    /* sortie du mode graphique */
    SetGraphicMode(SHUTDOWN);

    /* sortie de l'�mulateur */
    printf(is_fr?"A bient�t !\n":"Goodbye !\n");

    /* sortie de l'�mulateur */
    exit(EXIT_SUCCESS);
}
