/*
*  C Implementation: ip6calc
*
* Description: a simple tool to calculate IPv6 addresses
*
*
* Author: Konrad Rosenbaum <konrad@silmor.de>, (C) 2009
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define FMT_SHORT 0
#define FMT_DETAIL 1
#define FMT_FULL 2

#define OUT_NET 1
#define OUT_HOST 2
#define OUT_MASK 4
#define OUT_MAC 8

#define OUT_IPM 7
#define OUT_IP 3
#define OUT_UNTOUCH 0x8000
#define OUT_INIT 0x8007


char shortopt[]="hsdfnHmM";
struct option longopt[]= {
 {"short",0,0,'s'},
 {"detail",0,0,'d'},
 {"full",0,0,'f'},
 {"netid",0,0,'n'},
 {"hostid",0,0,'H'},
 {"mask",0,0,'m'},
 {"mac",0,0,'M'},
 {"help",0,0,'h'},
 {0,0,0,0}
};

#define HELP \
 "Usage: %1$s [options] address/mask...\n" \
 "IPv6 calculator\n"\
 "(c) Konrad Rosenbaum, 2009\n"\
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
 "    displays this help text and exit\n" \
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
 "    print the network ID\n"\
 \
 "  -H | --hostid\n" \
 "    print the host ID\n"\
 \
 "  -m | --mask\n"\
 "    print the network mask of the last component\n"\
 \
 "  -M | --mac\n"\
 "    attempt to extract a MAC address from the resulting host ID\n"\
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

int format=FMT_SHORT,outputs=OUT_INIT;

void setoutput(int o)
{
	if(outputs&OUT_UNTOUCH)outputs=0;
	outputs|=o;
}

typedef unsigned short ip6addr[8];
ip6addr caddr={0,0,0,0, 0,0,0,0};
short cmask=0;

void printmac()
{
	if((outputs&OUT_MAC)==0)return;
	if((caddr[5]&0xff)==0xff && (caddr[6]&0xff00)==0xfe00){
		printf("MAC=%02x-%02x-%02x-%02x-%02x-%02x\n",
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

void printdetail()
{
	if(format==FMT_FULL)
		printf("%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:%04hx", caddr[0], caddr[1], caddr[2], caddr[3], caddr[4], caddr[5], caddr[6], caddr[7]);
	else
		printf("%hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx", caddr[0], caddr[1], caddr[2], caddr[3], caddr[4], caddr[5], caddr[6], caddr[7]);
}

void printshort()
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
		printdetail();
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

void printaddr()
{
	int m,i;
	ip6addr ip;
	if((outputs&OUT_IPM)==0)return;
	/*print address*/
	if(outputs&OUT_IP){
		/*null out unwanted parts*/
		if((outputs&OUT_IP)!=OUT_IP){
			memcpy(ip,caddr,sizeof(ip));
			m=cmask;
			for(i=0;i<8;i++){
				if(m>=16)ip[i]=0;else
				if(m<=0)caddr[i]=0;
				else switch(m){
					case 1:caddr[i]&=0x8000;ip[i]&=0x7fff;break;
					case 2:caddr[i]&=0xc000;ip[i]&=0x3fff;break;
					case 3:caddr[i]&=0xe000;ip[i]&=0x1fff;break;
					case 4:caddr[i]&=0xf000;ip[i]&=0x0fff;break;
					
					case 5:caddr[i]&=0xf800;ip[i]&=0x07ff;break;
					case 6:caddr[i]&=0xfc00;ip[i]&=0x03ff;break;
					case 7:caddr[i]&=0xfe00;ip[i]&=0x01ff;break;
					case 8:caddr[i]&=0xff00;ip[i]&=0x00ff;break;
					
					case  9:caddr[i]&=0xff80;ip[i]&=0x007f;break;
					case 10:caddr[i]&=0xffc0;ip[i]&=0x003f;break;
					case 11:caddr[i]&=0xffe0;ip[i]&=0x001f;break;
					case 12:caddr[i]&=0xfff0;ip[i]&=0x000f;break;
			
					case 13:caddr[i]&=0xfff8;ip[i]&=0x0007;break;
					case 14:caddr[i]&=0xfffc;ip[i]&=0x0003;break;
					case 15:caddr[i]&=0xfffe;ip[i]&=0x0001;break;
				}
				m-=16;
			}
			if((outputs&OUT_IP)==OUT_HOST)
				memcpy(caddr,ip,sizeof(ip));
		}
		/*actually print*/
		if(format==FMT_SHORT)
			printshort();
		else
			printdetail();
	}
	/*print mask*/
	if(outputs&OUT_MASK)printf("/%hi",cmask);
	/*newline*/
	printf("\n");
}

void mergeaddr(ip6addr ip,int mask)
{
	int i,m;
	/*stage 2: abbrev addrs*/
	m=cmask;
	for(i=0;i<8;i++){
		if(m>=16)ip[i]=0;else
		if(m<=0)caddr[i]=0;
		else switch(m){
			case 1:caddr[i]&=0x8000;ip[i]&=0x7fff;break;
			case 2:caddr[i]&=0xc000;ip[i]&=0x3fff;break;
			case 3:caddr[i]&=0xe000;ip[i]&=0x1fff;break;
			case 4:caddr[i]&=0xf000;ip[i]&=0x0fff;break;
			
			case 5:caddr[i]&=0xf800;ip[i]&=0x07ff;break;
			case 6:caddr[i]&=0xfc00;ip[i]&=0x03ff;break;
			case 7:caddr[i]&=0xfe00;ip[i]&=0x01ff;break;
			case 8:caddr[i]&=0xff00;ip[i]&=0x00ff;break;
			
			case  9:caddr[i]&=0xff80;ip[i]&=0x007f;break;
			case 10:caddr[i]&=0xffc0;ip[i]&=0x003f;break;
			case 11:caddr[i]&=0xffe0;ip[i]&=0x001f;break;
			case 12:caddr[i]&=0xfff0;ip[i]&=0x000f;break;
	
			case 13:caddr[i]&=0xfff8;ip[i]&=0x0007;break;
			case 14:caddr[i]&=0xfffc;ip[i]&=0x0003;break;
			case 15:caddr[i]&=0xfffe;ip[i]&=0x0001;break;
		}
		m-=16;
	}
	/*stage 3: merge*/
	for(i=0;i<8;i++)
		caddr[i]^=ip[i];
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
	/*merge*/
	mergeaddr(ip,64);
}

void parseaddr(const char*a)
{
	int i,j,k,l,mask=128,m,semcnt=0,haddblc=0;
	ip6addr ip={0,0,0,0, 0,0,0,0};
	/*stage 0: is it an IP or a MAC?*/
	if(strncmp("mac:",a,4)==0){
		parsemac(a);
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

int main(int argc,char**argv)
{
	int c,optindex=1;
	/*parse options*/
        while(1){
                c=getopt_long(argc,argv,shortopt,longopt,&optindex);
                if(c==-1)break;
                switch(c){
                        case 's':format=FMT_SHORT;break;
                        case 'd':format=FMT_DETAIL;break;
                        case 'f':format=FMT_FULL;break;
                        case 'n':setoutput(OUT_NET);break;
                        case 'H':setoutput(OUT_HOST);break;
                        case 'm':setoutput(OUT_MASK);break;
                        case 'M':setoutput(OUT_MAC);break;
                        default:
                                fprintf(stderr,"Syntax error in arguments.\n");
                                fprintf(stderr,HELP,argv[0]);
                                return 1;
                                break;
                        case 'h':
                                fprintf(stderr,HELP,argv[0]);
                                return 0;
                                break;
                }
        }
        /*parse addresses*/
        while(optind<argc){
        	parseaddr(argv[optind]);
        	optind++;
        }
        /*output*/
        printmac();
        printaddr();
	return 0;
}
