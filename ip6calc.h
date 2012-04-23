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

#ifndef IP6CALC_H
#define IP6CALC_H

//output.c

///efficient formatting: 2001:db8::1
#define FMT_SHORT 0
///more extensive formatting: 2001:db8:0:0:0:0:0:1
#define FMT_DETAIL 1
///extreme formatting: 2001:0db8:0000:0000:0000:0000:0000:0001
#define FMT_FULL 2

///print the network ID
#define IP6_NETID 1
///print the host ID
#define IP6_HOSTID 2
///print the prefix
#define IP6_PREFIX 4

///sets the output formatting of IPv6 addresses (use FMT_* constants as argument)
void setformat(int o);
///print the MAC part of the address
void printmac();
///print the last 32 bits as IPv4 address
void printip4();
///print the IPv6 address with the given components (see IP6_* constants)
void printaddr(int comp);

//ip6calc.c

///storage type for IPv6 addresses
typedef unsigned short ip6addr[8];
///buffer for current address
extern ip6addr caddr;
///current network mask
extern short cmask;

//parseaddr.c

///merge the host ID of the new address into the net of the old one
#define MERGE_HOST 0
///merge the net ID of the new address into the host of the old one
#define MERGE_NET 1
///sets the merge mode: see MERGE_* constants
void setmergemode(int mode);
///interface function for parsing an IP address and merging it into the current buffer
void parseaddr(const char*a);

//calculate.c

///mask the ip address, so only the network ID remains
void masknet(ip6addr,int);
///mask the ip address, so only the host ID remains
void maskhost(ip6addr,int);
///splits the ip address, leaves the net ID in the first one, moves the host ID into the second one
void splitaddr(ip6addr,ip6addr,int);

///shift the address to the right
void shiftright(const char*);
///roll the address to the right (bits shifted off reappear on the left)
void rolright(const char*);
///shift the address to the left
void shiftleft(const char*);
///roll the address to the left (bits shifted off reappear on the right)
void rolleft(const char*);


//help.c
///prints the help pages
void printhelp(const char*);
///prints a short syntactic error message
void printerror(const char*);
///prints the license
void printgpl();

#endif
