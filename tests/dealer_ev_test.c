#include <stdio.h>

#include "cache.h"
#include "card.h"
#include "dealer.h"
#include "hand.h"
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
    int s17 = 0;

    DealerCacheTable *dealer_cache_table_ptr = make_dealer_cache_table();
    double dealer_ev_result = dealer_ev(dealer_cache_table_ptr, player_hand, dealer_hand, deck, s17);

    printf("Cache size: %i\n", dealer_cache_table_ptr->size);
    free_cache_table(dealer_cache_table_ptr);
    printf("Dealer EV result: %f\n", dealer_ev_result);
}
