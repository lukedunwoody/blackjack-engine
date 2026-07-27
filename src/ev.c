#include "ev.h"

#include "card.h"
#include "deck.h"

double get_average_ev(double results[CARD_COUNT], Deck deck) {
    double average_ev = 0;

    for (Card card = ACE; card < CARD_COUNT; card++) {
        average_ev += results[card] * deck.counts[card]/deck.size;
    }

    return average_ev;
}
