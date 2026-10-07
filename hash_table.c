#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "hash_table.h"
#include "hash_table_iterator.h"
#include "common.h"

#define No_Buckets 17

typedef struct entry entry_t;

struct entry
{
  char *key;    
  elem_t value;     
  entry_t *next; 
};

struct hash_table
{
  // Each bucket contains a sentinel entry.
  // Actual entries are linked after the sentinel through bucket.next.
  entry_t buckets[No_Buckets];
};

struct hash_table_iterator
{
  ioopm_hash_table_t *ht;
  size_t current_bucket;
  entry_t *current_entry; 
};

static size_t string_knr_hash(const char *str)
{
  size_t result = 0;
  while (*str != '\0')
  {
    result = result * 31 + ((unsigned char)*str);
    str++;
  }
  return result;
}

// The hash table stores the key pointer and copies the elem_t value,
// but does not take ownership of the memory they reference.
static entry_t *entry_create(char *key, elem_t value, entry_t *next)
{
  entry_t *entry = malloc(sizeof(entry_t));
  entry->key = key;
  entry->value = value;
  entry->next = next;
  return entry;
}

static void entry_destroy(entry_t *entry)
{
  free(entry);
}

static size_t bucket_size(entry_t *entry)
{
  entry_t *current = entry;
  size_t count = 0;
  while (current != NULL)
  {
    count++;
    current = current->next;
  }
  return count;
}

static void advance_iterator_state(ioopm_hash_table_iterator_t *it)
{
  // advance to the next entry in the bucket
  it->current_entry = it->current_entry->next;

  // if it was null, meaning no more entries are in the bucket, advance to the next bucket
  if (it->current_entry == NULL)
  {
    it->current_bucket += 1;

    // Don't access the bucket array after the last bucket.
    if (it->current_bucket != No_Buckets)
    {
      it->current_entry = &it->ht->buckets[it->current_bucket];
    }
  }
}

// Skip empty buckets and position the iterator on the next real entry.
static void skip_sentinel_nodes(ioopm_hash_table_iterator_t *it)
{
  while (it->current_bucket != No_Buckets &&
         it->current_entry == &it->ht->buckets[it->current_bucket])
  {
    advance_iterator_state(it);
  }
}


// The first entry in each bucket is a sentinel and is not a real key-value entry.
// Real entries are stored in the sentinel's next pointer.
entry_t *find_previous_entry(ioopm_hash_table_t *ht, const char *key)
{
  assert(ht != NULL && key != NULL);
  size_t bucket = string_knr_hash(key) % No_Buckets;

  // Return the entry immediately before the matching entry.
  // If the key does not exist, return the last entry in the bucket.
  entry_t *previous = &ht->buckets[bucket];
  entry_t *current = ht->buckets[bucket].next;
  while (current != NULL && strcmp(current->key, key) != 0)
  {
    previous = current;
    current = current->next;
  }

  return previous;
}

ioopm_hash_table_t *ioopm_hash_table_create(void)
{
  return calloc(1, sizeof(ioopm_hash_table_t));
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
  assert(ht != NULL);
  for (size_t i = 0; i < No_Buckets; i++)
  {
    entry_t *current = ht->buckets[i].next;
    while (current != NULL)
    {
      entry_t *tmp = current->next;
      entry_destroy(current);
      current = tmp;
    }
  }
  free(ht);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, char *key, elem_t value)
{
  assert(ht != NULL && key != NULL);

  entry_t *previous = find_previous_entry(ht, key);

  if (previous->next != NULL)
  {
    previous->next->value = value;
  }
  else
  {
    previous->next = entry_create(key, value, NULL);
  }
}

bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, const char *key, elem_t *result)
{
  assert(ht != NULL && key != NULL);

  // find_previous_entry() returns the entry before the matching key,
  // so its next pointer is the matching entry if one exists.
  entry_t *current = find_previous_entry(ht, key)->next;

  // if the key exists, return the value, otherwise, indicate that the lookup failed
  if (current != NULL)
  {
    *result = current->value;
    return true;
  }
  else
  {
    return false;
  }
}

bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, const char *key, elem_t *result)
{
  assert(ht != NULL && key != NULL);

  // Find the entry before the one to remove so the linked list can be relinked.
  entry_t *previous = find_previous_entry(ht, key);
  entry_t *current = previous->next;

  if (current != NULL)
  {
    *result = current->value;
    previous->next = current->next;
    entry_destroy(current);
    return true;
  }
  else
  {
    return false;
  }
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, const char *key)
{
  assert(ht != NULL && key != NULL);
  elem_t result;
  return ioopm_hash_table_lookup(ht, key, &result);
}

size_t ioopm_hash_table_size(const ioopm_hash_table_t *ht)
{
  assert(ht != NULL);
  size_t size = 0;
  for (size_t i = 0; i < No_Buckets; i++)
  {
    size += bucket_size(ht->buckets[i].next);
  }
  return size;
}

bool ioopm_hash_table_is_empty(const ioopm_hash_table_t *ht)
{
  assert(ht != NULL);
  return ioopm_hash_table_size(ht) == 0;
}

ioopm_hash_table_iterator_t *ioopm_hash_table_iterator_create(ioopm_hash_table_t *ht)
{
  assert(ht != NULL);
  ioopm_hash_table_iterator_t *it = calloc(1, sizeof(ioopm_hash_table_iterator_t));
  it->ht = ht;
  it->current_bucket = 0;
  it->current_entry = &ht->buckets[0];
  skip_sentinel_nodes(it);
  return it;
}

void ioopm_hash_table_iterator_destroy(ioopm_hash_table_iterator_t *it)
{
  assert(it != NULL);
  free(it);
}

bool ioopm_hash_table_iterator_at_end(ioopm_hash_table_iterator_t *it)
{
  assert(it != NULL);
  return it->current_bucket == No_Buckets;
}

void ioopm_hash_table_iterator_advance(ioopm_hash_table_iterator_t *it)
{
  assert(it != NULL);
  if (it->current_entry->next != NULL)
    it->current_entry = it->current_entry->next;
  else
  {
    it->current_bucket++;
    it->current_entry = &it->ht->buckets[it->current_bucket];
    skip_sentinel_nodes(it);
  }
}

char *ioopm_hash_table_iterator_current_key(ioopm_hash_table_iterator_t *it)
{
  assert(it != NULL);
  return it->current_entry->key;
}

elem_t ioopm_hash_table_iterator_current_value(ioopm_hash_table_iterator_t *it)
{
  assert(it != NULL);
  return it->current_entry->value;
}