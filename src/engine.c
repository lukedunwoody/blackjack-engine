// Only add something to the player_ev after confirming it is the best choice

#include "engine.h"

#include "cache.h"
#include "card.h"
#include "dealer.h"
#include "deck.h"
#include "ev.h"
#include "hand.h"

double ev_hit(PlayerCacheTable *player_cache_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck, int s17) {
    // assumes player_hand has at least 2 cards

    double results[CARD_COUNT];

    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (deck.counts[card] == 0) {
            results[card] = 0;
            continue;
        }

        // Append card
        Hand new_hand = append_card(player_hand, card);
        Deck new_deck = remove_card(deck, card);

        // Get rid of busts right away
        if (is_bust(new_hand)) {
            results[card] = LOSE;
            continue;
        }

        // Check cache
        int position = get_player_cache_position(player_cache_table_ptr, new_hand);
        if (position != -1) {
            results[card] = get_player_cache_best_move_ev(player_cache_table_ptr, position).ev;
            continue;
        }

        // Make dealer cache table
        DealerCacheTable *dealer_cache_table_ptr = make_dealer_cache_table();

        // EV logic
        double stand_ev = dealer_ev(dealer_cache_table_ptr, new_hand, dealer_hand, new_deck, s17);
        free_cache_table(dealer_cache_table_ptr);

        double hit_ev = get_hand_value(new_hand) < 21 ?
            ev_hit(player_cache_table_ptr, new_hand, dealer_hand, new_deck, s17) : -1;

        MoveEV best_move_ev;
        if (stand_ev > hit_ev) {
            best_move_ev.move = STAND;
            best_move_ev.ev = stand_ev;
        } else {
            best_move_ev.move = HIT;
            best_move_ev.ev = hit_ev;
        }
        add_player_cache(player_cache_table_ptr, new_hand, best_move_ev);

        results[card] = (stand_ev > hit_ev) ? stand_ev : hit_ev;
    }

    double average_ev = get_average_ev(results, deck);
    return average_ev;
}

double ev_double(Hand player_hand, Hand dealer_hand, Deck deck, int s17) {
    // assumes player_hand is 2 cards

    double results[CARD_COUNT];

    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (deck.counts[card] == 0) {
            results[card] = 0;
            continue;
        }

        // Append card
        Hand new_hand = append_card(player_hand, card);
        Deck new_deck = remove_card(deck, card);

        // Get rid of busts right away
        if (is_bust(new_hand)) {
            results[card] = LOSE;
            continue;
        }

        // Make dealer cache table
        DealerCacheTable *dealer_cache_table_ptr = make_dealer_cache_table();

        // EV logic
        results[card] = dealer_ev(dealer_cache_table_ptr, new_hand, dealer_hand, new_deck, s17);
        free_cache_table(dealer_cache_table_ptr);
    }

    double average_ev = get_average_ev(results, deck);
    return average_ev*2;
}

double ev_surrender() {
    // bro just here for the vibes
    return -0.5;
}

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
    int dealer_peeks) {

    // Takes 2 card player_hand
    // Dealer is not bj atp
    // can_split should already be disabled if hand is a pair

    if (is_blackjack(player_hand)) {
        MoveEV bj_move;
        bj_move.ev = bj_payout;
        bj_move.move = -1;
        return bj_move;
    }

    if (is_bust(player_hand)) {
        MoveEV bust_move;
        bust_move.ev = -1;
        bust_move.move = -1;
        return bust_move;
    }

    int position = get_player_cache_position(player_cache_table_ptr, player_hand);
    if (position != -1) {
        return get_player_cache_best_move_ev(player_cache_table_ptr, position);
    }

    double move_evs[MOVE_COUNT];

    DealerCacheTable *dealer_cache_table_ptr = make_dealer_cache_table();
    move_evs[STAND] = dealer_ev(dealer_cache_table_ptr, player_hand, dealer_hand, deck, s17);
    free_cache_table(dealer_cache_table_ptr);

    move_evs[HIT] = ev_hit(player_cache_table_ptr, player_hand, dealer_hand, deck, s17);

    can_double = can_double && !double_restrictions[get_hand_value(player_hand)];
    move_evs[DOUBLE] = can_double ? ev_double(player_hand, dealer_hand, deck, s17) : -1;

    move_evs[SURRENDER] = sur_allowed ? ev_surrender() : -1;

    move_evs[SPLIT] = can_split ? ev_split(player_hand, dealer_hand, deck,
        bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks) : -1;

    MoveEV best_move_ev = {-1};
    for (int move = 0; move < MOVE_COUNT; move++) {
        if (move_evs[move] > best_move_ev.ev) {
            best_move_ev.ev = move_evs[move];
            best_move_ev.move = move;
        }
    }
    add_player_cache(player_cache_table_ptr, player_hand, best_move_ev);

    return best_move_ev;
}

MoveEV hitted_hand_ev(PlayerCacheTable *player_cache_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck, int s17) {
    // 3 or more card hand
    // not already busted

    int position = get_player_cache_position(player_cache_table_ptr, player_hand);
    if (position != -1) {
        return get_player_cache_best_move_ev(player_cache_table_ptr, position);
    }

    // Make dealer cache table
    DealerCacheTable *dealer_cache_table_ptr = make_dealer_cache_table();

    // EV logic
    double stand_ev = dealer_ev(dealer_cache_table_ptr, player_hand, dealer_hand, deck, s17);
    free_cache_table(dealer_cache_table_ptr);

    double hit_ev = ev_hit(player_cache_table_ptr, player_hand, dealer_hand, deck, s17);

    MoveEV best_move_ev;
    if (stand_ev > hit_ev) {
        best_move_ev.move = STAND;
        best_move_ev.ev = stand_ev;
    } else {
        best_move_ev.move = HIT;
        best_move_ev.ev = hit_ev;
    }
    add_player_cache(player_cache_table_ptr, player_hand, best_move_ev);

    return best_move_ev;
}

double one_card_ev(Hand player_hand, Hand dealer_hand, Deck deck,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks) {

    // Hand is always 1 card here
    // This is intended to be the entry point of pre_deal_ev calc
    PlayerCacheTable *player_cache_table_ptr = make_player_cache_table();

    double results[CARD_COUNT];

    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (deck.counts[card] == 0) {
            results[card] = 0;
            continue;
        }

        Hand new_hand = append_card(player_hand, card);
        Deck new_deck = remove_card(deck, card);

        int can_split = is_pair(new_hand);
        results[card] = start_hand_ev(player_cache_table_ptr, new_hand, dealer_hand, new_deck,
            1, 1, can_split,
            bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks).ev;
    }

    free_cache_table(player_cache_table_ptr);

    double average_ev = get_average_ev(results, deck);
    return average_ev;
}

double ev_split(Hand player_hand, Hand dealer_hand, Deck deck,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks) {

    // assumes 2 card pair
    remaining_splits--;

    PlayerCacheTable *player_cache_table_ptr = make_player_cache_table();

    Card split_card;
    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (player_hand.counts[card]) {
            split_card = card;
        }
    }

    Hand split_hand = remove_card(player_hand, split_card);

    double results[CARD_COUNT];

    for (Card card = ACE; card < CARD_COUNT; card++) {
        if (deck.counts[card] == 0) {
            results[card] = 0;
            continue;
        }

        Hand new_hand = append_card(split_hand, card);
        Deck new_deck = remove_card(deck, card);

        int can_play = psa && split_card == ACE && is_pair(new_hand);
        int can_double = das;
        int can_split = (rsa && split_card == ACE) && is_pair(new_hand) && remaining_splits > 0;

        results[card] = start_hand_ev(player_cache_table_ptr, new_hand, dealer_hand, new_deck,
            can_play, can_double, can_split,
            bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks).ev;
    }

    free_cache_table(player_cache_table_ptr);

    double average_ev = get_average_ev(results, deck);
    return average_ev*2;
}

double pre_deal_ev(Deck deck,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks) {

    double results0[CARD_COUNT];

    for (Card player_card = ACE; player_card < CARD_COUNT; player_card++) {
        if (deck.counts[player_card] == 0) {
            results0[player_card] = 0;
            continue;
        }

        Hand player_hand = {0};
        player_hand = append_card(player_hand, player_card);

        Deck new_deck0 = remove_card(deck, player_card);

        double results1[CARD_COUNT];

        for (Card dealer_card = ACE; dealer_card < CARD_COUNT; dealer_card++) {
            if (new_deck0.counts[dealer_card] == 0) {
                results1[dealer_card] = 0;
                continue;
            }

            Hand dealer_hand = {0};
            dealer_hand = append_card(dealer_hand, dealer_card);

            Deck new_deck1 = remove_card(new_deck0, dealer_card);

            results1[dealer_card] = one_card_ev(player_hand, dealer_hand, new_deck1,
                bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks);
        }

        results0[player_card] = get_average_ev(results1, new_deck0);
    }

    return get_average_ev(results0, deck);
}

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
    int dealer_peeks) {

    // Comment to fix intends
    MoveEV best_move_ev;

    PlayerCacheTable *player_cache_table_ptr = make_player_cache_table();

    if (player_hand.size == 2) {
        best_move_ev = start_hand_ev(player_cache_table_ptr, player_hand, dealer_hand, deck, can_play, can_double, can_split,
            bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks);
    } else {
        best_move_ev = hitted_hand_ev(player_cache_table_ptr, player_hand, dealer_hand, deck, s17);
    }

    free_cache_table(player_cache_table_ptr);

    return best_move_ev;
}
