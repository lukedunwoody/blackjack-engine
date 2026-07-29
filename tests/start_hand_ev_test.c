// This works at a usable speed, not as fast as I wanted though

#include <stdio.h>

#include "deck.h"
#include "engine.h"
#include "input.h"

int main() {
    printf("Enter player hand 0, 1, dealer upcard\n");
    Card player_card0 = get_card_input();
    Card player_card1 = get_card_input();
    Card dealer_card0 = get_card_input();

    Hand player_hand = {0};
    player_hand = append_card(player_hand, player_card0);
    player_hand = append_card(player_hand, player_card1);

    Hand dealer_hand = {0};
    dealer_hand = append_card(dealer_hand, dealer_card0);

    Deck deck = generate_deck(8);
    int double_restrictions[DOUBLE_RESTRICTION_COUNT] = {0};

    PlayerCacheTable *player_cache_table_ptr = make_player_cache_table();

    // Fairly standard American casino rules
    MoveEV shev = start_hand_ev(player_cache_table_ptr, player_hand, dealer_hand, deck,
        1, 1, is_pair(player_hand),
        1.5, 1, 1, 1, 0, 0, 4, double_restrictions, 1);

    printf("Move key: STAND: %i, HIT %i, DOUBLE %i, SURRENDER %i, SPLIT %i\n", STAND, HIT, DOUBLE, SURRENDER, SPLIT);
    printf("Best move: %i, EV: %f\n", shev.move, shev.ev);
}
