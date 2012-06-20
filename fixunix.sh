#!/bin/sh

echo "Configuring Teo sources for the Linux platform..."

echo "# generated by fixunix.sh" > makefile
echo "MAKEFILE_INC = makefile.lnx" >> makefile
echo "include makefile.all" >> makefile

if [ "$1" != "--quick" ]; then
   find . -type d "(" \
      -name ".hg" -prune \
      ")" -o \
      -type f "(" \
      -name "*.c" -o -name "*.h" -o -name "*.rc" -o -name "*.rh" -o \
      -name "*.xpm" -o -name "*.sh" -o -name "makefile.*" -o \
      -name "*.txt" -o -name "*.log" -o -name "*.htm*" \
      ")" \
      -exec sh -c "echo -n '.';
                   mv {} _tmpfile;
                   tr -d \\\r < _tmpfile > {};
                   touch -r _tmpfile {};
                   rm _tmpfile" \;

   chmod +x *.sh misc/*.sh
   echo
fi

echo "Done!"
