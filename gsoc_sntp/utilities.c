#include "utilities.h"

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

void 
l_fp_output_bin (
		l_fp *ts,
		FILE *output
		)
{
	register int a, b, bexp;

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

