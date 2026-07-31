#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"
#include "card.h"
#include "dealer.h"
#include "deck.h"
#include "engine.h"
#include "hand.h"
#include "io.h"

Deck get_and_remove_dealer_cards(Hand dealer_hand, Deck deck, int s17) {
    while (should_dealer_hit(dealer_hand, s17)) {
        printf("Enter the card the dealer just showed: ");
        Card card = get_card_input();
        deck = remove_card(deck, card);
        dealer_hand = append_card(dealer_hand, card);
    }

    return deck;
}

Deck get_and_remove_other_player_cards(Deck deck) {
    while (1) {
        printf("Enter any cards the other players have received ('f' to finish): ");

        char input;

        scanf(" %c", &input);

        if (input == 'f' || input == 'F') {
            break;
        }

        ungetc(input, stdin);

        Card card = get_card_input();
        deck = remove_card(deck, card);
    }

    return deck;
}

Deck simulate_hand(Hand player_hand, Hand dealer_hand, Deck deck,
    int can_play, int can_double, int can_split,
    float bj_payout,
    int sur_allowed,
    int s17,
    int das,
    int rsa,
    int psa,
    int remaining_splits,
    int double_restrictions[DOUBLE_RESTRICTION_COUNT],
    int dealer_peeks,
    int dealer_no_bj_confirmed) {

    if (is_bust(player_hand)) {
        printf("Your hand busted. Moving on...\n");
        return deck;
    }

    if (is_blackjack(player_hand)) {
        printf("You got a Blackjack! Moving on...\n");
        return deck;
    }

    if (get_hand_value(player_hand) == 21) {
        printf("You have a 21 and cannot improve. Moving on...\n");
        return deck;
    }

    if (!can_play) {
        printf("You currently have a hand, but can't play it. Moving on...\n");
        return deck;
    }

    MoveEV best_move_ev = any_hand_ev(player_hand, dealer_hand, deck,
        can_play, can_double, can_split,
        bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks, dealer_no_bj_confirmed);

    print_best_move_ev(best_move_ev);

    int user_move = get_user_move(can_double, can_split, sur_allowed);

    if (user_move == STAND || user_move == SURRENDER) {
        deck = get_and_remove_dealer_cards(dealer_hand, deck, s17);

    } else if (user_move == HIT) {
        printf("Please enter the card you just recieved: ");
        Card card = get_card_input();

        deck = remove_card(deck, card);
        player_hand = append_card(player_hand, card);

        deck = simulate_hand(player_hand, dealer_hand, deck,
            can_play, can_double, can_split,
            bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits, double_restrictions, dealer_peeks, dealer_no_bj_confirmed);

    } else if (user_move == DOUBLE) {
        printf("Please enter the card you just recieved: ");
        Card card = get_card_input();

        deck = remove_card(deck, card);
        player_hand = append_card(player_hand, card);

        deck = get_and_remove_dealer_cards(dealer_hand, deck, s17);

    } else {
        // Can safely assume this is a pair
        Card split_card;
        for (Card card = ACE; card < CARD_COUNT; card++) {
            if (player_hand.counts[card]) {
                split_card = card;
                break;
            }
        }
        player_hand.counts[split_card] = 1;
        player_hand.size = 1;

        printf("Please enter the card you just recieved: ");
        Card card0 = get_card_input();

        deck = remove_card(deck, card0);
        Hand hand0 = append_card(player_hand, card0);

        deck = simulate_hand(hand0, dealer_hand, deck,
            psa || split_card != ACE, das && double_restrictions[get_hand_value(hand0)], (rsa || split_card != ACE) && remaining_splits > 0,
            bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits - 1, double_restrictions, dealer_peeks, dealer_no_bj_confirmed);

        printf("Please enter the card you just recieved: ");
        Card card1 = get_card_input();

        deck = remove_card(deck, card1);
        Hand hand1 = append_card(player_hand, card1);

        deck = simulate_hand(hand1, dealer_hand, deck,
            psa || split_card != ACE, das && double_restrictions[get_hand_value(hand0)], (rsa || split_card != ACE) && remaining_splits > 0,
            bj_payout, sur_allowed, s17, das, rsa, psa, remaining_splits - 1, double_restrictions, dealer_peeks, dealer_no_bj_confirmed);
    }

    return deck;
}

int main(int argc, char *argv[]) {
    printf("Welcome to Luke's Blackjack Engine\n");

    // Set defaults
    int num_decks = 8;
    float bj_payout = 1.5;
    int sur_allowed = 0;
    int s17 = 0;
    int das = 1;
    int rsa = 0;
    int psa = 1;
    int max_splits = 4;
    int double_restrictions[DOUBLE_RESTRICTION_COUNT];
    int dealer_peeks = 1;

    for (int i = 0; i < DOUBLE_RESTRICTION_COUNT; i++) {
        double_restrictions[i] = 1;
    }

    // Parser
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            num_decks = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            bj_payout = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            sur_allowed = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-s17") == 0 && i + 1 < argc) {
            s17 = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-das") == 0 && i + 1 < argc) {
            das = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-rsa") == 0 && i + 1 < argc) {
            rsa = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-psa") == 0 && i + 1 < argc) {
            psa = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-ms") == 0 && i + 1 < argc) {
            max_splits = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-dp") == 0 && i + 1 < argc) {
            dealer_peeks = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-dr") == 0) {
            for (int i = 0; i < DOUBLE_RESTRICTION_COUNT; i++) {
                double_restrictions[i] = 0;
            }

            while (i + 1 < argc && argv[i + 1][0] != '-') {
                int total = atoi(argv[++i]);

                if (total >= 1 && total < DOUBLE_RESTRICTION_COUNT) {
                    double_restrictions[total] = 1;
                }
            }
        }
    }

    Deck deck = generate_deck(num_decks);

    while (1) {
        printf("Enter your first card: ");
        Card player_card0 = get_card_input();
        deck = remove_card(deck, player_card0);

        printf("Enter your second card: ");
        Card player_card1 = get_card_input();
        deck = remove_card(deck, player_card1);

        printf("Enter the dealer's upcard: ");
        Card dealer_upcard = get_card_input();
        deck = remove_card(deck, dealer_upcard);

        Hand player_hand = {0};
        player_hand = append_card(player_hand, player_card0);
        player_hand = append_card(player_hand, player_card1);

        Hand dealer_hand = {0};
        dealer_hand = append_card(dealer_hand, dealer_upcard);

        deck = get_and_remove_other_player_cards(deck);

        int dealer_no_bj_confirmed = 0;
        if (dealer_peeks && (dealer_upcard == ACE || dealer_upcard == TEN)) {
            printf("Does the dealer have a blackjack (y/N): ");
            char answer;
            scanf(" %c", &answer);

            if (answer == 'y' || answer == 'Y') {
                if (dealer_upcard == ACE) {
                    deck = remove_card(deck, TEN);
                } else {
                    deck = remove_card(deck, ACE);
                }
            }

            dealer_no_bj_confirmed = 1;
        }

        int can_double = double_restrictions[get_hand_value(player_hand)];
        int can_split = is_pair(player_hand) && max_splits > 0;

        deck = simulate_hand(player_hand, dealer_hand, deck,
            1, can_double, can_split,
            bj_payout, sur_allowed, s17, das, rsa, psa, max_splits, double_restrictions, dealer_peeks, dealer_no_bj_confirmed);

        printf("Press 'r' if the dealer is shuffling. Press 'q' to quit. Otherwise hit any key to continue: ");
        char input_char = getchar();

        if (input_char == 'r' || input_char == 'R') {
            deck = generate_deck(num_decks);
        } else if (input_char == 'q' || input_char == 'Q') {
            break;
        }
    }
}
