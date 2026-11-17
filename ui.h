#ifndef UI_H
#define UI_H

#include "list_manager.h"
#include <stddef.h>

// Safe input functions
int ui_read_line(const char *prompt, char *buffer, size_t size);
int ui_read_int(const char *prompt, int min, int max, int *value);
int ui_read_double(const char *prompt, double min, double max, double *value);

// Screen utilities
void ui_clear_screen(void);
void ui_pause(void);

// Display functions
void ui_print_expenses(const ExpenseList *list);
void ui_print_expense_details(const Expense *expense);

// Menu functions
int ui_main_menu(void);
int ui_expense_menu(void);

// Expense selection
int ui_prompt_expense_id(int *expense_id);

#endif
