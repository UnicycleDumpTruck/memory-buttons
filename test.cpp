#include <iostream>


int main() {
    // Color for each of the 16 LEDs in the strip. Starts as off.
    uint64_t led_strip[] = {0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};
    
    // Next position in the ORDER of buttons that could be filled. 0 when empty, 7 when full.
    int next_lit_position = 0;

    // Whether the order is correct so far, since the last clearing.
    bool order_correct = true;


    /* 
     * The following arrays store data for each of the eight buttons.
     * All of button zero's data is in index zero of each of these.
     */

    // Color for each button:
    uint64_t color[] = {0xFF0000, 0xFF0000, 0x888800, 0x888800, 0x00FF00, 0x00FF00, 0x0000FF, 0x0000FF};
    
    // Correct place in line each button should be pressed:
    int correct_position[] = {6, 2, 4, 7, 1, 5, 3, 0}; // TODO: randomize this after success
    
    // Last electrical state button was read at, true == HIGH == not-pressed
    bool last_state[] = {true, true, true, true, true, true, true, true};

    // Last time (monotonic) each button was changed, used for debouncing:
    long last_time_changed[8];

    // Whether leds for each button are lit:
    bool lit[] = {false, false, false, false, false, false, false, false};

    // Variable to store input from std in:
    int input = -1;

    while(true) {
        // Prompt and read button number input from std in.
        std::cout << "Command: ";
        std::cin >> input;
        if ((input > 7) or (input < 0)) {
            continue;
        }

        if (lit[input] == false) { // if this button not lit
            if (next_lit_position > 7) { // if solved
                continue; // ignore input
            }
            
            // Debug statement:
            std::cout << "nlp:" << next_lit_position << "cor_pos:" << correct_position[input] << std::endl;
            
            if (next_lit_position != correct_position[input]) { // if wrong button
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

            // Light the button's leds
            lit[input] = true; // Record button as lit.
            led_strip[correct_position[input] * 2] = color[input];
            led_strip[(correct_position[input] * 2) + 1] = color[input];
            next_lit_position++; // Advance the order position tracker.

            // If that was the right button, record order as correct so far.
            if ((correct_position[input] == 0) && (next_lit_position == 1)) {
                order_correct = true;
            }

        } else {
            // Button was already lit
            std::cout << "button already lit" << std::endl;
            // TODO: Possibly unlight the button, but would that make it too easy,
            // as one could just light and unlight buttons until they found the
            // next one to light in order?
        }
        
        // Debug statement, print LED strip color contents.
        for (int i=0; i<16; i++) {
            std::cout << led_strip[i] << ' ';
        }
        std::cout << std::endl;
    }
}

