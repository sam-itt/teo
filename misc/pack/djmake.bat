@echo off

REM ---- Configures Teo sources for the DOS(djgpp) platform ----

echo "Configuring Teo sources for the DOS(djgpp) platform..."

echo # generated by fixdjgpp.bat > makefile
echo MAKEFILE_INC = makefile.dj >> makefile
echo include makefile.all >> makefile

REM ---- Creates 'misc\pack\msdos' folder if not exists ----

echo test > misc\pack\msdos\test.txt
if exist misc\pack\msdos\test.txt goto SkipMsdos
echo "Creating misc\pack\msdos folder..."
mkdir misc\pack\msdos
goto ExitMsdos
:SkipMsdos
del misc\pack\msdos\test.txt
:ExitMsdos

REM ---- Creates 'misc\pack\msdos\en' folder if not exists ----

echo test > misc\pack\msdos\en\test.txt
if exist misc\pack\msdos\en\test.txt goto SkipEn
echo "Creating misc\pack\msdos\en folder..."
mkdir misc\pack\msdos\en
goto ExitEn
:SkipEn
del misc\pack\msdos\en\test.txt
:ExitEn

REM ---- Creates 'misc\pack\msdos\fr' folder if not exists ----

echo test > misc\pack\msdos\fr\test.txt
if exist misc\pack\msdos\fr\test.txt goto SkipFr
echo "Creating misc\pack\msdos\fr folder..."
mkdir misc\pack\msdos\fr
goto ExitFr
:SkipFr
del misc\pack\msdos\fr\test.txt
:ExitFr

REM ---- Cleans destination directories ----

if exist misc\pack\msdos\fr\makefile.* del misc\pack\msdos\fr\makefile.*
if exist misc\pack\msdos\en\makefile.* del misc\pack\msdos\en\makefile.*
if exist misc\pack\msdos\fr\*.exe del misc\pack\msdos\fr\*.exe
if exist misc\pack\msdos\en\*.exe del misc\pack\msdos\en\*.exe

REM ---- Compiles Teo ----

echo "Creating Teo executable for MSDOS in English..."

make veryclean
make EN_LANG=1
make depend
copy teo.exe misc\pack\msdos\en\teo.exe
copy obj\djgpp\makefile.dep misc\pack\msdos\en\makefile.dep

echo "Creating Teo executable for MSDOS in French..."

make veryclean
make FR_LANG=1
make depend
copy teo.exe misc\pack\msdos\fr\teo.exe
copy obj\djgpp\makefile.dep misc\pack\msdos\fr\makefile.dep
make veryclean

REM ---- Compiles saptools ----

echo "Creating Saptools executables for MSDOS in English..."

cd tools
cd sap
make clean
make EN_LANG=1
cd ..
cd ..
copy tools\sap\sap2.exe misc\pack\msdos\en\sap2.exe
copy tools\sap\sapfs.exe misc\pack\msdos\en\sapfs.exe

echo "Creating Saptools executables for MSDOS in French..."

cd tools
cd sap
make clean
make FR_LANG=1
cd ..
cd ..
copy tools\sap\sap2.exe misc\pack\msdos\fr\sap2.exe
copy tools\sap\sapfs.exe misc\pack\msdos\fr\sapfs.exe

cd tools
cd sap
make clean
cd ..
cd ..

REM ---- Compiles k7tools ----

echo "Creating K7tools executables for MSDOS in English..."

cd tools
cd k7tools
make clean
make EN_LANG=1
cd ..
cd ..
copy tools\k7tools\wav2k7.exe misc\pack\msdos\en\wav2k7.exe

echo "Creating K7tools executables for MSDOS in French..."

cd tools
cd k7tools
make clean
make FR_LANG=1
cd ..
cd ..
copy tools\k7tools\wav2k7.exe misc\pack\msdos\fr\wav2k7.exe

cd tools
cd k7tools
make clean
cd ..
cd ..

echo "Done!"

