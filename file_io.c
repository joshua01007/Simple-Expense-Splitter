#include <stdio.h>
#include <stdlib.h>
#include "file_io.h"
#include "list_manager.h"

// Load expenses from file
void load_expenses_from_file(ExpenseList *list)
{
    if (list == NULL)
    {
        return;
    }

    FILE *fp = fopen("expenses.dat", "rb");

    if (fp == NULL)
    {
        printf("No previous expenses found.\n");
        return;
    }

    Expense temp;

    while (fread(&temp, sizeof(Expense), 1, fp) == 1)
    {
        Node *newNode = malloc(sizeof(Node));

        if (newNode == NULL)
        {
            printf("Memory allocation failed.\n");
            fclose(fp);
            return;
        }

        newNode->data = temp;
        newNode->next = NULL;

        // Insert node into linked list
        if (list->head == NULL)
        {
            list->head = newNode;
        }
        else
        {
            Node *current = list->head;

            while (current->next != NULL)
            {
                current = current->next;
            }

            current->next = newNode;
        }

        list->total_expenses++;
    }

    if (ferror(fp))
    {
        printf("Error reading expenses file.\n");
    }

    fclose(fp);

    printf("Expenses loaded successfully.\n");
}


// Save expenses to file
void save_expenses_to_file(ExpenseList *list)
{
    if (list == NULL)
    {
        return;
    }

    FILE *fp = fopen("expenses.dat", "wb");

    if (fp == NULL)
    {
        printf("Unable to open expenses file.\n");
        return;
    }

    Node *current = list->head;

    while (current != NULL)
    {
        if (fwrite(&current->data, sizeof(Expense), 1, fp) != 1)
        {
            printf("Error saving expenses.\n");
            fclose(fp);
            return;
        }

        current = current->next;
    }

    fclose(fp);

    printf("Expenses saved successfully.\n");
}