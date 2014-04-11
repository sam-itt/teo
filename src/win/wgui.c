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
 *  Module     : win/gui.c
 *  Version    : 1.8.3
 *  Cr�� par   : Eric Botcazou 28/11/2000
 *  Modifi� par: Eric Botcazou 28/10/2003
 *               Fran�ois Mouret 17/09/2006 28/08/2011 18/03/2012
 *                               21/09/2012 18/09/2013 11/04/2014
 *
 *  Interface utilisateur Windows native.
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <windows.h>
   #include <shellapi.h>
   #include <commctrl.h>
#endif

#include "alleg/gfxdrv.h"
#include "win/dialog.rh"
#include "win/gui.h"
#include "teo.h"

/* ressources globales de l'application */
#define NBTABS_MASTER 5
HINSTANCE prog_inst;
HWND prog_win;
HICON prog_icon;
HWND hTab[NBTABS_MASTER];

static int nCurrentTab = 0;



/* ShowTab:
 * Masque l'onglet actif et affiche l'onglet demand�
 */
static void
ShowTab(HWND hDlg)
{
    ShowWindow(hTab[nCurrentTab], SW_HIDE);
    nCurrentTab = SendMessage(GetDlgItem(hDlg, CONTROL_TAB), TCM_GETCURSEL, 0, 0);
    ShowWindow(hTab[nCurrentTab], SW_SHOW);
}



/* CreateTab:
 * Cr�e un onglet
 */
static HWND CreateTab(HWND hDlg, WORD number, char *title, WORD id,
                      int (CALLBACK *prog)(HWND, UINT, WPARAM, LPARAM))
{
    RECT rect0;
    RECT rect1;
    HWND hTabItem;
    HWND hMyTab;
    TCITEM tcitem;

    tcitem.mask = TCIF_TEXT;
    hTabItem = GetDlgItem(hDlg, CONTROL_TAB);

    /* cr�ation du dialogue enfant */
    hMyTab = CreateDialog(prog_inst, MAKEINTRESOURCE(id), hDlg, prog);

    /* ajout de l'onglet */
    tcitem.pszText = (LPTSTR)title;
    SendMessage(hTabItem, TCM_INSERTITEM, number, (LPARAM)&tcitem);

    /* d�finit le rectangle en rapport � la bo�te de dialogue parente */
    GetWindowRect(hTabItem, &rect0);
    SendMessage(hTabItem, TCM_ADJUSTRECT, FALSE, (LPARAM)&rect0);
    GetWindowRect(hMyTab, &rect1);
    SetWindowPos(hMyTab, NULL, rect0.left-rect1.left,
                               rect0.top-rect1.top,
                               rect0.right-rect0.left,
                               rect0.bottom-rect0.top,
                               SWP_NOZORDER|SWP_NOREDRAW);
    /* masque l'onglet */
    ShowWindow(hMyTab, SW_HIDE);

    return hMyTab;
}



/* ControlDialogProc:
 *  Proc�dure du panneau de contr�le.
 */
static BOOL CALLBACK ControlDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   LPNMHDR lpnmhdr;
   int i;
   int response;

   switch(uMsg)
   {
      case WM_INITDIALOG:
#ifdef FRENCH_LANGUAGE
         SetWindowText(hDlg, "Panneau de contr�le");
         SetWindowText(GetDlgItem(hDlg, RESET_BUTTON), "Reset � chaud");
         SetWindowText(GetDlgItem(hDlg, COLDRESET_BUTTON), "Reset � froid");
         SetWindowText(GetDlgItem(hDlg, FULLRESET_BUTTON), "Reset total");
         SetWindowText(GetDlgItem(hDlg, ABOUT_BUTTON), "A propos");
         SetWindowText(GetDlgItem(hDlg, QUIT_BUTTON), "Quitter");
#else
         SetWindowText(hDlg, "Control panel");
         SetWindowText(GetDlgItem(hDlg, RESET_BUTTON), "Warm reset");
         SetWindowText(GetDlgItem(hDlg, COLDRESET_BUTTON), "Cold reset");
         SetWindowText(GetDlgItem(hDlg, FULLRESET_BUTTON), "Full reset");
         SetWindowText(GetDlgItem(hDlg, ABOUT_BUTTON), "About");
         SetWindowText(GetDlgItem(hDlg, QUIT_BUTTON), "Quit");
#endif
         /* Cr�e les onglets */
         hTab[0] = CreateTab(hDlg, 0, is_fr?"R�glage":"Setting", SETTING_TAB, wsetting_TabProc);
         hTab[1] = CreateTab(hDlg, 1, is_fr?"Disquette":"Disk", DISK_TAB, wdisk_TabProc);
         hTab[2] = CreateTab(hDlg, 2, is_fr?"Cassette":"Tape", K7_TAB, wcass_TabProc);
         hTab[3] = CreateTab(hDlg, 3, is_fr?"Cartouche":"Cartridge", MEMO7_TAB, wmemo_TabProc);
         hTab[4] = CreateTab(hDlg, 4, is_fr?"Imprimante":"Printer", PRINTER_TAB, wprinter_TabProc);
         SendMessage(GetDlgItem(hDlg, CONTROL_TAB), TCM_SETCURSEL, nCurrentTab, 0);
         ShowTab(hDlg);

         /* mise en place de l'ic�ne */
         SetClassLong(hDlg, GCL_HICON,   (LONG) prog_icon);
         SetClassLong(hDlg, GCL_HICONSM, (LONG) prog_icon);

         /* cr�e les tooltips */
         wgui_CreateTooltip (hDlg, RESET_BUTTON,
								 is_fr?"Red�marre � chaud sans " \
                                       "effacer la m�moire RAM"
                                      :"Warm reset without to\n"
                                       "clear the RAM memory");
         wgui_CreateTooltip (hDlg, COLDRESET_BUTTON,
                                 is_fr?"Red�marre � froid sans " \
                                       "effacer la m�moire RAM"
                                      :"Cold reset without to\n" \
                                       "clear the RAM memory");
         wgui_CreateTooltip (hDlg, FULLRESET_BUTTON,
                                 is_fr?"Red�marre � froid et " \
                                       "efface la m�moire RAM"
                                      :"Cold reset and\n" \
                                       "clear the RAM memory");
         return TRUE;

      case WM_DESTROY :
         for(i=0;i<NBTABS_MASTER;i++)
            if (hTab[i] != NULL)
                DestroyWindow(hTab[i]);
         return TRUE;

      case WM_COMMAND:
         switch(LOWORD(wParam))
         {
            case IDOK:
               EndDialog(hDlg, IDOK);
               break;

            case QUIT_BUTTON:
               EndDialog(hDlg, IDCANCEL);
               break; 

            case ABOUT_BUTTON:
               (void)DialogBox (prog_inst, MAKEINTRESOURCE(ABOUT_DIALOG),
                                hDlg, (DLGPROC)wabout_Proc);
               break;

            case RESET_BUTTON:
               teo.command = TEO_COMMAND_RESET;
               EndDialog(hDlg, IDOK);
               break;

            case COLDRESET_BUTTON:
               teo.command = TEO_COMMAND_COLD_RESET;
               EndDialog(hDlg, IDOK);
               break;

            case FULLRESET_BUTTON:
               response = MessageBox(NULL, is_fr?"Toute la m�moire RAM sera effac�e."
                                                :"All the RAM memory will be cleared.",
                                           is_fr?"Teo - Question":"Teo - Question",
                                           MB_OKCANCEL | MB_ICONEXCLAMATION);
		       if (response == IDOK)
		       {
                    teo.command = TEO_COMMAND_FULL_RESET;
                    EndDialog(hDlg, IDOK);
               }
               break;

            case WM_DESTROY:
               EndDialog(hDlg, IDOK);
               break;
         }
         return TRUE;

      case WM_NOTIFY :
         /* Change d'onglet */
         lpnmhdr = (LPNMHDR)lParam;
         if(lpnmhdr->code == TCN_SELCHANGE)
             ShowTab(hDlg);
         return FALSE;

      default:
         return FALSE;
   }
}


/* ------------------------------------------------------------------------- */


/* wgui_CreateTooltip:
 *  Cr�e une info-bulle.
 */
void wgui_CreateTooltip (HWND hWnd, WORD id, char *text)
{
    RECT rect;
    TOOLINFO ti;
    HWND hwndTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
                            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            hWnd, NULL, prog_inst,
                            NULL);

    GetClientRect (hWnd, &rect);

    memset (&ti, 0x00, sizeof(TOOLINFO));
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = GetDlgItem(hWnd, id);
    ti.hinst = prog_inst;
    ti.lpszText = text;
    ti.rect.left = rect.left;
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
    SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}



/* wgui_Free:
 *  Lib�re la m�moire utilis�e par l'interface
 */
void wgui_Free (void)
{
    wmemo_Free ();
    wcass_Free ();
    wdisk_Free ();
}



/* wgui_Panel:
 *  Affiche le panneau de contr�le natif Windows.
 */
void wgui_Panel(void)
{
   static int first = 1;
   int ret;

   if (first)
   {
      /* initialise la librairie comctl32.dll */
      InitCommonControls();
      first = 0;
   }

   ret = DialogBox(prog_inst, "CONTROL_DIALOG", prog_win, ControlDialogProc);

   if (ret == IDCANCEL)
   {
      if (teo.command == TEO_COMMAND_COLD_RESET)
          teo_ColdReset();
      teo.command = TEO_COMMAND_QUIT;
   }
}
