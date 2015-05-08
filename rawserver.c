#include "headers.c"
int main (void)
{
  char randomness[]="Kaushik want to send big data.\0";
  int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);	/* open raw socket */
  char datagram[1500];	/* this buffer will contain ip header, tcp header,
			   and payload. we'll point an ip header structure
			   at its beginning, and a tcp header structure after
			   that to write the header values into it */
  struct ip *iph = (struct ip *) datagram;
  struct tcphdr *tcph = (struct tcphdr *) datagram + sizeof (struct ip);
  struct buffer *buf1 = (struct buffer *) datagram + sizeof (struct tcphdr);
  struct sockaddr_in sin;
			/* the sockaddr_in containing the dest. address is used
			   in sendto() to determine the datagrams path */

  sin.sin_family = AF_INET;
  sin.sin_port = htons (P);/* you byte-order >1byte header values to network
			      byte order (not needed on big endian machines) */
  sin.sin_addr.s_addr = inet_addr ("127.0.0.1");

  memset (datagram, 0, 1500);	/* zero out the buffer */

  printf("\nhere1\n");  
/* we'll now fill in the ip/tcp header values, see above for explanations */
  iph->ip_hl = 5;
  iph->ip_v = 4;
  iph->ip_tos = 0;
  iph->ip_len = sizeof (struct ip) + sizeof (struct tcphdr) + sizeof (randomness);	
  iph->ip_id = htonl (54321);	/* the value doesn't matter here */
  iph->ip_off = 0;
  iph->ip_ttl = 255;
  iph->ip_p = 6;
  iph->ip_sum = 0;		/* set it to 0 before computing the actual checksum later */
  iph->ip_src= inet_addr ("127.0.0.1");/* SYN's can be blindly spoofed */
  iph->ip_dst= sin.sin_addr.s_addr;
  tcph->th_sport = htons (0);	/* arbitrary port */
  tcph->th_dport = htons (0);
  tcph->th_seq = random();/* in a SYN packet, the sequence is a randomness */
  tcph->th_ack = 0;/* number, and the ack sequence is 0 in the 1st packet */
  tcph->th_x2 = 0;
  tcph->th_off = 0;		/* first and only tcp segment */
  tcph->th_flags = 0;	/* initial connection request */
  tcph->th_win = htonl (65535);	/* maximum allowed window size */
  tcph->th_sum = 0;/* if you set a checksum to zero, your kernel's IP stack
		      should fill in the correct checksum during transmission */
  tcph->th_urp = 0;
  printf("\nhere1.1\n"); 
  strcpy(buf1->buff,randomness);
  printf("\nhere1.2\n");  
  printf("\nhere2\n"); 
  iph->ip_sum = csum ((unsigned short *) datagram, iph->ip_len >> 1);
  printf("\nhere3\n"); 
  //strcpy(datagram,randomness);
  //strcpy(datagram,"\0");
/* finally, it is very advisable to do a IP_HDRINCL call, to make sure
   that the kernel knows the header is included in the data, and doesn't
   insert its own header into the packet before our data */
 //printf("\nhere\n");

  {				/* lets do it the ugly way.. */
    int one = 1;
    const int *val = &one;
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
      printf ("Warning: Cannot set HDRINCL!\n");
  }
  int counter=1;
 
  while (counter<=8)
    {
     sleep(1);
      if (sendto (s,		/* our socket */
		  datagram,	/* the buffer containing headers and data */
		  iph->ip_len,	/* total length of our datagram */
		  0,		/* routing flags, normally always 0 */
		  (struct sockaddr *) &sin,	/* socket addr, just like in */
		  sizeof (sin)) < 0)		/* a normal send() */
	err(errno,NULL);
      else
 
	printf ("\nsent somtheing\n");
      counter++;
    }

  return 0;
}
