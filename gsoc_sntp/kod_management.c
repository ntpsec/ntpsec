/* TODO check for memory leaks */
#include <string.h>

#include "kod_management.h"
#include "log.h"
#include "sntp-opts.h"
#define DEBUG

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

void 
add_entry (
		char *hostname,
		char *type
	  )
{
	if(kod_init) {
		struct kod_entry *new_entry = (struct kod_entry *) malloc(sizeof(struct kod_entry));
		strcpy(new_entry->hostname, hostname);
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
		char *db_file
		)
{
	if(kod_init)
		return;

#ifdef DEBUG
	printf("Initializing KOD DB...\n");
#endif

	register int a, b;

	/* Max. of 255 characters for hostname, 10 for timestamp, 4 for kisscode, 2 for format : and 1 for \n */
	char fbuf[272];
	char *obuf = fbuf;
	char error = 0;


	db_s = fopen(db_file, "r");

	if(db_file == NULL) {
		char msg[80];

		snprintf(msg, 80, "kod_init_kod_db(): Cannot open KOD db file %s", db_file);

#ifdef DEBUG
		debug_msg(msg);
		printf("%s\n", msg);
#endif

		log_msg(msg, 2);
	}

	printf("Starting to read KOD file %s...\n", db_file);
	/* First let's see how many entries there are and check for right syntax */

	int scan_value = 0;
	while(!feof(db_s)) {
		fgets(fbuf, 272, db_s);
		
		int sepc = 0;
		for(a=0; a<strlen(fbuf); a++) {
			if(fbuf[a] == ':') 
				sepc++;
			
			if(fbuf[a] == '\n') {
				if(sepc != 2) {
					char msg[80];
					snprintf(msg, 80, "Syntax error in KOD db file %s in line %i (missing :)", db_file, (entryc + 1));

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
	for(b=0; (!feof(db_s) || !ferror(db_s)) && b<entryc; b++) {
		char *str_ptr = fgets(fbuf, 272, db_s);

		int j = sscanf(fbuf, "%255[^:]", &(kod_db[b].hostname));
		j += sscanf(fbuf + j, "%*[^:]:%i:%4s", &kod_db[b].timestamp, &(kod_db[b].type));

		if(str_ptr == NULL) {
			b = entryc;
			error = 1;
		} 
		else {
			if(b > 0) 
				kod_db[b-1].next = &kod_db[b];
		}
	}

#ifdef DEBUG
	for(a=0; a<entryc; a++)
		printf("KOD entry %i: %s at %i type %s\n", a, kod_db[a].hostname, 
				kod_db[a].timestamp, kod_db[a].type);

	printf("\n");
#endif

	if(ferror(db_s) || error) {
		char msg[80];
		snprintf(msg, 80, "An error occured while parsing the KOD db file %s", db_file);

#ifdef DEBUG
		debug_msg(msg);
#endif

		log_msg(msg, 2);

		return;
	}


	kod_db[b].next = NULL;

	fclose(db_s);

	kod_db_file = db_file;

	kod_init = 1;
	atexit(kod_atexit);
}
