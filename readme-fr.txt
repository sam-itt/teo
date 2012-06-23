
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
                              version 1.8.2

    Copyright (C) 1997-2012 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
                            J�r�mie Guillaume, Fran�ois Mouret,
                            Samuel Devulder


Introduction
------------
Teo est un �mulateur du micro-ordinateur Thomson TO8 pour PC, fonctionnant
sous MSDOS, Windows et Linux. Il a �t� initi� par Gilles F�tis et d�velopp�
par Gilles F�tis, Eric Botcazou, Alexandre Pukall, J�r�mie Guillaume,
Fran�ois Mouret et Samuel Devulder.


Comment l'obtenir ?
-------------------
En le t�l�chargeant depuis la page:

   http://sourceforge.net/projects/teoemulator/

L'archive principale contient le programme �x�cutable de l'�mulateur et la
documentation compl�te.


Compatibilit� avec le TO8
-------------------------
La compatibilit� est proche de 100% pour les logiciels n'utilisant pas de
p�riph�riques non �mul�s et ne contenant pas de protection physique. En
d'autres termes, si un logiciel ne tourne pas sous Teo, alors probablement:
- il requiert la pr�sence d'un p�riph�rique externe autre que la souris,
  le crayon optique, les manettes, les lecteurs de cassettes et disquettes
  (et donc il ne tournera pas tant que ce p�riph�rique ne sera pas �mul�),
- ou sa protection physique l'a fait �chou�.

Nous maintenons une liste des logiciels tournant sous Teo; si vous en
poss�dez un qui pose probl�me, envoyez-le nous, nous essaierons d'identifier
la cause du dysfonctionnement et vous dirons s'il est possible d'y rem�dier.


Probl�mes connus
----------------
- la d�tection automatique de la carte son dans la version MSDOS peut
  �chouer; vous pouvez dans ce cas sp�cifier manuellement les
  caract�ristiques de la carte (type de carte, adresse du port, canal DMA
  et num�ro d'IRQ) en �ditant le fichier teo.cfg du r�pertoire principal.

