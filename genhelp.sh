#!/bin/sh

echo '#define HELP \' >help.h
echo ' "Usage: %1$s [parameters...]\n\n" \' >>help.h
cat help.txt |sed 's/"/\\"/g' | sed 's/^\(.*\)$/ "\1\\n"\\/' >>help.h
echo '""' >>help.h
