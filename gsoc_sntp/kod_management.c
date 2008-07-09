/* TODO check for memory leaks */
#include <string.h>

#include "kod_management.h"
#include "log.h"
#include "sntp-opts.h"

int kod_init = 0, entryc = 0;
char *kod_db_file;
struct kod_entry *kod_db;
FILE *db_s;


int
search_entry (
		char *hostname,
		struct kod_entry **dst
	     )
{
	register int a, b, resc = 0;
	struct kod_entry *sptr = kod_db;

	for(a=0; a<entryc && sptr; a++) {
		if(strcmp(sptr->hostname, hostname)) 
			resc++;

		sptr = sptr->next;
	}

	dst = (struct kod_entry **) malloc(sizeof(struct kod_entry) * resc);

	b=0;
	for(a=0; a<entryc && sptr; a++) {
		if(strcmp(sptr->hostname, hostname)) {
			dst[b] = sptr;
			b++;
		}
	}

	return resc;
}


void 
add_entry (
		char *hostname,
		char *type
	  )
{
	if(kod_init) {
		struct kod_entry *new_entry = (struct kod_entry *) malloc(sizeof(struct kod_entry));
		new_entry->hostname = hostname;
		strlcpy(new_entry->type, type, 4);
		new_entry->next = NULL;

		kod_db[entryc-1].next = new_entry;
		entryc++;
	}
}

void
kod_atexit (
	   )
{
	if(kod_init)
		write_kod_db();
}

void 
delete_entry (
		char *hostname,
		char *type
	     )
{
	register int a;

	struct kod_entry *nptr = kod_db;

	for(a=0; a<entryc && nptr != NULL; a++) {
		if(!strcmp(nptr->hostname, hostname) && !strncmp(nptr->type, type, 4)) {
			struct kod_entry *cptr = nptr;
			struct kod_entry *nnptr = nptr;

			nptr--;
			nnptr++;

			nptr->next = nnptr;
			free(cptr);
			nptr = NULL;
		}
		else {
			nptr = nptr->next;
		}
	}
}
								



void 
write_kod_db (
	     )
{
	if(!kod_init)
		return;

	register int a;
	struct kod_entry *nptr = kod_db;

	if(fopen(kod_db_file, "w") == NULL) {
		char msg[80];
		snprintf(msg, 80, "Can't open KOD db file %s for writing!", kod_db_file);

		if(debug)
			debug_msg(msg);

		log_msg(msg, 2);

		return;
	}
			
	for(a=0; a<entryc && nptr != NULL; a++) {
		fprintf(db_s, "%s:%i%s\n", nptr->hostname, nptr->timestamp, nptr->type);
		nptr = nptr->next;
	}

	fflush(db_s);
	fclose(db_s);
}
		
void 
kod_init_kod_db (
		char *db_file
		)
{
	if(kod_init)
		return;

	register int a, b;

	char *fbuf = (char *) malloc(sizeof(char) * 80);
	char *obuf = fbuf;

	db_s = fopen(db_file, "r");

	if(db_file == NULL) {
		char msg[80];

		snprintf(msg, 80, "Cannot open KOD db file %s", db_file);

		if(debug) 
			debug(msg);

		log_msg(msg, 2);
	}

	/* First let's see how many entries there are and check for right syntax */
	while(fread(&fbuf, 80, sizeof(char), db_s) != 0) {
		int sepc = 0;
		for(a=0; a<strlen(fbuf); a++) {
			if(fbuf[a] == ':') 
				sepc++;
			
			if(fbuf[a] == '\n') {
				if(sepc != 2) {
					char msg[80];
					snprintf(msg, 80, "Syntax error in KOD db file %s in line %i", db_file, (entryc + 1));

					if(debug)
						debug(msg);

					log_msg(msg, 1);

					return;
				}

				sepc = 0;
				entryc++;
			}
		}
	}

	rewind(db_s);

	kod_db = (struct kod_entry *) malloc(sizeof(struct kod_entry) * entryc);


	/* Read contents of file and make a linked list */
	for(b=0; (!feof(db_s) || !ferror(db_s)) && b<entryc; b++) {
		fbuf = obuf;
		fgets(fbuf, 80,  db_s);

		char fstart = 0;
		int limit = strlen(fbuf);
		char argn;

		for(a=0; a<limit; a++) {
			argn = 0;

			if(fbuf[a] == ':' || fbuf[a] == '\n' || fbuf[a] == '\0') {
				char *tmp;
				switch(argn) {
					case 0: 
						kod_db[b].hostname = (char *) malloc(sizeof(char) * a);
						strlcpy(kod_db[b].hostname, fbuf, a);
						break;

					case 1: 
						tmp = (char *) malloc(sizeof(char) * a);
						strlcpy(tmp, fbuf, a);
						kod_db[b].timestamp = atoi(tmp);
						free(tmp);
						break;

					case 2:
						tmp = (char *) malloc(sizeof(char) * a);
						strlcpy(tmp, fbuf, a);
						strlcpy(kod_db[b].type, tmp, 4);
						free(tmp);
						break;

				}

				fstart = a+1;
				fbuf = &fbuf[fstart];
				limit -= fstart;
				a = 0;
				argn++;
			}
		}

		if(b > 0) 
			kod_db[b-1].next = &kod_db[b];

	}

	if(ferror(db_s)) {
		char msg[80];
		snprintf(msg, 80, "ferror indicates an error after parsing KOD db file %s", db_file);

		if(debug)
			debug(msg);

		log_msg(msg, 2);

		return;
	}


	kod_db[b].next = NULL;

	fclose(db_s);

	kod_db_file = db_file;

	kod_init = 1;
	atexit(kod_atexit);
}
