#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "backend.h"
#include "hash_table_general_key.h"
#include "hash_table_iterator.h"
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "common.h"

struct merch
{
  char *name;
  char *desc;
  size_t price;
  ioopm_list_t *locs;
  size_t reserved;
};

struct shelf
{
  char *location;
  size_t quantity;
};

static bool location_empty(ioopm_hash_table_t *storage_ht, char *location)
{
  return !ioopm_hash_table_has_key(storage_ht, string_elem(location));
}

static bool is_storage_nmr(char *str)
{
  int len = strlen(str);
  int digits = 1;
  if (tolower(str[0]) < 'a' || tolower(str[0]) > 'z' || len != 3)
    return false;
  for (int i = 1; i < len; i++)
  {
    if (isdigit(str[i]))
    {
      digits++;
    }
  }
  if (digits == len)
  {
    return true;
  }
  else
  {
    return false;
  }
}

static shelf_t *create_shelf(char *location, int quantity)
{
  shelf_t *shelf = calloc(1, sizeof(shelf_t));
  shelf->location = location;
  shelf->quantity = quantity;
  return shelf;
}

char *get_location(shelf_t *shelf) {
  return shelf->location;
}

size_t get_quantity(shelf_t *shelf) {
  return shelf->quantity;
}

size_t get_price(merch_t *merch)
{
  return merch->price;
}

size_t get_reserved(merch_t *merch) {
  return merch->reserved;
}

void reserve(merch_t *merch, size_t amount) {
  size_t current = get_reserved(merch);
  merch->reserved = current + amount;
}

void unreserve(merch_t *merch, size_t amount) {
  size_t current = get_reserved(merch);
  merch->reserved = current - amount;
}

char *get_name(merch_t *merch)
{
  return merch->name;
}

merch_t *find_merch_item(ioopm_hash_table_t *merch_ht, char *name)
{
  elem_t result;

  if(!ioopm_hash_table_lookup(merch_ht, string_elem(name), &result)) return NULL;
  merch_t *merch = result.p;
  return merch;
}

static void destroy_shelf(shelf_t *shelf)
{
  free(shelf->location);
  free(shelf);
}

merch_t *create_merch_item(ioopm_hash_table_t *merch_ht, char *name, char *desc, size_t price)
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = strdup(name);
  merch->desc = strdup(desc);
  merch->price = price;
  merch->locs = ioopm_list_create();

  ioopm_hash_table_insert(merch_ht, string_elem(merch->name), ptr_elem(merch));
  return merch;
}

void remove_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name)
{
  merch_t *merch = find_merch_item(merch_ht, name);
  ioopm_list_t *list = get_stock_list(merch_ht, name);
  elem_t result;
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  while (!ioopm_list_iterator_at_end(it))
  {
    shelf_t *shelf = ioopm_list_iterator_remove(it).p;
    ioopm_hash_table_remove(storage_ht, string_elem(shelf->location), &result);
    destroy_shelf(shelf);
  }
  ioopm_list_iterator_destroy(it);

  ioopm_list_destroy(list);
  ioopm_hash_table_remove(merch_ht, string_elem(name), &result);
  free(merch->name);
  free(merch->desc);
  free(merch);
}

void edit_merch_item(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, char *name, char *new_name, char *new_desc, size_t new_price)
{
  merch_t *merch = find_merch_item(merch_ht, name);
  ioopm_list_t *stock_list = get_stock_list(merch_ht, name);

  elem_t result;
  assert(ioopm_hash_table_remove(merch_ht, string_elem(name), &result));
  free(merch->name);
  free(merch->desc);
  merch->name = strdup(new_name);
  merch->desc = strdup(new_desc);
  merch->price = new_price;

  ioopm_hash_table_insert(merch_ht, string_elem(merch->name), ptr_elem(merch));

  ioopm_list_iterator_t *stock_list_iter = ioopm_list_iterator_create(stock_list);
  while (!ioopm_list_iterator_at_end(stock_list_iter))
  {
    shelf_t *current = ioopm_list_iterator_current(stock_list_iter).p;
    ioopm_hash_table_insert(storage_ht, string_elem(current->location), string_elem(merch->name));
    ioopm_list_iterator_advance(stock_list_iter);
  }
  ioopm_list_iterator_destroy(stock_list_iter);
}

bool merch_item_exists(ioopm_hash_table_t* merch_ht, char *name)
{
  return(ioopm_hash_table_has_key(merch_ht, string_elem(name)));
}

ioopm_list_t *list_merch(ioopm_hash_table_t *merch_ht)
{
  ioopm_hash_table_iterator_t *merch_it = ioopm_hash_table_iterator_create(merch_ht);
  ioopm_list_t *merch_list = ioopm_list_create();
  while (!ioopm_hash_table_iterator_at_end(merch_it))
  {
    ioopm_list_append(merch_list, ioopm_hash_table_iterator_current_value(merch_it));
    ioopm_hash_table_iterator_advance(merch_it);
  }
  ioopm_hash_table_iterator_destroy(merch_it);
  return merch_list;
}

bool replenish_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name, char *location)
{
  ioopm_list_t *stock_list = get_stock_list(merch_ht, name);
  ioopm_list_iterator_t *stock_list_iter = ioopm_list_iterator_create(stock_list);
  while (!ioopm_list_iterator_at_end(stock_list_iter))
  {
    shelf_t *current = ioopm_list_iterator_current(stock_list_iter).p;
    if (strcmp(current->location, location) == 0)
    {
      current->quantity++;
      ioopm_list_iterator_destroy(stock_list_iter);
      return true;
    }
    ioopm_list_iterator_advance(stock_list_iter);
  }

  ioopm_list_iterator_destroy(stock_list_iter);

  if (!location_empty(storage_ht, location) || !is_storage_nmr(location))
    return false;

  shelf_t *new_shelf = create_shelf(strdup(location), 1);

  ioopm_hash_table_insert(storage_ht, string_elem(new_shelf->location), string_elem(name));
  ioopm_list_append(stock_list, ptr_elem(new_shelf));
  return true;
}

bool remove_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, char *name, size_t amt)
{
  if(amt > get_tot_stock(merch_ht, name)) return false;
  ioopm_list_t *stock_list = get_stock_list(merch_ht, name);
  ioopm_list_iterator_t *stock_list_iter = ioopm_list_iterator_create(stock_list);
  for (size_t i = 0; i < amt; i++)
  {
    shelf_t *current = ioopm_list_iterator_current(stock_list_iter).p;
    current->quantity--;
    if (current->quantity <= 0)
    {
      char *location = current->location;
      ioopm_list_iterator_remove(stock_list_iter);

      elem_t result;
      ioopm_hash_table_remove(storage_ht, string_elem(location), &result);
      destroy_shelf(current);
    }
  }
  ioopm_list_iterator_destroy(stock_list_iter);
  return true;
}

ioopm_list_t *get_stock_list(ioopm_hash_table_t *merch_ht, char *name)
{
  merch_t *merch = find_merch_item(merch_ht, name);
  return merch->locs;
}

size_t get_tot_stock(ioopm_hash_table_t *merch_ht, char *name)
{
  ioopm_list_t *list = get_stock_list(merch_ht, name);
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  size_t tot = 0;
  while (!ioopm_list_iterator_at_end(it))
  {
    shelf_t *current_shelf = ioopm_list_iterator_current(it).p;
    size_t quantity = current_shelf->quantity;
    tot += quantity;
    ioopm_list_iterator_advance(it);
  }
  ioopm_list_iterator_destroy(it);
  return tot;
}

void destroy_all_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht)
{
  while (!ioopm_hash_table_is_empty(merch_ht))
  {
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(merch_ht);

    merch_t *merch = ioopm_hash_table_iterator_current_value(it).p;

    char *name = merch->name;

    ioopm_hash_table_iterator_destroy(it);

    remove_merch_item(storage_ht, merch_ht, name);
  }
  ioopm_hash_table_destroy(merch_ht);
  ioopm_hash_table_destroy(storage_ht);
}