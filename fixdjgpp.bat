@echo off
echo Configuring Teo sources for the DOS(djgpp) platform...

echo # generated by fixdjgpp.bat > makefile
echo MAKEFILE_INC = makefile.dj >> makefile
echo include makefile.all >> makefile

if [%1] == [--quick] goto done

REM for %%a in (*.*) do echo %%a

cd src
utod .../*.c .../*.rc .../*.xpm .../*.txt .../*.log
cd ..
cd misc
utod .../*.bat .../*.txt
cd ..
cd k7tools
utod .../*.c .../makefile.* .../*.txt
cd ..
cd include
utod .../*.h .../*.rh
cd ..
cd fonts
utod .../*.txt
cd ..
cd doc
utod .../*.htm*
cd ..
utod *.bat makefile.* *.txt *.log

:done

echo Done!
