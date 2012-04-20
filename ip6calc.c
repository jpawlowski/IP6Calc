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

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ip6calc.h"


//obsolete!
#define OUT_NET 1
#define OUT_HOST 2
#define OUT_MASK 4
#define OUT_MAC 8
#define OUT_IP4 0x10

#define OUT_IPM 7
#define OUT_IP 3
#define OUT_UNTOUCH 0x8000
#define OUT_INIT 0x8007


static char shortopt[]="-hsdfnNHmiIM64DR:r:L:l:eE";
static struct option longopt[]= {
 {"short",0,0,'s'},
 {"detail",0,0,'d'},
 {"full",0,0,'f'},
 {"netid",0,0,'n'},
 {"netmask",0,0,'N'},
 {"hostid",0,0,'H'},
 {"hostmask",0,0,'I'},
 {"mask",0,0,'m'},
 {"mac",0,0,'M'},
 {"help",0,0,'h'},
 {"debug",0,0,'D'},
 {"ipaddr",0,0,'i'},
 {"ipv6",0,0,'6'},
 {"ipv4",0,0,'4'},
 {"shr",1,0,'r'},
 {"shift-right",1,0,'r'},
 {"shl",1,0,'l'},
 {"shift-left",1,0,'l'},
 {"ror",1,0,'R'},
 {"rotate-right",1,0,'R'},
 {"rol",1,0,'L'},
 {"rotate-left",1,0,'L'},
 {"merge-host",0,0,'e'},
 {"merge-net",0,0,'E'},
 {0,0,0,0}
};

ip6addr caddr={0,0,0,0, 0,0,0,0};
short cmask=64;

int main(int argc,char**argv)
{
	int c,optindex=1,np=0;
	/*parse options*/
        while(1){
                c=getopt_long(argc,argv,shortopt,longopt,&optindex);
                if(c==-1)break;
                np=1;
                switch(c){
			//formatting
                        case 's':setformat(FMT_SHORT);break;
                        case 'd':setformat(FMT_DETAIL);break;
                        case 'f':setformat(FMT_FULL);break;
			//output
                        case 'n':np=0;printaddr(IP6_NETID);break;
                        case 'H':np=0;printaddr(IP6_HOSTID);break;
                        case 'm':np=0;printaddr(IP6_PREFIX);break;
			case 'N':np=0;printaddr(IP6_NETID|IP6_PREFIX);break;
			case 'I':np=0;printaddr(IP6_HOSTID|IP6_PREFIX);break;
			case 'i':np=0;printaddr(IP6_NETID|IP6_HOSTID);break;
			case '6':np=0;printaddr(IP6_NETID|IP6_HOSTID|IP6_PREFIX);break;
			case '4':printip4();break;
                        case 'M':printmac();break;
			//mode
			case 'e':setmergemode(MERGE_HOST);break;
			case 'E':setmergemode(MERGE_NET);break;
			case 'D':break;
			//shifty stuff
			case 'r':shiftright(optarg);break;
			case 'R':rolright(optarg);break;
			case 'l':shiftleft(optarg);break;
			case 'L':rolleft(optarg);break;
			//parsing
			case 1:parseaddr(optarg);break;
			//calculation
			//errors
                        default:
                                printerror(argv[0]);
                                return 1;
                                break;
                        case 'h':
                                printhelp(argv[0]);
                                return 0;
                                break;
                }
        }
        /*parse addresses after regular arguments*/
        while(optind<argc){
        	parseaddr(argv[optind]);
        	optind++;
        }
        //print last changes
	if(np)printaddr(IP6_NETID|IP6_HOSTID|IP6_PREFIX);
        //done
	return 0;
}
