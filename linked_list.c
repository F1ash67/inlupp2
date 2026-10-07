#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "common.h"

typedef struct link link_t;

struct list
{
  size_t size;
  link_t *first;
  link_t *last;
};

struct link
{
  elem_t value;
  link_t *next;
};

struct list_iterator
{
  size_t index;
  ioopm_list_t *list;
  link_t *current_link;
};

ioopm_list_t *ioopm_list_create(void)
{
  ioopm_list_t *l = calloc(1, sizeof(ioopm_list_t));
  return l;
}
 
void ioopm_list_destroy(ioopm_list_t *list)
{
  assert(list != NULL);
  link_t *current = list->first;
  while (current != NULL)
  {
    link_t *next = current->next;
    free(current);
    current = next;
  }
  free(list);
}

void ioopm_list_append(ioopm_list_t *list, elem_t value)
{
  assert(list != NULL);
  link_t *new = calloc(1, sizeof(link_t));
  new->next = NULL;
  new->value = value;
  if (list->last == NULL)
  {
    list->last = new;
    list->first = new;
  }
  else
  {
    list->last->next = new;
    list->last = new;
  }
  list->size++;
}

void ioopm_list_prepend(ioopm_list_t *list, elem_t value)
{
  assert(list != NULL);
  link_t *new = calloc(1, sizeof(link_t));
  new->value = value;
  if (list->first == NULL)
  {
    new->next = NULL;
    list->last = new;
    list->first = new;
  }
  else
  {
    new->next = list->first;
    list->first = new;
  }
  list->size++;
}

elem_t ioopm_list_head(const ioopm_list_t *list)
{
  assert(list != NULL);
  return list->first->value;
}

elem_t ioopm_list_last(const ioopm_list_t *list)
{
  assert(list != NULL);
  return list->last->value;
}

void ioopm_list_insert(ioopm_list_t *list, size_t index, elem_t value)
{ 
  assert(list != NULL && index <= list->size);
  if(index == 0)
  {
    ioopm_list_prepend(list, value);
    return;
  }
  
  if(index == list->size)
  {
    ioopm_list_append(list, value);
    return;
  }

  link_t *new = calloc(1, sizeof(link_t));
  new->value = value;
  link_t *current = list->first;
  
  for (size_t i = 0; i < index-1; i++)
  {
    current = current->next;
  }

  new->next = current->next;
  current->next = new;

  list->size++;
}

elem_t ioopm_list_remove(ioopm_list_t *list, size_t index)
{
  assert(list != NULL && index < list->size);

  link_t *current = list->first;
  link_t *prev = NULL;
  
  for (size_t i = 0; i < index; i++)
  {
    prev = current;
    current = current->next;
  }
  
  // Update the first pointer when removing the first element.
  if (current == list->first)
  {
    list->first = list->first->next;
  }
  else
  {
    prev->next = current->next;
  }

  // Update the last pointer when removing the last element.
  if (current == list->last)
  {
    list->last = prev;
  }
  
  elem_t val = current->value;
  list->size--;
  free(current);
  return val;
}

elem_t ioopm_list_get(const ioopm_list_t *list, size_t index)
{
  assert(list != NULL && index < list->size);
  link_t *current = list->first;
  for (size_t i = 0; i < index; i++)
  {
    current = current->next;
  }
  return current->value;
}

size_t ioopm_list_size(const ioopm_list_t *list)
{
  assert(list != NULL);
  return list->size;
}

bool ioopm_list_is_empty(const ioopm_list_t *list)
{
  assert(list != NULL);
  return list->size == 0;
}

ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *list)
{
  assert(list != NULL);
  ioopm_list_iterator_t *iter = calloc(1, sizeof(ioopm_list_iterator_t));
  iter->list = list;
  iter->index = 0;
  iter->current_link = list->first;
  return iter;
}

void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter)
{
  assert(iter != NULL);
  free(iter);
}

bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter)
{
  assert(iter != NULL);
  return iter->current_link == NULL;
}

void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter)
{
  assert(iter != NULL);
  iter->current_link = iter->current_link->next;
  iter->index++;
}

elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter)
{
  assert(iter != NULL);
  return iter->current_link->value;
}

elem_t ioopm_list_iterator_remove(ioopm_list_iterator_t *iter)
{
  assert(iter != NULL);
  // Move to the next link before removing the current one,
  // since the current link will no longer exist afterwards.
  iter->current_link = iter->current_link->next;
  return ioopm_list_remove(iter->list, iter->index);
}

void ioopm_list_iterator_insert(ioopm_list_iterator_t *iter, elem_t element)
{
  assert(iter != NULL);
  ioopm_list_insert(iter->list, iter->index, element);
  // Recalculate current_link because inserting an element may have changed
  // which link corresponds to the iterator's current index.
  iter->current_link = iter->list->first;
  for(size_t i = 0; i < iter->index; i++)
  {
    iter->current_link = iter->current_link->next;
  }
}