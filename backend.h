#pragma once

typedef struct merch merch_t;


merch_t *create_merch_item(char *name);

void destroy_merch_item(merch_t *merch);

void add_merch_item(ioopm_hash_table_t *merch_ht, merch_t *merch);

bool add_merch_loc(ioopm_hash_table_t *storage_ht, merch_t *merch, shelf_t *shelf);