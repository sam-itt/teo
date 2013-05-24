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
 *  Module     : win/gui.h
 *  Version    : 1.8.3
 *  Cr�� par   : Eric Botcazou 28/11/2000
 *  Modifi� par: Eric Botcazou 04/12/2000
 *               Fran�ois Mouret 01/04/2012
 *
 *  Interface utilisateur Windows native.
 */


#ifndef WIN_GUI_H
#define WIN_GUI_H

#ifndef SCAN_DEPEND
   #include <windows.h>
#endif

#define PROGNAME_STR  "Teo"
#define BUFFER_SIZE  512
#define NOT_FOUND   -1

extern HINSTANCE prog_inst;
extern HWND prog_win;
extern HICON prog_icon;

extern void SelectGraphicMode(int *, int *);
extern void wgui_Panel(void);
extern void wgui_Free (void);

extern int  CALLBACK wabout_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void wabout_Free (void);

extern int  CALLBACK wsetting_TabProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void wsetting_Free (void);

extern int  CALLBACK wcass_TabProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void wcass_Free (void);

extern int  CALLBACK wmemo_TabProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void wmemo_Free (void);

extern int  CALLBACK wdisk_TabProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void wdisk_Free (void);

extern int  CALLBACK wprinter_TabProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void wprinter_Free (void);

extern void wgui_CreateTooltip (HWND hWnd, WORD id, char *text);

#endif
