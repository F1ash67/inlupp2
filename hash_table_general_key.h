#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "common.h"

/**
 * @file hash_table.h
 * @author Edvin Fältenhag
 * @date 14/09/26
 * @brief Simple hash table that maps string keys to integer values.
 * 
 */


typedef struct hash_table ioopm_hash_table_t;

/// @brief Create a new hash table
/// @param hash_fn the function which generates the hash from a key
/// @param key_eq_fn the function which compares keys to find the right one
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn, ioopm_eq_function *key_eq_fn);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result the value with that key
/// @return true or false depending on if the key exists
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @param result the value that is removed
/// @return true or false depending on if the key exists
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @brief check if a hash table has a specific key
/// @param ht hash table operated upon
/// @param key key to check
/// @return true or false depending on if the key exists
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief check if a hash table is empty
/// @param ht hash table operated upon
/// @return true or false depending on if the hash table is empty
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief get the size of the hash table
/// @param ht hash table operated upon
/// @return the size of the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief reads a file and prints the frequency for each word
/// @pre file_src is a valid file path
/// @param file_src the file to be read
void ioopm_read_file(char *file_src, ioopm_hash_table_t *ht);

void ioopm_sort_by_frequency(ioopm_hash_table_t *ht);

size_t str_hash(elem_t key);

bool str_eq(elem_t a, elem_t b);