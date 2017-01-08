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
 *  Module     : linux/ugui/umemo.c
 *  Version    : 1.8.4
 *  Cr�� par   : Eric Botcazou juillet 1999
 *  Modifi� par: Eric Botcazou 19/11/2006
 *               Gilles F�tis 27/07/2011
 *               Fran�ois Mouret 07/08/2011 24/03/2012 12/06/2012
 *                               04/11/2012 31/05/2015
 *
 *  Gestion des cartouches.
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <unistd.h>
   #include <string.h>
   #include <gtk/gtk.h>
#endif

#include "media/memo.h"
#include "linux/gui.h"
#include "teo.h"
#include "std.h"
#include "errors.h"

static GtkWidget *combo;
static gulong combo_id;
static GtkWidget *emptying_button;
static gulong emptying_button_id;
static int entry_max=0;
static GList *name_list = NULL;
static GList *path_list = NULL;



static void block_all (void)
{
    g_signal_handler_block (combo, combo_id);
    g_signal_handler_block (emptying_button, emptying_button_id);
}



static void unblock_all (void)
{
    g_signal_handler_unblock (combo, combo_id);
    g_signal_handler_unblock (emptying_button, emptying_button_id);
}



/* update_params:
 *  Ajuste les param�tres de cartouche.
 */
static void update_params (void)
{
    int combo_index;
    
    if (combo_id != 0)
    {
        block_all ();

        combo_index = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

        if (combo_index == 0)
            gtk_widget_set_sensitive (emptying_button, FALSE);
        else
            gtk_widget_set_sensitive (emptying_button, TRUE);

        unblock_all ();
    }
}



/* add_combo_entry:
 *  Ajoute une entr�e dans le combobox si inexistante.
 *  et s�lectionne l'entr�e demand�e
 */
static void add_combo_entry (const char *name, const char *path)
{
    GList *path_node = g_list_find_custom (path_list, (gconstpointer)path, (GCompareFunc)g_strcmp0);
    GList *name_node = g_list_find_custom (name_list, (gconstpointer)name, (GCompareFunc)g_strcmp0);
    gint path_index = g_list_position (path_list, path_node);
    gint name_index = g_list_position (name_list, name_node);

    if ((path_node != NULL) && (name_index == path_index))
    {
        gtk_combo_box_set_active (GTK_COMBO_BOX(combo), name_index);
    }
    else
    {
        name_list = g_list_append (name_list, (gpointer)(g_strdup_printf (name,"%s")));
        path_list = g_list_append (path_list, (gpointer)(g_strdup_printf (path,"%s")));
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(combo), NULL, name);
        gtk_combo_box_set_active (GTK_COMBO_BOX(combo), entry_max);
        entry_max++;
    }
}



/* init_combo:
 *  Remplit un combo vide.
 */
static void init_combo (void)
{
    add_combo_entry (is_fr?"(Aucun)":"(None)", "");
}



/* reset_combo:
 *  Ejecte la m�mo7 et vide le combobox (sauf l'entr�e "aucune cartouche") (callback).
 */
static void reset_combo (GtkButton *button, gpointer data)
{
    block_all ();

    umemo_Free ();
    gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT(combo));
    init_combo();
    memo_Eject();

    unblock_all ();

    update_params ();

    (void)button;
    (void)data;
}



/* combo_changed:
 *  Changement de s�lection du combobox (callback).
 */
static void combo_changed (GtkComboBox *combo_box, gpointer data)
{
    char *filename;
    
    filename = (char *) g_list_nth_data (path_list, gtk_combo_box_get_active (combo_box));

    if (*filename == '\0')
        memo_Eject();
    else
    if (memo_Load(filename) < 0)
        ugui_Error (teo_error_msg, wControl);

    update_params ();

    teo.command=TEO_COMMAND_COLD_RESET;

    (void)data;
}



/* open_file:
 *  Charge une nouvelle cartouche.
 */
static void open_file (GtkButton *button, gpointer data)
{
    static int first=1;
    GtkFileFilter *filter;
    static GtkWidget *dialog;
    char *folder_name;
    char *file_name;

    if (first) {
        dialog = gtk_file_chooser_dialog_new (
                 is_fr?"Sélectionner une cartouche":"Select a cartridge",
                 (GtkWindow *) wControl, GTK_FILE_CHOOSER_ACTION_OPEN,
                 is_fr?"_Annuler":"_Cancel", GTK_RESPONSE_CANCEL,
                 is_fr?"_Ouvrir":"_Open", GTK_RESPONSE_ACCEPT, NULL);
        filter = gtk_file_filter_new ();
        gtk_file_filter_set_name (filter, is_fr?"Fichiers cartouche (.m7)":"Cartridge files (.m7)");
        gtk_file_filter_add_pattern (filter, "*.m7");
        gtk_file_filter_add_pattern (filter, "*.M7");
        gtk_file_chooser_add_filter ((GtkFileChooser *)dialog, filter);

        /* Attend que le dialog ait tout assimil� */
        while (gtk_events_pending ())
            gtk_main_iteration ();

        first=0;
    }

    if (teo.memo.file != NULL)
    {
        folder_name = std_strdup_printf ("%s", teo.memo.file);
        std_CleanPath (folder_name);
        (void)gtk_file_chooser_set_current_folder((GtkFileChooser *)dialog, folder_name);
        folder_name = std_free (folder_name);
    }
    else
    if (teo.default_folder != NULL)
        (void)gtk_file_chooser_set_current_folder((GtkFileChooser *)dialog, teo.default_folder);
    else
    if (access("./disks/", F_OK) == 0)
        (void)gtk_file_chooser_set_current_folder((GtkFileChooser *)dialog, "./memo/");

    if (gtk_dialog_run ((GtkDialog *)dialog) == GTK_RESPONSE_ACCEPT)
    {
        file_name = gtk_file_chooser_get_filename((GtkFileChooser *)dialog);
        if (memo_Load(file_name) < 0)
            ugui_Error (teo_error_msg, wControl);
        else
        {
            block_all ();

            add_combo_entry (teo.memo.label, teo.memo.file);
            teo.command = TEO_COMMAND_COLD_RESET;
            folder_name = gtk_file_chooser_get_current_folder ((GtkFileChooser *)dialog);
            teo.default_folder = std_free(teo.default_folder);
            if (folder_name != NULL)
                teo.default_folder = std_strdup_printf ("%s",folder_name);
            g_free (folder_name);

            unblock_all ();

            update_params ();
        }
        g_free (file_name);
    }
    gtk_widget_hide(dialog);
    (void)button;
    (void)data;
}


/* ------------------------------------------------------------------------- */


/* umemo_Init:
 *  Initialise la frame du notebook pour la cartouche.
 */
void umemo_Init (GtkWidget *notebook)
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *widget;
    GtkWidget *image;
    GtkWidget *frame;

    /* frame de la cartouche */
    frame=gtk_frame_new("");
    gtk_frame_set_shadow_type( GTK_FRAME(frame), GTK_SHADOW_NONE);
    gtk_frame_set_label_align( GTK_FRAME(frame), 0.985, 0.0);
    widget=gtk_label_new((is_fr?"Cartouche":"Cartridge"));
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), frame, widget);

    /* bo�te verticale associ�e � la frame */
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width( GTK_CONTAINER(vbox), 5);
    gtk_container_add( GTK_CONTAINER(frame), vbox);

    /* bo�te horizontale */
    hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start( GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    /* bouton de vidange */
    emptying_button = gtk_button_new ();
    image = gtk_image_new_from_icon_name ("edit-clear", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(emptying_button), image);
    gtk_widget_set_tooltip_text (emptying_button, is_fr?"Vide la liste des fichiers":"Empty the file list");
    gtk_box_pack_start( GTK_BOX(hbox), emptying_button, FALSE, FALSE, 0);
    emptying_button_id = g_signal_connect(G_OBJECT(emptying_button), "clicked", G_CALLBACK(reset_combo), (gpointer) NULL);

    /* combobox pour le rappel de cartouche */
    combo=gtk_combo_box_text_new();
    gtk_box_pack_start( GTK_BOX(hbox), combo, TRUE, TRUE,0);
    init_combo();
    if ((teo.memo.label != NULL) && (teo.memo.file != NULL))
        add_combo_entry (teo.memo.label, teo.memo.file);
    combo_id = g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(combo_changed), (gpointer) NULL);

    /* bouton d'ouverture de fichier */
    widget = gtk_button_new ();
    image = gtk_image_new_from_icon_name ("document-open", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(widget), image);
    gtk_widget_set_tooltip_text (widget, is_fr?"Ouvrir un fichier cartouche":"Open a cartridge file");
    gtk_box_pack_start( GTK_BOX(hbox), widget, FALSE, FALSE, 0);
    (void)g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(open_file), (gpointer) NULL);
    
    update_params ();
}



/* umemo_Free
 *  Lib�re la m�moire utilis�e par la liste des cartouches.
 */
void umemo_Free (void)
{
    g_list_foreach (name_list, (GFunc)g_free, (gpointer) NULL);
    g_list_foreach (path_list, (GFunc)g_free, (gpointer) NULL);
    g_list_free (name_list);
    g_list_free (path_list);
    name_list=NULL;
    path_list=NULL;
    entry_max = 0;
}

