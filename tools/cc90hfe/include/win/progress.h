/*********************************************************
 * cc90hfe (c) Teo Developers
 *********************************************************
 *
 *  Copyright (C) 2012-2013 Yves Charriau, Fran�ois Mouret
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
 *  Module     : win/progress.h
 *  Version    : 0.5.0
 *  Cr�� par   : Fran�ois Mouret 27/02/2013
 *  Modifi� par:
 *
 *  Progression.
 */

#ifndef WIN_PROGRESS_H
#define WIN_PROGRESS_H 1

#define BAR_LENGTH  1000

extern int progress_on;

extern void  progress_Run (int (*process)(void));
extern void  progress_Update (int percent);
extern void  progress_Stop (void);

#endif

