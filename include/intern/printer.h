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
 *  Module     : printer.h
 *  Version    : 1.8.1
 *  Cr�� par   : Eric Botcazou 22/03/2001
 *  Modifi� par: Eric Botcazou 24/03/2001
 *               Fran�ois Mouret 18/04/2012
 *
 *  Emulation de l'imprimante.
 */


#ifndef PRINTER_H
#define PRINTER_H

#include "to8.h"

struct LPRT_CONFIG
{
    int  number;
    int  nlq;
    int  dip;
    int  raw_output;
    int  txt_output;
    int  gfx_output;
    char folder[MAX_PATH];
};

extern struct LPRT_CONFIG *printer_get_config (void);
extern void  printer_set_config (struct LPRT_CONFIG *config);

extern void printer_WriteData(int mask, int value);
extern void printer_SetStrobe(int state);
extern void printer_Close(void);
extern void InitPrinter(void);

#endif

