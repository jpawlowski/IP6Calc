#!/bin/sh

#generate start
echo '//THIS IS AN AUTOMATICALLY GENERATED FILE' >help.h
echo '//DO NOT EDIT IT DIRECTLY, IT WILL BE OVERWRITTEN' >>help.h
echo '//See help.txt and COPYING for the original content' >>help.h
echo '//See genhelp.sh for the generator' >>help.h

generate(){
	echo '#define' $1 '\' >>help.h
	cat $2 |sed 's/"/\\"/g' | sed 's/^\(.*\)$/ "\1\\n"\\/' >>help.h
	echo '""' >>help.h
}

#generate HELP text
generate HELP help.txt

#generate COPY text
generate COPY COPYING

#generic end
echo '//END OF GENERATED FILE' >>help.h

