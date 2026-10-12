#pragma once

#include <stdbool.h>
#include "hash_table_general_key.h"
#include "linked_list.h"

typedef struct merch merch_t;
typedef struct shelf shelf_t;

size_t get_price(merch_t *merch);

merch_t *find_merch_item(ioopm_hash_table_t *merch_ht, char *name);

merch_t *create_merch_item(ioopm_hash_table_t *merch_ht, char *name, char *desc, size_t price);

void remove_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name);

void edit_merch_item(ioopm_hash_table_t *merch_ht, char *name, char *new_name, char *new_desc, size_t new_price);

bool replenish_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name, char *location);

ioopm_list_t *get_stock_list(ioopm_hash_table_t *ht, char *name);

size_t get_tot_stock(ioopm_hash_table_t *merch_ht, char* name);

void destroy_all_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);