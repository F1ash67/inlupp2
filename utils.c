#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

bool not_empty(char *str)
{
  return strlen(str) > 0;
}

answer_t ask_question(char *question, check_func *check, convert_func *convert){
  int buf_siz = 255;
  char res[buf_siz];
  bool correct = false;
  answer_t answer;
  while (!correct)
  {
    puts(question);
    read_string(res, buf_siz);
    correct = check(res);
  }
  answer = convert(res);
  return answer;
}

bool is_number(char *str)
{
  int len = strlen(str);
  int digits = 0;
  if(len == 0 || (len == 1 && str[0] == '-')) return false;
  for(int i = 0; i < len; i++){
    if(isdigit(str[i]) || (str[i] == '-' && i == 0)){
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


int ask_question_int(char *question)
{
  answer_t answer = ask_question(question, is_number, (convert_func *) atoi);
  return answer.int_value; // svaret som ett heltal
}

char *ask_question_string(char *question)
{
  return ask_question(question, not_empty, (convert_func *) strdup).string_value;
}

int add(int a, int b)
{
  return a + b;
}

int foldl_int_int(int numbers[], int numbers_siz, int_fold_func *f)
{
  int result = 0;

  // Loopa över arrayen och för varje element e utför result = f(result, e)
  for (int i = 0; i < numbers_siz; ++i)
  {
    result = f(result, numbers[i]);
  }

  return result;
}

long sum(int numbers[], int numbers_siz)
{
  return foldl_int_int(numbers, numbers_siz, add);
}

void print(char string[]){
  int current = 0;
  while (string[current] != '\0')
  {
    putchar(string[current]);
    current++;
  }
}

void println(char string[]){
  print(string);
  putchar('\n');
}

void clear_input_buffer(){
  int c;
  do
    {
      c = getchar();
    }
  while (c != '\n' && c != EOF);
}

/*int ask_question_int(char *question)
{

  int result = 0;
  int conversions = 0;
  do
    {
      printf("%s\n", question);
      conversions = scanf("%d", &result);
      clear_input_buffer();
      putchar('\n');
    }
  while (conversions < 1);
  return result;
}*/

int read_string(char *buf, int buf_siz){
  int amt = 0;
  int c = getchar();
  while (c != '\n' && c != EOF)
  {
    if(amt < buf_siz-1){
      buf[amt] = c;
      amt++;
    }
    else{
      clear_input_buffer();
      break;
    }
    c = getchar();
  }
  buf[amt] = '\0';
  putchar('\n');
  return amt;
}

/*char *ask_question_string(char *question, char *buf, int buf_siz)
{
  int chars;
  do
    {
      printf("%s\n", question);
      chars = read_string(buf, buf_siz);
    }
  while (chars == 0);
  return buf;
}*/