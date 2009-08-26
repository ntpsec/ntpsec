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
#include <sys/types.h>
#include <sys/stat.h>

#include "kod_management.h"
#include "log.h"
#include "sntp-opts.h"
#include "ntp_stdlib.h"
#define DEBUG

int kod_init = 0, kod_db_cnt = 0;
const char *kod_db_file;
struct kod_entry **kod_db;	/* array of pointers to kod_entry */


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

	for (a = 0; a < kod_db_cnt; a++)
		if (!strcmp(kod_db[a]->hostname, hostname)) 
			resc++;

	if (!resc)
		return 0;

	*dst = emalloc(resc * sizeof(**dst));

	b = 0;
	for (a = 0; a < kod_db_cnt; a++)
		if (!strcmp(kod_db[a]->hostname, hostname)) {
			(*dst)[b] = *kod_db[a];
			b++;
		}

	return resc;
}


void 
add_entry(
	char *hostname,
	char *type	/* 4 bytes not \0 terminated */
	)
{
	int n;

	n = kod_db_cnt++;
	kod_db = erealloc(kod_db, kod_db_cnt * sizeof(kod_db[0]));
	kod_db[n] = emalloc(sizeof(*kod_db[n]));

	kod_db[n]->timestamp = time(NULL);

	memcpy(kod_db[n]->type, type, 4);
	kod_db[n]->type[sizeof(kod_db[n]->type) - 1] = '\0';

	strncpy(kod_db[n]->hostname, hostname,
		sizeof(kod_db[n]->hostname));
	kod_db[n]->hostname[sizeof(kod_db[n]->hostname) - 1] = '\0';
}


void 
delete_entry(
	char *hostname,
	char *type
	)
{
	register int a;

	for (a = 0; a < kod_db_cnt; a++)
		if (!strcmp(kod_db[a]->hostname, hostname)
		    && !strcmp(kod_db[a]->type, type))
			break;

	if (a == kod_db_cnt)
		return;

	free(kod_db[a]);
	kod_db_cnt--;

	if (a < kod_db_cnt)
		memmove(&kod_db[a], &kod_db[a + 1], 
			(kod_db_cnt - a) * sizeof(kod_db[0]));
}


void 
write_kod_db(void)
{
	FILE *db_s;
	char *pch;
	int dirmode;
	register int a;

	db_s = fopen(kod_db_file, "w");

	/*
	 * If opening fails, blindly attempt to create each directory
	 * in the path first, then retry the open.
	 */
	if (NULL == db_s && strlen(kod_db_file)) {
		dirmode = S_IRUSR | S_IWUSR | S_IXUSR
			| S_IRGRP | S_IXGRP
			| S_IROTH | S_IXOTH;
		pch = strchr(kod_db_file + 1, DIR_SEP);
		while (NULL != pch) {
			*pch = '\0';
			mkdir(kod_db_file, dirmode);
			*pch = DIR_SEP;
			pch = strchr(pch + 1, DIR_SEP);
		}
	}
			
	db_s = fopen(kod_db_file, "w");

	if (NULL == db_s) {
		char msg[80];

		snprintf(msg, sizeof(msg), 
			 "Can't open KOD db file %s for writing!",
			 kod_db_file);
#ifdef DEBUG
		debug_msg(msg);
#endif
		log_msg(msg, 2);

		return;
	}
			
	for (a = 0; a < kod_db_cnt; a++) {
		fprintf(db_s, "%16.16llx %s %s\n", (unsigned long long)
			kod_db[a]->timestamp, kod_db[a]->type,
			kod_db[a]->hostname);
	}

	fflush(db_s);
	fclose(db_s);
}


void 
kod_init_kod_db(
	const char *db_file
	)
{
	/*
	 * Max. of 254 characters for hostname, 10 for timestamp, 4 for
	 * kisscode, 2 for spaces, 1 for \n, and 1 for \0
	 */
	char fbuf[272];
	FILE *db_s;
	int a, b, sepc;
	unsigned long long ull;
	char *str_ptr;
	char error = 0;

	atexit(write_kod_db);

#ifdef DEBUG
	printf("Initializing KOD DB...\n");
#endif

	kod_db_file = estrdup(db_file);


	db_s = fopen(db_file, "r");

	if (NULL == db_s) {
		char msg[80];

		snprintf(msg, sizeof(msg), "kod_init_kod_db(): Cannot open KoD db file %s", db_file);
#ifdef DEBUG
		debug_msg(msg);
		printf("%s\n", msg);
#endif
		log_msg(msg, 2);

		return;
	}

	if(ENABLED_OPT(NORMALVERBOSE)) 
		printf("Starting to read KoD file %s...\n", db_file);
	/* First let's see how many entries there are and check for right syntax */

	while(!feof(db_s)) {
		fgets(fbuf, sizeof(fbuf), db_s);

		/* ignore blank lines */
		if ('\n' == fbuf[0])
			continue;
		
		sepc = 0;
		for(a=0; a<strlen(fbuf); a++) {
			if (' ' == fbuf[a]) 
				sepc++;
			
			if ('\n' == fbuf[a]) {
				if (sepc != 2) {
					if (strcmp(db_file, "/dev/null")) {
						char msg[80];
						snprintf(msg, sizeof(msg), "Syntax error in KoD db file %s in line %i (missing space)", db_file, (kod_db_cnt + 1));

	#ifdef DEBUG
						debug_msg(msg);
						printf("%s\n", msg);
	#endif

						log_msg(msg, 1);
					}

					return; 
				}

				sepc = 0;
				kod_db_cnt++;
			}
		}
	}

	kod_db_cnt--;

#ifdef DEBUG
	printf("KoD DB %s contains %d entries, reading...\n", db_file, kod_db_cnt);
#endif

	rewind(db_s);

	kod_db = emalloc(sizeof(kod_db[0]) * kod_db_cnt);

	/* Read contents of file */
	for(b=0; !feof(db_s) && !ferror(db_s) && b < kod_db_cnt; b++) {

		str_ptr = fgets(fbuf, sizeof(fbuf), db_s);
		if (NULL == str_ptr) {
			error = 1;
			break;
		}

		/* ignore blank lines */
		if ('\n' == fbuf[0]) {
			b--;
			continue;
		}
		
		kod_db[b] = emalloc(sizeof(*kod_db[b]));

		if (3 != sscanf(fbuf, "%llx %4s %254s", &ull,
		    kod_db[b]->type, kod_db[b]->hostname)) {

			free(kod_db[b]);
			kod_db[b] = NULL;
			error = 1;
			break;
		}

		kod_db[b]->timestamp = (time_t)ull;
	}

	if (ferror(db_s) || error) {
		char msg[80];

		snprintf(msg, sizeof(msg), "An error occured while parsing the KoD db file %s", db_file);
#ifdef DEBUG
		debug_msg(msg);
#endif
		log_msg(msg, 2);

		return;
	}

#ifdef DEBUG
	for (a = 0; a < kod_db_cnt; a++)
		printf("KoD entry %d: %s at %llx type %s\n", a,
		       kod_db[a]->hostname,
		       (unsigned long long)kod_db[a]->timestamp,
		       kod_db[a]->type);

	printf("\n");
#endif


	fclose(db_s);
}
