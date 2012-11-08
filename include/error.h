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
 *  Copyright (C) 1997-2012 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
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
 *  Module     : intern/errors.h
 *  Version    : 1.8.2
 *  Cr�� par   : Eric Botcazou 22/11/2000
 *  Modifi� par: Fran�ois Mouret 01/11/2012
 *
 *  Gestion des erreurs g�n�r�es par l'�mulateur.
 */


#ifndef ERRORS_H
#define ERRORS_H

enum {
    TEO_ERROR_MULTIPLE_INIT = -1000,
    TEO_ERROR_BAD_ALLOC,
    TEO_ERROR_CANNOT_FIND_FILE,
    TEO_ERROR_CANNOT_OPEN_FILE,
    TEO_ERROR_BAD_FILE_FORMAT,
    TEO_ERROR_UNSUPPORTED_MODEL,
    TEO_ERROR_BAD_JOYSTICK_NUM,
    TEO_ERROR_BAD_MEMO_HEADER_CHECKSUM,
    TEO_ERROR_BAD_MEMO_HEADER_NAME,
    TEO_ERROR_MEDIA_ALREADY_SET,
    TEO_ERROR_CANNOT_CREATE_DISK,
    TEO_ERROR_SAP_NOT_VALID,
    TEO_ERROR_FILE_TOO_LARGE,
    TEO_ERROR_FILE_IS_EMPTY,
    TEO_ERROR_SAP_DIRECTORY_FULL,
    TEO_ERROR
};

extern char *teo_error_msg;
extern int error_Message(int error, const char moreinfo[]);

#endif
