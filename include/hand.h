#ifndef HAND_H
#define HAND_H

#include <stdint.h>

#include "card.h"

typedef CardCount Hand;

int get_hand_value(Hand hand);
int is_soft(Hand hand);
int is_blackjack(Hand hand);
int is_bust(Hand hand);
int is_pair(Hand hand);
int are_hands_equal(Hand hand0, Hand hand1);

#endif
