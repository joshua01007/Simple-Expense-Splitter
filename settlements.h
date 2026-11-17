
#ifndef SETTLEMENTS_H
#define SETTLEMENTS_H

#include "list_manager.h"

typedef struct ParticipantPayment {
    char name[50];
    double share;
    double amount_paid;
    struct ParticipantPayment *next;
} ParticipantPayment;

typedef struct {
    Expense *expense;
    char payer[50];
    ParticipantPayment *participants;
    int participant_count;
    double share;
} Settlement;

int init_settlement(Settlement *settlement, Expense *expense,
                    const char *payer);

int add_participant(Settlement *settlement, const char *name);

double calculate_share(const Expense *expense);

int record_payment(Settlement *settlement, const char *name,
                   double amount);

void print_settlement(const Settlement *settlement);

void free_settlement(Settlement *settlement);

#endif
