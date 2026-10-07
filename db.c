#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

struct item
{
  char *name;
  char *desc;
  int price;
  char *storage;
};

typedef struct item item_t;

void print_item(item_t *item)
{
  char *name = item->name;
  char *desc = item->desc;
  int sek = item->price/100;
  int cents = item->price%100;
  char *storage = item->storage;
  printf("Name: %s\nDesc: %s\nPrice: %d.%d SEK\nShelf: %s\n", name, desc, sek, cents, storage);
}


item_t make_item(char *name, char *desc, int price, char *storage)
{
  item_t item = {name, desc, price, storage};
  return item;
}

bool is_storage_nmr(char *str)
{
  int len = strlen(str);
  int digits = 1;
  if(tolower(str[0]) < 'a' || tolower(str[0]) > 'z') return false;
  for(int i = 1; i < len; i++){
    if(isdigit(str[i])){
      digits++;
    }
  }
  if(digits == len){
    return true;
  }
  else{
    return false;
  }
}

char *ask_question_shelf(char *question)
{
  return ask_question(question, is_storage_nmr, (convert_func *) strdup).string_value;
}

item_t input_item(void)
{
  char *name = ask_question_string("Please write the item name: ");
  char *desc = ask_question_string("Please write the item description: ");
  int price = ask_question_int("Please write the price of the item: ");
  char *storage = ask_question_shelf("Please write the shelf to store this item: ");
  item_t item = make_item(name, desc, price, storage);
  return item;
}

char *magick(char *arr1[], char* arr2[], char *arr3[], int len)
{
  char buf[255];
  int pos = 0;
  int rand1 = random()%len;
  char *word1 = arr1[rand1];
  for(int i = 0; i < strlen(word1); i++){
    buf[pos] = word1[i];
    pos++;
  }
  buf[pos] = '-';
  pos++;
  int rand2 = random()%len;
  char *word2 = arr2[rand2];
  for(int i = 0; i < strlen(word2); i++){
    buf[pos] = word2[i];
    pos++;
  }
  buf[pos] = ' ';
  pos++;

  int rand3 = random()%len;
  char *word3 = arr3[rand3];
  for(int i = 0; i < strlen(word3); i++){
    buf[pos] = word3[i];
    pos++;
  }
  buf[pos] = '\0';

  return strdup(buf);
}

void list_db(item_t *items, int no_items)
{
  for(int i = 0; i < no_items; i++)
  {
    printf("%d: %s\n", i+1, items[i].name);
  }
}

void edit_db(item_t *items, int no_items)
{
  int num = ask_question_int("Enter the number in the database to edit: ") - 1;
  while(num < 0 || num >= no_items)
  {
    printf("Please enter a valid number...\n");
    num = ask_question_int("Enter the number in the database to edit: ") - 1;
  }
  item_t item = items[num];
  print_item(&item);
  printf("Do you want to edit this listing?(Y/N)");
  char c = toupper(getchar());
  while(c != 'Y' && c != 'N')
  {
    printf("Please write Y or N");
    c = toupper(getchar());
  }
  if(c == 'Y')
  {
    clear_input_buffer();
    item_t new_item = input_item();
    items[num] = new_item;
  }
  else if(c == 'N') return;
}

void print_menu(void)
{
  fprintf(stdout, "[L]: Lägg till en vara\n" 
                  "[T]: Ta bort en vara\n"
                  "[R]: Redigera en vara\n"
                  "[G]: Ångra senaste ändring\n"
                  "[H]: Lista hela varukatalogen\n"
                  "[A]: Avsluta\n");
}

char ask_question_menu(void)
{
  print_menu();
  char c = toupper(getchar());
  clear_input_buffer();
  while(c != 'L' && c != 'T' && c != 'R' && c != 'G' && c != 'H' && c != 'A')
  {
    print_menu();
    printf("Please enter a valid option: ");
    c = toupper(getchar());
    clear_input_buffer();
  }

  return c;
}

int add_item_to_db(item_t *items, int no_items)
{
  items[no_items] = input_item();
  return no_items + 1;
}

int remove_item_from_db(item_t *items, int no_items)
{
  list_db(items, no_items);
  int num = ask_question_int("Enter the number in the database to remove: ")-1;
  while(num < 0 || num >=no_items)
  {
    printf("Please enter a valid number...\n");
    num = ask_question_int("Enter the number in the database to remove: ")-1;
  }
  for(int i = num; i < no_items-1; i++)
  {
    items[i] = items[i+1];
  }

  return no_items-1;
}

int event_loop(item_t *items, int no_items)
{
  int db_siz = no_items;
  char c = ask_question_menu();
  if(c == 'L') db_siz = add_item_to_db(items, db_siz);
  else if(c == 'T') db_siz = remove_item_from_db(items, db_siz);
  else if(c == 'R') edit_db(items, db_siz);
  else if(c == 'G') printf("NOT YET IMPLEMENTED!\n");
  else if(c == 'H') list_db(items, db_siz);
  else if(c == 'A') return -1;
  return db_siz;
}

int main(int argc, char *argv[])
{
  char *array1[] = {"Laser", "Polka", "Extra"}; 
  char *array2[] = {"förnicklad", "smakande", "ordinär"}; 
  char *array3[] = {"skruvdragare", "kola", "uppgift"}; 
  int len = 3;

  if (argc < 2)
  {
    printf("Usage: %s number\n", argv[0]);
  }
  else
  {
    item_t db[20]; // Array med plats för 16 varor
    int db_siz = 0; // Antalet varor i arrayen just nu

    int items = atoi(argv[1]); // Antalet varor som skall skapas

    if (items > 0 && items <= 16)
    {
      for (int i = 0; i < items; ++i)
      {
        // Läs in en vara, lägg till den i arrayen, öka storleksräknaren
        item_t item = input_item();
        db[db_siz] = item;
        ++db_siz;
      }
    }
    else
    {
      puts("Sorry, must have [1-16] items in database.");
      return 1; // Avslutar programmet!
    }

    for (int i = db_siz; i < 16; ++i)
      {
        char *name = magick(array1, array2, array3, len);
        char *desc = magick(array1, array2, array3, len);
        int price = random() % 200000;
        char shelf[] = { random() % ('Z'-'A') + 'A',
                         random() % 10 + '0',
                         random() % 10 + '0',
                         '\0' };
        item_t item = make_item(name, desc, price, strdup(shelf));

        db[db_siz] = item;
        ++db_siz;
      }

    // Skriv ut innehållet
    while (db_siz != -1)
    {
    db_siz = event_loop(db, db_siz);
    }
  }
  return 0;
}
