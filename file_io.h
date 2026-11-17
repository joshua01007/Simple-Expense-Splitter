#ifndef FILE_IO_H
#define FILE_IO_H
#include "list_manager.h"

void load_expenses_from_file(ExpenseList *list);
void save_expenses_to_file(ExpenseList *list);

#endif