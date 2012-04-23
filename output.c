/*
*  C Implementation: ip6calc
*
* Description: printing addresses or parts of them
*
*
* Author: Konrad Rosenbaum <konrad@silmor.de>, (C) 2009,2012
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ip6calc.h"

//efficient formatting: 2001:db8::1
#define FMT_SHORT 0
//more extensive formatting: 2001:db8:0:0:0:0:0:1
#define FMT_DETAIL 1
//extreme formatting: 2001:0db8:0000:0000:0000:0000:0000:0001
#define FMT_FULL 2


int format=FMT_SHORT;

void setformat(int f)
{
	format=f;
}

void printmac()
{
	if((caddr[5]&0xff)==0xff && (caddr[6]&0xff00)==0xfe00){
		printf("%02x-%02x-%02x-%02x-%02x-%02x\n",
			(caddr[4]>>8)^0x02,
			caddr[4]&0xff,
			caddr[5]>>8,
			caddr[6]&0xff,
			caddr[7]>>8,
			caddr[7]&0xff
		);
	}else{
		fprintf(stderr,"Result does not contain a MAC address.\n");
		exit(1);
	}
}

void printip4()
{
	printf("%i.%i.%i.%i\n",caddr[6]>>8,caddr[6]&0xff,caddr[7]>>8,caddr[7]&0xff);
}

void printdetail(ip6addr caddr)
{
	if(format==FMT_FULL)
		printf("%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:%04hx", caddr[0], caddr[1], caddr[2], caddr[3], caddr[4], caddr[5], caddr[6], caddr[7]);
	else
		printf("%hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx", caddr[0], caddr[1], caddr[2], caddr[3], caddr[4], caddr[5], caddr[6], caddr[7]);
}

void printshort(ip6addr caddr)
{
	int zs=-1,zl=0,mzs=-1,mzl=0,i;
	/*shortcut: is it all zero?*/
	if(!caddr[0] && !caddr[1] && !caddr[2] && !caddr[3] && !caddr[4] && !caddr[5] && !caddr[6] && !caddr[7]){
		printf("::");
		return;
	}
	/*find longest stretch of zeroes*/
	for(i=0;i<7;i++){
		if(caddr[i]){
			/*check for maximum*/
			if(zl>mzl){
				mzl=zl;
				mzs=zs;
			}
			/*reset*/
			zl=0;zs=-1;
		}else{
			/*if a zero and previous not, remember pos*/
			if(!zl)zs=i;
			/*increase length*/
			zl++;
		}
	}
	/*check again*/
	if(zl>mzl){mzl=zl;mzs=zs;}
	/*print*/
	if(mzs<0 || mzl<2){
		printdetail(caddr);
		return;
	}
	for(i=0;i<mzs;i++){
		if(i)printf(":");
		printf("%hx",caddr[i]);
	}
	if(i>=7)return;
	printf("::");
	for(i=mzs+mzl;i<8;i++){
		printf("%hx",caddr[i]);
		if(i<7)printf(":");
	}
}

void printaddr(int comp)
{
	ip6addr ip;
	memcpy(ip,caddr,sizeof(ip));
	/*print address*/
	if(comp&3){
		//just part of it?
		if((comp&3)==IP6_HOSTID)maskhost(ip,cmask);
		if((comp&3)==IP6_NETID)masknet(ip,cmask);
		/*actually print*/
		if(format==FMT_SHORT)
			printshort(ip);
		else
			printdetail(ip);
	}
	/*print mask*/
	if(comp&IP6_PREFIX)printf("/%hi",cmask);
	/*newline*/
	printf("\n");
}
