#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

extern char *strdup(const char *);

typedef int int_fold_func(int, int);
typedef bool check_func(char*);
typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;
typedef answer_t convert_func(char*);

bool not_empty(char *str);
answer_t ask_question(char *question, check_func *check, convert_func *convert);
int add(int a, int b);
int foldl_int_int(int numbers[], int numbers_siz, int_fold_func *f);
long sum(int numbers[], int numbers_siz);
char *ask_question_string(char *question);
int read_string(char *buf, int buf_siz);
bool is_number(char *str);
int ask_question_int(char *question);
//char *ask_question_string(char *question, char *buf, int buf_siz);
void clear_input_buffer();
void println(char string[]);
void print(char string[]);

#endif 