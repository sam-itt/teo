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
 *                          J�r�mie Guillaume
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
 *  Module     : to8dbg.h
 *  Version    : 1.8.2
 *  Cr�� par   : Eric Botcazou octobre 1999
 *  Modifi� par: Eric Botcazou 23/11/2000
 *
 *  D�clarations d'objets priv�s n�cessaires au d�bogage.
 */


#ifndef TO8DBG_H
#define TO8DBG_H

#include "mc68xx/mc6821.h"
#include "mc68xx/mc6846.h"
#include "defs.h"

#define  LOAD_BYTE(addr)       (int) mempager.segment[((addr)>>12)&0xF][(addr)&0xFFF]
#define STORE_BYTE(addr, val)  mempager.segment[((addr)>>12)&0xF][(addr)&0xFFF]=(uint8) (val)

extern struct MEMORY mem;
extern struct MEMORY_PAGER mempager;
extern struct MC6846_PIA mc6846;
extern struct MC6821_PIA pia_int,pia_ext;
extern struct GATE_ARRAY  mode_page;
extern struct DISK_CTRL disk_ctrl;

#endif
