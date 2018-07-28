#ifndef __MENU_HANDLING_H
#define __MENU_HANDLING_H
#include "stdint.h"
#include "str_math.h"


typedef enum
{
  MENU_SELECTOR1_ANSWER_MODE = 0,
  MENU_SELECTOR1_DUMMY,
} MenuSelector1ModeType;

typedef struct
{
  char*                         menu_caption;
  uint8_t                       number_of_items;
  uint8_t                       selected_item;
  MenuSelector1ModeType         menu_type;
  char*                         item_names[10];
} MenuSelector1Type;

void display_draw_menu_handler(void);


void menu_move_cursor_down(void);
void menu_move_cursor_up(void);
void menu_leave(void);
void menu_exit(void);

void draw_selector1_menu(void);

void prepare_menu_selector1_for_answer_mode(void);

#endif
