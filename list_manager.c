#include "list_manager.h"

void init_expens_list(ExpenseList *list){
    if(list == NULL) return;
    list -> head = NULL;
    list -> total_expenses = 0;
}



