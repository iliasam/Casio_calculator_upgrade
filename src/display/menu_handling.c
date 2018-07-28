//Routines that draw something at LCD must be placed here
//Data path is: display_handling.c (generate text) -> lcd_worker.c (text draw) -> ST7565R.c (send image to LCD)

#include "menu_handling.h"

#include "display_handling.h"

#include "text_functions.h"
#include "lcd_worker.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

#define MENU_SAVE_CELL_LIST_CNT 6//Number of items displayed in menu

extern ModeStateType            mode_state;
extern FormulaInputStateType    formula_input_state;

extern CalcAnswerType calc_result;
extern AnswerType answer_type_mode;

MenuSelector1Type menu_selector1_data;

uint8_t save_memory_cell = 0;// Number of cell to save ansver


//#############################################################################


void draw_save_mem_menu(void);
void draw_save_mem_menu_sub(void);
void draw_selector1_menu(void);
void leave_save_mem_menu(void);
void leave_menu_selector1(void);

//*****************************************************************************

void display_draw_menu_handler(void)
{
  switch (mode_state)
  {
    case SELECTOR1_MENU_MODE:
      draw_selector1_menu();
    break;
    
    case SAVE_MEM_MENU_MODE:
      draw_save_mem_menu();
    break;
  }
}

//Save ansver to memory cell menu
void draw_save_mem_menu(void)
{
  draw_caption_bar(FONT_SIZE_8);
  lcd_draw_string(" SAVE VALUE:", 0, 0*FONT_SIZE_8, FONT_SIZE_8, LCD_INVERTED_FLAG);
  draw_answer_in_line(calc_result, 1);
  
  if (calc_result.Error != CACL_ERR_NO)
  {
    lcd_draw_string(" NOTHING TO SAVE", 0, 3*FONT_SIZE_8, FONT_SIZE_8, 0);
    return;
  }
  
  draw_save_mem_menu_sub();
}

//Draw items of the list
void draw_save_mem_menu_sub(void)
{
  uint8_t i;
  char str[32];
  uint8_t tmp_cursor_pos = 0;
  uint8_t items_remain = MEMORY_CELLS_NUMBER - save_memory_cell;
  uint8_t start_item = 0;
  uint8_t font_width =  get_font_width(FONT_SIZE_8);
  uint16_t max_text_lng = (LCD_RIGHT_OFFSET - LCD_LEFT_OFFSET) / get_font_width(FONT_SIZE_8) + 1;//maximum text width in symbols
  
  if (save_memory_cell < (MENU_SAVE_CELL_LIST_CNT / 2))
  {
    tmp_cursor_pos = save_memory_cell;
    start_item = 0;
  }
  else if (items_remain < MENU_SAVE_CELL_LIST_CNT)
  {
    start_item = MEMORY_CELLS_NUMBER - MENU_SAVE_CELL_LIST_CNT;
    tmp_cursor_pos = MENU_SAVE_CELL_LIST_CNT - items_remain;
  }
  else
  {
    start_item = save_memory_cell - (MENU_SAVE_CELL_LIST_CNT / 2);
    tmp_cursor_pos = (MENU_SAVE_CELL_LIST_CNT / 2);
  }
  
  for (i = 0; i < MENU_SAVE_CELL_LIST_CNT; i++)
  {
    memset(str, ' ', max_text_lng);
    uint8_t item = start_item + i;
    
    str[1] = 'A' + item;
    str[2] = ':';
    double tmp_value = str_math_get_from_mem_cell(item);
    draw_value_at_right((uint8_t*)str, tmp_value, (uint8_t)max_text_lng);

    lcd_draw_string(str, 0, (i + 2) * FONT_SIZE_8, FONT_SIZE_8, 0);
  }
  lcd_draw_char('>', 0, (tmp_cursor_pos + 2) * FONT_SIZE_8, FONT_SIZE_8, 0);//cursor
  
  //sprintf(str, "item: %d", (uint8_t)save_memory_cell);
  //lcd_draw_string(str, 0, 1 * FONT_SIZE_8, FONT_SIZE_8, 0);
  
}

void draw_selector1_menu(void)
{
  uint8_t i;
  uint8_t font_width =  get_font_width(FONT_SIZE_8);
  draw_caption_bar(FONT_SIZE_8);
  lcd_draw_string(menu_selector1_data.menu_caption, 0, 0*FONT_SIZE_8, FONT_SIZE_8, LCD_INVERTED_FLAG);
  
  for (i=0; i < menu_selector1_data.number_of_items; i++)
  {
    if (menu_selector1_data.selected_item == i)
      lcd_draw_char('>', 0, (i+1)*FONT_SIZE_8, FONT_SIZE_8, 0);//cursor
    
    lcd_draw_char(((uint8_t)'0' + i + 1), font_width, (i+1)*FONT_SIZE_8, FONT_SIZE_8, 0);//print item number
    lcd_draw_char(':', font_width*2, (i+1)*FONT_SIZE_8, FONT_SIZE_8, 0);
    
    lcd_draw_string(menu_selector1_data.item_names[i], (font_width*3), (i+1)*FONT_SIZE_8, FONT_SIZE_8, 0);
  }
}

void prepare_menu_selector1_for_answer_mode(void)
{
  static const char menu_name[] = "ANSWER TYPE:";
  static const char item0[] = "NORMAL";
  static const char item1[] = "SCIENCE";
  static const char item2[] = "ENGINEERING";
  
  menu_selector1_data.menu_caption = (char*)menu_name;
  menu_selector1_data.number_of_items = 3;
  menu_selector1_data.selected_item = (uint8_t)answer_type_mode;
  menu_selector1_data.item_names[0] = (char*)item0;
  menu_selector1_data.item_names[1] = (char*)item1;
  menu_selector1_data.item_names[2] = (char*)item2;
  menu_selector1_data.menu_type     = MENU_SELECTOR1_ANSWER_MODE;
}

void menu_move_cursor_down(void)
{
  if (mode_state == SELECTOR1_MENU_MODE)
  {
    menu_selector1_data.selected_item++;
    if (menu_selector1_data.selected_item > (menu_selector1_data.number_of_items-1))
      menu_selector1_data.selected_item = menu_selector1_data.number_of_items-1;
  }
  else if (mode_state == SAVE_MEM_MENU_MODE)
  {
    save_memory_cell++;
    if (save_memory_cell > (MEMORY_CELLS_NUMBER - 1))
      save_memory_cell = MEMORY_CELLS_NUMBER - 1;
  }
}

void menu_move_cursor_up(void)
{
  if (mode_state == SELECTOR1_MENU_MODE)
  {
    if (menu_selector1_data.selected_item > 0)
      menu_selector1_data.selected_item--;
  }
  else if (mode_state == SAVE_MEM_MENU_MODE)
  {
    if (save_memory_cell > 0)
      save_memory_cell--;
  }
}

void menu_exit(void)
{
  mode_state = FORMULA_INPUT;
}

//When "EXE" pressed
void menu_leave(void)
{
  if (mode_state == SELECTOR1_MENU_MODE)
    leave_menu_selector1();
  else if (mode_state == SAVE_MEM_MENU_MODE)
    leave_save_mem_menu();
}

void leave_menu_selector1(void)
{
  if (menu_selector1_data.menu_type == MENU_SELECTOR1_ANSWER_MODE)
  {
    answer_type_mode = (AnswerType)menu_selector1_data.selected_item;
    mode_state = FORMULA_INPUT;
  }
}

void leave_save_mem_menu(void)
{
  if (calc_result.Error == CACL_ERR_NO)
  {
    str_math_save_to_mem_cell(save_memory_cell, calc_result.Answer);
  }
  
  mode_state = FORMULA_INPUT;
}
