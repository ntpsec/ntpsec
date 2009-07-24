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

/* TODO check for memory leaks */
#include <string.h>

#include "kod_management.h"
#include "log.h"
#include "sntp-opts.h"
#define DEBUG

int kod_init = 0, entryc = 0;
const char *kod_db_file;
struct kod_entry *kod_db;
FILE *db_s;


/*
 * Search for a KOD entry
 */
int
search_entry (
		char *hostname,
		struct kod_entry **dst
	     )
{
	register int a, b, resc = 0;
	struct kod_entry *sptr;

	sptr = kod_db;
	for (a = 0; a < entryc && sptr; a++) {
		if (!strcmp(sptr->hostname, hostname)) 
			resc++;

		sptr = sptr->next;
	}

	if (!resc)
		return 0;

	*dst = malloc(sizeof(struct kod_entry) * resc);
	if (NULL == *dst)
		return 0;

	sptr = kod_db;
	b = 0;
	for (a = 0; a < entryc && sptr; a++) {
		if (!strcmp(sptr->hostname, hostname)) {
			(*dst)[b] = *sptr;
			(*dst)[b].next = &((*dst)[b + 1]);
			b++;
		}

		sptr = sptr->next;
	}
	if (b)
		(*dst)[b - 1].next = NULL;

	return resc;
}

#if 0	/* presently useless */
int
kod_entry_exists (
		char *search_str
		)
{
	struct kod_entry **dummy = NULL;

	if(dummy == NULL)
		return 0;

	else
		return 1;
}
#endif

void 
add_entry (
		char *hostname,
		char *type
	  )
{
	if(kod_init) {
		struct kod_entry *new_entry = (struct kod_entry *) malloc(sizeof(struct kod_entry));
		strcpy(new_entry->hostname, hostname);
		strncpy(new_entry->type, type, 4);
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


#ifdef DEBUG
		debug_msg(msg);
#endif

		log_msg(msg, 2);

		return;
	}
			
	for(a=0; a<entryc && nptr != NULL; a++) {
		fprintf(db_s, "%s:%i:%s\n", nptr->hostname, nptr->timestamp, nptr->type);
		nptr = nptr->next;
	}

	fflush(db_s);
	fclose(db_s);
}
		
void 
kod_init_kod_db (
		const char *db_file
		)
{
	register int a, b;

	/* Max. of 255 characters for hostname, 10 for timestamp, 4 for kisscode, 2 for format : and 1 for \n */
	char fbuf[272];
	char error = 0;

	if (kod_init)
		return;

#ifdef DEBUG
	printf("Initializing KOD DB...\n");
#endif

	db_s = fopen(db_file, "r");

	if(db_s == NULL) {
		char msg[80];

		snprintf(msg, 80, "kod_init_kod_db(): Cannot open KOD db file %s", db_file);

#ifdef DEBUG
		debug_msg(msg);
		printf("%s\n", msg);
#endif

		log_msg(msg, 2);
	}

	if(ENABLED_OPT(NORMALVERBOSE)) 
		printf("Starting to read KOD file %s...\n", db_file);
	/* First let's see how many entries there are and check for right syntax */

	while(!feof(db_s)) {
		int sepc = 0;

		fgets(fbuf, sizeof(fbuf), db_s);
		
		for(a=0; a<strlen(fbuf); a++) {
			if(fbuf[a] == ':') 
				sepc++;
			
			if(fbuf[a] == '\n') {
				if(sepc != 2) {
					char msg[80];
					snprintf(msg, sizeof(msg), "Syntax error in KOD db file %s in line %i (missing :)", db_file, (entryc + 1));

#ifdef DEBUG
					debug_msg(msg);
					printf("%s\n", msg);
#endif

					log_msg(msg, 1);

					return; 
				}

				sepc = 0;
				entryc++;
			}
		}
	}

	entryc--;

#ifdef DEBUG
	printf("KOD DB %s contains %i entries, reading...\n", db_file, entryc);
#endif

	rewind(db_s);

	kod_db = (struct kod_entry *) malloc(sizeof(struct kod_entry) * entryc);

	/* Read contents of file and make a linked list */
	for(b=0; !feof(db_s) && !ferror(db_s) && b < entryc; b++) {
		char *str_ptr;
		int j;

		str_ptr = fgets(fbuf, sizeof(fbuf), db_s);
		if (NULL == str_ptr) {
			error = 1;
			break;
		}

		j = sscanf(fbuf, "%255[^:]", (char *) &(kod_db[b].hostname));
		/* sscanf returns count of fields, not characters, so this looks iffy */
		/* also why not a single call to sscanf combining both of these? */
		j += sscanf(fbuf + j, "%*[^:]:%i:%4s", &kod_db[b].timestamp, (char *) &(kod_db[b].type));

		kod_db[b].next = NULL;
		if (b > 0) 
			kod_db[b-1].next = &kod_db[b];
	}

	if (ferror(db_s) || error) {
		char msg[80];

		snprintf(msg, sizeof(msg), "An error occured while parsing the KOD db file %s", db_file);
#ifdef DEBUG
		debug_msg(msg);
#endif
		log_msg(msg, 2);

		return;
	}

#ifdef DEBUG
	for(a=0; a<entryc; a++)
		printf("KOD entry %i: %s at %i type %s\n", a, kod_db[a].hostname, 
				kod_db[a].timestamp, kod_db[a].type);

	printf("\n");
#endif


	fclose(db_s);

	kod_db_file = db_file;

	kod_init = 1;
	atexit(kod_atexit);
}
