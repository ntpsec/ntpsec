#ifndef KOD_MANAGEMENT_H
#define KOD_MANAGEMENT_H

struct kod_entry {
	char hostname[255];
	unsigned int timestamp;
	char type[5];
	struct kod_entry *next;
};

int search_entry (char *hostname, struct kod_entry **dst);
int kod_entry_exists (char *search_str);

void add_entry (char *hostname, char *type);
void delete_entry (char *hostname, char *type);
void init_kod_db (char *db_file);
void write_kod_db (void);
void kod_atexit (void);


#endif
