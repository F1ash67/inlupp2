#pragma once

typedef struct cart cart_t;

void add_merch(ioopm_hash_table_t *merch_ht);

void list_merch(ioopm_hash_table_t *merch_ht);

void remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void edit_merch(ioopm_hash_table_t *merch_ht);

void show_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

void replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

/*
void create_cart();

void remove_cart(cart_t *c);

void add_to_cart(cart_t *c, *db);

void remove_from_cart(cart_t *c);

void calculate_cost(cart_t *c);

void check_out(cart_t *c);

void undo();

void quit();

void save_to_file();
*/