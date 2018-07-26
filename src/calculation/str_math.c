//Модуль вычисления математических выражений


#include "str_math.h"

#include "math_defines.h"

#include "stdio.h"
#include "stdint.h"
#include <ctype.h>

#include "text_functions.h"
#include "main.h"
#include "str_operate.h"
#include "math.h"


//All numbers are replaced by symbols started from this code
//2+2 -> 192+193
#define REPLACE_SYMB_CODE       192
#define MAX_FORMULA_LENGTH      100
#define MEMORY_CELLS_SIZE       28
#define NUMBER_CELLS_SIZE       28

#define MEMORY_START_SYMB       'A'
#define MEMORY_ANSWER_SYMB      'R'


uint8_t work_buffer[MAX_FORMULA_LENGTH];//буфер, в котором находится обрабатываемая строка
uint8_t work_buffer_length = 0;//количество символов в буфере

uint8_t sub_buffer[MAX_FORMULA_LENGTH];//буфер, в котором находится обрабатываемая подстрока
uint8_t sub_buffer_length = 0;//количество символов в буфере подстроки

double numbers[NUMBER_CELLS_SIZE];//Buffer for holding numbers. This numbers are replaced by chars with codes starting from REPLACE_SYMB_CODE
uint8_t work_buffer_num_count = 0;//Counter of numbers in "numbers" buffer

double memory_cells[MEMORY_CELLS_SIZE];//Memory - replaced by A B C D ... symbols

void prepeare_solved_result(CalcAnswerType *tmp_answer);

//Mathematical operations registers
double x;
double y;
double z;

uint8_t max_bracket_levell;// Maximul lelel of brackets in working buffer

CalcErrorType errors = CACL_ERR_NO;
double answer = 0.0;
uint8_t in_bin_flag = 0;//Binary number appeared in input formula
uint8_t in_hex_flag = 0;//Hex number appeared in input formula

//*****************************************************************************

//вычисляет значение по формуле txt, length - длина формулы в символах
//ответ помещается в переменную answer
//функция возврашает 0, если ошибок нет, либо код ошибки
CalcAnswerType solve(uint8_t *txt,uint8_t length)
{
  CalcAnswerType tmp_answer;
  
  tmp_answer.Answer = 0.0;
  tmp_answer.Error = CACL_ERR_NO;
  work_buffer_num_count = 0;
  errors = CACL_ERR_NO;
  in_bin_flag = 0;
  in_hex_flag = 0;
  
  fill_work_buffer(txt, length);
  replace_symbols_in_work_buffer();
  replace_functions();
  replace_metric_symbols();
  find_numbers();//заменяет числа одиночными символами - А Б В ...
  bracket_anlyse();//проверяет четность скобок
  if (errors!= CACL_ERR_NO)
  {
    tmp_answer.Error = errors;
    return tmp_answer;
  }
  
  if (max_bracket_levell == 0)//если скобок нет
  {
    fill_sub_buffer(&work_buffer[0],work_buffer_length);
    solve_sub_buffer();
    prepeare_solved_result(&tmp_answer);
    return tmp_answer;
  }
  // Process brackets
  solve_work_buffer();
  
  prepeare_solved_result(&tmp_answer);  

  return tmp_answer;
}

void prepeare_solved_result(CalcAnswerType *tmp_answer)
{
  tmp_answer->Answer = answer;
  tmp_answer->Error = errors;
  tmp_answer->BinFlag = in_bin_flag;
  tmp_answer->HexFlag = in_hex_flag;
  save_answer_to_memory();
}

//Save last answer to memory cell ('R')
void save_answer_to_memory(void)
{
  uint8_t pos = MEMORY_ANSWER_SYMB - MEMORY_START_SYMB;
  if (errors == CACL_ERR_NO)
    memory_cells[pos] = answer;
  else
    memory_cells[pos] = 0.0;
}


void replace_functions(void)
{
  //заменяет функции на их знаки
  //обработчик работает с функциями-знаками (функция обозначается одним знаком)
  uint8_t i=0;
  do
  { 
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "atg", (char)SYMB_ATAN_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "asin",(char)SYMB_ASIN_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "acos",(char)SYMB_ACOS_CODE);
    
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "sin", (char)SYMB_SIN_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "cos", (char)SYMB_COS_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "tg",  (char)SYMB_TAN_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "ln",  (char)SYMB_LN_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "log", (char)SYMB_LOG_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "exp", (char)SYMB_EXP_CODE);
    work_buffer_length = work_buffer_length - check_and_replace_substring((char*)&work_buffer[i], "Ans", (char)MEMORY_ANSWER_SYMB);
    
    i++;  
  } 
  while (i<=work_buffer_length-2);
}

//Replace some symbols in "work_buffer"
//@ - high priority multiply
void replace_symbols_in_work_buffer(void)
{
  uint8_t i;
  for (i = 0; i < work_buffer_length; i++)
  {
    if (work_buffer[i] == SYMB_EXP10_CODE) 
    {
      work_buffer[i] = 'e';//This needed for replacing numbers
      if (work_buffer[i+1] == SYMB_MINUS_CODE) 
        work_buffer[i+1] = '-'; //Convert special minus symbol to conventional minus
    }
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], SYMB_DEG_CODE, "@1.7453292520e-2");//replace deg symbol
    if (isdigit(work_buffer[i - 1]))
      work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], SYMB_PI_CODE, "@3.141592654");//replace PI symbol
    else
      work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], SYMB_PI_CODE, "3.141592654");//replace PI symbol
  }
}


//Replace metric_symbols like "m", "M", "G" to numbers "*1e-3", "*1e3", "*1e6"
//This function MUST be calld after replace_functions() bum before find_numbers()
void replace_metric_symbols(void)
{
  uint8_t i = 0;
  do
  {
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], 'm', "@1e-3");//@ - high priority multiply
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], (char)0xB5, "@1e-6");//micro
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], 'n', "@1e-9");
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], 'p', "@1e-12");
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], 'K', "@1e3");
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], 'M', "@1e6");
    work_buffer_length = work_buffer_length + check_and_replace_symbol((char*)&work_buffer[i], 'G', "@1e9");
    i++;  
  } 
  while (i < work_buffer_length);
}





//ищет в рабочем буфере(строке) числа
//преобразовывает их в double, помещает их в массив numbers[]
//и заменяет их в массиве символами чисел(А,Б,В,Г.....)
//также заменяет символы регистров памяти (A B C D)
void find_numbers(void)
{
  uint8_t i=0;
  uint8_t current_chr = 0;
  char *end_ptr;//указатель на конец числа
  
  uint16_t lng;
  uint8_t found = 0;//Number found flag
  
  do
  {   
      found = 0;
      i = 0;

      //Do steps untill needed code would not be found
      do
      {
        current_chr = work_buffer[i];
        i++;
      } while ((char_is_number(current_chr) == 0) && (is_mem_sumbol(current_chr) == 0) && (i <= work_buffer_length-1));//ищем цифру, симол памяти или пи
      
      if (is_mem_sumbol(current_chr)!= 0)//Current symbol is memory cell
      {
        //Memory cell will be replaced by number
        double mem_value = memory_cells[current_chr - MEMORY_START_SYMB];
        add_new_number_to_buffer(mem_value);
        replace_by_char(&work_buffer[0], i-1, 1, (REPLACE_SYMB_CODE-1) + work_buffer_num_count);
        found = 1;
      }
  
      if (char_is_number(current_chr)!= 0)//проверка на случай если буфер закончился, а цифра не нашлась
      {
        if (current_chr == SYMB_MINUS_CODE)
          work_buffer[i-1] = '-'; //Convert special minus symbol to conventional minus
        
        uint8_t bin_flag = 0;
        if (current_chr == '0')
        {
          if (work_buffer[i] == 'b') //0b
          {
            long int bin_value = strtol((const char *)&work_buffer[i+1], &end_ptr, 2);
            bin_flag = 1;
            in_bin_flag = 1;
            add_new_number_to_buffer((double)bin_value);
          }
          if (work_buffer[i] == 'x')//0x
          {
            in_hex_flag = 1;
          }
        }
        
        if (bin_flag == 0)
        {
          double new_value = strtod((const char *)&work_buffer[i-1], &end_ptr);
          add_new_number_to_buffer(new_value);
        }
        
        uint32_t str_begin = (uint32_t)&work_buffer[i-1];//Start of the number
        lng = (uint16_t)((uint32_t)end_ptr - str_begin);//Length of number in chars
        replace_by_char(&work_buffer[0], i-1, (uint8_t)lng, (REPLACE_SYMB_CODE-1) + work_buffer_num_count);//REPLACE_SYMB_CODE - symbol 'А' in ASCII
        work_buffer_length = work_buffer_length - (uint8_t)lng + 1;
        
        found = 1;
      }//end if

    } while (found == 1);
}

//Check if symbol is digit or special minus
uint8_t char_is_number(uint8_t chr)
{
    if (chr == SYMB_MINUS_CODE)
      return 1;
    
    return isdigit(chr);
}

//Add new number to the buffer of replaced numbers
void add_new_number_to_buffer(double new_value)
{
  numbers[work_buffer_num_count] = new_value;
  work_buffer_num_count++;
}


void fill_work_buffer(uint8_t *txt, uint8_t length)
{
  //заполняет буфер данными из внешней строки
  //dest, source
  memset((char*)work_buffer, 0, sizeof(work_buffer));
  memcpy((char*)work_buffer, (char*)txt, length);
  work_buffer[length] = 0;
  work_buffer_length = length;
  
  if (length == 0) //добавляем "0" в пустую строку
  {
    work_buffer[0] = '0';
    work_buffer[1] = 0;//null termination
    work_buffer_length = 1;
  }
}

void fill_sub_buffer(uint8_t *txt, uint8_t length)
{
  //заполняет буфер данными из внешней строки
  //dest, source
  memset((char*)sub_buffer, 0, sizeof(sub_buffer));
  memcpy((char*)sub_buffer, (char*)txt, length);
  sub_buffer[length+1] = 0;//null termination
  sub_buffer_length = length;
}

void solve_sub_buffer(void)
{
//этот буфер НЕ СОДЕРЖИТ скобок.
  uint8_t i;
  uint8_t chr;
  uint8_t found=0;
  uint8_t level=1;
  
  if (sub_buffer_length == 0)
  {
    errors = CACL_ERR_NO_ARGUMENT; 
    return;
  }
  
  do
  {
    do//проходит строку, пока попадаются функции данного уровня
    {
        found=0;
        i=0;
        do//проходит символы строки в происках функции X уровня
        {
          chr=sub_buffer[i];
          i++;
          if (is_x_level_func(chr,level)==1)
          {
            solve_func(i-1);//если нашлась функция, вычислить ее
            found=1;
          }
        } while ((i<=sub_buffer_length-1)&&(found==0)&&(errors==0));
    } while ((found==1)&&(errors==0));
    level++;//если во всей подстроке функций такого уровня больше нет
  } while ((level<6)&&(errors==0));
  
  if (errors!= 0)
    return;
  
  if (sub_buffer_length > 1)
  {
    errors = CACL_ERR_BAD_FORMULA2;
    return;
  }
  
  if (is_num_sumbol(sub_buffer[0]) == 1) 
    answer = numbers[sub_buffer[0]-REPLACE_SYMB_CODE];
  else 
  {
    errors = CACL_ERR_BAD_FORMULA3;
    return;
  }
      
}


void solve_work_buffer(void)
{
  uint8_t i;
  uint8_t chr;
  
  uint8_t sub_begin = 0;//открывающая скобка
  uint8_t sub_end = 0;//закрывающая скобка
  uint8_t bracket_cnt = 0;
  
  do
  {
    i = 0;
    bracket_cnt=0;
    do//проходит символы строки в происках скобок
    {      
      chr = work_buffer[i];
      if (chr=='(')
        bracket_cnt++;
      if (chr==')')
        bracket_cnt--;
      
      if (bracket_cnt == max_bracket_levell)//ищем скобки максимального уровня
      {
        sub_begin = i;
        while (work_buffer[i]!=')')
          i++;//ищем закрывающую скобку
        sub_end = i;
        fill_sub_buffer(&work_buffer[sub_begin+1], (sub_end - sub_begin - 1));
        solve_sub_buffer();
        
        if (errors == 0)
        {
          replace_by_char(&work_buffer[0], sub_begin,(uint8_t)(sub_end-sub_begin+1), sub_buffer[0]);
          work_buffer_length = work_buffer_length - (sub_end-sub_begin);
          //found=1;
          i = sub_begin;
          bracket_cnt--;
        }
      }//end of if
      
      i++;
    } while ((i <= work_buffer_length-1) && (errors == 0));
    max_bracket_levell--;
  }
  while ((max_bracket_levell > 0) && (errors == 0));
  
  if (errors!= CACL_ERR_NO)
    return;
  
  if (work_buffer_length > 1)//если скобок нет
  {
    fill_sub_buffer(&work_buffer[0],work_buffer_length);
    solve_sub_buffer();
    
    if (is_num_sumbol(sub_buffer[0]) == 1) 
      answer = numbers[sub_buffer[0]-REPLACE_SYMB_CODE];
    else 
    {
      errors = CACL_ERR_BAD_FORMULA3;
      return;
    }
  }
  else
  {
    if (is_num_sumbol(work_buffer[0]) == 1) 
      answer = numbers[work_buffer[0]-REPLACE_SYMB_CODE];
    else 
    {
      errors = CACL_ERR_BAD_FORMULA3;
      return;
    }
  }
      
}



//вычисяет функцию расположенную по указанной позиции
void solve_func(uint8_t pos)
{
  switch (sub_buffer[pos])
  {
    case '+':   solve_plus(pos);break;
    case '-':   solve_minus(pos);break;
    case '*':   solve_multiply(pos);break;
    case '^':   solve_pow(pos);break;
    case '/':   solve_div(pos); break;
    case SYMB_SQUARE_CODE:      solve_dbl(pos); break;
    case SYMB_SIN_CODE:         solve_sin(pos); break;
    case SYMB_COS_CODE:         solve_cos(pos); break;
    case SYMB_TAN_CODE:         solve_tg(pos); break;
    case SYMB_ASIN_CODE:        solve_asin(pos); break;
    case SYMB_ACOS_CODE:        solve_acos(pos); break;
    case SYMB_ATAN_CODE:        solve_atan(pos); break;
    case SYMB_SQRT_CODE:        solve_sqrt(pos); break;
    case SYMB_LN_CODE:          solve_ln(pos); break;
    case SYMB_LOG_CODE:         solve_log(pos); break;
    case SYMB_EXP_CODE:         solve_exp(pos); break;
    case '@':                   solve_multiply(pos);break;//high priority muliply
    default: break;
  }
}

//************************************************************************************
//Functions below try to calculate some mathematical functions, that must be present at "pos" in "sub_buffer"
//All this mathematical functions cn work only with coded numbers like "sin('CODED_NUMBER')" 'CODED_NUMBER' - single_char (>=REPLACE_SYMB_CODE)
//No brakets allowed

void solve_plus(uint8_t pos)
{
  //проверка, чтобы символы слева и справа от + были числовыми (А Б В Г)
  if ((is_num_sumbol(sub_buffer[pos-1]) == 1) && (is_num_sumbol(sub_buffer[pos+1]) == 1))
  {
    x = numbers[sub_buffer[pos-1] - REPLACE_SYMB_CODE];
    y = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    z = x + y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE] = z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_minus(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1]) == 1) && (is_num_sumbol(sub_buffer[pos+1]) == 1))
  {
    x = numbers[sub_buffer[pos-1] - REPLACE_SYMB_CODE];
    y = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    z = x - y;
    numbers[sub_buffer[pos-1] - REPLACE_SYMB_CODE] = z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
    
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_multiply(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1]) == 1) && (is_num_sumbol(sub_buffer[pos+1]) == 1))
  {
    x = numbers[sub_buffer[pos-1] - REPLACE_SYMB_CODE];
    y = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    z = x * y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE] = z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_div(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1]) == 1) && (is_num_sumbol(sub_buffer[pos+1]) == 1))
  {
    x = numbers[sub_buffer[pos-1] - REPLACE_SYMB_CODE];
    y = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    if (y == 0) 
    {
      errors = CACL_ERR_ZERO_DIV;
      return;
    }
    z = x / y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_pow(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1]) == 1) && (is_num_sumbol(sub_buffer[pos+1]) == 1))
  {
    x = numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    y = numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z = pow(x,y);
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE] = z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_dbl(uint8_t pos)//x^2
{
  if (is_num_sumbol(sub_buffer[pos-1]) == 1)
  {
    x = numbers[sub_buffer[pos-1] - REPLACE_SYMB_CODE];
    z = pow(x,2);      
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_sin(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    z = sin(x);      
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_cos(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1]) == 1)
  {
    x = numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z = cos(x);      
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_tg(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1]) == 1)
  {
    x = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    z = tan(x);      
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

//argument in radians
void solve_asin(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1]) == 1)
  {
    x = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    if ((x < -1.00000) || (x > 1.00000))
    {
      errors = CACL_ERR_TRIGON;
      return;
    }
    z = asin(x);   
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0], pos, 1);
    sub_buffer_length = sub_buffer_length - 1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

//argument in radians
void solve_acos(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1]) == 1)
  {
    x = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    if ((x < -1.00000) || (x > 1.00000))
    {
      errors = CACL_ERR_TRIGON;
      return;
    }
    z = acos(x);
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0], pos, 1);
    sub_buffer_length = sub_buffer_length - 1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

//argument in radians
void solve_atan(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1]) == 1)
  {
    x = numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE];
    z = atan(x);
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0], pos, 1);
    sub_buffer_length = sub_buffer_length - 1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_sqrt(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x = numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    if (x < 0)
    {
      errors = CACL_ERR_ROOT;
      return;
    }
    z = sqrt(x);
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_exp(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x = numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z = exp(x);      
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

void solve_ln(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1]) == 1)
  {
    x = numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    if (x < 0.0)
    {
      errors = CACL_ERR_LOG; 
      return;
    }
    z = log(x);     
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

//log10
void solve_log(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos + 1]) == 1)
  {
    x = numbers[sub_buffer[pos + 1] - REPLACE_SYMB_CODE];
    if (x < 0.0)
    {
      errors = CACL_ERR_LOG;
      return;
    }
    z = log10(x);
    numbers[sub_buffer[pos+1] - REPLACE_SYMB_CODE] = z;
    cut_from_str(&sub_buffer[0], pos, 1);
    sub_buffer_length = sub_buffer_length - 1;
    work_buffer_num_count--;
  }
  else 
    errors = CACL_ERR_NO_ARGUMENT;
}

//Return 1 if the symbol replacing number (А Б В ...)
uint8_t is_num_sumbol(uint8_t chr)
{
  if ((chr > (REPLACE_SYMB_CODE-1)) && (chr < 212))
    return 1;
  else
    return 0;
}

//Return 1 if the symbol replacing memory cell (A B C D...)
uint8_t is_mem_sumbol(uint8_t chr)
{
  uint8_t end_char = MEMORY_START_SYMB + MEMORY_CELLS_SIZE;
  if ((chr >= MEMORY_START_SYMB) && (chr < end_char))
    return 1;
  else
    return 0;
}


//Check if the level of function (chr) equals to x
uint8_t is_x_level_func(uint8_t chr, uint8_t x)
{
  uint8_t funct_lvl = return_function_level(chr);
  if (funct_lvl == x)
    return 1;
  else
    return 0;
}

//Return level on function (function is presented by it's char code)
uint8_t return_function_level(uint8_t funct_code)
{
  switch (funct_code)
  {
    case '+': return 5;
    case '-': return 5;
    case '*': return 4;
    case '/': return 4;
    case '^': return 3;
    case SYMB_SQUARE_CODE:      return 3;
    case SYMB_SIN_CODE:         return 2;
    case SYMB_COS_CODE:         return 2;
    case SYMB_TAN_CODE:         return 2;
    case SYMB_ASIN_CODE:        return 2;
    case SYMB_ACOS_CODE:        return 2;
    case SYMB_ATAN_CODE:        return 2;
    case SYMB_LN_CODE:          return 2;
    case SYMB_LOG_CODE:         return 2;
    case SYMB_EXP_CODE:         return 2;
    case SYMB_SQRT_CODE:        return 2;
    case '@':                   return 1;
    default: return 0;
  }
}


//проверяет, чтобы количество скобок было четным и определет макс уровень скобок
void bracket_anlyse(void)
{
  uint8_t i;
  uint8_t bracket_cnt = 0;
  max_bracket_levell = 0;
  
  for (i=0; i<work_buffer_length; i++)
  {
    if (work_buffer[i] == '(')
      bracket_cnt++;
    if (work_buffer[i] == ')')
      bracket_cnt--;
    if (bracket_cnt > max_bracket_levell)
      max_bracket_levell = bracket_cnt;
  }
  if (bracket_cnt != 0)
    errors = CACL_ERR_BRACKETS;//нечетное число скобок
}