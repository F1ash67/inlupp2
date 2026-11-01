/**
 * @file actions.h
 * @brief The webstore's menu actions.
 *
 * Every action reads its input from the user (stdin) and prints its
 * result or an error message (stdout). Invalid input, such as an unknown
 * name, an unknown cart id or a non-positive amount, prints an error and
 * returns without changing anything.
 */

#pragma once

#include "hash_table_general_key.h"
#include "shopping_cart.h"



/// @brief Asks for a name, a descripton and a price, and adds a new merch item to the database
/// @param merch_ht The hash table containing all merch items
void add_merch(ioopm_hash_table_t *merch_ht);

/// @brief Lists all merch items in the database
/// @param merch_ht The hash table containing all merch items
void list_merch_items(ioopm_hash_table_t *merch_ht);

/// @brief Asks for a merch name, then removes the specified merch item from the database on successful confirmation
/// @param merch_ht The hash table containing all merch items
/// @param storage_ht The hash table containing the stock and storage locations for all merch items
void remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

/// @brief Asks for a merch name, a new name, description and price for the specified merch,
/// and edits the merch item on successful confirmation
/// @param merch_ht The hash table containing all merch items
/// @param storage_ht The hash table containing the stock and storage locations for all merch items
void edit_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);


/// @brief Asks for a merch name, and prints the available stock and storage locations for the specified merch
/// @param merch_ht The hash table containing all merch items
void show_stock(ioopm_hash_table_t *merch_ht);

/// @brief Asks for a merch name and an amount, and replenishes the specified merch item with the specified amount
/// @param merch_ht The hash table containing all merch items
/// @param storage_ht The hash table containing the stock and storage locations for all merch items
void replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

/// @brief Creates a new, empty shopping cart, and prints the id of the cart
/// @param carts The hash table containing all shopping carts
/// @param count The shopping cart counter
void create_sc(ioopm_hash_table_t *carts, cart_count_t *count);

/// @brief Asks for a shopping cart id, and removes the specified cart on successful confirmation
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
void remove_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

/// @brief Asks for a shopping cart id, a merch name and amount,
/// and adds the specified merch with the given amount to the specified shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
void add_to_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

/// @brief Asks for a shopping cart id, a merch name and amount,
/// and removes the specified amount from the given merch from the specified shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
void remove_from_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

/// @brief Asks for a shopping cart id, and calculates the price for the given cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
void calculate_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht);

/// @brief Asks for a shooping cart id, and checks out the specified shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
/// @param storage_ht The hash table containing the stock and storage locations for all merch items
void checkout_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);

/// @brief Quits the webstore on succesful confirmation
/// @return 0 on aborted quits, otherwise -1
int quit(void);

/// @brief The event loop that calls all actions
/// @param merch_ht The hash table containing all merch items
/// @param storage_ht The hash table containing the stock and storage locations for all merch items
/// @param carts The hash table containing all shopping carts
/// @param count The shopping cart counter
/// @return -1 on succesful quit, otherwise 0
int event_loop(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *carts, cart_count_t *count);