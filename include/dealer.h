#ifndef DEALER_H
#define DEALER_H

#include "cache.h"
#include "deck.h"
#include "hand.h"

int should_dealer_hit(Hand hand, int s17);
CacheTable make_dealer_table();
double dealer_ev(CacheTable *dealer_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck, int s17);

#endif
