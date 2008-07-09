/* TODO!!!
 * adopt type spefication of ntp for portability 
 */
 
#include "netutils.h"

unsigned int dns_query_count = 0;

/* Datatype for the DNS header. They are ordered as in a actual "message" */
typedef struct dns_header {
	unsigned short id; /* Identifier */

	bool qr; /* Query (=0) or response (=1) */
	
	char opcode; /* Specifies what kind of message this is. Only 4 bits are
			   relevant. */
	bool aa; /* In a server reply this indicates if the asked server is a authority
		    for the domain name in question. */

	bool tc; /* Indicates truncation */
	
	bool rd; /* Recursion desired bit (relevant obviously only for queries */
	
	bool ra; /* Recursion available (set by the DNS server) */

	bool z; /* 1 bit of whatever */
	
	bool ad; /* Set by the server if the data is authenticated */

	bool cd; /* Checking disabled bit */

	char rcode; /* Return code */

	unsigned short qdcount; /* Number of entries in the question section */
	
	unsigned short ancount; /* Number of entries in the answer section */
	
	unsigned short nscount; /* Number of entries in the answer section */
	
	unsigned short arcount; /* Number of resource records */
} dns_header_t;

typedef struct dns_query {
	char *domain_name; /* *domain_name and *ength will be used to generate 
			   	the QNAME field that consists of a domain name 
				in octets of labels and their size. */
	unsigned short length;  /* Indicates the length of the domain name
				   (must be smaller or equal than 63) */
		
	unsigned short qtype;
	unsigned short qclass;
} dns_query_t;


typedef struct dns_resource_record {
	char *name; /* A domainname to which this resource belongs */
	unsigned short type; /* The type of this resouce (RDATA) */
	unsigned short class; /* The class of this resource (RDATA) */
	unsigned int ttl; /* The TTL of the resource before it should be discharged */
	unsigned short rdlength; /* Length of the octets in the RDATA fields */
	char *rdata; /* RDATA types that describe the resource */
} dns_resource_record_t;


int async_resolve (
		char *hname,
		char *adr_ptr[]
		)
{
	register int a;

	/* Prepare a dns header */
	dns_header_t header;
	header.id = dns_query_count; dns_query_count++;
	header.qr = false;
	header.opcode = 0;
	header.aa = false;
	header.tc = false;
	header.rd = false;
	header.ra = false;
	header.z = false;
	header.ad = false;
	header.cd = false;
	header.rcode = 0;
	header.qdcount = 1;
	header.ancount = 0;
	header.nscount = 0;
	header.arcount = 0;

	/* Prepare a dns query */
	dns_query_t query;

	query.domain_name = hname;

	query.domain_name = hname;
	query.length = (short) strlen(hname);
	query.qtype = 0;
	query.class = 1; /* 1 = IN = Internet ... or better 255 = ANY? */

	char **header; char **query;

	size_t header_s = compile_header(&header);
	size_t query_s = compile_header(&query);

}

/* Translate the header structure into the header format. Always returns 96 
 * bits of data. Consumes a header structure and a destination pointer 
 */
void compile_header (
		dns_header_t *header,
		char **dst
		)
{
	unsigned char *comph = (unsigned char *) malloc(sizeof(char) * 12);
	
	/* When declaring id_high unsigned char, will the char cast on the right side
	 * convert to unsigned char, too?
	 */

	/* Split 16 bit into two 8bit unsinged chars */
	comph[0] = (unsigned char) ((header->id & 0xff00) >> 8);
	comph[1] = (unsigned char) (header->id & 0x00ff);

	/* 8 bits from qr to rd */
	unsigned char qr_rd = 0;

	if(header->qr) 
		qr_rd = ((char) 1) << 7;

	header->opcode &= 0x0f;
	header->opcode <<= 3;

	qr_rd |= header->opcode;

	if(header->aa) 
		qr_rd |= ((char) 1) << 2;
	
	if(header->tc) 
		qr_rd |= ((char) 1) << 1;

	if(header->rd) 
		qr_rd |= 1;

	comph[2] = qr_rd;

	unsigned char ra_rcode = 0;

	if(header->ra) 
		ra_rcode = ((char) 1) << 7;

	if(header->z)
		ra_rcode |= ((char) 1) << 6;

	if(header->ad) 
		ra_rcode |= ((char) 1) << 5;

	if(header->cd) 
		ra_rcode |= ((char) 1) << 4;

	header->rcode &= 0x0f;

	ra_rcode |= header->rcode;

	comph[3] = ra_rcde;

	comph[4] = (char) ((header->qdcount & 0xff00) >> 8);
	comph[5] = (char) (header->qdcount & 0x00ff);
	comph[6] = (char) ((header->ancount & 0xff00) >> 8);
	comph[7] = (char) (header->ancount & 0x00ff);
	comph[8] = (char) ((header->nscount & 0xff00) >> 8);
	comph[9] = (char) (header->nscount & 0x00ff);
	comph[10] = (char) ((header->arcount & 0xff00) >> 8);
	comph[11] = (char) (header->arcount & 0x00ff);

	*dst = comph;
	
	return;
}

/* Translate the query into the query format. It is of variable length so 
 * it'll return the queries size in bytes 
 */

size_t compile_query (
		dns_query_t *query,
		char **dst
		)	 
{
	register int a;

	char **qname;
	size_t dlength = domain_to_name(query->domain_name, qname); 
	size_t length = dlength;

	
	char qtype_high = (char) ((query->qtype & 0xff00) >> 8);
	char qtype_low = (char) (query->qtype & 0x00ff);
	char qclass_high = (char) ((query->qclass & 0xff00) >> 8);
	char qclass_low = (char) (query->qclass & 0x00ff);

	length += 4;

	char *res = (char *) malloc(sizeof(char) * length);

	for(a=0; a<dlength; a++) {
		res[a] = *qname[a];
	}

	res[a+1] = qtype_high;
	res[a+2] = qtype_low;
	res[a+3] = qclass_high;
	res[a+4] = qclass_low;

	*dst = res;

	return length;
}

size_t compile_resource_record (
		dns_resource_record_t *resc,
		char **dst
		)
{
	register int a, b;

	char **qname;

	size_t dlength = domain_to_qname(resc->name, qname); 

	size_t length = (dlength + 10 + rdlength);

	char *res = (char *) malloc(sizeof(char) * length);

	for(a=0; a<dlength; a++) {
		res[a] = *qname[a];
	}

	res[a+1] = (char) ((resc->type & 0xff00) >> 8);
	res[a+2] = (char) (resc->type & 0x00ff);
	res[a+3] = (char) ((resc->class & 0xff00) >> 8);
	res[a+4] = (char) (resc->class & 0x00ff);

	res[a+5] = (char) ((resc->ttl & 0xff000000) >> 24);
	res[a+6] = (char) ((resc->ttl & 0x00ff0000) >> 16);
	res[a+7] = (char) ((resc->ttl & 0x0000ff00) >> 8);
	res[a+8] = (char) (resc->ttl & 0x000000ff);

	res[a+9] = (char) ((resc->rdlength & 0xff00) >> 8);
	res[a+10] = (char) (resc->rdlength & 0x00ff);

	a += 11;

	for(b=0; b<resc->rd_length; b++, a++) {
		res[a] = resc->rdata[b];
	}

	*dst = res;

	return length;
}

size_t domain_to_qname (
		char *hostname,
		char **dst
		)
{
	register int a, b, c, dotc;

	for(a=0, dotc=0; a<strlen(hostname); a++) {
		if(hostname[a] == '.') 
			dotc++;
	}

	char *res = (char *) malloc(sizeof(char) * strlen(hostname) + dotc + 1);
	char *len = (char *) malloc(sizeof(char) * dotc);

	char prevd = 0;
	for(a=0, b=0; a<strlen(hostname); a++) {
		if(hostname[a] == '.') {
			len[b] = a - prevd;
			prevd = a;
			b++;
		}
	}

	for(a=0, b=0; a<prevd; a++) {
		res[b] = len[a];
		b++;

		for(c=0; hostname[c] != '.' && c < strlen(hostname); c++, b++) {
			res[b] = hostname[c];
		}

		b++; c++;
	}

	res[b] = 0;

	*dst = res;

	return strlen(res);
}

size_t short_length (short num) {
	size_t length = 1;

	while(num > 10) {
		num /= 10;
		length++;
	}

	return length;
}

int retrv_dnsadr (
		char *adr_ptr[]
		)
{}
	
