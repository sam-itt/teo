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
 *  Module     : linux/question.c
 *  Version    : 1.8.1
 *  Cr�� par   : Fran�ois Mouret 30/07/2011
 *  Modifi� par:
 *
 *  Biblioth�que de fonctions pour compatibilit� GTK.
 */


#ifndef SCAN_DEPEND
   #include <gtk/gtk.h>
   #include <gdk/gdkx.h>
#endif

#include "linux/xgui.h"


/* xgtk_signal_connect:
 *  Assure la compatibilit� pour la connexion du signal
 */
void xgtk_signal_connect (void *inst, const char *signame, void *func, gpointer data)
{
#if BEFORE_GTK_2_MIN
    gtk_signal_connect( GTK_OBJECT(inst), signame, GTK_SIGNAL_FUNC(func), data);
#else
    g_signal_connect (G_OBJECT (inst), signame, G_CALLBACK (func), data);
#endif
}



GSList *xgtk_radio_button_group (GtkWidget *widget)
{
#if BEFORE_GTK_2_MIN
    return gtk_radio_button_group( GTK_RADIO_BUTTON(widget));
#else
    return gtk_radio_button_get_group (GTK_RADIO_BUTTON(widget));
#endif
}

