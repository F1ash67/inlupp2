#pragma once

#include "hash_table_general_key.h"
#include "shopping_cart.h"

char ask_question_menu(void);

void add_merch(ioopm_hash_table_t *merch_ht);

void list_merch_items(ioopm_hash_table_t *merch_ht);

void remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void edit_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void show_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void create_sc(ioopm_hash_table_t *carts, cart_count_t *count);

void remove_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

void add_to_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

void remove_from_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

void calculate_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

void checkout_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

int quit(void);

int event_loop(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *carts, cart_count_t *count);