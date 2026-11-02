#include "actions.h"
#include "shopping_cart.h"
#include "hash_table_general_key.h"
#include "backend.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == NULL) {
            perror(argv[1]);
            return 1;
        }
    }

    ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
    ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);
    ioopm_hash_table_t *cart_ht = ioopm_hash_table_create(int_hash, int_eq);
    cart_count_t *count = initiate_cart_count();

    while(event_loop(merch_ht, storage_ht, cart_ht, count) != -1);

    destroy_carts(cart_ht, merch_ht);
    destroy_all_merch(merch_ht, storage_ht);
    destroy_cart_count(count);
    return 0;
}