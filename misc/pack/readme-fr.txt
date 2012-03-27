
                            CREATION DES PACKAGES

Les scripts suivants permettent de de cr�er facilement les packages
pour la distribution de Teo.
'pack.sh' doit �tre ex�cut� en dernier.

djmake.bat
----------

Lancer 'misc\pack\djmake.bat' � partir du r�pertoire 'teo\'.
'djmake.bat' compile les ex�cutables MSDOS en Fran�ais et en Anglais
dans le r�pertoire 'misc\pack\msdos' (le r�pertoire est cr�� si il
n'existe pas).

mgwmake.bat
-----------

Lancer 'misc\pack\mgwmake.bat' � partir du r�pertoire 'teo\'.
'mgwmake.bat' compile les ex�cutables 'teow-en.exe' et 'teow-fr.exe'
dans le r�pertoire 'misc\pack\inno'. Les packages auto-extractibles
seront alors cr��s en double-cliquant et compilant les fichiers '*.iss'.
Inno Setup doit �tre install� (http://www.innosetup.com/).

'inno\teo-big-img.bmp' et 'inno\teo-small-img.bmp' sont utilis�s
par les fichiers '*.iss' pour d�corer la fen�tre de l'installeur.

pack.sh
-------

Lancer './misc/pack/pack.sh' � partir du r�pertoire 'teo/'.

A l'ex�cution, 'pack.sh' :
- Compile l'ex�cutable Linux en mode DEBIAN
- Cr�e le DEBIAN distribuable Linux pour l'ex�cutable
- Compile l'ex�cutable Linux pour le TAR.GZ
- Cr�e le TAR.GZ distribuable Linux
- Cr�e le ZIP distribuable Window en Fran�ais
- Cr�e le ZIP distribuable Window en Anglais
- Cr�e le ZIP distribuable MsDos en Fran�ais
- Cr�e le ZIP distribuable MsDos en Anglais
- Cr�e le ZIP pour les sources
- Cr�e le TAR.GZ pour les sources

Tous les packages sont alors dans le r�pertoire 'misc/pack/'.

