//������ ���������� �������������� ���������


#include "str_math.h"
#include "stdio.h"
#include "stdint.h"
#include "text_functions.h"
#include "main.h"

//All number are replaced by symbols started from this code
//2+2 -> 192+193
#define REPLACE_SYMB_CODE       192


#include "str_operate.h"//������ �� ��������
#include "math.h"
#include <ctype.h>

uint8_t work_buffer[50];//�����, � ������� ��������� �������������� ������
uint8_t work_buffer_length=0;//���������� �������� � ������

uint8_t sub_buffer[50];//�����, � ������� ��������� �������������� ���������
uint8_t sub_buffer_length=0;//���������� �������� � ������ ���������

double numbers[30];//�����, ���������� �� �������� ������
uint8_t work_buffer_num_count=0;//���������� ����� � ������

double memory_num[4];//������ - ���������� A B C D

uint8_t errors=0;

double x;//�������� ������� ��������
double y;
double z;

uint8_t max_bracket_levell;//������������ ������� ������ ������� ������

double answer = 0.0;

//��������� �������� �� ������� txt, length - ����� ������� � ��������
//����� ���������� � ���������� answer
//������� ���������� 0, ���� ������ ���, ���� ��� ������

uint8_t solve(uint8_t *txt,uint8_t length)
{

  work_buffer_num_count=0;
  errors=0;
  
  fill_work_buffer(txt,length);
  replace_functions();
  find_numbers();//�������� ����� ���������� ��������� - � � � ...
  bracket_anlyse();//��������� �������� ������
  if (errors!=0){return errors;}
  if (max_bracket_levell==0)//���� ������ ���
  {
    fill_sub_buffer(&work_buffer[0],work_buffer_length);
    solve_sub_buffer();
    if (errors!=0){return errors;} else {return 0;}
  }
  //��������� ������
  solve_work_buffer();
  if (errors!=0){return errors;}

  return 0;
}


void replace_functions(void)
{
//�������� ������� �� �� �����
//���������� �������� � ���������-������� (������� ������������ ����� ������)
uint8_t i=0;
      do
      {
        if ((work_buffer[i]==115)&&(work_buffer[i+1]==105)){cut_from_str(&work_buffer[0],i+1,2);work_buffer_length=work_buffer_length-2;}//sin
        if ((work_buffer[i]==99)&&(work_buffer[i+1]==111)){cut_from_str(&work_buffer[0],i+1,2);work_buffer_length=work_buffer_length-2;}//cos
        if ((work_buffer[i]==116)&&(work_buffer[i+1]==103)){cut_from_str(&work_buffer[0],i+1,1);work_buffer_length=work_buffer_length-1;}//tg
        if ((work_buffer[i]==108)&&(work_buffer[i+1]==110)){cut_from_str(&work_buffer[0],i+1,1);work_buffer_length=work_buffer_length-1;}//ln
        if ((work_buffer[i]==101)&&(work_buffer[i+1]==120)){replace_by_char(&work_buffer[0],i,3,120);work_buffer_length=work_buffer_length-2;}//exp to x ����� e ����� ���������� ��� E
        i++;  
      } while (i<=work_buffer_length-1);
}



void find_numbers(void)
{
  //���� � ������� ������(������) �����
  //��������������� �� � double, �������� �� � ������ numbers[]
  //� �������� �� � ������� ��������� �����(�,�,�,�.....)
  //����� �������� ������� ��������� ������ (A B C D)
  
  uint8_t i=0;
  uint8_t chr=0;
  char *end_ptr;//��������� �� ����� �����
  
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
      } while ((isdigit(chr)==0)&&(i<=work_buffer_length-1)&&(is_mem_sumbol(chr)==0)&&(chr!=3));//���� �����, ����� ������ ��� ��
      
      if (chr==3)//���� ������� ��
      {
        numbers[work_buffer_num_count]=3.141592654;
        work_buffer_num_count++;
        replace_by_char(&work_buffer[0],i-1,1,(REPLACE_SYMB_CODE-1)+work_buffer_num_count);
        if (is_num_sumbol(work_buffer[i-2])!=0)//���� ����� �� ����� �����
        {
          //add_to_str(&tmp,work_buffer,1,i-1);
          text_insert_string((char*)work_buffer, (char*)&tmp, i-1, 1);
          work_buffer_length++;
        }
        found=1;
      }
      
      if (is_mem_sumbol(chr)!=0)//���� ������� ������ ��������
      {
        numbers[work_buffer_num_count]=memory_num[chr-65];
        work_buffer_num_count++;
        replace_by_char(&work_buffer[0],i-1,1,(REPLACE_SYMB_CODE-1)+work_buffer_num_count);
        found=1;
      }
  
      if (isdigit(chr)!=0)//�������� �� ������ ���� ����� ���������� � ����� �� �������
      {
        numbers[work_buffer_num_count]=strtod((const char *)&work_buffer[i-1],&end_ptr);
        work_buffer_num_count++;
  
        str_begin = (uint32_t)&work_buffer[i-1];//2017
        str_end = (uint32_t)end_ptr;
        
        lng = (uint16_t)(str_end-str_begin);//����� ����� � ��������
        replace_by_char(&work_buffer[0],i-1,(uint8_t)lng,(REPLACE_SYMB_CODE-1)+work_buffer_num_count);//REPLACE_SYMB_CODE - ����� '�' � ASCII
        work_buffer_length=work_buffer_length-(uint8_t)lng+1;
        if ((work_buffer[i-2]==2)&&(i>1))//�������� �� ����� (��� ��� - 2)
        {
          numbers[work_buffer_num_count-1]=numbers[work_buffer_num_count-1]*(-1);
          cut_chr_from_str(&work_buffer[0],i-2);//������� �����
          work_buffer_length--;          
        }
        found=1;
      }//end if

    } while(found==1);//�� ��� ���, ���� ���-�� ���������
}





void fill_work_buffer(uint8_t *txt, uint8_t length)
{
  //��������� ����� ������� �� ������� ������
  //dest, source
  memcpy((char*)work_buffer, (char*)txt, length);
  work_buffer[length+1] = 0;
  work_buffer_length = length;
  
  if (length == 0) //��������� "0" � ������ ������
  {
    work_buffer[0] = '0';
    work_buffer[1] = 0;
    work_buffer_length = 1;
  }
}

void fill_sub_buffer(uint8_t *txt, uint8_t length)
{
  //��������� ����� ������� �� ������� ������
  //dest, source
  memcpy((char*)sub_buffer, (char*)txt, length);
  sub_buffer[length+1] = 0;
  sub_buffer_length = length;
}

void solve_sub_buffer(void)
{
//���� ����� �� �������� ������.
  uint8_t i;
  uint8_t chr;
  uint8_t found=0;
  uint8_t level=1;
  
  do
  {
    do//�������� ������, ���� ���������� ������� ������� ������
    {
        found=0;
        i=0;
        do//�������� ������� ������ � �������� ������� X ������
        {
          chr=sub_buffer[i];
          i++;
          if (is_x_level_func(chr,level)==1){solve_func(i-1);found=1;}//���� ������� �������, ��������� ��
        } while ((i<=sub_buffer_length-1)&&(found==0)&&(errors==0));
    }while ((found==1)&&(errors==0));
    level++;//���� �� ���� ��������� ������� ������ ������ ������ ���
  }while ((level<6)&&(errors==0));
  
  if (errors!=0){return;}
  
  if (sub_buffer_length>1){errors=2;return;}
  if (is_num_sumbol(sub_buffer[0])==1) {answer=numbers[sub_buffer[0]-REPLACE_SYMB_CODE];}else {errors=4;return;}
      
}


void solve_work_buffer(void)
{
  uint8_t i;
  uint8_t chr;

  
  uint8_t sub_begin=0;//������ ���������
  uint8_t sub_end=0;//����� ���������
  uint8_t bracket_cnt=0;

    do
    {
        i=0;
        bracket_cnt=0;
        do//�������� ������� ������ � �������� ������
        {
          chr=work_buffer[i];
          i++;
          if (chr=='('){bracket_cnt++;}
          if (chr==')'){bracket_cnt--;}
          if (bracket_cnt==max_bracket_levell)//���� ������ ������������ ������
          {
            sub_begin=i;
            while (work_buffer[i]!=')'){i++;}//���� ����������� ������
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
    
  if (work_buffer_length>1)//���� ������ ���
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
//�������� ������� ������������� �� ��������� �������

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
  //��������, ����� ������� ����� � ������ �� + ���� ��������� (� � � �)
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
  //���������� 1 ���� ������ ��������� � ������ (� � � ...)
  if ((chr>(REPLACE_SYMB_CODE-1))&&(chr<212)){return 1;}
  return 0;
}

uint8_t is_mem_sumbol(uint8_t chr)
{
  //���������� 1 ���� ������ ��������� � �������� ��������� ������ (A B C D)
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
  //���������� 1 ���� + ��� -
  if ((chr==43)||(chr==45)){return 1;}
  return 0;
}

uint8_t is_4_level_func(uint8_t chr)
{
  //���������� 1 ���� * ��� /
  if ((chr==42)||(chr==47)){return 1;}
  return 0;
}

uint8_t is_3_level_func(uint8_t chr)
{
  //���������� 1 ���� ^
  if (chr==94){return 1;}
  return 0;
}

uint8_t is_2_level_func(uint8_t chr)
{
  //���������� 1 ���� ^2
  if (chr==250){return 1;}
  return 0;
}

uint8_t is_1_level_func(uint8_t chr)
{
  //���������� 1 ���� c ��� s ��� t ��� sqrt ��� l ��� x
  if ((chr==115)||(chr==99)||(chr==116)||(chr==SYMB_SQRT_CODE)||(chr==108)||(chr==120))
    return 1;
  return 0;
}



void bracket_anlyse(void)
{
  //���������, ����� ���������� ������ ���� ������ � ��������� ���� ������� ������
  
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
    errors=5;//�������� ����� ������
}