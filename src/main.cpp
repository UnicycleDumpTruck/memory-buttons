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
#define DATAPIN A4
#define CLOCKPIN A5
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
uint32_t color = 0xFF0000; // 'On' color (starts red)



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
  // vsAudioSetup();
  delay(100);
  // radioSetup();


  if (!mcp.begin_I2C())
  {
      Serial.println("Error initializing MCP23017.");
      //while (1)
      //    ;
  }

  // configure button pin for input with pull up
  // mcp.pinMode(BUTTON_PIN, INPUT_PULLUP);

  for(int i=0; i<8; i++) {
    mcp.pinMode(i, INPUT_PULLUP);
    buttons[i].last_state = mcp.digitalRead(i);
    buttons[i].last_time_changed = millis();
  }



  // BOUNCE SETUP

  // SELECT ONE OF THE FOLLOWING :
  // 1) IF YOUR INPUT HAS AN INTERNAL PULL-UP
  // bounce.attach(BOUNCE_PIN, INPUT_PULLUP); // USE INTERNAL PULL-UP
  // 2) IF YOUR INPUT USES AN EXTERNAL PULL-UP
  // bounce.attach( BOUNCE_PIN, INPUT ); // USE EXTERNAL PULL-UP

  // DEBOUNCE INTERVAL IN MILLISECONDS
  // bounce.interval(5); // interval in ms


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
        } else {
          Serial.printf("Button %d released.\n", i);
        }
        buttons[i].last_state = state;
        buttons[i].last_time_changed = millis();
      }
    }
  }


  strip.setPixelColor(head, color); // 'On' pixel at head
  strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
  strip.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if (++head >= NUMPIXELS)
  {                           // Increment head index.  Off end of strip?
      head = 0;               //  Yes, reset head index to start
      if ((color >>= 8) == 0) //  Next color (R->G->B) ... past blue now?
          color = 0xFF0000;   //   Yes, reset to red
  }
  if (++tail >= NUMPIXELS)
      tail = 0; // Increment, reset tail index


  Watchdog.reset();
}