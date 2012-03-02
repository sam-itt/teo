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
 *                          J�r�mie Guillaume, Samuel Devulder
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
 *  Module     : xargs.h
 *  Version    : 1.8.1
 *  Cr�� par   : Samuel Devulder 30/07/2011
 *  Modifi� par:
 *
 *  Module de traitement des arguments suppl�mentaires en ligne de commande.
 */

#ifndef XARGS_H
#define XARGS_H

typedef struct {
    char *disk[4];        /* arguments de -disk1..4 */
    char *k7;        /* argument de -k7 */
    char *memo;             /* cartouche en ligne de cmd */
    char *dir[4];           /* repertoires pass�s en diskette */
    char *sapfs;        /* chemin d'acc�s vers sapfs */
    int   interlaced;       /* mode gfx entrelac� */
    
    /*
     * Rempli buf avec le nom d'un fichier
     * temporaire (taille maxi = maxlen).
     *
     * Retourne buf si tout est ok, NULL sinon.
     */
    char* (*tmpFile)(char *buf, int maxlen);
    
    /*
     * Lance la commande "cmd" par l'os dans le dossier "dir".
     */
    void (*sysExec)(char *cmd, const char *dir);
    
    /*
     * Affiche un message d'erreur et quitte le programme.
     */
    void (*exitMsg)(char *msg);
    
    /*
     * Retourne TRUE si le chemin est un fichier.
     */
    int (*isFile)(char *path);
    
    /*
     * Retourne FALSE si le chemin est un dossier.
     */
    int (*isDir)(char *path);
    
    /*
     * Efface un fichier
     */
    void (*rmFile)(char *path);
    
    /*
     * fonction appellee quand un argument inconnu est pr�sent�.
     * retourne le nombre d'arguments suivants � ignorer.
     */
    int (*unknownArg)(char *arg);
} xargs;

/* xargs_init:
 *   initialise la structure xargs � vide
 */
extern void xargs_init(xargs *args);

/* xargs_parse:
 *  parse argc, argv et rempli la structure xargs. Les pointeurs
 *  sur fonctions doivent avoir �t�s remplis si on veut avoir acces
 *  a certaines fonctionnalit�s.
 */
extern void xargs_parse(xargs *args, int argc, char **argv);

/* xargs_start:
 *   met en place les donn�es de TEO en fonction des arguments
 *   suppl�mentaires.
 */
extern void xargs_start(xargs *args);

/* xargs_exit:
 *   nettoie les donnees utilis�es par ce module
 */
extern void xargs_exit(xargs *args);

#endif

