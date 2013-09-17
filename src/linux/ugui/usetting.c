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
 *  Module     : linux/ugui/usetting.c
 *  Version    : 1.8.3
 *  Cr�� par   : Eric Botcazou juillet 1999
 *  Modifi� par: Eric Botcazou 19/11/2006
 *               Gilles F�tis 27/07/2011
 *               Fran�ois Mouret 07/08/2011 24/03/2012 19/10/2012
 *                               15/09/2013
 *
 *  Gestion des pr�f�rences.
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <unistd.h>
   #include <string.h>
   #include <libgen.h>
   #include <gtk/gtk.h>
#endif

#include "linux/gui.h"
#include "teo.h"

static GtkWidget *sound_widget = NULL;


/* toggle_speed:
 *  Positionne la vitesse de l'�mulateur
 */
static void toggle_speed (GtkWidget *button, gpointer data)
{
    teo.setting.exact_speed=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    if (sound_widget != NULL)
        gtk_widget_set_sensitive (sound_widget, teo.setting.exact_speed ? TRUE : FALSE);
    (void)data;
}



/* toggle_interlaced:
 *  Positionne l'�tat du mode entrelac�
 */
static void toggle_interlace (GtkWidget *button, gpointer data)
{
    teo.setting.interlaced_video = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    (void)data;
}


/* toggle_sound:
 *  Positionne l'activation du son
 */
static void toggle_sound (GtkWidget *button, gpointer data)
{
    teo.setting.sound_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    (void)data;
}



static GtkWidget *create_new_frame (GtkWidget *mainbox, const char *title)
{
    GtkWidget *vbox;
    GtkWidget *frame;

    /* frame */
    frame=gtk_frame_new (title);
    gtk_box_pack_start (GTK_BOX(mainbox), frame, TRUE, TRUE, 0);

    /* vertical box */
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER(vbox), 5);
    gtk_container_add( GTK_CONTAINER(frame), vbox);

    return vbox;
}



static GtkWidget *create_new_hbox (GtkWidget *mainbox)
{
    GtkWidget *hbox;

    hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    gtk_container_add( GTK_CONTAINER(mainbox), hbox);

    return hbox;
}


/* ------------------------------------------------------------------------- */


/* usetting_Init:
 *  Initialise la frame du notebook pour les r�glages
 */
void usetting_Init (GtkWidget *notebook)
{
    GtkWidget *hbox, *hbox2;
    GtkWidget *vbox, *vbox2;
    GtkWidget *widget;
    GtkWidget *frame;

    /* frame des commandes et r�glages */
    frame=gtk_frame_new("");
    gtk_frame_set_shadow_type( GTK_FRAME(frame), GTK_SHADOW_NONE);
    gtk_frame_set_label_align( GTK_FRAME(frame), 0.985, 0.0);
    widget=gtk_label_new((is_fr?"Réglages":"Settings"));
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), frame, widget);
    
    /* bo�te verticale associ�e � la frame */
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width( GTK_CONTAINER(vbox), 0);
    gtk_container_add( GTK_CONTAINER(frame), vbox);

    /* bo�te horizontale */
    hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    gtk_container_set_border_width( GTK_CONTAINER(hbox), 5);
    gtk_box_set_homogeneous (GTK_BOX(hbox), TRUE);
    gtk_container_add( GTK_CONTAINER(vbox), hbox);

    /* ---------------- Speed ------------------ */

    /* Cr�ation de la frame */
    vbox2 = create_new_frame (hbox, is_fr?"Vitesse":"Speed");

    /* bouton de vitesse maximale */
    hbox2 = create_new_hbox (vbox2);
    widget=gtk_radio_button_new_with_label(NULL, (is_fr?"rapide":"fast"));
    gtk_box_pack_end( GTK_BOX(hbox2), widget, TRUE, TRUE, 0);

    /* bouton de vitesse exacte */
    hbox2 = create_new_hbox (vbox2);
    widget=gtk_radio_button_new_with_label_from_widget(
                            GTK_RADIO_BUTTON (widget),
                            is_fr?"exacte":"exact");
    gtk_box_pack_end( GTK_BOX(hbox2), widget, TRUE, TRUE, 0);

    /* Buttons connection */
    g_signal_connect(G_OBJECT(widget),
                     "toggled",
                     G_CALLBACK(toggle_speed),
                     (gpointer)NULL);
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(widget),
                                  teo.setting.exact_speed ? TRUE : FALSE);

    /* ---------------- Sound ------------------ */


    /* Cr�ation de la frame */
    vbox2 = create_new_frame (hbox, is_fr?"Son":"Sound");

    /* checkbox du son */
    hbox2 = create_new_hbox (vbox2);
    sound_widget=gtk_check_button_new_with_label((is_fr?"Actif":"Activated"));
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(sound_widget),
                                  teo.setting.sound_enabled);
    g_signal_connect(G_OBJECT(sound_widget),
                     "toggled",
                     G_CALLBACK(toggle_sound),
                     (gpointer)NULL);
    gtk_box_pack_start( GTK_BOX(hbox2), sound_widget, TRUE, TRUE, 0);
    gtk_widget_set_sensitive (sound_widget,
                              teo.setting.exact_speed ? TRUE : FALSE);

    /* ---------------- Display ------------------ */

    /* Cr�ation de la frame */
    vbox2 = create_new_frame (hbox, is_fr?"Affichage":"Display");

    /* checkbox du mode entrelac� */
    hbox2 = create_new_hbox (vbox2);
    widget=gtk_check_button_new_with_label((is_fr?"Vidéo entrelacé"
                                                 :"Interlaced video"));
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(widget),
                                  teo.setting.interlaced_video);
    g_signal_connect(G_OBJECT(widget),
                     "toggled",
                     G_CALLBACK(toggle_interlace),
                     (gpointer)NULL);
    gtk_box_pack_start( GTK_BOX(hbox2), widget, TRUE, TRUE, 0);
}

