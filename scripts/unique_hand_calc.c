// Calculates how many different unique 2 card blackjack hands can exist

#include <stdio.h>

int combine_chars(char c1, char c2) {
    return (c1 << 8) | c2;
}

int linear_search(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return 1;
        }
    }
    return 0;
}

int main() {
    const char cards[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    int cached[100];
    int current = 0;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int hand;
            if (cards[i] > cards[j]) {
                hand = combine_chars(cards[j], cards[i]);
            } else {
                hand = combine_chars(cards[i], cards[j]);
            }

            if (current == 0) {
                cached[0] = hand;
                current++;
            } else {
                if (linear_search(cached, current, hand) == 0) {
                    cached[current] = hand;
                    current++;
                }
            }
        }
    }

    printf("Unique hands: %i\n", current);
    getchar();
}
