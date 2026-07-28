#ifndef ENGINE_H
#define ENGINE_H

#include "cache.h"
#include "deck.h"
#include "hand.h"

#define MAX_HAND_VALUE 21
#define DOUBLE_RESTRICTION_COUNT (MAX_HAND_VALUE + 1)

double ev_hit(PlayerCacheTable *player_cache_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck, int s17);
double ev_double(Hand player_hand, Hand dealer_hand, Deck deck, int s17);
double ev_surrender();

MoveEV hitted_hand_ev(PlayerCacheTable *player_cache_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck, int s17);

MoveEV start_hand_ev(PlayerCacheTable *player_cache_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck,
    int can_play, int can_double, int can_split,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks);

double one_card_ev(Hand player_hand, Hand dealer_hand, Deck deck,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks);

double ev_split(Hand player_hand, Hand dealer_hand, Deck deck,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks);

double pre_deal_ev(Deck deck,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks);

MoveEV any_hand_ev(Hand player_hand, Hand dealer_hand, Deck deck,
    int can_play, int can_double, int can_split,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks);

#endif
