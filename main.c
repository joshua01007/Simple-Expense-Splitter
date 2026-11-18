#include <stdio.h>
#include <stdlib.h>
#include "list_manager.h"
#include "file_io.h"
#include "ui.h"
#include "analytics.h"

int main(void) {
    ExpenseList app_list;
    init_expense_list(&app_list);

    // Startup: Load disk records into the linked list
    load_expenses_from_file(&app_list);

    int running = 1;
    while (running) {
        ui_clear_screen();
        int choice = ui_main_menu();

        switch (choice) {
            case 1: // List All Active Expenses
                ui_print_expenses(&app_list);
                ui_pause();
                break;

            case 2: { // Add a New Expense
                Expense new_exp;
                printf("\n--- Add New Expense ---\n");
                ui_read_int("Enter Expense ID: ", 1, 999999, &new_exp.expense_id);
                ui_read_line("Enter Description: ", new_exp.description, sizeof(new_exp.description));
                ui_read_line("Enter Paid By: ", new_exp.paid_by, sizeof(new_exp.paid_by));
                ui_read_double("Enter Total Amount: ", 0.01, 1000000.0, &new_exp.total_amount);
                ui_read_int("Enter Participant Count: ", 1, 100, &new_exp.participant_count);
                new_exp.status = EXPENSE_UNSETTLED;

                add_expense_node(&app_list, new_exp);
                printf("Expense successfully added to memory.\n");
                ui_pause();
                break;
            }

            case 3: { // Select / Search Expense
                int search_type;
                printf("\n1. Select Expense by ID\n2. Search by Payer\n");
                if (ui_read_int("Choose an option: ", 1, 2, &search_type)) {
                    if (search_type == 1) {
                        int exp_id;
                        if (ui_prompt_expense_id(&exp_id)) {
                            Node *found = find_expense_by_id(&app_list, exp_id);

                            if (found != NULL) {
                                int expense_menu_running = 1;
                                while (expense_menu_running) {
                                    ui_clear_screen();
                                    ui_print_expense_details(&found->data);

                                    int exp_choice = ui_expense_menu();
                                    switch (exp_choice) {
                                        case 1: // View Details & Split Share
                                            ui_print_expense_details(&found->data);
                                            ui_pause();
                                            break;
                                        case 2: // Mark Expense as Settled
                                            if (found->data.status == EXPENSE_UNSETTLED) {
                                                found->data.status = EXPENSE_SETTLED;
                                                printf("Expense %d marked as SETTLED.\n", exp_id);
                                            } else {
                                                printf("Expense is already settled.\n");
                                            }
                                            ui_pause();
                                            break;
                                        case 3: // Edit Expense Details
                                            ui_read_line("Enter New Description: ", found->data.description, sizeof(found->data.description));
                                            printf("Expense successfully updated.\n");
                                            ui_pause();
                                            break;
                                        case 4: // Delete Expense
                                            remove_expense_node(&app_list, exp_id);
                                            printf("Expense %d has been removed.\n", exp_id);
                                            expense_menu_running = 0;
                                            ui_pause();
                                            break;
                                        case 5: // Return to Main Menu
                                            expense_menu_running = 0;
                                            break;
                                    }
                                }
                            } else {
                                printf("No active expense found with ID %d.\n", exp_id);
                                ui_pause();
                            }
                        }
                    } else if (search_type == 2) {
                        char payer[50];
                        ui_read_line("Enter Payer Name: ", payer, sizeof(payer));
                        search_by_payer(&app_list, payer);
                        ui_pause();
                    }
                }
                break;
            }

            case 4: // Generate Summary & Balance Audit Report
                generate_summary_report(&app_list, "summary_audit.txt");
                ui_pause();
                break;

            case 5: // Save & Exit
                save_expenses_to_file(&app_list);
                free_expense_list(&app_list);
                running = 0;
                printf("Data saved successfully. Exiting Application...\n");
                break;
        }
    }
    return 0;
}