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
#include "hash_table_iterator.h"

void print_menu(void)
{
  fprintf(stdout, "[A]: Add merchandise\n" 
                  "[L]: List merchandise\n"
                  "[D]: Remove merchandise\n"
                  "[E]: Edit merchandise\n"
                  "[S]: Show stock\n"
                  "[P]: Replenish\n");
}

char ask_question_menu(void)
{
  print_menu();
  char c = toupper(getchar());
  clear_input_buffer();
  while(c != 'A' && c != 'L' && c != 'D' && c != 'E' && c != 'S' && c != 'P')
  {
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

void list_merch(ioopm_hash_table_t *merch_ht) {

}

void remove_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch(merch_ht);

    char* merch = ask_question_string("Enter name of merch to be removed:");

    char* confirm = ask_question_string("Confirm removal (Y/y):");

    if(strcmp(confirm, "Y") || strcmp(confirm, "y")) {
        remove_merch_item(storage_ht, merch_ht, merch);
        return;
    }
    print("Aborting removal");
    return;
}

void edit_merch(ioopm_hash_table_t *merch_ht) {
    list_merch(merch_ht);

    char *old_name = ask_question_string("Enter name of merch to be edited:");

    char *new_name = ask_question_string("New name:");
    char *new_desc = ask_question_string("New description:");
    char *new_price = ask_question_int("New price:");

    char *confirm = ask_question_string("Confirm removal (Y/y):");

    if(strcmp(confirm, "Y") || strcmp(confirm, "y")) {
        edit_merch_item(merch_ht, old_name, new_name, new_desc, new_price);
        return;
    }
    print("Aborting removal");
    return;
}

void show_stock(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch(merch_ht);

    char *merch = ask_question_string("Enter name of the merch to show stock for:");

    get_stock(storage_ht, merch);
}

void replenish(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht) {
    list_merch(merch_ht);

    char *merch = ask_question_string("Enter the name of the merch to replenish:");
    char *location = ask_question_string("Enter the storage location to replenish:");
    char *amount = ask_question_string("Enter the amount to replenish:");

    replenish_merch_item(storage_ht, merch_ht, merch, location);
}

void event_loop(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht)
{
  char c = ask_question_menu();
  if(c == 'A') add_merch(merch_ht);
  else if(c == 'L') list_merch(merch_ht);
  else if(c == 'D') remove_merch(merch_ht, storage_ht);
  else if(c == 'E') edit_merch(merch_ht);
  else if(c == 'S') show_stock(merch_ht, storage_ht);
  else if(c == 'P') replenish(merch_ht, storage_ht);
}