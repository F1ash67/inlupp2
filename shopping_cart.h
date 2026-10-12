#pragma once

#include <stdlib.h>
#include "hash_table_general_key.h"
#include "linked_list.h"

void create_cart(ioopm_list_t *carts);

void remove_cart(ioopm_list_t *carts, size_t id);

void add_to_cart(ioopm_list_t *carts, size_t id, char *merch, size_t amount);

void remove_from_cart(ioopm_list_t *carts, size_t id, char *merch, size_t amount);

int calculate_cost(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht, size_t id);

void checkout(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, size_t id);