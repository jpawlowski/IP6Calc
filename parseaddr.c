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

void parsemac(const char*a)
{
	int i;
	ip6addr ip={0xfe80,0,0,0, 0,0,0,0};
	unsigned char mac[6]={0,0,0, 0,0,0};
	const char*s=a+4;
	/*parse*/
	if(strlen(s)!=17){
		fprintf(stderr,"address %s is not a valid MAC: must be 12 '-' or ':'  separated hex digits\n",a);
		exit(1);
	}
	for(i=0;i<17;i++){
		if((i%3)==2){
			if(s[i]!='-' && s[i]!=':'){
				fprintf(stderr,"address %s is not a valid MAC: invalid separator\n",a);
				exit(1);
			}
			continue;
		}
		mac[i/3]<<=4;
		if(s[i]>='0' && s[i]<='9')mac[i/3]|=s[i]-'0';else
		if(s[i]>='a' && s[i]<='f')mac[i/3]|=s[i]-'a'+10;else
		if(s[i]>='A' && s[i]<='F')mac[i/3]|=s[i]-'A'+10;
		else{
			fprintf(stderr,"address %s is not a valid MAC: it contains non-hex digits\n",a);
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

void parseaddr(const char*a)
{
	int i,j,k,l,mask=255,m,semcnt=0,haddblc=0;
	ip6addr ip={0,0,0,0, 0,0,0,0};
	/*stage 0: is it an IP or a MAC?*/
	if(strncmp("mac:",a,4)==0){
		parsemac(a);
		return;
	}
	if(strncmp("ip4:",a,4)==0){
		parseip4(a+4);
		return;
	}
	/*stage 1: parse local addr*/
	if(a[0]==':' && a[1]!=':'){
		fprintf(stderr,"address %s is not valid: it must start with a hex digit or ::\n",a);
		exit(1);
	}
	m=-1;
	for(i=j=0;a[i];i++){
		if(j>7){
			fprintf(stderr,"address %s is not valid: more than 8 segments\n",a);
			exit(1);
		}
		if(a[i]=='/')break;
		if(a[i]==':'){
			semcnt++;
			if(!i)continue;
			if(m<0){
				/*its :: */
				if(haddblc){
					fprintf(stderr,"address %s is not valid: only one :: is allowed\n",a);
					exit(1);
				}
				haddblc++;
				l=0;
				for(k=i+1;a[k];k++)if(a[k]==':')l++;
				if((7-l)<j){
					fprintf(stderr,"address %s is not valid: too many :\n",a);
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
				fprintf(stderr,"address %s is not valid: %c is not a hex digit\n",a,a[i]);
				exit(1);
			}
			if(m>0xffff){
				fprintf(stderr,"address %s is not valid: max. 4 hex digits per segment\n",a);
				exit(1);
			}
		}
	}
	/*sanity checks*/
	if(i){
		if(semcnt<2 || semcnt>7){
			fprintf(stderr,"address %s is not valid: not enough or too many segments\n",a);
			exit(1);
		}
		if(i>2)
		if(a[i-1]==':'&&a[i-2]!=':'){
			fprintf(stderr,"address %s is not valid: it must not end with a single :\n",a);
			exit(1);
		}
		if(j!=7){
			fprintf(stderr,"address %s is not valid: wrong number of segments\n",a);
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
