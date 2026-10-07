#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "hash_table.h"
#include "linked_list.h"
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


void add_merch_item(ioopm_hash_table_t *ht, merch_t *merch)
{
  elem_t elem = ptr_elem(merch);
  ioopm_hash_table_insert(ht, merch->name, elem);
}

merch_t *create_merch_item(char *name)
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = name;
  merch->locs = ioopm_list_create();
  return merch;
}

void add_merch_loc(merch_t *merch, shelf_t *shelf)
{
  if(check_loc_empty(shelf->location)) ioopm_list_append(merch->locs, ptr_elem(shelf)); //DODGE
  
}
