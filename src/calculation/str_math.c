//Модуль вычисления математических выражений


#include "str_math.h"
#include "stdio.h"
#include "stdint.h"
#include "text_functions.h"
#include "main.h"

//All number are replaced by symbols started from this code
//2+2 -> 192+193
#define REPLACE_SYMB_CODE       192


#include "str_operate.h"//работа со строками
#include "math.h"
#include <ctype.h>

uint8_t work_buffer[50];//буфер, в котором находится обрабатываемая строка
uint8_t work_buffer_length=0;//количество символов в буфере

uint8_t sub_buffer[50];//буфер, в котором находится обрабатываемая подстрока
uint8_t sub_buffer_length=0;//количество символов в буфере подстроки

double numbers[30];//числа, полученные из исходной строки
uint8_t work_buffer_num_count=0;//количество чисел в буфере

double memory_num[4];//память - переменные A B C D

uint8_t errors=0;

double x;//регистры математ операций
double y;
double z;

uint8_t max_bracket_levell;//максимальный уровень скобок рабочем буфере

double answer = 0.0;

//вычисляет значение по формуле txt, length - длина формулы в символах
//ответ помещается в переменную answer
//функция возврашает 0, если ошибок нет, либо код ошибки

uint8_t solve(uint8_t *txt,uint8_t length)
{

  work_buffer_num_count=0;
  errors=0;
  
  fill_work_buffer(txt,length);
  replace_functions();
  find_numbers();//заменяет числа одиночными символами - А Б В ...
  bracket_anlyse();//проверяет четность скобок
  if (errors!=0){return errors;}
  if (max_bracket_levell==0)//если скобок нет
  {
    fill_sub_buffer(&work_buffer[0],work_buffer_length);
    solve_sub_buffer();
    if (errors!=0){return errors;} else {return 0;}
  }
  //обработка скобок
  solve_work_buffer();
  if (errors!=0){return errors;}

  return 0;
}


void replace_functions(void)
{
//заменяет функции на их знаки
//обработчик работает с функциями-знаками (функция обозначается одним знаком)
uint8_t i=0;
      do
      {
        if ((work_buffer[i]==115)&&(work_buffer[i+1]==105)){cut_from_str(&work_buffer[0],i+1,2);work_buffer_length=work_buffer_length-2;}//sin
        if ((work_buffer[i]==99)&&(work_buffer[i+1]==111)){cut_from_str(&work_buffer[0],i+1,2);work_buffer_length=work_buffer_length-2;}//cos
        if ((work_buffer[i]==116)&&(work_buffer[i+1]==103)){cut_from_str(&work_buffer[0],i+1,1);work_buffer_length=work_buffer_length-1;}//tg
        if ((work_buffer[i]==108)&&(work_buffer[i+1]==110)){cut_from_str(&work_buffer[0],i+1,1);work_buffer_length=work_buffer_length-1;}//ln
        if ((work_buffer[i]==101)&&(work_buffer[i+1]==120)){replace_by_char(&work_buffer[0],i,3,120);work_buffer_length=work_buffer_length-2;}//exp to x иначе e будет воспринято как E
        i++;  
      } while (i<=work_buffer_length-1);
}



void find_numbers(void)
{
  //ищет в рабочем буфере(строке) числа
  //преобразовывает их в double, помещает их в массив numbers[]
  //и заменяет их в массиве символами чисел(А,Б,В,Г.....)
  //также заменяет символы регистров памяти (A B C D)
  
  uint8_t i=0;
  uint8_t chr=0;
  char *end_ptr;//указатель на конец числа
  
  uint32_t str_begin;
  uint32_t str_end;
  
  uint16_t lng;
  uint8_t found=0;
  uint8_t tmp=42;//chr - "*"
  do
  {   
      found=0;
      i=0;
      do
      {
        chr=work_buffer[i];
        i++;
      } while ((isdigit(chr)==0)&&(i<=work_buffer_length-1)&&(is_mem_sumbol(chr)==0)&&(chr!=3));//ищем цифру, симол памяти или пи
      
      if (chr==3)//если нашлось пи
      {
        numbers[work_buffer_num_count]=3.141592654;
        work_buffer_num_count++;
        replace_by_char(&work_buffer[0],i-1,1,(REPLACE_SYMB_CODE-1)+work_buffer_num_count);
        if (is_num_sumbol(work_buffer[i-2])!=0)//если перед пи стоит число
        {
          //add_to_str(&tmp,work_buffer,1,i-1);
          text_insert_string((char*)work_buffer, (char*)&tmp, i-1, 1);
          work_buffer_length++;
        }
        found=1;
      }
      
      if (is_mem_sumbol(chr)!=0)//если нашелся символ регистра
      {
        numbers[work_buffer_num_count]=memory_num[chr-65];
        work_buffer_num_count++;
        replace_by_char(&work_buffer[0],i-1,1,(REPLACE_SYMB_CODE-1)+work_buffer_num_count);
        found=1;
      }
  
      if (isdigit(chr)!=0)//проверка на случай если буфер закончился а цифра не нашлась
      {
        numbers[work_buffer_num_count]=strtod((const char *)&work_buffer[i-1],&end_ptr);
        work_buffer_num_count++;
  
        str_begin = (uint32_t)&work_buffer[i-1];//2017
        str_end = (uint32_t)end_ptr;
        
        lng = (uint16_t)(str_end-str_begin);//длина числа в символах
        replace_by_char(&work_buffer[0],i-1,(uint8_t)lng,(REPLACE_SYMB_CODE-1)+work_buffer_num_count);//REPLACE_SYMB_CODE - номер 'А' в ASCII
        work_buffer_length=work_buffer_length-(uint8_t)lng+1;
        if ((work_buffer[i-2]==2)&&(i>1))//проверка на минус (его код - 2)
        {
          numbers[work_buffer_num_count-1]=numbers[work_buffer_num_count-1]*(-1);
          cut_chr_from_str(&work_buffer[0],i-2);//удалить минус
          work_buffer_length--;          
        }
        found=1;
      }//end if

    } while(found==1);//до тех пор, пока что-то находится
}





void fill_work_buffer(uint8_t *txt, uint8_t length)
{
  //заполняет буфер данными из внешней строки
  //dest, source
  memcpy((char*)work_buffer, (char*)txt, length);
  work_buffer[length+1] = 0;
  work_buffer_length = length;
  
  if (length == 0) //добавляем "0" в пустую строку
  {
    work_buffer[0] = '0';
    work_buffer[1] = 0;
    work_buffer_length = 1;
  }
}

void fill_sub_buffer(uint8_t *txt, uint8_t length)
{
  //заполняет буфер данными из внешней строки
  //dest, source
  memcpy((char*)sub_buffer, (char*)txt, length);
  sub_buffer[length+1] = 0;
  sub_buffer_length = length;
}

void solve_sub_buffer(void)
{
//этот буфер НЕ СОДЕРЖИТ скобок.
  uint8_t i;
  uint8_t chr;
  uint8_t found=0;
  uint8_t level=1;
  
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
          if (is_x_level_func(chr,level)==1){solve_func(i-1);found=1;}//если нашлась функция, вычислить ее
        } while ((i<=sub_buffer_length-1)&&(found==0)&&(errors==0));
    }while ((found==1)&&(errors==0));
    level++;//если во всей подстроке функций такого уровня больше нет
  }while ((level<6)&&(errors==0));
  
  if (errors!=0){return;}
  
  if (sub_buffer_length>1){errors=2;return;}
  if (is_num_sumbol(sub_buffer[0])==1) {answer=numbers[sub_buffer[0]-REPLACE_SYMB_CODE];}else {errors=4;return;}
      
}


void solve_work_buffer(void)
{
  uint8_t i;
  uint8_t chr;

  
  uint8_t sub_begin=0;//начало подстроки
  uint8_t sub_end=0;//конец полстроки
  uint8_t bracket_cnt=0;

    do
    {
        i=0;
        bracket_cnt=0;
        do//проходит символы строки в происках скобок
        {
          chr=work_buffer[i];
          i++;
          if (chr=='('){bracket_cnt++;}
          if (chr==')'){bracket_cnt--;}
          if (bracket_cnt==max_bracket_levell)//ищем скобки максимаьного уровня
          {
            sub_begin=i;
            while (work_buffer[i]!=')'){i++;}//ищем закрывающую скобку
            sub_end=i;
            fill_sub_buffer(&work_buffer[sub_begin],sub_end-sub_begin);
            solve_sub_buffer();
            if (errors==0)
            {
              replace_by_char(&work_buffer[0],sub_begin-1,(uint8_t)(sub_end-sub_begin+2),sub_buffer[0]);
              work_buffer_length=work_buffer_length-(sub_end-sub_begin+1);
              //found=1;
              i=sub_begin;
              bracket_cnt--;
            }
          }
         
        } while ((i<=work_buffer_length-1)&&(errors==0));
        max_bracket_levell--;
    }while ((max_bracket_levell>0)&&(errors==0));
    
  if (errors!=0){return;}
    
  if (work_buffer_length>1)//если скобок нет
  {
    fill_sub_buffer(&work_buffer[0],work_buffer_length);
    solve_sub_buffer();
    if (is_num_sumbol(sub_buffer[0])==1) {answer=numbers[sub_buffer[0]-REPLACE_SYMB_CODE];}else {errors=4;return;}
  }
  else
  {
    if (is_num_sumbol(work_buffer[0])==1) {answer=numbers[work_buffer[0]-REPLACE_SYMB_CODE];}else {errors=4;return;}
  }
      
}




void solve_func(uint8_t pos)
{
//вычисяет функцию расположенную по указанной позиции

  switch (sub_buffer[pos])
  {
  case 43:  solve_plus(pos);break;
  case 45:  solve_minus(pos);break;
  case 42:  solve_multiply(pos);break;
  case 94:  solve_pow(pos);break;
  case 250: solve_dbl(pos);break;
  case 47:  solve_div(pos);break;
  case 115: solve_sin(pos);break;
  case 99:  solve_cos(pos);break;
  case 116: solve_tg(pos);break;
  case SYMB_SQRT_CODE:   solve_sqrt(pos);break;
  case 108: solve_ln(pos);break;
  case 120: solve_exp(pos);break;
  default: break;
    
  }
}



void solve_plus(uint8_t pos)
{
  //проверка, чтобы символы слева и справа от + были числовыми (А Б В Г)
  if ((is_num_sumbol(sub_buffer[pos-1])==1)&&(is_num_sumbol(sub_buffer[pos+1])==1))
  {
    x=numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    y=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=x+y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
    
  }
  else {errors=1;}
}

void solve_minus(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1])==1)&&(is_num_sumbol(sub_buffer[pos+1])==1))
  {
    x=numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    y=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=x-y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
    
  }
  else {errors=1;}
}

void solve_multiply(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1])==1)&&(is_num_sumbol(sub_buffer[pos+1])==1))
  {
    x=numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    y=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=x*y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_div(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1])==1)&&(is_num_sumbol(sub_buffer[pos+1])==1))
  {
    x=numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    y=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    if (y==0) {errors=3;return;}
    z=x/y;
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_pow(uint8_t pos)
{
  if ((is_num_sumbol(sub_buffer[pos-1])==1)&&(is_num_sumbol(sub_buffer[pos+1])==1))
  {
    x=numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    y=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=pow(x,y);
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    
    cut_from_str(&sub_buffer[0],pos,2);
    sub_buffer_length=sub_buffer_length-2;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_dbl(uint8_t pos)//x^2
{
  if (is_num_sumbol(sub_buffer[pos-1])==1)
  {
    x=numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE];
    z=pow(x,2);      
    numbers[sub_buffer[pos-1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_sin(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=sin(x);      
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_cos(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=cos(x);      
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_tg(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=tan(x);      
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_sqrt(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    if (x<0){errors=6;return;}
    z=sqrt(x);      
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_exp(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    z=exp(x);      
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}

void solve_ln(uint8_t pos)
{
  if (is_num_sumbol(sub_buffer[pos+1])==1)
  {
    x=numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE];
    if (x<0){errors=7;return;}
    z=log(x);      
    numbers[sub_buffer[pos+1]-REPLACE_SYMB_CODE]=z;
    cut_from_str(&sub_buffer[0],pos,1);
    sub_buffer_length=sub_buffer_length-1;
    work_buffer_num_count--;
  }
  else {errors=1;}
}







uint8_t is_num_sumbol(uint8_t chr)
{
  //возвращает 1 если символ относится к числам (А Б В ...)
  if ((chr>(REPLACE_SYMB_CODE-1))&&(chr<212)){return 1;}
  return 0;
}

uint8_t is_mem_sumbol(uint8_t chr)
{
  //возвращает 1 если символ относится к символам регистров памяти (A B C D)
  if ((chr>64)&&(chr<69)){return 1;}
  return 0;
}



uint8_t is_x_level_func(uint8_t chr,uint8_t x)
{
uint8_t tmp=0;  
  switch (x)
  {
  case 5: tmp=is_5_level_func(chr);break;
  case 4: tmp=is_4_level_func(chr);break;
  case 3: tmp=is_3_level_func(chr);break;
  case 2: tmp=is_2_level_func(chr);break;
  case 1: tmp=is_1_level_func(chr);break;
  }
return tmp; 
}




uint8_t is_5_level_func(uint8_t chr)
{
  //возвращает 1 если + или -
  if ((chr==43)||(chr==45)){return 1;}
  return 0;
}

uint8_t is_4_level_func(uint8_t chr)
{
  //возвращает 1 если * или /
  if ((chr==42)||(chr==47)){return 1;}
  return 0;
}

uint8_t is_3_level_func(uint8_t chr)
{
  //возвращает 1 если ^
  if (chr==94){return 1;}
  return 0;
}

uint8_t is_2_level_func(uint8_t chr)
{
  //возвращает 1 если ^2
  if (chr==250){return 1;}
  return 0;
}

uint8_t is_1_level_func(uint8_t chr)
{
  //возвращает 1 если c или s или t или sqrt или l или x
  if ((chr==115)||(chr==99)||(chr==116)||(chr==SYMB_SQRT_CODE)||(chr==108)||(chr==120))
    return 1;
  return 0;
}



void bracket_anlyse(void)
{
  //проверяет, чтобы количество скобок было четным и определет макс уровень скобок
  
  uint8_t i;
  uint8_t bracket_cnt=0;
  max_bracket_levell=0;
  
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
    errors=5;//нечетное число скобок
}