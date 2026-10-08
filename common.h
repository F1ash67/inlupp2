#pragma once

#include <stdbool.h>

/**
* @file common.h
* @author Otis Hildebrand, Simon Bexander
* @date 2026-09-28
* @brief Generic element type used by the data structures.
*
* The union allows an element to contain different types of data. The pointer member 
* can be used to store arbitrary data through a void pointer. 
*/

typedef union elem elem_t;

union elem
{
  int i;
  unsigned int u;
  bool b;
  float f;
  void *p;
  char *s;
};

typedef bool ioopm_eq_function(elem_t a, elem_t b);
typedef size_t ioopm_hash_function(elem_t key);

#define int_elem(x)   ((elem_t) { .i = (x) })
#define bool_elem(x)  ((elem_t) { .b = (x) })
#define ptr_elem(x)   ((elem_t) { .p = (x) })
#define string_elem(x) ((elem_t) { .s = (x) })