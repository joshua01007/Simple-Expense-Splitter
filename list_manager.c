#include "list_manager.h"


void init_expense_list(ExpenseList *list) {
    if (list == NULL) return; 
    list->head = NULL;
    list->total_expenses = 0;
}


Node* create_node(Expense expense_data) {
    Node *new_node = (Node*) malloc(sizeof(Node));
    
    
    if (new_node == NULL) {
        printf("Error: Dynamic memory allocation failed for new node!\n");
        return NULL;
    }

    new_node->data = expense_data;
    new_node->next = NULL;
    return new_node;
}


void add_expense_node(ExpenseList *list, Expense expense_data) {
    if (list == NULL) return;

    Node *new_node = create_node(expense_data);
    if (new_node == NULL) return; 

    
    if (list->head == NULL) {
        list->head = new_node;
    } 
   
    else {
        Node *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    list->total_expenses++;
}


Node* find_expense_by_id(ExpenseList *list, int expense_id) {
    if (list == NULL) return NULL;

    Node *current = list->head;
    while (current != NULL) {
        if (current->data.expense_id == expense_id) {
            return current; 
        }
        current = current->next;
    }
    return NULL; 
}


int remove_expense_node(ExpenseList *list, int expense_id) {
    if (list == NULL || list->head == NULL) return 0;

    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL) {
        if (current->data.expense_id == expense_id) {
            
            if (previous == NULL) {
                list->head = current->next;
            } 
            
            else {
                previous->next = current->next;
            }

            free(current); 
            list->total_expenses--;
            return 1; 
        }
        previous = current;
        current = current->next;
    }

    return 0; 
}


void free_expense_list(ExpenseList *list) {
    if (list == NULL) return;

    Node *current = list->head;
    Node *next_node = NULL;

    while (current != NULL) {
        next_node = current->next; 
        free(current);             
        current = next_node;
    }

    list->head = NULL;
    list->total_expenses = 0;
}


