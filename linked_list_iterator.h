#pragma once
#include <stdbool.h>
#include "linked_list.h"
#include "common.h"

/**
* @file linked_list_iterator.h
* @author Otis Hildebrand, Simon Bexander
* @date 2026-09-28
* @brief Simple linked list iterator
*
* Linked list iterators provide an interface to iterate through all entries in a linked list.
* An iterator is either positioned at a link, called the current link, or it is positioned at-the-end, if it has already iterated through all links.
* it also saves the index of the current link. If the underlying linked list of an iterator is 
* modified using any non-iterator function, the iterator is invalidated and should not be used anymore.
*
*/


typedef struct list_iterator ioopm_list_iterator_t;

/// @brief Create a new iterator
/// @param l the list to iterate over
ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *l);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter);

/// @brief Check whether the iterator has reached the end.
/// @param iter the iterator
/// @return true if the iterator is at the end, otherwise false
bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter the iterator
void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element
elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element
elem_t ioopm_list_iterator_remove(ioopm_list_iterator_t *iter);

/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_list_iterator_insert(ioopm_list_iterator_t *iter, elem_t element);