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
 *  Module     : intern/disk.h
 *  Version    : 1.8.2
 *  Cr�� par   : Alexandre Pukall mai 1998
 *  Modifi� par: Eric Botcazou 24/10/2003
 *               Fran�ois Mouret 05/10/2012
 *  Gestion du format SAP 2.0: lecture et �criture disquette.
 */


#ifndef DISK_H
#define DISK_H

#include "defs.h"


extern struct DISK_CTRL disk_ctrl;

extern void disk_ctrl_cmd0(int);

#define DISK_CTRL_SET_FUNC(name)    \
    void disk_ctrl_##name(int val)  \
    {                               \
        disk_ctrl.name = val;       \
    }

extern inline DISK_CTRL_SET_FUNC(cmd1)
extern inline DISK_CTRL_SET_FUNC(cmd2)
extern inline DISK_CTRL_SET_FUNC(wdata)
extern inline DISK_CTRL_SET_FUNC(wclk)
extern inline DISK_CTRL_SET_FUNC(wsect)
extern inline DISK_CTRL_SET_FUNC(wtrck)
extern inline DISK_CTRL_SET_FUNC(wcell)

extern int disk_ctrl_stat0(void);
extern int disk_ctrl_stat1(void);

extern inline int disk_ctrl_rdata(void)
{
    return disk_ctrl.rdata;
}

extern void ResetDiskCtrl(int *);
extern void ReadSector(int *);
extern void WriteSector(int *);
extern void FormatDrive(int *);
extern void DiskNop(int *);

extern void  disk_Init(void);
extern int   disk_SetDirect(int drive);
extern int   disk_SetVirtual(int drive);
extern void  disk_UnloadDir (int drive);
extern void  disk_Eject(int drive);
extern void  disk_UnloadAll (void);
extern int   disk_Check (const char filename[]);
extern int   disk_Load(int drive, const char filename[]);
extern void  disk_FirstLoad (void);
extern int   disk_SetMode(int drive, int mode);

#endif

