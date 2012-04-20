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

#define HELP \
 "Usage: %1$s [options] address/mask...\n" \
 "IPv6 calculator\n"\
 "(c) Konrad Rosenbaum, 2009, 2012\n"\
 "this program is protected under the GNU GPLv3 or at your option any newer\n"\
 "\n"\
 "Address parameters:\n"\
 "  2001:db8::1/64 - address and mask in standard IPv6 notation\n" \
 "  2001:db8::1 - same address, mask is assumed to be /128\n" \
 "  /64 - keeps the currently calculated address, but changes the netmask\n"\
 "  mac:00-11-22-33-44-55 - the mac will be translated into a link local\n"\
 "    address fe80::211:22ff:fe33:4455/64\n"\
 "  mac:00-11-22-33-44-55/128 - same translation, but explicit netmask\n"\
 "  if multiple addresses are given the program attempts to merge them\n"\
 "\n"\
 "Output options:\n" \
 "  -h | --help\n" \
 "    display this help text and exit\n" \
 \
 "  -s | --short\n" \
 "    print short IPv6 output (eg. 2001:db8::1/128) - this is the default\n"\
 \
 "  -d | --detail\n" \
 "    print detailed IPv6 output (eg. 2001:db8:0:0:0:0:0:1/128)\n"\
 \
 "  -f | --full\n" \
 "    print full IPv6 output (eg. 2001:0db8:0000:0000:0000:0000:0000:0001)\n"\
 \
 "  -n | --netid\n" \
 "    include the network ID into the output\n"\
 \
 "  -H | --hostid\n" \
 "    include the host ID into the output\n"\
 \
 "  -m | --mask\n"\
 "    include the network mask of the last component\n"\
 \
 "  -M | --mac\n"\
 "    attempt to extract a MAC address from the resulting host ID\n"\
 \
 "  -6 | --ipv6\n"\
 "    alias for -Hnm, this is the default if there are no other output parameters\n"\
 \
 "  -4 | --ipv4\n"\
 "    print the last 32 bits of the address as an IPv4 address\n"\
 \
 "\n"\
 "Examples:\n"\
 "  %1$s 2001:db8::1/64\n  -> 2001:db8::/64\n"\
 "  %1$s 2001:db8::1/64 fe80::211:22ff:fe33:4455\n    -> 2001:db8::211:22ff:fe33:4455/128\n"\
 "  %1$s 2001:db8::1/64 fe80::211:22ff:fe33:4455 /64\n    -> 2001:db8::211:22ff:fe33:4455/64\n"\
 "  %1$s --netid 2001:db8::211:22ff:fe33:4455/64\n    -> 2001:db8::\n"\
 "  %1$s --hostid 2001:db8::211:22ff:fe33:4455/64\n    -> ::211:22ff:fe33:4455\n"\
 "  %1$s --mask 2001:db8::211:22ff:fe33:4455/64\n    -> /64\n"\
 "  %1$s --netid --hostid --mask 2001:db8::211:22ff:fe33:4455/64\n    -> 2001:db8::211:22ff:fe33:4455/64\n"\
 "  %1$s 2001:db8::211:22ff:fe33:4455/64\n    -> 2001:db8::211:22ff:fe33:4455/64\n"\
 "  %1$s --mac 2001:db8::211:22ff:fe33:4455\n    -> MAC=00-11-22-33-44-55\n"

void printhelp(const char*arg)
{
	fprintf(stderr,HELP,arg);
}

void printerror(const char*arg)
{
	fprintf(stderr,"Syntax error. Use '%s --help' to see options.\n",arg);
}
