/*
*  C Implementation: ip6calc
*
* Description: a simple tool to calculate IPv6 addresses
*
*
* Author: Konrad Rosenbaum <konrad@silmor.de>, (C) 2009,2012
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include "ip6calc.h"

#include <stdio.h>

#include "help.h"

void printhelp(const char*arg)
{
	fprintf(stderr,HELP,arg);
}

void printerror(const char*arg)
{
	fprintf(stderr,"Syntax error. Use '%s --help' to see options.\n",arg);
}
