#pragma once

#include <stdbool.h>
#include "hash_table.h"

typedef struct merch merch_t;
typedef struct shelf shelf_t;

merch_t *create_merch_item(char *name);

void destroy_merch_item(ioopm_hash_table_t *merch_ht, merch_t *merch);

void add_merch_item(ioopm_hash_table_t *merch_ht, merch_t *merch);

bool add_merch_loc(ioopm_hash_table_t *storage_ht, merch_t *merch, shelf_t *shelf);

shelf_t *create_shelf(char *location, int quantity);

void destroy_shelf(shelf_t *shelf);

void destroy_all_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);