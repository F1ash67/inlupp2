#pragma once

#include <stdlib.h>
#include "hash_table_general_key.h"
#include "linked_list.h"

typedef struct cart_count cart_count_t;

cart_count_t *initiate_cart_count();

void destroy_cart_count(cart_count_t *count);

size_t create_cart(ioopm_hash_table_t *carts, cart_count_t *count);

bool remove_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id);

int add_to_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id, char *merch, size_t amount);

int remove_from_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id, char *merch, size_t amount);

int calculate_cost(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id);

bool checkout(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, size_t id);