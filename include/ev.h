#ifndef EV_H
#define EV_H

#include "card.h"

typedef enum {
    LOSE = -1,
    DRAW = 0,
    WIN = 1
} Outcome;

double get_average_ev(double results[CARD_COUNT]);

#endif
