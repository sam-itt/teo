
                            CREATION DES PACKAGES

Les scripts suivants permettent de de cr�er facilement les packages
pour la distribution de Teo et de Cc90hfe.
'pack.sh' doit �tre ex�cut� en dernier.

djmake.bat
----------

Lancer 'misc\pack\djmake.bat' � partir du r�pertoire 'teoemulator-code\'.
'djmake.bat' compile les ex�cutables MSDOS dans les r�pertoires
'misc\pack\msdos\en' et 'misc\pack\msdos\fr' (les r�pertoires seront cr��s
s'ils n'existent pas).

mgwmake.bat
-----------

Lancer 'misc\pack\mgwmake.bat' � partir du r�pertoire 'teoemulator-code\'.
'mgwmake.bat' compile les ex�cutables dans le r�pertoire 'misc\pack\mingw\en'
et 'misc\pack\mingw\fr'  (les r�pertoires seront cr��s s'ils n'existent pas).
Le package auto-extractible sera cr�� en double-cliquant et compilant le
fichier '.iss'. Inno Setup doit �tre install� (http://www.innosetup.com/).

'inno\teo-big-img.bmp' et 'inno\teo-small-img.bmp' sont utilis�s
par les fichiers '*.iss' pour d�corer la fen�tre de l'installeur.

pack.sh
-------

Lancer './misc/pack/pack.sh' � partir du r�pertoire 'teoemulator-code/'.

Tous les packages sont alors dans le r�pertoire 'misc/pack/'.

