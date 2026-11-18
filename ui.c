#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ui.h"
#include "list_manager.h"

// 1. Safe string input using fgets()
int ui_read_line(const char *prompt, char *buffer, size_t size)
{
    if (buffer == NULL || size == 0)
        return 0;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return 0;
    }

    size_t length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
    }
    else
    {
        // Remove remaining characters from input buffer
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            // Discard extra input
        }
    }

    return 1;
}

// 2. Safe integer input
int ui_read_int(const char *prompt, int min, int max, int *value)
{
    char buffer[100];
    char *end;

    while (1)
    {
        if (!ui_read_line(prompt, buffer, sizeof(buffer)))
            return 0;

        errno = 0;
        long number = strtol(buffer, &end, 10);

        if (buffer[0] != '\0' &&
            *end == '\0' &&
            errno != ERANGE &&
            number >= min &&
            number <= max)
        {
            *value = (int)number;
            return 1;
        }

        printf("Invalid input! Enter a number between %d and %d.\n",
               min, max);
    }
}

// 3. Safe decimal input
int ui_read_double(const char *prompt, double min, double max,
                   double *value)
{
    char buffer[100];
    char *end;

    while (1)
    {
        if (!ui_read_line(prompt, buffer, sizeof(buffer)))
            return 0;

        errno = 0;
        double number = strtod(buffer, &end);

        if (buffer[0] != '\0' &&
            *end == '\0' &&
            errno != ERANGE &&
            number >= min &&
            number <= max)
        {
            *value = number;
            return 1;
        }

        printf("Invalid amount! Please enter a valid value.\n");
    }
}

// 4. Clear terminal screen
void ui_clear_screen(void)
{
    printf("\033[2J\033[H");
}

// 5. Pause screen
void ui_pause(void)
{
    char buffer[10];

    printf("\nPress Enter to continue...");
    fgets(buffer, sizeof(buffer), stdin);
}

// 6. Display all expenses in table format
void ui_print_expenses(const ExpenseList *list)
{
    if (list == NULL || list->head == NULL)
    {
        printf("\nNo expenses available.\n");
        return;
    }

    printf("\n");
    printf("-------------------------------------------------------------\n");
    printf("%-8s %-20s %-12s %-10s %-12s\n",
           "ID", "Description", "Paid By", "Amount", "Status");
    printf("-------------------------------------------------------------\n");

    Node *current = list->head;

    while (current != NULL)
    {
        Expense *e = &current->data;

        printf("%-8d %-20.20s %-12.12s Rs.%-7.2f %-12s\n",
               e->expense_id,
               e->description,
               e->paid_by,
               e->total_amount,
               e->status == EXPENSE_SETTLED ?
               "Settled" : "Unsettled");

        current = current->next;
    }

    printf("-------------------------------------------------------------\n");
    printf("Total Expenses: %d\n", list->total_expenses);
}

// 7. Display individual expense details
void ui_print_expense_details(const Expense *expense)
{
    if (expense == NULL)
    {
        printf("\nExpense not found.\n");
        return;
    }

    printf("\n========== EXPENSE DETAILS ==========\n");

    printf("Expense ID       : %d\n", expense->expense_id);
    printf("Description      : %s\n", expense->description);
    printf("Paid By          : %s\n", expense->paid_by);
    printf("Total Amount     : Rs. %.2f\n", expense->total_amount);
    printf("Participants     : %d\n", expense->participant_count);

    printf("Status           : %s\n",
           expense->status == EXPENSE_SETTLED ?
           "Settled" : "Unsettled");

    if (expense->participant_count > 0)
    {
        printf("Share Per Person : Rs. %.2f\n",
               expense->total_amount / expense->participant_count);
    }

    printf("=====================================\n");
}

// 8. Main application menu
int ui_main_menu(void)
{
    int choice;

    printf("\n====================================\n");
    printf("       SIMPLE EXPENSE SPLITTER      \n");
    printf("====================================\n");

    printf("1. List All Active Expenses\n");
    printf("2. Add a New Expense\n");
    printf("3. Select / Search Expense\n");
    printf("4. Generate Summary & Balance Audit Report\n");
    printf("5. Save & Exit\n");

    printf("====================================\n");

    if (!ui_read_int("Enter your choice: ", 1, 5, &choice))
        return 5;

    return choice;
}

// 9. Expense operations menu
int ui_expense_menu(void)
{
    int choice;

    printf("\n====================================\n");
    printf("       EXPENSE OPERATIONS           \n");
    printf("====================================\n");

    printf("1. View Expense Details & Split Share\n");
    printf("2. Mark Expense as Settled\n");
    printf("3. Edit Expense Details\n");
    printf("4. Delete Expense\n");
    printf("5. Return to Main Menu\n");

    printf("====================================\n");

    if (!ui_read_int("Enter your choice: ", 1, 5, &choice))
        return 5;

    return choice;
}

// 10. Ask user for an expense ID
int ui_prompt_expense_id(int *expense_id)
{
    printf("\n");

    return ui_read_int("Enter Expense ID: ",
                       1, 2147483647, expense_id);
}

