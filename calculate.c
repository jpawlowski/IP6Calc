/*
*  C Implementation: ip6calc
*
* Description: calculations on addresses - masking, shifting
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

void masknet(ip6addr caddr,int m)
{
	int i;
	for(i=0;i<8;i++){
		if(m<=0)caddr[i]=0;
		else switch(m){
			case 1:caddr[i]&=0x8000;break;
			case 2:caddr[i]&=0xc000;break;
			case 3:caddr[i]&=0xe000;break;
			case 4:caddr[i]&=0xf000;break;
			
			case 5:caddr[i]&=0xf800;break;
			case 6:caddr[i]&=0xfc00;break;
			case 7:caddr[i]&=0xfe00;break;
			case 8:caddr[i]&=0xff00;break;
			
			case  9:caddr[i]&=0xff80;break;
			case 10:caddr[i]&=0xffc0;break;
			case 11:caddr[i]&=0xffe0;break;
			case 12:caddr[i]&=0xfff0;break;
	
			case 13:caddr[i]&=0xfff8;break;
			case 14:caddr[i]&=0xfffc;break;
			case 15:caddr[i]&=0xfffe;break;
			
			default:break;
		}
		m-=16;
	}
}

void maskhost(ip6addr ip,int m)
{
	int i;
	for(i=0;i<8;i++){
		if(m>=16)ip[i]=0;
		else switch(m){
			case 1:ip[i]&=0x7fff;break;
			case 2:ip[i]&=0x3fff;break;
			case 3:ip[i]&=0x1fff;break;
			case 4:ip[i]&=0x0fff;break;
			
			case 5:ip[i]&=0x07ff;break;
			case 6:ip[i]&=0x03ff;break;
			case 7:ip[i]&=0x01ff;break;
			case 8:ip[i]&=0x00ff;break;
			
			case  9:ip[i]&=0x007f;break;
			case 10:ip[i]&=0x003f;break;
			case 11:ip[i]&=0x001f;break;
			case 12:ip[i]&=0x000f;break;
	
			case 13:ip[i]&=0x0007;break;
			case 14:ip[i]&=0x0003;break;
			case 15:ip[i]&=0x0001;break;
		}
		m-=16;
	}
}

void splitaddr(ip6addr ip1,ip6addr ip2,int mask)
{
	memcpy(ip2,ip1,sizeof(ip6addr));
	masknet(ip1,mask);
	maskhost(ip2,mask);
}

void shiftaddr(int ri)
{
}

int getnum(const char*s)
{
	int num;
	char*end;
	if(s==0){
		fprintf(stderr,"No shift/rotate distance given, must be a number between 0 and 128.\n");
		exit(1);
	}
	num=strtol(s,&end,10);
	if(num<0 || num>128 || *end){
		fprintf(stderr,"Invalid shift/rotate distance %s, must be a number between 0 and 128.\n",s);
	}
	return num;
}

void shiftright_(int num)
{
	int seg=num>>4,bit=num&0xf;
	ip6addr ip;
	//shift by segments
	if(seg){
		int i;
		for(i=0;i<8;i++)
			ip[(i+seg)&7]=caddr[i];
	}else memcpy(ip,caddr,sizeof(ip));
	//shift by bits
	if(bit){
		int i;
		for(i=0;i<7;i++)
			caddr[i+1]=ip[i+1]>>bit | ip[i]<<(16-bit);
		caddr[0]=ip[0]>>bit | ip[7]<<(16-bit);
	}else memcpy(caddr,ip,sizeof(ip));
}

void shiftright(const char*s)
{
	int ri=getnum(s);
	//base checks
	if(ri==0)return;
	if(ri>=128){
		memset(caddr,0,sizeof(ip6addr));
		return;
	}
	//shift
	shiftright_(ri);
	maskhost(caddr,ri);
}

void shiftleft(const char*s)
{
	int le=getnum(s);
	//base checks
	if(le==0)return;
	if(le>=128){
		memset(caddr,0,sizeof(ip6addr));
		return;
	}
	//shift
	shiftright_(128-le);
	masknet(caddr,128-le);
}

void rolright(const char*s)
{
	int ri=getnum(s);
	//base checks
	if(ri==0||ri==128)return;
	//roll
	shiftright_(ri);
}

void rolleft(const char*s)
{
	int le=getnum(s);
	//base checks
	if(le==0||le==128)return;
	//roll
	shiftright_(128-le);
}
