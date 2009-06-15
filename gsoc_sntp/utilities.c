/*
 * Copyright (C) 2008  Johannes Maximilian Kühn
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "utilities.h"

/* Display a NTP packet in hex with leading address offset 
 * e.g. offset: value, 0: ff 1: fe ... 255: 00
 */ 
void 
pkt_output (
		struct pkt *dpkg,
		int pkt_length, 
		FILE *output
	   )
{
	register int a;

	unsigned char *cpy = (unsigned char *) malloc(sizeof(char) * pkt_length);		

	for(a=0; a<pkt_length; a++) 
		cpy[a] = ((unsigned char *) dpkg)[a];


	fprintf(output, HLINE);

	for(a=0; a<pkt_length; a++) {
		if(a > 0 && a%8 == 0)
			fprintf(output, "\n");

		fprintf(output, "%i: %x \t", a, cpy[a]);
	}

	fprintf(output, "\n");
	fprintf(output, HLINE);
}

/* Output a long floating point value in hex in the style described above 
 */
void
l_fp_output (
		l_fp *ts,
		FILE *output
	    )
{
	register int a;

	fprintf(output, HLINE);

	for(a=0; a<8; a++) 
		fprintf(output, "%i: %x \t", a, ((unsigned char *) ts)[a]);

	fprintf(output, "\n");
	fprintf(output, HLINE);

}

/* Output a long floating point value in binary in the style described above
 */
void 
l_fp_output_bin (
		l_fp *ts,
		FILE *output
		)
{
	register int a, b;

	fprintf(output, HLINE);

	for(a=0; a<8; a++) {
		short tmp = ((unsigned char *) ts)[a];
		tmp++;

		fprintf(output, "%i: ", a);

		for(b=7; b>=0; b--) {
			int texp = (int) pow(2, b);

			if(tmp - texp > 0) {
				fprintf(output, "1");
				tmp -= texp;
			}
			else {
				fprintf(output, "0");
			}
		}

		fprintf(output, " ");
	}

	fprintf(output, "\n");
	fprintf(output, HLINE);
}

/* Output a long floating point value in decimal in the style described above
 */
void
l_fp_output_dec (
		l_fp *ts,
		FILE *output
	    )
{
	register int a;

	fprintf(output, HLINE);

	for(a=0; a<8; a++) 
		fprintf(output, "%i: %i \t", a, ((unsigned char *) ts)[a]);

	fprintf(output, "\n");
	fprintf(output, HLINE);

}

/* Convert a struct addrinfo to a string containing the address in style
 * of inet_ntoa
 */
char *
addrinfo_to_str (
		struct addrinfo *addr
		)
{
	char *buf = (char *) malloc(sizeof(char) * INET6_ADDRSTRLEN);

	getnameinfo(addr->ai_addr, addr->ai_addrlen, buf, 
			INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);

	return buf;
}

/* Convert a sockaddr_u to a string containing the address in
 * style of inet_ntoa
 * Why not switch callers to use stoa from libntp?  No free() needed
 * in that case.
 */
char *
ss_to_str (
		sockaddr_u *saddr
		)
{
	char *buf = (char *) malloc(sizeof(char) * INET6_ADDRSTRLEN);

	getnameinfo(&saddr->sa, SOCKLEN(saddr), buf,
			INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);


	return buf;
}

/* Converts a struct tv to a date string
 */
char *
tv_to_str (
		struct timeval *tv
	  )
{
	static const char *month_names[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	char *buf = (char *) malloc(sizeof(char) * 48);

	time_t cur_time = time(NULL);

	struct tm *tm_ptr = (struct tm *) malloc(sizeof(struct tm));
	tm_ptr = localtime(&cur_time);


	snprintf(buf, 48, "%i %s %.2d %.2d:%.2d:%.2d.%.3d", 
			tm_ptr->tm_year + 1900,
			month_names[tm_ptr->tm_mon],
			tm_ptr->tm_mday,
			tm_ptr->tm_hour,
			tm_ptr->tm_min,
			tm_ptr->tm_sec,
			(int)tv->tv_usec);

	return buf;
}



		
