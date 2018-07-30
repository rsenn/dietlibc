#include <resolv.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/nameser.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/utsname.h>

int main(int argc,char* argv[]) {
  int pnpfd;
  struct sockaddr_in6 pnpsa;
  struct pollfd pfd;
  struct ipv6_mreq opt;
  unsigned int interface=0;
  unsigned char inpkg[4095];

  if (argc<2) {
    puts("usage: pnpdnsd eth0");
    return 111;
  }
  interface=if_nametoindex(argv[1]);
  if (interface==0) {
    puts("pnpdnsd: network interface not found!");
    return 111;
  }
  pnpfd=socket(PF_INET6,SOCK_DGRAM,IPPROTO_UDP);

  /* bind to port 53 */
  memset(&pnpsa,0,sizeof(struct sockaddr_in6));
  pnpsa.sin6_family=AF_INET6;
  pnpsa.sin6_port=htons(53);
  pnpsa.sin6_scope_id=interface;
  if (bind(pnpfd,(struct sockaddr*)&pnpsa,sizeof(pnpsa))) {
    perror("bind");
    return 111;
  }

  /* join the multicast group */
  memmove(&opt.ipv6mr_multiaddr,"\xff\x02\x00\x00\x00\x00\x00\x00\x00\x00dnspnp",16);
  opt.ipv6mr_interface=interface;
  if (setsockopt(pnpfd,IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,&opt,sizeof opt)==-1) {
    perror("setsockopt");
    return 111;
  }

  {
    socklen_t len=sizeof(pnpsa);
    if (getsockname(pnpfd,(struct sockaddr*)&pnpsa,&len)==-1) {
      perror("getsockname");
      return 111;
    }
  }

  for (;;) {
    int len;
    int fromlen;
    int qdcount;
    struct sockaddr_in6 peersa;
    fromlen=sizeof(peersa);
    len=recvfrom(pnpfd,inpkg,sizeof(inpkg),0,(struct sockaddr*)&peersa,&fromlen);
    if (len==-1) {
      perror("recvfrom");
      return 111;
    }

    /* is it a recursive query, as libresolv would issue? */
    if (inpkg[2]!=1) continue;

    qdcount=((unsigned long)(inpkg[4])<<8) + inpkg[5];

    {
      int decofs;
      char name[257];
      char myname[257];
      struct utsname u;
      decofs=dn_expand(inpkg,inpkg+len,inpkg+12,name,sizeof(name)-1);
      puts(name);
      uname(&u);
      if (strlen(u.nodename)>63) u.nodename[63]=0;
      if (!strcasecmp(u.nodename,name)) {
	char packet[1500];
	char* c;
	packet[0]=inpkg[0];
	packet[1]=inpkg[1];
	packet[2]=inpkg[2] | 0x84;
	packet[3]=0;
	memset(packet+4,0,8);
	packet[7]=1;
	packet[12]=strlen(u.nodename);
	strcpy(packet+13,u.nodename);
	c=packet+14+strlen(u.nodename);
	if (((long)c)&1) { *c=0; ++c; }
	/* int16 type */
	c[0]=0; c[1]=28;	/* AAAA */
	/* int16 class */
	c[2]=0; c[3]=1;	/* IN */
	/* int32 ttl */
	c[4]=0; c[5]=0; c[6]=0; c[7]=23;	/* 23 seconds TTL */
	/* int16 rdlength */
	c[8]=0; c[9]=16;
	/* int16 rdata */
	memmove(c+10,&pnpsa.sin6_addr,16);

	if (sendto(pnpfd,packet,c+26-packet,0,(struct sockaddr*)&peersa,fromlen)==-1)
	  perror("sendto");
      }
    }
  }
}
