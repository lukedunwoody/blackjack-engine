#include "dealer.h"

#include "cache.h"
#include "card.h"
#include "deck.h"
#include "ev.h"
#include "hand.h"

int should_dealer_hit(Hand dealer_hand, int s17) {
    int value = get_hand_value(dealer_hand);
    int soft = is_soft(dealer_hand);

    return (value < 17) || (value == 17 && soft && s17);
}

double dealer_ev(DealerCacheTable *dealer_cache_table_ptr, Hand player_hand, Hand dealer_hand, Deck deck, int s17) {
    // assumes player_hand is not a bust and dealer_hand is at least 1 card and not a bust

    double results[CARD_COUNT];

    int player_value = get_hand_value(player_hand);

    for (Card card = ACE; card < CARD_COUNT; card++) {
        // Append card
        Hand new_hand = append_card(dealer_hand, card);
        Deck new_deck = remove_card(deck, card);

        // Get rid of busts right away
        if (is_bust(new_hand)) {
            results[card] = WIN;
            continue;
        }

        // Check cache
        int position = get_dealer_cache_position(dealer_cache_table_ptr, new_hand);

        if (position != -1) {
            results[card] = get_dealer_cache_ev(dealer_cache_table_ptr, position);
            continue;
        }

        // Actual EV logic
        if (should_dealer_hit(new_hand, s17)) {
            results[card] = dealer_ev(dealer_cache_table_ptr, player_hand, new_hand, new_deck, s17);
        } else {
            int dealer_value = get_hand_value(new_hand);

            if (player_value > dealer_value) {
                results[card] = WIN;
            } else if (player_value < dealer_value) {
                results[card] = LOSE;
            } else {
                results[card] = DRAW;
            }
        }
    }

    double average_ev = get_average_ev(results, deck);
    add_dealer_cache(dealer_cache_table_ptr, dealer_hand, average_ev);
    return average_ev;
}
