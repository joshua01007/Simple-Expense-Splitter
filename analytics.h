#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <stdio.h>
#include "list_manager.h"

/* Search by exact expense ID. Returns the node, or NULL if not found. */
Node *search_by_id(const ExpenseList *list, int expense_id);

/* Case-insensitive substring search on paid_by. Prints every match
 * and returns how many were found. */
int search_by_payer(const ExpenseList *list, const char *query);

/* Sorts the list in place by total_amount (descending). Returns 0 on
 * success, -1 if memory allocation failed (list is left unchanged). */
int sort_by_amount(ExpenseList *list);

/* Writes the summary & balance audit report to an open stream
 * (use stdout for the screen, or a FILE* for a file). */
void write_summary_report(const ExpenseList *list, FILE *out);

/* Writes the report to a file (e.g. "summary_report.txt").
 * Returns 0 on success, -1 on failure. */
int generate_summary_report(const ExpenseList *list, const char *filename);

#endif
