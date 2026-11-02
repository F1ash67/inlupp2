#pragma once

#include <stdbool.h>
#include "../../lib/hash_table_general_key.h"
#include "../../lib/linked_list.h"

/**
 * @file backend.h
 * @brief code for handling the backend of the webstore, mainly the storing
 * and buying of merch items
 * 
 * A merch item is stored using a linked list of shelves represented by a 
 * location code and the quantity of merch currently stored. The shelves are
 * stored in both the merch items themselves but also in a seperate hash table
 * to be able to quickly check if a location is occupied. The merch is then
 * also stored in a different hash table.
 */


/// @brief A struct to store a merch item, stores details like name, description,
/// price, a list of storage locations saved as shelves as well as the amount
/// of currently reserved items
typedef struct merch merch_t;

/// @brief A struct to store a shelf, the shelf stores the location in the 
/// warehouse, given as a code with one letter and two numbers, as well as the 
/// amount of items
typedef struct shelf shelf_t;

/// @brief Get the location of a given shelf
/// @param shelf The shelf to find the location of
/// @return The location as a string
char *backend_get_location(shelf_t *shelf);

/// @brief Get the quantity of items of a given shelf
/// @param shelf The shelf to get the quantity of
/// @return The quantity of items
size_t backend_get_quantity(shelf_t *shelf);

/// @brief Get the price of a merch item
/// @param merch The merch item
/// @return The price in SEK
size_t backend_get_price(merch_t *merch);

/// @brief Get the amount of reserved items of a given merch item
/// @param merch The merch item
/// @return The amount of reserved items
size_t backend_get_reserved(merch_t *merch);

/// @brief Reserve a given amount of a given merch to avoid multiple people 
/// buying from the same stock
/// @param merch The merch to reserve from, this merch item will be changed
/// @param amount The amount to be reserved
void backend_reserve(merch_t *merch, size_t amount);

/// @brief Unreserve a given amount of a given merch item that wasn't sold
/// @param merch The merch item to unreserve from, this merch item will be changed
/// @param amount The amount to unreserve
void backend_unreserve(merch_t *merch, size_t amount);

/// @brief Get the name of a merch item
/// @param merch The merch item
/// @return The name of the merch item
char *backend_get_name(merch_t *merch);

/// @brief Get a merch item based on the name of the item
/// @param merch_ht The hash table of merch items
/// @param name The name of the merch item
/// @return The merch item
/// @pre The merch hash table must contain an item with the specified name.
merch_t *backend_find_merch_item(ioopm_hash_table_t *merch_ht, char *name);

/// @brief Creates a new merch item and adds it to the merch hash table
/// @param merch_ht The hash table to store the merch item in
/// @param name The name of the merch item
/// @param desc The description of the merch item
/// @param price The price of the merch item in SEK
/// @return The merch item
merch_t *backend_create_merch_item(ioopm_hash_table_t *merch_ht, char *name, char *desc, size_t price);

/// @brief Removes a merch item from the database
/// @param storage_ht The hash table to remove the storage locations from
/// @param merch_ht The hash table to remove the merch item from
/// @param name The name of the merch item
/// @pre The merch hash table must contain an item with the specified name.
void backend_remove_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name);

/// @brief Edits an existing merch item and updates both hash tables
/// @param merch_ht The hash table that stores the merch items
/// @param storage_ht The hash table that stores the locations
/// @param name The current name of the merch item
/// @param new_name The new name of the merch item
/// @param new_desc The new description of the merch item
/// @param new_price The new price of the merch item
/// @pre The merch hash table must contain an item with the specified name.
void backend_edit_merch_item(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, char *name, char *new_name, char *new_desc, size_t new_price);

/// @brief Checks if merch item exists in the hash table
/// @param merch_ht The hash table that stores the merch items
/// @param name The name of the merch item
/// @return True if the merch item exists, else false
bool backend_merch_item_exists(ioopm_hash_table_t *merch_ht, char *name);

/// @brief Creates a list where every element is a merch item
/// @param merch_ht The hash table where the merch items are stored
/// @return A list of all elements in the merch hash table, this list must be destroyed by the user
ioopm_list_t *backend_list_merch(ioopm_hash_table_t *merch_ht);

/// @brief Adds one stock of a specific merch item
/// @param storage_ht The hash table where the storage locations are stored
/// @param merch_ht The hash table where merch items are stored
/// @param name The name of the merch item
/// @param location The location to store the new stock in, can either be a new 
/// location or a location already used by the same merch item
/// @return True if item was succesfully replensihed, false if location is already taken or does not exist
/// @pre The merch hash table must contain an item with the specified name.
bool backend_replenish_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name, char *location);

/// @brief Removes a given amount of stock from a given merch item, can be removed from any storage location
/// @param merch_ht The hash table where the merch is stored
/// @param storage_ht The hash table where the storage locations are stored
/// @param name The name of the merch item
/// @param amt The amount of stock to be removed
/// @return True if removing stock succeded, else false
/// @pre The total stock of the merchandise item must be at least amt.
bool backend_remove_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, char *name, size_t amt);

/// @brief Get the list of shelves of a given merch item
/// @param merch_ht The hash table where merch items are stored
/// @param name The name of the merch item
/// @return The list of locations the merch item is stored in, this list is 
/// automatically destroyed when the merch item is destroyed and should not be destroyed by the user
ioopm_list_t *backend_get_stock_list(ioopm_hash_table_t *merch_ht, char *name);

/// @brief Get the total amount of stock of a given merch item
/// @param merch_ht The hash table where merch items are stored
/// @param name The name of the merch item
/// @return The total stock of the merch item
/// @pre The merch hash table must contain an item with the specified name.
size_t backend_get_tot_stock(ioopm_hash_table_t *merch_ht, char* name);

/// @brief Removes all remaining merch from the database and frees their memory
/// @param merch_ht The hash table where merch items are stored
/// @param storage_ht The hash table where storage locations are stored
void backend_destroy_all_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht);