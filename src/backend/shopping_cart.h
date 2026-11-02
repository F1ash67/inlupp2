/**
 * @file shopping_cart.h
 * @brief The backend for shopping cart actions
 * 
 * Adding merch to a shopping cart reserves that amount of stock,
 * so other carts can't take it. The stock is only removed from storage on checkout.
 * Removing merch or a whole cart releases its reservation.
 * 
 * Codes:
 *      -1: Invalid shopping cart
 *      -2: Invalid merch name
 *      -3: Invalid merch amount
 *      -4: Success
 */

#pragma once

#include <stdlib.h>
#include "../../lib/hash_table_general_key.h"

/// @brief A struct containg a size_t element to give a new shopping cart an unused id
typedef struct cart_count cart_count_t;

/// @brief Initiates the cart count at 1 and saves it on the heap
/// @return A new cart counter
cart_count_t *cart_initiate_cart_count();

/// @brief Destroys the cart count removing it from the heap
/// @param count The cart count
void cart_destroy_cart_count(cart_count_t *count);

/// @brief Creates a new shopping cart and inserts it into the cart hash table
/// @param carts The hash table containing all shopping carts
/// @param count The cart count
/// @return The id of the new shopping cart
size_t cart_create_cart(ioopm_hash_table_t *carts, cart_count_t *count);

/// @brief Destroys the shopping cart hash table and all carts it contains, removing them from the heap
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
void cart_destroy_carts(ioopm_hash_table_t *carts, ioopm_hash_table_t * merch_ht);

/// @brief Removes a shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
/// @param id The id of the shopping cart to remove
/// @return True on succesful removal, otherwise false
bool cart_remove_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id);

/// @brief Adds an amount of a merch item to a shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
/// @param id The id of the shopping cart
/// @param merch The name of the merch
/// @param amount The amount of the merch
/// @return -4 on succesful addition, an error code or the available stock if the added amount is larger than the stock
int cart_add_to_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id, char *merch, size_t amount);

/// @brief Removes an amount of a merch item from a shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
/// @param id The id of the shopping cart
/// @param merch The name of the merch
/// @param amount The amount of the merch
/// @return -4 on succesful removal, 0 if the merch exists but isn't in the cart,
/// an error code or the current amount in the cart if the removing amount is larger
int cart_remove_from_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id, char *merch, size_t amount);

/// @brief Calculates the cost of a shopping cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
/// @param id The id of the shopping cart
/// @return The price of the shopping cart or an error code
int cart_calculate_cost(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id);

/// @brief Checks out a shopping cart removing its items from storage and then removing the cart
/// @param carts The hash table containing all shopping carts
/// @param merch_ht The hash table containing all merch items
/// @param storage_ht The hash table containing the storage locations for all merch items
/// @param id The id of the shopping cart
/// @return True on succesful checkout, otherwise false
bool cart_checkout(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, size_t id);