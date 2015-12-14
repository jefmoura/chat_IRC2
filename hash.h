#include "list.h"

struct entry_s {
	int key;
	char *value;
	struct entry_s *next;
	TipoLista list;
};

typedef struct entry_s entry_t;

struct hashtable_s {
	int size;
	struct entry_s **table;	
};

typedef struct hashtable_s hashtable_t;

hashtable_t *ht_create(int);
int ht_hash(hashtable_t *, int);
entry_t *ht_newpair(int, char *);
void ht_set(hashtable_t *, int, char *);
entry_t *ht_get(hashtable_t *, int);
