#include <Arduino.h>
#include <Adafruit_SleepyDog.h>

// Project Includes
#include "Version.h"
#include <pinout.h>
#include <audio.h>
// #include <radio.h>
#include <Adafruit_MCP23X17.h>


#include <Adafruit_DotStar.h>
#include <SPI.h> // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 16 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN A3
#define CLOCKPIN A4
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.



// Button Header
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
// #include <Bounce2.h>

#define DEBOUNCE_DELAY 50

struct Pushbutton {
  long last_time_changed;
  bool last_state;
  int led_one;
  int led_two;
};

// INSTANTIATE A Bounce OBJECT
// b = Bounce()

Pushbutton buttons[8];


Adafruit_MCP23X17 mcp;


int head = 0, tail = -10;  // Index of first 'on' and 'off' pixels
//uint32_t color = 0xFF0000; // 'On' color (starts red)




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
uint64_t color[] = {0x00FF00, 0x00FF00, 0x888800, 0x888800, 0xFF0000, 0xFF0000, 0x0000FF, 0x0000FF};

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


void clearStrip(){
  for (int i=0; i<8; i++) {
      lit[i] = false;
      int pos_1 = correct_position[i]*2;
      int pos_2 = (correct_position[i]*2) + 1;
      led_strip[pos_1] = 0x000000;
      led_strip[pos_2] = 0x000000;
      strip.setPixelColor(pos_1, 0x000000);
      strip.setPixelColor(pos_2, 0x000000);
      Serial.printf("Cleared positions %d and %d\n", pos_1, pos_2);
      next_lit_position = 0;
  }
}

void processButton(int input) {
  startAudio(input);
  if (lit[input] == false) { // if this button not lit
    Serial.printf("nlp: %d, cor_pos: %d", next_lit_position, correct_position[input]);

    if (next_lit_position > 7) { // if not yet solved
      clearStrip();
    } else {
      if (next_lit_position != correct_position[input]) { // if wrong button
        Serial.println("Wrong Order.");
        order_correct = false;
      }
      if (order_correct == false) {
        Serial.println("Clearing the led strip.");
        clearStrip();
      }

      // Light the button's leds
      lit[input] = true; // Record button as lit.
      int pos_1 = correct_position[input]*2;
      int pos_2 = (correct_position[input]*2) + 1;
      led_strip[pos_1] = color[input];
      led_strip[pos_2] = color[input];
      strip.setPixelColor(pos_1, color[input]);
      strip.setPixelColor(pos_2, color[input]);
      Serial.printf("Added color to pixels %d and %d\n", pos_1, pos_2);

      next_lit_position++; // Advance the order position tracker.

      // If that was the right button, record order as correct so far.
      if ((correct_position[input] == 0) && (next_lit_position == 1)) {
          order_correct = true;
      }
    }

  } else {
    if (next_lit_position > 7) { // if not yet solved
      Serial.println("Puzzle was solved, now clearing strip.");
      clearStrip();
    }

      // Button was already lit
    Serial.println("button already lit");
      // TODO: Possibly unlight the button, but would that make it too easy,
      // as one could just light and unlight buttons until they found the
      // next one to light in order?
  }
  
  // Debug statement, print LED strip color contents.
  for (int i=0; i<16; i++) {
      Serial.printf("%d", led_strip[i]);
  }
  Serial.println(" ");
  strip.show();

  if (next_lit_position == 8) {
    startAudio(8);
    delay(5000);
    clearStrip();
  }


} // End processButton()


// ███████╗███████╗████████╗██╗   ██╗██████╗
// ██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
// ███████╗█████╗     ██║   ██║   ██║██████╔╝
// ╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
// ███████║███████╗   ██║   ╚██████╔╝██║
// ╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝

void setup()
{
  Serial.begin(9600);
  // while (!Serial)
  // {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }
  Serial.printf("\nProject version v%s, built %s\n", VERSION, BUILD_TIMESTAMP);
  Serial.println("Setup function commencing...");
  vsAudioSetup();
  delay(100);
  // radioSetup();

  if (!mcp.begin_I2C())
  {
      Serial.println("Error initializing MCP23017.");
      //while (1)
      //    ;
  }

  for(int i=0; i<8; i++) {
    mcp.pinMode(i, INPUT_PULLUP);
    buttons[i].last_state = mcp.digitalRead(i);
    buttons[i].last_time_changed = millis();
  }

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  Watchdog.enable(4000);
  Serial.println("Setup Complete");
}

// ██╗      ██████╗  ██████╗ ██████╗
// ██║     ██╔═══██╗██╔═══██╗██╔══██╗
// ██║     ██║   ██║██║   ██║██████╔╝
// ██║     ██║   ██║██║   ██║██╔═══╝
// ███████╗╚██████╔╝╚██████╔╝██║
// ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝

void loop()
{
  // Update the Bounce instance (YOU MUST DO THIS EVERY LOOP)
  // bounce.update();

  for(int i=0; i<8; i++) {
    int state = mcp.digitalRead(i);
    if ((millis() - buttons[i].last_time_changed) > DEBOUNCE_DELAY) {
      if (state != buttons[i].last_state) {
        if (state == LOW) {
          // Play sound
          //startAudio(); // TODO: pass sound number //

          // If LED array looks right, add self, otherwise clear and add self
          Serial.printf("Button %d pressed.\n", i);
          processButton(i);
        } else {
          Serial.printf("Button %d released.\n", i);
        }
        buttons[i].last_state = state;
        buttons[i].last_time_changed = millis();
      }
    }
  }

  Watchdog.reset();
}
