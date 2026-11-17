
#include "settlements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double calculate_share(const Expense *expense) {
    if (expense == NULL || expense->participant_count <= 0 ||
        expense->total_amount <= 0) {
        return 0.0;
    }

    return expense->total_amount / expense->participant_count;
}

int init_settlement(Settlement *settlement, Expense *expense,
                    const char *payer) {
    if (settlement == NULL || expense == NULL || payer == NULL ||
        payer[0] == '\0' || expense->participant_count <= 0 ||
        expense->total_amount <= 0 ||
        expense->status != EXPENSE_UNSETTLED) {
        return 0;
    }

    settlement->expense = expense;
    settlement->participants = NULL;
    settlement->participant_count = 0;
    settlement->share = calculate_share(expense);

    if (strlen(payer) >= sizeof(settlement->payer)) {
        return 0;
    }

    strcpy(settlement->payer, payer);

    return add_participant(settlement, payer);
}

int add_participant(Settlement *settlement, const char *name) {
    if (settlement == NULL || name == NULL || name[0] == '\0' ||
        settlement->expense == NULL ||
        settlement->participant_count >=
        settlement->expense->participant_count ||
        strlen(name) >= 50) {
        return 0;
    }

    ParticipantPayment *current = settlement->participants;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return 0;
        }
        current = current->next;
    }

    ParticipantPayment *new_participant =
        malloc(sizeof(ParticipantPayment));

    if (new_participant == NULL) {
        return 0;
    }

    strcpy(new_participant->name, name);
    new_participant->share = settlement->share;
    new_participant->amount_paid =
        (strcmp(name, settlement->payer) == 0)
        ? settlement->share : 0.0;

    new_participant->next = settlement->participants;
    settlement->participants = new_participant;
    settlement->participant_count++;

    return 1;
}

int record_payment(Settlement *settlement, const char *name,
                   double amount) {
    if (settlement == NULL || name == NULL ||
        amount <= 0 || !isfinite(amount) ||
        settlement->expense == NULL ||
        settlement->expense->status == EXPENSE_SETTLED ||
        settlement->participant_count !=
        settlement->expense->participant_count) {
        return 0;
    }

    ParticipantPayment *current = settlement->participants;

    while (current != NULL && strcmp(current->name, name) != 0) {
        current = current->next;
    }

    if (current == NULL ||
        strcmp(name, settlement->payer) == 0) {
        return 0;
    }

    double remaining = current->share - current->amount_paid;

    if (amount > remaining + 0.000001) {
        return 0;
    }

    current->amount_paid += amount;

    if (current->share - current->amount_paid < 0.000001) {
        current->amount_paid = current->share;
    }

    int fully_paid = 1;

    ParticipantPayment *participant = settlement->participants;

    while (participant != NULL) {
        if (participant->amount_paid + 0.000001 <
            participant->share) {
            fully_paid = 0;
            break;
        }
        participant = participant->next;
    }

    if (fully_paid) {
        settlement->expense->status = EXPENSE_SETTLED;
    }

    return 1;
}

void print_settlement(const Settlement *settlement) {
    if (settlement == NULL || settlement->expense == NULL) {
        return;
    }

    printf("\n--- SETTLEMENT DETAILS ---\n");
    printf("Expense ID: %d\n", settlement->expense->expense_id);
    printf("Payer: %s\n", settlement->payer);
    printf("Total amount: Rs. %.2f\n",
           settlement->expense->total_amount);
    printf("Share per participant: Rs. %.2f\n",
           settlement->share);

    ParticipantPayment *current = settlement->participants;

    while (current != NULL) {
        printf("\nParticipant: %s\n", current->name);
        printf("Share: Rs. %.2f\n", current->share);
        printf("Paid: Rs. %.2f\n", current->amount_paid);
        printf("Remaining: Rs. %.2f\n",
               current->share - current->amount_paid);

        current = current->next;
    }

    printf("\nStatus: %s\n",
        settlement->expense->status == EXPENSE_SETTLED
        ? "Settled" : "Unsettled");
}

void free_settlement(Settlement *settlement) {
    if (settlement == NULL) {
        return;
    }

    ParticipantPayment *current = settlement->participants;

    while (current != NULL) {
        ParticipantPayment *next = current->next;
        free(current);
        current = next;
    }

    settlement->participants = NULL;
    settlement->participant_count = 0;
}
