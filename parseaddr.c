/*
*  C Implementation: ip6calc
*
* Description: parsing addresses, and MACs
*
*
* Author: Konrad Rosenbaum <konrad@silmor.de>, (C) 2009,2012
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ip6calc.h"

//switches merge to pass-on
static int mergefirst=1;

static int mergenet=0;
void setmergemode(int mode)
{
	mergenet=mode;
}

void mergeaddr(ip6addr ip,int mask)
{
	int i;
	//stage 1: shortcut for the first addr
	if(mergefirst){
		memcpy(caddr,ip,sizeof(caddr));
		if(mask>=0 && mask<=128)cmask=mask;
		mergefirst=0;
		return;
	}
	/*stage 2: decide which way to merge; mask the addresses*/
	if(mergenet){
		maskhost(caddr,cmask);
		masknet(ip,cmask);
	}else{
		masknet(caddr,cmask);
		maskhost(ip,cmask);
	}
	/*stage 3: merge*/
	for(i=0;i<8;i++)
		caddr[i]|=ip[i];
	/*stage 4: change the mask, if one was given*/
	if(mask>=0 && mask<=128)
		cmask=mask;
}

void parsemac(const char*s)
{
	int i;
	ip6addr ip={0xfe80,0,0,0, 0,0,0,0};
	unsigned char mac[6]={0,0,0, 0,0,0};
	/*parse*/
	if(strlen(s)!=17){
		fprintf(stderr,"address %s is not a valid MAC: must be 12 '-' or ':'  separated hex digits\n",s);
		exit(1);
	}
	for(i=0;i<17;i++){
		if((i%3)==2){
			if(s[i]!='-' && s[i]!=':'){
				fprintf(stderr,"address %s is not a valid MAC: invalid separator\n",s);
				exit(1);
			}
			continue;
		}
		mac[i/3]<<=4;
		if(s[i]>='0' && s[i]<='9')mac[i/3]|=s[i]-'0';else
		if(s[i]>='a' && s[i]<='f')mac[i/3]|=s[i]-'a'+10;else
		if(s[i]>='A' && s[i]<='F')mac[i/3]|=s[i]-'A'+10;
		else{
			fprintf(stderr,"address %s is not a valid MAC: it contains non-hex digits\n",s);
			exit(1);
		}
	}
	/*convert*/
	ip[4]=0x200^((mac[0]<<8)|mac[1]);
	ip[5]=(mac[2]<<8)|0xff;
	ip[6]=0xfe00 | mac[3];
	ip[7]=(mac[4]<<8)|mac[5];
	/*merge, do not change mask*/
	mergeaddr(ip,640);
}

void parseip4(const char*s)
{
	int i;
	ip6addr ip={0,0,0,0, 0,0,0,0};
	/*check*/
	int len=strlen(s),cnt,num;
	if(len<7 || len>16){
		fprintf(stderr,"address %s is not a valid IPv4 address: must be 4 dot separated decimals\n",s);
		exit(1);
	}
	for(i=cnt=0;i<len;i++)
		if(s[i]=='.')cnt++;
	if(cnt!=3){
		fprintf(stderr,"address %s is not a valid IPv4 address: must be 4 dot separated decimals\n",s);
		exit(1);
	}
	/*parse*/
	for(num=cnt=i=0;i<len;i++){
		//found a dot
		if(s[i]=='.'){
			//check
			if(num<0 || num>255){
				fprintf(stderr,"address %s contains an illegal number %i\n",s,num);
				exit(1);
			}
			//flush
			if(cnt==0)ip[6]=num<<8;else
			if(cnt==1)ip[6]|=num;
			else ip[7]=num<<8;
			//continue
			cnt++;num=0;
			continue;
		}
		//a digit
		if(s[i]>='0' && s[i]<='9'){
			num*=10;
			num+=s[i]-'0';
			continue;
		}
		//something else
		fprintf(stderr,"address %s contains illegal character '%c'\n",s,s[i]);
		exit(1);
	}
	//flush last component
	ip[7]|=num;
	/*merge, don't change mask*/
	mergeaddr(ip,300);
}

int isip4(const char*);

void parseip6(const char*a)
{
	int i,j,k,l,mask=255,m,semcnt=0,haddblc=0;
	ip6addr ip={0,0,0,0, 0,0,0,0};
	/*stage 1: parse local addr*/
	if(a[0]==':' && a[1]!=':'){
		fprintf(stderr,"address %s is not a valid IPv6 address: it must start with a hex digit or ::\n",a);
		exit(1);
	}
	m=-1;
	for(i=j=0;a[i];i++){
		if(j>7){
			fprintf(stderr,"address %s is not a valid IPv6 address: more than 8 segments\n",a);
			exit(1);
		}
		if(a[i]=='/')break;
		if(a[i]==':'){
			semcnt++;
			if(!i)continue;
			if(m<0){
				/*its :: */
				if(haddblc){
					fprintf(stderr,"address %s is not a valid IPv6 address: only one :: is allowed\n",a);
					exit(1);
				}
				haddblc++;
				l=0;
				for(k=i+1;a[k];k++)if(a[k]==':')l++;
				if((7-l)<j){
					fprintf(stderr,"address %s is not a valid IPv6 address: too many :\n",a);
					exit(1);
				}
				j=7-l;
			}else{
				ip[j++]=m;m=-1;
			}
		}else{
			if(m<0)m=0;
			m<<=4;
			if(a[i]>='0'&&a[i]<='9')m|=a[i]-'0';else
			if(a[i]>='a'&&a[i]<='f')m|=a[i]-'a'+10;else
			if(a[i]>='A'&&a[i]<='F')m|=a[i]-'A'+10;
			else{
				fprintf(stderr,"address %s is not a valid IPv6 address: %c is not a hex digit\n",a,a[i]);
				exit(1);
			}
			if(m>0xffff){
				fprintf(stderr,"address %s is not a valid IPv6 address: max. 4 hex digits per segment\n",a);
				exit(1);
			}
		}
	}
	/*sanity checks*/
	if(i){
		if(semcnt<2 || semcnt>7){
			fprintf(stderr,"address %s is not a valid IPv6 address: not enough or too many segments\n",a);
			exit(1);
		}
		if(i>2)
		if(a[i-1]==':'&&a[i-2]!=':'){
			fprintf(stderr,"address %s is not a valid IPv6 address: it must not end with a single :\n",a);
			exit(1);
		}
		if(j!=7){
			fprintf(stderr,"address %s is not a valid IPv6 address: wrong number of segments\n",a);
			exit(1);
		}
		if(m>=0)ip[j]=m;
	}
	if(a[i]=='/'){
		/*parse mask*/
		char*p;
		i++;
		if(a[i]==0){
			fprintf(stderr,"invalid mask: must contain a decimal number 0<=mask<=128\n");
			exit(1);
		}
		mask=strtol(a+i,&p,10);
		if(*p!=0){
			fprintf(stderr,"invalid mask: not a decimal number\n");
			exit(1);
		}
		if(mask<0 || mask>128){
			fprintf(stderr,"invalid mask: must be between 0 and 128\n");
			exit(1);
		}
	}
	/*stage 1.5: if it is just a mask: skip stage 2*/
	if(a[0]=='/'){
		cmask=mask;
		return;
	}
	/*stage 2: merge*/
	mergeaddr(ip,mask);
}

///check whether the string is an IPv4 address
int isip4(const char*a)
{
	int num=0,ctrdot=0;
	const char*c;
	//check it is not the NULL string
	if(!a || *a==0)return 0;
	//check it only contains dots and digits
	for(c=a;*c;c++)
		if(*c!='.' && (*c<'0' || *c>'9'))return 0;
	//check syntax: last and first are not dots
	c--;
	if(*a=='.' || *c=='.')return 0;
	//check numbers are 0..255
	for(c=a;*c;c++){
		if(*c=='.'){
			if(++ctrdot > 3)return 0;
			num=0;
		}else{
			num+=*c-'0';
			if(num>255)return 0;
		}
	}
	//done and survived so far
	return 1;
}

///check that it is a MAC address
int ismac(const char*a)
{
	int i;
	//MACs are always 17 chars long
	if(strlen(a)!=17)return 0;
	//check for pattern
	for(i=0;i<17;i++){
		if(i%3 == 2){
			if(a[i]!='-' && a[i]!=':')return 0;
		}else
			if((a[i]<'0' || a[i]>'9')&&
			   (a[i]<'a' || a[i]>'f')&&
			   (a[i]<'A' || a[i]>'F'))
				return 0;
	}
	//done
	return 1;
}

void parseaddr(const char*a)
{
	/*stage 0: does it have a prefix?*/
	if(strncmp("mac:",a,4)==0){
		parsemac(a+4);
		return;
	}
	if(strncmp("ip4:",a,4)==0){
		parseip4(a+4);
		return;
	}
	if(strncmp("ip6:",a,4)==0){
		parseip6(a+4);
		return;
	}
	/*stage 1: try to detect syntax (IPv4 and MAC are scanned first, since
	they are syntactically more strict)*/
	if(isip4(a)){
		parseip4(a);
		return;
	}
	if(ismac(a)){
		parsemac(a);
		return;
	}
	/*stage 2: treat as IPv6*/
	parseip6(a);
}
