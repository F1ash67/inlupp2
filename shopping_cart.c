#include "hash_table_general_key.h"
#include "hash_table_iterator_general_key.h"
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "shopping_cart.h"
#include "backend.h"
#include <stdlib.h>

//To-Do: Implement checkout(), checks to control that carts exist etc.

size_t cart = 0;

void create_cart(ioopm_list_t *carts) {
    ioopm_hash_table_t *sc = ioopm_hash_table_create(str_hash, str_eq);
    
    ioopm_list_append(carts, ptr_elem(sc));
    cart++;
}

void remove_cart(ioopm_list_t *carts, size_t id) {
    ioopm_hash_table_t *sc = ioopm_list_remove(carts, id).p;
    ioopm_hash_table_destroy(sc);
}

void add_to_cart(ioopm_list_t *carts, size_t id, char *merch, size_t amount) {
    ioopm_hash_table_t *sc = ioopm_list_get(carts, id).p;

    ioopm_hash_table_insert(sc, string_elem(merch), int_elem(amount));
}

void remove_from_cart(ioopm_list_t *carts, size_t id, char *merch, size_t amount) {
    ioopm_hash_table_t *sc = ioopm_list_get(carts, id).p;

    elem_t result;
    ioopm_hash_table_lookup(sc, string_elem(merch), &result);
    ioopm_hash_table_insert(sc, string_elem(merch), int_elem(result.i - amount));
}

int calculate_cost(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht, size_t id) {
    ioopm_hash_table_t *sc = ioopm_list_get(carts, id).p;

    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(sc);

    size_t cost = 0;

    while(!ioopm_hash_table_iterator_at_end(it)) {
        char *name = ioopm_hash_table_iterator_current_key(it).s;
        size_t amount = ioopm_hash_table_iterator_current_value(it).i;
        merch_t *merch = find_merch_item(merch_ht, name);
        size_t price = get_price(merch);

        cost = cost + price * amount;
        ioopm_hash_table_iterator_advance(it);
    }
    return cost;
}

void checkout(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, size_t id) {
    ioopm_hash_table_t *sc = ioopm_list_get(carts, id).p;
    
    ioopm_hash_table_iterator_t *it = ioopm_list_iterator_create(sc);

    while(!ioopm_hash_table_iterator_at_end) {
        char *name = ioopm_hash_table_iterator_current_key(it).s;
        size_t amount = ioopm_hash_table_iterator_current_value(it).i;
        merch_t *merch = find_merch_item(merch_ht, name);
        
        //Finish with edit stock function
    }
}

