#ifndef LIST_MANAGER_H
#define LIST_MANAGER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
EXPENSE_UNSETTLED,
EXPENSE_SETTLED
} ExpenseStatus;

typedef struct Expense {
int expense_id; // Unique identifier (e.g., 5001)
char description[50]; // Item details (e.g., "Dinner at Hotel", "Cab Fare")
char paid_by[50]; // Name of the person who paid upfront
double total_amount; // Total bill amount in INR (₹)
int participant_count; // Number of people sharing this expense
ExpenseStatus status; // EXPENSE_UNSETTLED or EXPENSE_SETTLED
} Expense;

typedef struct Node {
Expense data;
struct Node *next;
} Node;

typedef struct {
Node *head;
int total_expenses;
} ExpenseList;

#endif