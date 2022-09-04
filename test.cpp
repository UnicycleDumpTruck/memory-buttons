#include <iostream>


int main() {
    uint64_t led_strip[] = {0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};
    int lit_buttons[8];
    int next_lit_position = 0;
    bool order_correct = true;

    uint64_t color[] = {0xFF0000, 0xFF0000, 0x888800, 0x888800, 0x00FF00, 0x00FF00, 0x0000FF, 0x0000FF};
    int correct_position[] = {6, 2, 4, 7, 1, 5, 3, 0}; // TODO: randomize this after success
    // leds for given button are position*2 and position*2+1
    bool last_state[] = {true, true, true, true, true, true, true, true};
    long last_time_changed[8];
    bool lit[] = {false, false, false, false, false, false, false, false};

    int input = -1;
    while(true) {
        std::cout << "Command: ";
        std::cin >> input;
        if ((input > 7) or (input < 0)) {
            continue;
        }
        if (lit[input] == false) { // if this button not lit
            if (next_lit_position > 7) {
                continue;
            }
            std::cout << "nlp:" << next_lit_position << "cor_pos:" << correct_position[input] << std::endl;
            if (next_lit_position != correct_position[input]) { // if correct place for this button
                // Light the button's leds
                std::cout << "wrong order" << std::endl;
                order_correct = false;
            }
            if (order_correct == false) {
                // Clear the led strip
                std::cout << "Clearing the led strip." << std::endl;
                for (int i=0; i<8; i++) {
                    lit[i] = false;
                    led_strip[correct_position[i]*2] = 0x000000;
                    led_strip[correct_position[i]*2+1] = 0x000000;
                    next_lit_position = 0;
                }
            }
            lit[input] = true;
            lit_buttons[next_lit_position] = input;
            led_strip[correct_position[input] * 2] = color[input];
            led_strip[(correct_position[input] * 2) + 1] = color[input];
            next_lit_position++;

            if ((correct_position[input] == 0) && (next_lit_position == 1)) {
                order_correct = true;
            }
        } else {
            // Button was already lit
            std::cout << "button already lit" << std::endl;
        }

        for (int i=0; i<16; i++) {
            std::cout << led_strip[i] << ' ';
        }
        std::cout << std::endl;
    }
}

