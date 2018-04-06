
            TTTTTTTTTTTTTT  EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
            TTTTTTTTTTTTTT  EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
                  TT        EE              OO          OO
                  TT        EE              OO          OO
                  TT        EE              OO          OO
                  TT        EEEEEEEEEE      OO          OO
                  TT        EEEEEEEEEE      OO          OO
                  TT        EE              OO          OO
                  TT        EE              OO          OO
                  TT        EE              OO          OO
                  TT        EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
                  TT        EEEEEEEEEEEEEE  OOOOOOOOOOOOOO

                        L'�mulateur Thomson TO8
                              version 1.8.5

    Copyright (C) 1997-2018 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
                            J�r�mie Guillaume, Fran�ois Mouret,
                            Samuel Devulder


1. Introduction
---------------
Le code source de l'�mulateur Teo est distribu� sous licence GPL. Il est
�crit, pour sa partie ind�pendante de la plateforme, en Standard C89 avec
deux extensions mineures Standard C99 support�es par la plupart des
compilateurs: le modificateur "inline" et le type "long long int".


L'arbre du code est le suivant:

src         : l'�mulateur TO8 proprement dit
 |
 --- alleg  : interface avec la librairie Allegro
 |
 --- dos    : version MSDOS/DPMI, code �crit pour GCC (DJGPP)
 |
 --- linux  : version Linux/X11, code �crit pour GCC
 |
 --- mc68xx : �mulateurs des circuits Motorola MC6809E, MC6821 et MC6846
 |
 --- win    : version Windows/DirectX, code �crit pour GCC (MinGW)


2. Version MSDOS/DPMI
---------------------
La plateforme de compilation de r�f�rence est la suivante:
- djgpp 2.03
- GCC 2.95.3
- binutils 2.11
- make 3.79.1
- librairie Allegro 4.0.3


3. Version Windows/DirectX
--------------------------
La plateforme de compilation de r�f�rence est la suivante:
- MinGW 1.0.1
- GCC 2.95.3
- binutils 2.11.90
- make 3.79.1
- librairie Allegro 4.0.3


4. Version Linux/X11
--------------------
La plateforme de compilation de r�f�rence est la suivante:
- kernel 2.6.8
- glibc 3.3.4
- GCC 3.3.4
- binutils 2.15.91
- make 3.80
- X.Org 6.8.1
- GTK+ 2.4.9


Eric Botcazou
e-mail: ebotcazou@libertysurf.fr

Fran�ois Mouret
e-mail: fjjm@orange.fr

