#ifndef DEALER_H
#define DEALER_H

#include "cache.h"
#include "deck.h"
#include "hand.h"

int should_dealer_hit(Hand hand, int s17);
double dealer_ev(DealerCacheTable *dealer_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck,
    int s17, int dealer_no_bj_confirmed);

#endif
