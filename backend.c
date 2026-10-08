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

static bool check_loc_empty(ioopm_hash_table_t *storage_ht, char *location)
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

merch_t *create_merch_item(char *name, char *desc, size_t price)
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = name;
  merch->desc = desc;
  merch->price = price;
  merch->locs = ioopm_list_create();
  return merch;
}

void destroy_merch_item(ioopm_hash_table_t *storage_ht, merch_t *merch)
{ 
  ioopm_list_t *list = get_stock(merch);
  elem_t result;
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  while (!ioopm_list_iterator_at_end(it))
  {
    shelf_t *shelf = ioopm_list_iterator_current(it).p;
    ioopm_hash_table_remove(storage_ht, shelf->location, &result);
    destroy_shelf(shelf);
    ioopm_list_iterator_advance(it);
  }
  ioopm_list_iterator_destroy(it);
        
  ioopm_list_destroy(list);
  free(merch);
}

void edit_merch_item(merch_t *merch, char *new_name, char*new_desc, size_t new_price)
{
  merch->name = new_name;
  merch->desc = new_desc;
  merch->price = new_price;
}

ioopm_list_t *get_stock(merch_t *merch)
{
  return merch->locs;
}

size_t get_tot_quantity(merch_t *merch)
{
  ioopm_list_t *list = get_stock(merch);
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  size_t tot = 0;
  while (!ioopm_hash_table_iterator_at_end(it))
  {
    shelf_t *current_shelf = ioopm_hash_table_iterator_current_value(it).p;
    size_t quantity = current_shelf->quantity;
    tot+=quantity;
    ioopm_hash_table_iterator_advance(it);
  }
  return tot;
}

void add_merch_item(ioopm_hash_table_t *merch_ht, merch_t *merch)
{
  elem_t elem = ptr_elem(merch);
  ioopm_hash_table_insert(merch_ht, merch->name, elem);
}

bool add_merch_loc(ioopm_hash_table_t *storage_ht, merch_t *merch, shelf_t *shelf)
{
  if(check_loc_empty(storage_ht, shelf->location)) 
  {
    ioopm_hash_table_insert(storage_ht, shelf->location, string_elem(merch->name));
    ioopm_list_append(merch->locs, ptr_elem(shelf));
    return true;
  }

  return false;
}

shelf_t *create_shelf(char *location, int quantity)
{
  assert(is_storage_nmr(location));
  shelf_t *shelf = calloc(1, sizeof(shelf_t));
  shelf->location = location;
  shelf->quantity = quantity;
  return shelf;
}

void destroy_shelf(shelf_t *shelf)
{
  free(shelf);
}

void destroy_all_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht)
{
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(merch_ht);

    while (!ioopm_hash_table_iterator_at_end(it))
    {
        merch_t *merch = ioopm_hash_table_iterator_current_value(it).p;
        destroy_merch_item(storage_ht, merch);
        
        ioopm_hash_table_iterator_advance(it);
    }

    ioopm_hash_table_iterator_destroy(it);
}