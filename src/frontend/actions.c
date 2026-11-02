#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../../lib/utils.h"
#include "actions.h"
#include "../backend/backend.h"
#include "../../lib/hash_table_general_key.h"
#include "../../lib/hash_table_iterator_general_key.h"
#include "../../lib/linked_list.h"
#include "../../lib/linked_list_iterator.h"
#include "../backend/shopping_cart.h"

static void print_menu(void)
{
    fprintf(stdout, "[A]: Add merchandise\n" 
                    "[L]: List merchandise\n"
                    "[D]: Remove merchandise\n"
                    "[E]: Edit merchandise\n"
                    "[S]: Show stock\n"
                    "[P]: Replenish\n"
                    "[C]: Create cart\n"
                    "[R]: Remove cart\n"
                    "[+]: Add to cart\n"
                    "[-]: Remove from cart\n"
                    "[=]: Calculate cost\n"
                    "[O]: Checkout\n"
                    "[Q]: Quit\n\n");
}

static bool ask_confirm(char *question)
{
    char *answer = ask_question_string(question);
    bool yes = toupper(answer[0]) == 'Y';
    free(answer);
    return yes;
}

static char ask_question_menu(void)
{
    print_menu();
    int ch = getchar();
    if(ch == EOF) return 'X';
    char c = toupper(ch);
    clear_input_buffer();
    while(c != 'A' && c != 'L' && c != 'D' && c != 'E' && c != 'S' && c != 'P' && 
          c != 'C' && c != 'R' && c != '+' && c != '-' && c != '=' && c != 'O' && c != 'Q') {
      print_menu();
      printf("Please enter a valid option: ");
      c = toupper(getchar());
      clear_input_buffer();
    }

    return c;
}



void actions_add_merch(ioopm_hash_table_t *merch_ht) {
    char *name = ask_question_string("Enter name:");
    char *desc = ask_question_string("Enter description:");
    int price = ask_question_int("Enter price:");

    if(price < 0) {
        printf("Invalid price\n");
        free(name);
        free(desc);
        return;
    }

    backend_create_merch_item(merch_ht, name, desc, price);
    free(name);
    free(desc);
}

void actions_list_merch_items(ioopm_hash_table_t *merch_ht) {
    ioopm_list_t *l = backend_list_merch(merch_ht);
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(l);

    size_t i = 0;
    while(!ioopm_list_iterator_at_end(it)) {
        printf("%s\n", backend_get_name(ioopm_list_iterator_current(it).p));
        ioopm_list_iterator_advance(it);
        i++;

        if(i % 20 == 0 && !ioopm_list_iterator_at_end(it)) {
            char *confirm  = ask_question_string("Do you want to keep listing merch? If no: (N/n)");

            bool stop = !strcmp(confirm, "N") || !strcmp(confirm, "n");
            free(confirm);
            if(stop) break;
        }
    }
    printf("\n");
    ioopm_list_iterator_destroy(it);
    ioopm_list_destroy(l);
}

void actions_remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    actions_list_merch_items(merch_ht);

    char* merch = ask_question_string("Enter name of merch to be removed:");

    if(!backend_merch_item_exists(merch_ht, merch)) {
        printf("Invalid merch\n");
        free(merch);
        return;
    }
    if(backend_get_reserved(backend_find_merch_item(merch_ht, merch)) > 0) {
        printf("Can not remove merch that is currently in a shopping cart\n");
        free(merch);
        return;
    }

    if(ask_confirm("Confirm removal (Y/y):")) {
        backend_remove_merch_item(storage_ht, merch_ht, merch);
        free(merch);
        return;
    }
    print("Aborting removal\n");
    free(merch);
    return;
}

void actions_edit_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    actions_list_merch_items(merch_ht);

    char *old_name = ask_question_string("Enter name of merch to be edited:");

    if(!backend_merch_item_exists(merch_ht, old_name)) {
        printf("Invalid merch\n");
        free(old_name);
        return;
    }
    if(backend_get_reserved(backend_find_merch_item(merch_ht, old_name)) > 0) {
        printf("Can not edit merch that is currently in a shopping cart\n");
        free(old_name);
        return;
    }

    char *new_name = ask_question_string("New name:");
    char *new_desc = ask_question_string("New description:");
    int new_price = ask_question_int("New price:");

    if(new_price < 0) {
        printf("Invalid price\n");
        free(old_name);
        free(new_name);
        free(new_desc);
        return;
    }

    if(ask_confirm("Confirm edit (Y/y):")) {
        backend_edit_merch_item(merch_ht, storage_ht, old_name, new_name, new_desc, new_price);
        free(old_name);
        free(new_name);
        free(new_desc);
        return;
    }
    print("Aborting edit\n");
    free(old_name);
    free(new_name);
    free(new_desc);
    return;
}

void actions_show_stock(ioopm_hash_table_t *merch_ht) {
    actions_list_merch_items(merch_ht);

    char *merch = ask_question_string("Enter name of the merch to show stock for:");

    if(!backend_merch_item_exists(merch_ht, merch)) {
        printf("Invalid merch\n");
        free(merch);
        return;
    }

    ioopm_list_t *l = backend_get_stock_list(merch_ht, merch);
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(l);

    while(!ioopm_list_iterator_at_end(it)) {
        char *location = backend_get_location(ioopm_list_iterator_current(it).p);
        size_t quantity = backend_get_quantity(ioopm_list_iterator_current(it).p);

        printf("%s: %zu\n", location, quantity);
        ioopm_list_iterator_advance(it);
    }
    printf("\n");
    ioopm_list_iterator_destroy(it);
    free(merch);
}

void actions_replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    actions_list_merch_items(merch_ht);

    char *location = ask_question_string("Enter the storage location to replenish:");
    char *merch = ask_question_string("Enter the name of the merch to replenish:");
    int amount = ask_question_int("Enter the amount to replenish:");

    if(!ioopm_hash_table_has_key(merch_ht, string_elem(merch))) {
        printf("Invalid merch\n");
        free(merch);
        free(location);
        return;
    }

    if(amount <= 0) {
        printf("Invalid amount\n");
        free(merch);
        free(location);
        return;
    }

    if(!backend_replenish_merch_item(storage_ht, merch_ht, merch, location)) {
        printf("Invalid location\n");
        free(merch);
        free(location);
        return;
    }
    
    for(int i = 1; i < amount; i++) {
        backend_replenish_merch_item(storage_ht, merch_ht, merch, location);
    }
    free(merch);
    free(location);
}

void actions_create_sc(ioopm_hash_table_t *carts, cart_count_t *count) {
    size_t id = cart_create_cart(carts, count);
    printf("\nCreated shopping cart %zu\n\n", id);
}

void actions_remove_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht) {
    size_t id = ask_question_int("Enter the id of the shopping cart to remove:");

    if(ask_confirm("Confirm removal (Y/y):")) {
        if(!cart_remove_cart(carts, merch_ht, id)) {
            printf("Invalid id\n");
            return;
        }
        return;
    }
    print("Aborting removal\n");
    return;
}

void actions_add_to_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht) {
    size_t id = ask_question_int("Enter the id of the shopping cart to add to:");
    char *merch = ask_question_string("Enter the name of the merch to add to the shopping cart:");
    size_t amount = ask_question_int("Enter the amount of the merch to add to the shopping cart");

    int available = cart_add_to_cart(carts, merch_ht, id, merch, amount);

    if(available == -1) {
        printf("Invalid id\n");
        free(merch);
        return;
    }
    else if(available == -2) {
        printf("Invalid merch\n");
        free(merch);
        return;
    }
    else if(available == -3) {
        printf("Invalid amount\n");
        free(merch);
        return;
    }
    else if(available != -4) {
        printf("Only %d available\n", available);
        free(merch);
        return;
    }
    free(merch);

}

void actions_remove_from_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht) {
    size_t id = ask_question_int("Enter the id of the shopping cart to remove from:");
    char *merch = ask_question_string("Enter the name of the merch to remove from the shopping cart:");
    size_t amount = ask_question_int("Enter the amount of the merch to remove from the shopping cart");

    int available = cart_remove_from_cart(carts, merch_ht, id, merch, amount);

    if(available == -1) printf("Invalid id\n");
    else if(available == -2) printf("Invalid merch\n");
    else if(available == -3) printf("Invalid amount\n");
    else if(available != -4) printf("You only have %d items\n", available);
    free(merch);
}

void actions_calculate_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht) {
    size_t id = ask_question_int("Enter the id of the shopping cart to calculate:");

    int cost = cart_calculate_cost(carts, merch_ht, id);

    if(cost == -1){
        printf("Invalid id\n"); 
        return;
    }
    printf("Your items cost %dkr\n", cost);
}

void actions_checkout_sc(ioopm_hash_table_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
  size_t id = ask_question_int("Enter the id of the shopping cart to check out:");

  if(!cart_checkout(carts, merch_ht, storage_ht, id)) printf("Invalid id\n");
}

int actions_quit() {
    if(ask_confirm("Confirm exit (Y/y):")) return -1;

    print("Aborting exit\n");
    return 0;
}


int actions_event_loop(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, ioopm_hash_table_t *carts, cart_count_t *count)
{
  char c = ask_question_menu();
  if(c == 'A') actions_add_merch(merch_ht);
  else if(c == 'L') actions_list_merch_items(merch_ht);
  else if(c == 'D') actions_remove_merch(merch_ht, storage_ht);
  else if(c == 'E') actions_edit_merch(merch_ht, storage_ht);
  else if(c == 'S') actions_show_stock(merch_ht);
  else if(c == 'P') actions_replenish(merch_ht, storage_ht);
  else if(c == 'C') actions_create_sc(carts, count);
  else if(c == 'R') actions_remove_sc(carts, merch_ht);
  else if(c == '+') actions_add_to_sc(carts, merch_ht);
  else if(c == '-') actions_remove_from_sc(carts, merch_ht);
  else if(c == '=') actions_calculate_sc(carts, merch_ht);
  else if(c == 'O') actions_checkout_sc(carts, merch_ht, storage_ht);
  else if(c == 'Q') return actions_quit();
  else if (c == 'X') return -1;
  return 0;
}