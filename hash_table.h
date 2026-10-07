#pragma once
#include <stdbool.h>
#include "common.h"
/**
* @file hash_table.h
* @author Otis Hildebrand, Simon Bexander
* @date 2026-09-28
* @brief Simple hash table that maps string keys to generic values.
*
* The hash table uses buckets which include the entries into the hash table,
* at the moment the amount of buckets is hardcoded to 17, each entry includes the (key, value) pair
* and a pointer to the next entry in the bucket(or NULL if no more entries exist).
*/

typedef struct hash_table ioopm_hash_table_t;

/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(void);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, char *key,  elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result a pointer to the place to store the result
/// @return true if the lookup succeeded, and if it succeeded it also writes the resulting 
/// value to the memory location result points to.
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, const char *key, elem_t *result);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @param result a pointer to the place to store the removed value
/// @return true if the removal succeeded, and if it succeeded it also writes the removed 
/// value to the memory location result points to.
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, const char *key, elem_t *result);

/// @brief check if a hash table has a certain key
/// @param ht hash table that will be checked
/// @param key the key to look for (cannot be NULL)
/// @return true if the key exists, otherwise false
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, const char *key);

/// @brief check if a hash table is empty
/// @param ht the hash table to check
/// @return true if the hash table is empty, otherwise false
bool ioopm_hash_table_is_empty(const ioopm_hash_table_t *ht);

/// @brief check the size of a hash table
/// @param ht the hash table to check
/// @return the size of the hash table as a size_t
size_t ioopm_hash_table_size(const ioopm_hash_table_t *ht);