#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "hash_table.h"
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "backend.h"
#include "common.h"


struct merch{
  char* name;
  ioopm_list_t *locs;
};

typedef struct shelf shelf_t;

struct shelf{
  char *location;
  int quantity;
};

static bool check_loc_empty(ioopm_hash_table_t *ht, char *location)
{
  return !ioopm_hash_table_has_key(ht, location);
}

merch_t *create_merch_item(char *name)
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = name;
  merch->locs = ioopm_list_create();
  return merch;
}

void destroy_merch_item(merch_t *merch)
{
  ioopm_list_destroy(merch->locs);
  free(merch);
}

void add_merch_item(ioopm_hash_table_t *merch_ht, merch_t *merch)
{
  elem_t elem = ptr_elem(merch);
  ioopm_hash_table_insert(merch_ht, merch->name, elem);
}

bool add_merch_loc(ioopm_hash_table_t *storage_ht, merch_t *merch, shelf_t *shelf)
{
  if(check_loc_empty(storage_ht, shelf->location) && is_storage_nmr(shelf->location)) //DODGE 
  {
    ioopm_list_append(merch->locs, ptr_elem(shelf));
    return true;
  }

  return false;
}
