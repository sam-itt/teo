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
 *  Version    : 1.8.2
 *  Cr�� par   : Eric Botcazou 22/11/2000
 *  Modifi� par: Eric Botcazou 06/03/2001
 *               Fran�ois Mouret 08/2011 13/01/2012
 *
 *  Gestion des erreurs g�n�r�es par l'�mulateur.
 */


#include "teo.h"

char *to8_error_msg = NULL;

struct ERROR_TABLE {
    int   err;
    char  *str;
};

static const struct ERROR_TABLE error_table_fr[]= {
#ifdef UNIX_TOOL
    { TO8_MULTIPLE_INIT           , "Initialisation multiple de l'émulateur." },
    { TO8_BAD_ALLOC               , "Allocation d'espace impossible."          },
    { TO8_CANNOT_FIND_FILE        , "Impossible de trouver "                   },
    { TO8_CANNOT_OPEN_FILE        , "Ouverture impossible."                    },
    { TO8_BAD_FILE_FORMAT         , "Mauvais format de fichier."               },
    { TO8_UNSUPPORTED_MODEL       , "Image d'un modèle non supporté."        },
    { TO8_BAD_JOYSTICK_NUM        , "Nombre de joysticks incorrect."           },
    { TO8_BAD_MEMO_HEADER_CHECKSUM, "Checksum d'en-tête de memo erroné."     },
    { TO8_BAD_MEMO_HEADER_NAME    , "Nom d'en-tête de memo incorrect."        },
    { TO8_MEDIA_ALREADY_SET       , "Plus de media libre."                     },
    { TO8_CANNOT_CREATE_DISK      , "Impossible de créer la disquette."       },
    { TO8_SAP_NOT_VALID           , "Archive SAP invalide."                    },
    { TO8_FILE_TOO_LARGE          , "Fichier trop important."                  },
    { TO8_FILE_IS_EMPTY           , "Fichier vide."                            },
    { TO8_SAP_DIRECTORY_FULL      , "Répertoire SAP plein"                    },
    { 0                           , "Erreur inconnue."                         }
#else
    { TO8_MULTIPLE_INIT           , "Initialisation multiple de l'�mulateur."  },
    { TO8_BAD_ALLOC               , "Allocation d'espace impossible."          },
    { TO8_CANNOT_FIND_FILE        , "Impossible de trouver "                   },
    { TO8_CANNOT_OPEN_FILE        , "Ouverture impossible."                    },
    { TO8_BAD_FILE_FORMAT         , "Mauvais format de fichier."               },
    { TO8_UNSUPPORTED_MODEL       , "Image d'un mod�le non support�."          },
    { TO8_BAD_JOYSTICK_NUM        , "Nombre de joysticks incorrect."           },
    { TO8_BAD_MEMO_HEADER_CHECKSUM, "Checksum d'en-t�te de memo erron�."       },
    { TO8_BAD_MEMO_HEADER_NAME    , "Nom d'en-t�te de memo incorrect."         },
    { TO8_MEDIA_ALREADY_SET       , "Plus de media libre."                     },
    { TO8_CANNOT_CREATE_DISK      , "Impossible de cr�er la disquette."        },
    { TO8_SAP_NOT_VALID           , "Archive SAP invalide."                    },
    { TO8_FILE_TOO_LARGE          , "Fichier trop important."                  },
    { TO8_FILE_IS_EMPTY           , "Fichier vide."                            },
    { TO8_SAP_DIRECTORY_FULL      , "R�pertoire SAP plein"                     },
    { 0                           , "Erreur inconnue."                         }
#endif
};

static const struct ERROR_TABLE error_table_en[]= {
    { TO8_MULTIPLE_INIT           , "Multiple initialization of the emulator." },
    { TO8_BAD_ALLOC               , "Cannot allocate space."                   },
    { TO8_CANNOT_FIND_FILE        , "Unable to find "                          },
    { TO8_CANNOT_OPEN_FILE        , "Unable to open."                          },
    { TO8_BAD_FILE_FORMAT         , "Bad file format."                         },
    { TO8_UNSUPPORTED_MODEL       , "Unsupported image format."                },
    { TO8_BAD_JOYSTICK_NUM        , "Bad count of joysticks."                  },
    { TO8_BAD_MEMO_HEADER_CHECKSUM, "Bad checksum for memo header."            },
    { TO8_BAD_MEMO_HEADER_NAME    , "Bad name for memo header."                },
    { TO8_MEDIA_ALREADY_SET       , "Media not free."                          },
    { TO8_CANNOT_CREATE_DISK      , "Cannot create disk."                      },
    { TO8_SAP_NOT_VALID           , "SAP archive not valid."                   },
    { TO8_FILE_TOO_LARGE          , "File toot large."                         },
    { TO8_FILE_IS_EMPTY           , "File is empty."                           },
    { TO8_SAP_DIRECTORY_FULL      , "Directory SAP full."                      },
    { 0                           , "Unknown error."                           }
};



/* error_Message:
 *  Renvoie une erreur g�n�r�e par l'�mulateur.
 */
int error_Message(int error, const char moreinfo[])
{
    int i = 0;
    const struct ERROR_TABLE *err_table = is_fr? error_table_fr
                                               : error_table_en;
    if (error < ERR_ERROR)
    {
        while ((err_table[i].err != 0)
           && (err_table[i].err != error))
            i++;
    
        to8_error_msg = std_free (to8_error_msg);
        
#ifdef DJGPP
        to8_error_msg = std_strdup_printf ("%s.", err_table[i].str);
#else
        if (moreinfo == NULL)
            to8_error_msg = std_strdup_printf ("%s.", err_table[i].str);
        else
            to8_error_msg = std_strdup_printf ("%s : %s", err_table[i].str, moreinfo);
#endif
    }
    return ERR_ERROR;
}

