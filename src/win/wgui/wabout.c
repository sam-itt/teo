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
 *  Copyright (C) 1997-2017 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
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
 *  Module     : win/wgui/wabout.c
 *  Version    : 1.8.4
 *  Cr�� par   : Eric Botcazou 28/11/2000
 *  Modifi� par: Eric Botcazou 28/10/2003
 *               Fran�ois Mouret 17/09/2006 28/08/2011 18/03/2012
 *                               03/11/2012 10/05/2014
 *
 *  Fen�tre "A Propos".
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
#endif

#include "teo.h"
#include "win/gui.h"

/* ------------------------------------------------------------------------- */


/* AboutProc:
 *  Proc�dure pour la bo�te "A propos"
 */
int CALLBACK wabout_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND aboutLink;
   static HWND aboutForum;
   static HWND aboutTitle;
   static HWND aboutCopyright;
   static HWND aboutLicense;
   static HFONT hLinkStyle;
   static HFONT hTitleStyle;
   static HFONT hCopyrightStyle;
   static HFONT hLicenseStyle;

   switch(uMsg)
   {
      case WM_INITDIALOG:
         aboutLink  = GetDlgItem (hDlg, IDC_ABOUT_STATIC_LINK);
         aboutForum = GetDlgItem (hDlg, IDC_ABOUT_STATIC_FORUM);
         aboutTitle = GetDlgItem (hDlg, IDC_ABOUT_CTEXT_TITLE);
         aboutCopyright = GetDlgItem (hDlg, IDC_ABOUT_CTEXT_COPYRIGHT);
         aboutLicense = GetDlgItem (hDlg, IDC_ABOUT_CTEXT_LICENSE);
         hTitleStyle = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE,
                           FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
         hLinkStyle = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE,
                           TRUE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
         hCopyrightStyle = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE,
                           FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
         hLicenseStyle = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE,
                           FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
         SendMessage(aboutLink, WM_SETFONT, (WPARAM)hLinkStyle, TRUE);
         SendMessage(aboutForum, WM_SETFONT, (WPARAM)hLinkStyle, TRUE);
         SendMessage(aboutTitle, WM_SETFONT, (WPARAM)hTitleStyle, TRUE);
         SendMessage(aboutLicense, WM_SETFONT, (WPARAM)hLicenseStyle, TRUE);
         SendMessage(aboutCopyright, WM_SETFONT, (WPARAM)hCopyrightStyle, TRUE);
#ifdef FRENCH_LANGUAGE
         SetWindowText(hDlg, "Teo - A propos");
         SetWindowText(aboutLink, "Teo sur SourceForge");
         SetWindowText(aboutForum, "Forum sur le Web");
#else
         SetWindowText(hDlg, "Teo - About");
         SetWindowText(aboutLink, "Teo on SourceForge");
         SetWindowText(aboutForum, "Web forum");
#endif
         SetWindowText(aboutCopyright, "Copyright � 1997-"TEO_YEAR_STRING"\n" \
                           " Gilles F�tis, Eric Botcazou, Alexandre Pukall, " \
                           "J�r�mie Guillaume, Fran�ois Mouret, " \
                           "Samuel Devulder" );
         SetClassLongPtr(aboutLink, GCLP_HCURSOR,
                         (ULONG_PTR)LoadCursor(NULL, IDC_HAND));
         return TRUE;

      case WM_CTLCOLORSTATIC :
          if ((HWND)lParam == aboutCopyright)
          {
              SetTextColor((HDC)wParam, RGB(0, 0, 0));
              SetBkMode((HDC)wParam, TRANSPARENT);
              return (BOOL)GetStockObject(HOLLOW_BRUSH);
          }
          if ((HWND)lParam == aboutTitle)
          {
              SetTextColor((HDC)wParam, RGB(0, 0, 0));
              SetBkMode((HDC)wParam, TRANSPARENT);
              return (BOOL)GetStockObject(HOLLOW_BRUSH);
          }
          if ((HWND)lParam == aboutLink)
          {
              SetTextColor((HDC)wParam, RGB(0, 0, 255));
              SetBkMode((HDC)wParam, TRANSPARENT);
              return (BOOL)GetStockObject(HOLLOW_BRUSH);
          }
          if ((HWND)lParam == aboutForum)
          {
              SetTextColor((HDC)wParam, RGB(0, 0, 255));
              SetBkMode((HDC)wParam, TRANSPARENT);
              return (BOOL)GetStockObject(HOLLOW_BRUSH);
          }
          if ((HWND)lParam == aboutLicense)
          {
              SetTextColor((HDC)wParam, RGB(0, 0, 0));
              SetBkMode((HDC)wParam, TRANSPARENT);
              return (BOOL)GetStockObject(HOLLOW_BRUSH);
          }
          return TRUE;

      case WM_CTLCOLOREDIT :
          return TRUE;

      case WM_DESTROY :
          (void)DeleteObject((HGDIOBJ)hLinkStyle);
          (void)DeleteObject((HGDIOBJ)hTitleStyle);
          (void)DeleteObject((HGDIOBJ)hCopyrightStyle);
          (void)DeleteObject((HGDIOBJ)hLicenseStyle);
          EndDialog(hDlg, IDOK);
          return TRUE;

      case WM_COMMAND:
         switch(LOWORD(wParam))
         {
            case IDOK:
               (void)DeleteObject((HGDIOBJ)hLinkStyle);
               (void)DeleteObject((HGDIOBJ)hTitleStyle);
               (void)DeleteObject((HGDIOBJ)hCopyrightStyle);
               (void)DeleteObject((HGDIOBJ)hLicenseStyle);
               EndDialog(hDlg, IDOK);
               break;

            case IDC_ABOUT_STATIC_LINK :
               ShellExecute(NULL, "open",
                            "http://sourceforge.net/projects/teoemulator/",
                            0, 0, SW_SHOWNORMAL);

            case IDC_ABOUT_STATIC_FORUM :
               ShellExecute(NULL, "open",
                            "http://www.logicielsmoto.com/",
                            0, 0, SW_SHOWNORMAL);
               break;

         }
         return TRUE;

      default:
         return FALSE;
   }
}
