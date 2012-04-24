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

int format=FMT_SHORT,embed=EMB_AUTO;

void setformat(int f)
{
	format=f;
}

void setembed(int e)
{
	embed=e;
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

int isembedded(const ip6addr ip)
{
	//check forced formatting
	if(embed==EMB_ALWAYS)return 1;
	if(embed==EMB_NEVER)return 0;
	//range ::/88 has two embedded ranges
	if(ip[0]==0 && ip[1]==0 && ip[2]==0 && ip[3]==0 && ip[4]==0){
		// range ::/96, except ::0 and ::1
		if(ip[5]==0)
			return ip[6]!=0 || ip[7]>1;
		// range ::ffff:0:0/96
		return ip[5]==0xffff;
	}
	//range 64:ff9b::/96
	if(ip[0]==0x64 && ip[1]==0xff9b && ip[2]==0 && ip[3]==0 && ip[4]==0 && ip[5]==0)
		return 1;
	//not embedded
	return 0;
}

void printdetail(ip6addr caddr)
{
	const char*fmts1,*fmts2;
	if(format==FMT_FULL){
		fmts1="%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:";fmts2="%04hx:%04hx";
	}else{
		fmts1="%hx:%hx:%hx:%hx:%hx:%hx:";fmts2="%hx:%hx";
	}
	printf(fmts1, caddr[0], caddr[1], caddr[2], caddr[3], caddr[4], caddr[5]);
	if(isembedded(caddr))
		printf("%i.%i.%i.%i", caddr[6]>>8, caddr[6]&0xff, caddr[7]>>8, caddr[7]&0xff);
	else
		printf(fmts2, caddr[6], caddr[7]);
}

void printshort(ip6addr caddr)
{
	int zs=-1,zl=0,mzs=-1,mzl=0,i,size=8;
	/*shortcut: is it all zero?*/
	if(!caddr[0] && !caddr[1] && !caddr[2] && !caddr[3] && !caddr[4] && !caddr[5] && !caddr[6] && !caddr[7]){
		printf("::");
		return;
	}
	/*print in embedded format?*/
	if(isembedded(caddr))size=6;
	/*find longest stretch of zeroes*/
	for(i=0;i<size;i++){
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
	if(i<(size-1)){
		printf("::");
		for(i=mzs+mzl;i<size;i++){
			printf("%hx",caddr[i]);
			if(i<7)printf(":");
		}
	}
	//print embedded
	if(size<8)
		printf("%i.%i.%i.%i", caddr[6]>>8, caddr[6]&0xff, caddr[7]>>8, caddr[7]&0xff);
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
