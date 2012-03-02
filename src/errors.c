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
 *  Copyright (C) 1997-2001 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
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
 *  Module     : errors.c
 *  Version    : 1.8.1
 *  Cr�� par   : Eric Botcazou 22/11/2000
 *  Modifi� par: Eric Botcazou 06/03/2001
 *               Fran�ois Mouret 08/2011 13/01/2012
 *
 *  Gestion des erreurs g�n�r�es par l'�mulateur.
 */


#ifndef SCAN_DEPEND
   #include <stddef.h>
   #include <string.h>
#endif

#include "intern/errors.h"  /* MacOS */
#include "to8err.h"
#include "to8.h"


static const char *default_error_table_fr[TO8_ERROR_MAX]= {
#ifdef OS_LINUX
    "Erreur: initialisation multiple de l'émulateur."
    "Erreur: allocation d'espace impossible.",
    "Erreur: impossible de trouver ",
    "Erreur: ouverture impossible.",  
    "Erreur: mauvais format de fichier.",
    "Erreur: image d'un modèle non supporté.",
    "Erreur: nombre de joysticks incorrect."
#else
    "Erreur: initialisation multiple de l'�mulateur."
    "Erreur: allocation d'espace impossible.",
    "Erreur: impossible de trouver ",
    "Erreur: ouverture impossible.",  
    "Erreur: mauvais format de fichier.",
    "Erreur: image d'un mod�le non support�.",
    "Erreur: nombre de joysticks incorrect."
#endif
};

static const char *default_error_table_en[TO8_ERROR_MAX]= {
    "Error: multiple initialization of the emulator.",
    "Error: cannot allocate space.",
    "Error: unable to find ",
    "Error: unable to open.",  
    "Error: bad file format.",
    "Error: unsupported image format.",
    "Error: bad count of joysticks."
};

static const char **custom_error_table = NULL;



/* ErrorMessage:
 *  Renvoie une erreur g�n�r�e par l'�mulateur.
 */
int ErrorMessage(int error, const char moreinfo[])
{
    if (custom_error_table)
        strcpy(to8_error_msg, custom_error_table[error]);
    else
        strcpy(to8_error_msg, is_fr?default_error_table_fr[error]:default_error_table_en[error]);

    if (moreinfo)
        strcat(to8_error_msg, moreinfo);
 
    return TO8_ERROR;
}



/**********************************/
/* partie publique                */
/**********************************/


char to8_error_msg[TO8_MESSAGE_MAX_LENGTH+1] = "";  /* 127 caract�res au maximum */

/* RegisterErrorTable:
 *  Inscrit une table d'erreur personnalis�e.
 */
void to8_RegisterErrorTable(const char *table[])
{
    custom_error_table = table;
}

