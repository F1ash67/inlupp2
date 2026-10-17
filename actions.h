#pragma once

void add_merch(ioopm_hash_table_t *merch_ht);

void list_merch_items(ioopm_hash_table_t *merch_ht);

void remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void edit_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void show_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void create_sc(ioopm_list_t *carts);

void remove_sc(ioopm_list_t *carts);

void add_to_sc(ioopm_list_t *carts);

void remove_from_sc(ioopm_list_t *carts);

void calculate_sc(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht);

void check_out_sc(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

int quit();

int event_loop(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, ioopm_list_t);