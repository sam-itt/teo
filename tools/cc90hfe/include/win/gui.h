/*********************************************************
 * cc90hfe (c) Teo Developers
 *********************************************************
 *
 *  Copyright (C) 2012-2014 Yves Charriau, Fran�ois Mouret
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
 *  Version    : 0.7.0
 *  Cr�� par   : Fran�ois Mouret 27/02/2013
 *  Modifi� par:
 *
 *  Windows GUI.
 */

#ifndef WIN_GUI_H
#define WIN_GUI_H 1

extern HWND main_window;
extern HWND main_dialog;
extern HINSTANCE hInst;

extern  void  archive_Prog (void);
extern  void  extract_Prog (void);
extern  void  install_Prog (HINSTANCE hInst, HWND hDlg);
extern  void  about_Prog (HINSTANCE hInst, HWND hDlg);

extern  void  gui_SetProgressText (char *text);
extern  void  gui_SetProgressBar (int value);
extern  void  gui_EnableButtons (int flag);
extern  void  gui_ResetProgress (void);
extern  void  gui_ProgressUpdate (int percent);
extern  void  gui_ErrorDialog (char *message);
extern  int   gui_InformationDialog (char *message);

extern  int   gui_OpenFile (int flags, char *title);
extern  void  gui_EmitStop (void);

#endif

