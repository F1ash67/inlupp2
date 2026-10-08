#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "backend.h"
#include "hash_table.h"
#include "hash_table_iterator.h"
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "common.h"

struct merch{
  char* name;
  char* desc;
  size_t price;
  ioopm_list_t *locs;
};

struct shelf{
  char *location;
  size_t quantity;
};

static bool location_empty(ioopm_hash_table_t *storage_ht, char *location)
{
  return !ioopm_hash_table_has_key(storage_ht, location);
}

static bool is_storage_nmr(char *str)
{
  int len = strlen(str);
  int digits = 1;
  if(tolower(str[0]) < 'a' || tolower(str[0]) > 'z' || len != 3) return false;
  for(int i = 1; i < len; i++){
    if(isdigit(str[i])){
      digits++;
    }
  }
  if(digits == len){
    return true;
  }
  else{
    return false;
  }
}


static shelf_t *create_shelf(char *location, int quantity)
{
  assert(is_storage_nmr(location));
  shelf_t *shelf = calloc(1, sizeof(shelf_t));
  shelf->location = location;
  shelf->quantity = quantity;
  return shelf;
}

static merch_t *find_merch_item(ioopm_hash_table_t *merch_ht, char *name)
{
  elem_t result;
  
  assert(ioopm_hash_table_lookup(merch_ht, name, &result));
  merch_t *merch = result.p;
  return merch;
}

static void destroy_shelf(shelf_t *shelf)
{
  free(shelf);
}

merch_t *create_merch_item(ioopm_hash_table_t *merch_ht, char *name, char *desc, size_t price)
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = name;
  merch->desc = desc;
  merch->price = price;
  merch->locs = ioopm_list_create();
  
  ioopm_hash_table_insert(merch_ht, merch->name, ptr_elem(merch));
  return merch;
}

void remove_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name)
{ 
  merch_t *merch = find_merch_item(merch_ht, name);
  ioopm_list_t *list = get_stock(merch_ht, name);
  elem_t result;
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  while (!ioopm_list_iterator_at_end(it))
  {
    shelf_t *shelf = ioopm_list_iterator_remove(it).p;
    ioopm_hash_table_remove(storage_ht, shelf->location, &result);
    destroy_shelf(shelf);
  }
  ioopm_list_iterator_destroy(it);
  
  ioopm_list_destroy(list);
  ioopm_hash_table_remove(merch_ht, name, &result);
  free(merch);
}

void edit_merch_item(ioopm_hash_table_t *merch_ht, char *name, char *new_name, char*new_desc, size_t new_price)
{
  merch_t *merch = find_merch_item(merch_ht, name);
  merch->name = new_name;
  merch->desc = new_desc;
  merch->price = new_price;
}

bool replenish_merch_item(ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *merch_ht, char *name, char *location)
{
  ioopm_list_t *stock_list = get_stock(merch_ht, name);
  ioopm_list_iterator_t *stock_list_iter = ioopm_list_iterator_create(stock_list);
  while (!ioopm_list_iterator_at_end(stock_list_iter))
  {
    shelf_t *current = ioopm_list_iterator_current(stock_list_iter).p;
    if(strcmp(current->location, location) == 0)
    {
      current->quantity++;
      ioopm_list_iterator_destroy(stock_list_iter);
      return true;
    }
    ioopm_list_iterator_advance(stock_list_iter);
  }

  ioopm_list_iterator_destroy(stock_list_iter);

  if(!location_empty(storage_ht, location)) return false;

  shelf_t *new_shelf = create_shelf(location, 1);

  ioopm_hash_table_insert(storage_ht, location, string_elem(name));
  ioopm_list_append(stock_list, ptr_elem(new_shelf));
  return true;
}

ioopm_list_t *get_stock(ioopm_hash_table_t *merch_ht, char *name)
{
  merch_t *merch = find_merch_item(merch_ht, name);
  return merch->locs;
}

size_t get_tot_quantity(ioopm_hash_table_t *merch_ht, char* name)
{
  ioopm_list_t *list = get_stock(merch_ht, name);
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  size_t tot = 0;
  while (!ioopm_list_iterator_at_end(it))
  {
    shelf_t *current_shelf = ioopm_list_iterator_current(it).p;
    size_t quantity = current_shelf->quantity;
    tot+=quantity;
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
}