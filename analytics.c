#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "analytics.h"

/* ---------- helpers ---------- */

static void to_lower_copy(const char *src, char *dst, size_t size)
{
    size_t i;
    for (i = 0; i + 1 < size && src[i] != '\0'; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static const char *status_text(ExpenseStatus s)
{
    return (s == EXPENSE_SETTLED) ? "SETTLED" : "UNSETTLED";
}

/* Share per person; guards against divide-by-zero. */
static double share_of(const Expense *e)
{
    if (e->participant_count <= 0) {
        return e->total_amount;
    }
    return e->total_amount / e->participant_count;
}

/* ---------- searching ---------- */

Node *search_by_id(const ExpenseList *list, int expense_id)
{
    if (list == NULL) {
        return NULL;
    }
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        if (cur->data.expense_id == expense_id) {
            return cur;
        }
    }
    return NULL;
}

int search_by_payer(const ExpenseList *list, const char *query)
{
    char needle[50];
    char hay[50];
    int found = 0;

    if (list == NULL || query == NULL || query[0] == '\0') {
        return 0;
    }
    to_lower_copy(query, needle, sizeof needle);

    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        to_lower_copy(cur->data.paid_by, hay, sizeof hay);
        if (strstr(hay, needle) != NULL) {
            if (found == 0) {
                printf("%-6s %-25s %-15s %12s %-10s\n",
                       "ID", "Description", "Paid By", "Amount (INR)", "Status");
            }
            printf("%-6d %-25.25s %-15.15s %12.2f %-10s\n",
                   cur->data.expense_id, cur->data.description,
                   cur->data.paid_by, cur->data.total_amount,
                   status_text(cur->data.status));
            found++;
        }
    }
    return found;
}

/* ---------- sorting ---------- */

static int cmp_amount_desc(const void *a, const void *b)
{
    const Expense *x = (const Expense *)a;
    const Expense *y = (const Expense *)b;
    if (x->total_amount < y->total_amount) return 1;
    if (x->total_amount > y->total_amount) return -1;
    return (x->expense_id > y->expense_id) - (x->expense_id < y->expense_id);
}

/* Copy data into an array, qsort it, copy it back. Nodes are never
 * relinked or freed, so pointers held by other modules stay valid. */
int sort_by_amount(ExpenseList *list)
{
    int n = 0;
    Expense *arr;
    Node *cur;

    if (list == NULL || list->head == NULL || list->head->next == NULL) {
        return 0; /* nothing to sort */
    }
    for (cur = list->head; cur != NULL; cur = cur->next) {
        n++;
    }
    arr = malloc((size_t)n * sizeof *arr);
    if (arr == NULL) {
        return -1;
    }
    int i = 0;
    for (cur = list->head; cur != NULL; cur = cur->next) {
        arr[i++] = cur->data;
    }
    qsort(arr, (size_t)n, sizeof *arr, cmp_amount_desc);
    i = 0;
    for (cur = list->head; cur != NULL; cur = cur->next) {
        cur->data = arr[i++];
    }
    free(arr);
    return 0;
}

/* ---------- reporting ---------- */

typedef struct {
    char name[50];
    double total_paid;      /* everything they have ever paid upfront     */
    double pending_owed;    /* what others still owe them (unsettled only) */
} PayerSummary;

/* "Rahul" and "rahul" are the same person. */
static int names_equal_ignore_case(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

static PayerSummary *find_or_add_payer(PayerSummary **arr, int *count,
                                       int *cap, const char *name)
{
    for (int i = 0; i < *count; i++) {
        if (names_equal_ignore_case((*arr)[i].name, name)) {
            return &(*arr)[i];
        }
    }
    if (*count == *cap) {
        int new_cap = (*cap == 0) ? 8 : *cap * 2;
        PayerSummary *tmp = realloc(*arr, (size_t)new_cap * sizeof **arr);
        if (tmp == NULL) {
            return NULL;
        }
        *arr = tmp;
        *cap = new_cap;
    }
    PayerSummary *p = &(*arr)[*count];
    strncpy(p->name, name, sizeof p->name - 1);
    p->name[sizeof p->name - 1] = '\0';
    p->total_paid = 0.0;
    p->pending_owed = 0.0;
    (*count)++;
    return p;
}

void write_summary_report(const ExpenseList *list, FILE *out)
{
    int settled = 0, unsettled = 0, count = 0, cap = 0;
    double total_all = 0.0, total_settled = 0.0, total_unsettled = 0.0;
    PayerSummary *payers = NULL;
    time_t now = time(NULL);

    if (out == NULL) {
        return;
    }
    fprintf(out, "==============================================\n");
    fprintf(out, "   EXPENSE SUMMARY & BALANCE AUDIT REPORT\n");
    fprintf(out, "   Generated: %s", ctime(&now));
    fprintf(out, "==============================================\n\n");

    if (list == NULL || list->head == NULL) {
        fprintf(out, "No expenses recorded.\n");
        return;
    }

    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        const Expense *e = &cur->data;
        total_all += e->total_amount;
        if (e->status == EXPENSE_SETTLED) {
            settled++;
            total_settled += e->total_amount;
        } else {
            unsettled++;
            total_unsettled += e->total_amount;
        }

        PayerSummary *p = find_or_add_payer(&payers, &count, &cap, e->paid_by);
        if (p == NULL) {
            fprintf(out, "Error: out of memory while building report.\n");
            free(payers);
            return;
        }
        p->total_paid += e->total_amount;
        if (e->status == EXPENSE_UNSETTLED) {
            p->pending_owed += e->total_amount - share_of(e);
        }
    }

    fprintf(out, "Overall\n-------\n");
    fprintf(out, "Total expenses      : %d\n", settled + unsettled);
    fprintf(out, "Settled             : %d (INR %.2f)\n", settled, total_settled);
    fprintf(out, "Unsettled           : %d (INR %.2f)\n", unsettled, total_unsettled);
    fprintf(out, "Grand total         : INR %.2f\n\n", total_all);

    fprintf(out, "Per-payer balances\n------------------\n");
    fprintf(out, "%-20s %15s %20s\n", "Payer", "Total Paid", "Still Owed To Them");
    for (int i = 0; i < count; i++) {
        fprintf(out, "%-20.20s %15.2f %20.2f\n",
                payers[i].name, payers[i].total_paid, payers[i].pending_owed);
    }

    fprintf(out, "\nUnsettled expense details\n-------------------------\n");
    if (unsettled == 0) {
        fprintf(out, "(none - everything is settled)\n");
    }
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        const Expense *e = &cur->data;
        if (e->status == EXPENSE_UNSETTLED) {
            fprintf(out, "#%d %-20.20s paid by %-12.12s INR %.2f (%d people, INR %.2f each)\n",
                    e->expense_id, e->description, e->paid_by,
                    e->total_amount, e->participant_count, share_of(e));
        }
    }
    free(payers);
}

int generate_summary_report(const ExpenseList *list, const char *filename)
{
    FILE *fp;
    if (filename == NULL) {
        return -1;
    }
    fp = fopen(filename, "w");
    if (fp == NULL) {
        return -1;
    }
    write_summary_report(list, fp);
    if (fclose(fp) != 0) {
        return -1;
    }
    return 0;
}