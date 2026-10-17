#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "actions.h"
#include "backend.h"
#include "hash_table_general_key.h"
#include "hash_table_iterator_general_key.h"
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "shopping_cart.h"

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
                    "[Q]: Quit\n");
}

char ask_question_menu(void)
{
    print_menu();
    char c = toupper(getchar());
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



void add_merch(ioopm_hash_table_t *merch_ht) {
    char *name = ask_question_string("Enter name:");
    char *desc = ask_question_string("Enter description:");
    size_t price = ask_question_int("Enter price:");

    create_merch_item(merch_ht, name, desc, price);
}

void list_merch_items(ioopm_hash_table_t *merch_ht) {
    ioopm_list_t *l = list_merch(merch_ht);
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(l);

    size_t i = 0;
    while(!ioopm_list_iterator_at_end(it)) {
        printf("%s\n", ioopm_list_iterator_current(it).s);
        ioopm_list_iterator_advance(it);
        i++;

        if(i % 20 == 0 && !ioopm_list_iterator_at_end(it)) {
            char *confirm  = ask_question_string("Do you want to keep listing merch? If no: (N/n)");

            if(!strcmp(confirm, "N") || !strcmp(confirm, "n")) {
                break;
            }
        }
    }
    ioopm_list_iterator_destroy(it);
    ioopm_list_destroy(l);
}

void remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch_items(merch_ht);

    char* merch = ask_question_string("Enter name of merch to be removed:");

    char* confirm = ask_question_string("Confirm removal (Y/y):");

    if(strcmp(confirm, "Y") || strcmp(confirm, "y")) {
        remove_merch_item(storage_ht, merch_ht, merch);
        return;
    }
    print("Aborting removal\n");
    return;
}

void edit_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch_items(merch_ht);

    char *old_name = ask_question_string("Enter name of merch to be edited:");

    char *new_name = ask_question_string("New name:");
    char *new_desc = ask_question_string("New description:");
    char *new_price = ask_question_int("New price:");

    char *confirm = ask_question_string("Confirm removal (Y/y):");

    if(strcmp(confirm, "Y") || strcmp(confirm, "y")) {
        edit_merch_item(merch_ht, storage_ht, old_name, new_name, new_desc, new_price);
        return;
    }
    print("Aborting removal\n");
    return;
}

void show_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch_items(merch_ht);

    char *merch = ask_question_string("Enter name of the merch to show stock for:");

    get_stock(storage_ht, merch);
}

void replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch_items(merch_ht);

    char *merch = ask_question_string("Enter the name of the merch to replenish:");
    char *location = ask_question_string("Enter the storage location to replenish:");
    char *amount = ask_question_string("Enter the amount to replenish:");

    replenish_merch_item(storage_ht, merch_ht, merch, location);
}

void create_sc(ioopm_list_t *carts) {
    size_t id = create_cart(carts);
    printf("Created shopping cart %zu\n", id);
}

void remove_sc(ioopm_list_t *carts) {
    size_t id = ask_question_int("Enter the id of the shopping cart to remove:");

    char *confirm = ask_question_string("Confirm removal (Y/y):");

    if(strcmp(confirm, "Y") || strcmp(confirm, "y")) {
        remove_cart(carts, id);
        return;
    }
    print("Aborting removal\n");
    return;
}

void add_to_sc(ioopm_list_t *carts) {
    size_t id = ask_question_int("Enter the id of the shopping cart to add to:");
    char *merch = ask_question_string("Enter the name of the merch to add to the shopping cart:");
    size_t amount = ask_question_int("Enter the amount of the merch to add to the shopping cart");

    add_to_cart(carts, id, merch, amount);
}

void remove_from_sc(ioopm_list_t *carts) {
    size_t id = ask_question_int("Enter the id of the shopping cart to remove from:");
    char *merch = ask_question_string("Enter the name of the merch to remove from the shopping cart:");
    size_t amount = ask_question_int("Enter the amount of the merch to remove from the shopping cart");

    remove_from_cart(carts, id, merch, amount);
}

void calculate_sc(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht) {
    size_t id = ask_question_int("Enter the id of the shopping cart to calculate:");

    size_t cost = calculate_cost(carts, merch_ht, id);
    printf("Your items cost %zukr\n", cost);
}

void checkout_sc(ioopm_list_t *carts, ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
  size_t id = ask_question_int("Enter the id of the shopping cart to check out:");

  checkout(carts, merch_ht, storage_ht, id);
}

int quit() {
    char *confirm = ask_question_string("Confirm exit (Y/y)");

    if(strcmp(confirm, "Y") || strcmp(confirm, "y")) {
        return -1;
    }
    print("Aborting removal\n");
    return 0;
}


int event_loop(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht, ioopm_list_t *carts)
{
  char c = ask_question_menu();
  if(c == 'A') add_merch(merch_ht);
  else if(c == 'L') list_merch_items(merch_ht);
  else if(c == 'D') remove_merch(merch_ht, storage_ht);
  else if(c == 'E') edit_merch(merch_ht, storage_ht);
  else if(c == 'S') show_stock(merch_ht, storage_ht);
  else if(c == 'P') replenish(merch_ht, storage_ht);
  else if(c == 'C') create_sc(carts);
  else if(c == 'R') remove_sc(carts);
  else if(c == '+') add_to_sc(carts);
  else if(c == '-') remove_from_sc(carts);
  else if(c == '=') calculate_sc(carts, merch_ht);
  else if(c == 'O') checkout_sc(carts, merch_ht, storage_ht);
  else if(c == 'Q') quit();
}