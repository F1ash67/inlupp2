#include "hash_table_general_key.h"
#include "hash_table_iterator_general_key.h"
#include "shopping_cart.h"
#include "backend.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct cart_count{
    size_t id;
};

cart_count_t *initiate_cart_count() {
    cart_count_t *count = malloc(sizeof(cart_count_t));
    count->id = 0;
    return count;
}

void destroy_cart_count(cart_count_t *count) {
    free(count);
}

static ioopm_hash_table_t *get_cart(ioopm_hash_table_t *carts, size_t id) {
    elem_t sc;
    if(ioopm_hash_table_lookup(carts, int_elem(id), &sc)) {
        return sc.p;
    }
    return NULL;

}

size_t create_cart(ioopm_hash_table_t *carts, cart_count_t *count) {
    ioopm_hash_table_t *sc = ioopm_hash_table_create(str_hash, str_eq);
    
    ioopm_hash_table_insert(carts, int_elem(count->id), ptr_elem(sc));
    size_t id = count->id;
    count->id = id + 1;
    return id;
}

void destroy_carts(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht) {
    while(!ioopm_hash_table_is_empty(carts)) {
        ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(carts);
        size_t id = ioopm_hash_table_iterator_current_key(it).i;
        ioopm_hash_table_iterator_destroy(it);
        remove_cart(carts, merch_ht, id);
    }
    ioopm_hash_table_destroy(carts);
}

bool remove_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id) {
    ioopm_hash_table_t *sc = get_cart(carts, id);
    if(sc == NULL) {
        return false;
    }
    else {
        ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(sc);

        while(!ioopm_hash_table_iterator_at_end(it)) {
            char *name = ioopm_hash_table_iterator_current_key(it).s;
            merch_t *merch = find_merch_item(merch_ht, name);
            size_t amount = ioopm_hash_table_iterator_current_value(it).i;
        
            unreserve(merch, amount);
            ioopm_hash_table_iterator_advance(it);
        }
        elem_t removed;
        ioopm_hash_table_iterator_destroy(it);
        ioopm_hash_table_remove(carts, int_elem(id), &removed);
        ioopm_hash_table_destroy(sc);
        return true;
    }
}

int add_to_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id, char *name, size_t amount) {
    ioopm_hash_table_t *sc = get_cart(carts, id);
    if(sc == NULL) {
        return -1;
    }
    merch_t *merch = find_merch_item(merch_ht, name);
    if(merch == NULL) {
        return -2;
    }
    if(amount == 0) {
        return -3;
    }

    size_t stock = get_tot_stock(merch_ht, name);
    size_t reserved = get_reserved(merch);
    size_t available = stock - reserved;
    
    if(available >= amount) {
        elem_t current = int_elem(0);
        ioopm_hash_table_lookup(sc, string_elem(get_name(merch)), &current);
        ioopm_hash_table_insert(sc, string_elem(get_name(merch)), int_elem(amount + current.i));
        reserve(merch, amount);
        return -4;
    }
    return available;
}

int remove_from_cart(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id, char *name, size_t amount) {
    ioopm_hash_table_t *sc = get_cart(carts, id);
    if(sc == NULL) {
        return -1;
    }
    merch_t *merch = find_merch_item(merch_ht, name);
    if(merch == NULL) {
        return -2;
    }
    if(amount == 0) {
        return -3;
    }

    elem_t result = int_elem(0);
    ioopm_hash_table_lookup(sc, string_elem(name), &result);
    size_t current = result.i;

    if(current >= amount) {
        if(current == amount) {
            ioopm_hash_table_remove(sc, string_elem(name), &result);
        }
        else {
            ioopm_hash_table_insert(sc, string_elem(name), int_elem(current - amount));
        }
        unreserve(merch, amount);
        return -4;
    }
    return current;
}

int calculate_cost(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, size_t id) {
    ioopm_hash_table_t *sc = get_cart(carts, id);
    if(sc == NULL) {
        return -1;
    }

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
    ioopm_hash_table_iterator_destroy(it);
    return cost;
}

bool checkout(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, size_t id) {
    ioopm_hash_table_t *sc = get_cart(carts, id);
    if(sc == NULL) {
        return false;
    }
    else {
        ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(sc);

        while(!ioopm_hash_table_iterator_at_end(it)) {
            char *name = ioopm_hash_table_iterator_current_key(it).s;
            size_t amount = ioopm_hash_table_iterator_current_value(it).i;
        
            remove_stock(merch_ht, storage_ht, name, amount);
            ioopm_hash_table_iterator_advance(it);
        }
        ioopm_hash_table_iterator_destroy(it);
        remove_cart(carts, merch_ht, id);
        return true;
    }
}