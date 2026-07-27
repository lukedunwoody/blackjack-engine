#include "ev.h"

#include "card.h"

double get_average_ev(double results[CARD_COUNT]) {
    double total_ev = 0;

    for (Card card = ACE; card < CARD_COUNT; card++) {
        total_ev += results[card];
    }

    return total_ev/CARD_COUNT;
}
