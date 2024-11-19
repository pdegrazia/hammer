#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//file with tones for piezo
#include "pitches.h"

#define BUZZER_PIN     4
#define LED_PIN     8
#define NUM_LEDS    40
#define BRIGHTNESS  50
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON 5

#define UPDATES_PER_SECOND 100

//I2C screen address
//0x3c

//set to false for real case
boolean gameStarted = false;
int timeout = 30000;
int currentIndex = 0;
int buttonPressed = 0;
int currentStatusButton = 1;
float timeLeft = 20.0;
long timer;
long startTime;

float score = 0;

CRGB currentColour = CRGB(0, 0, 255);


int melody[] = {

  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  delay( 3000 ); // power-up safety delay
  //LED Init
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
 
  //seed for random numbers
  randomSeed(analogRead(0));

  //init for button
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);

  // //init display
  display.begin(SCREEN_ADDRESS, true); // Address 0x3C default
 
  display.display();
  display.setTextColor(SH110X_WHITE);
  delay(2000);
  
  pinMode(BUZZER_PIN, OUTPUT);
}

int startFrequency = 100;

void loop() {
  if (gameStarted){
    //countdown timer on screen
    if (millis() - timer > 1000){
      timer = millis();
      timeLeft--;
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(score,0);
    display.setCursor(0, 32);
    display.print(timeLeft,0);
    display.display();
    if(millis() - startTime < 20000){
      buttonPressed = digitalRead(BUTTON);
      if (buttonPressed == 0) {
        if (buttonPressed != currentStatusButton){
          currentStatusButton = 0;
          //increase score of player 1
          currentIndex++;
          score+=1.0;
          tone(4, startFrequency);
          startFrequency += 10;
          delay(30);
          noTone(4);
        }
      } else {
        currentStatusButton = 1;
      }

      for (int i=0; i<currentIndex; i++){
          leds[i]=currentColour;
      }
      FastLED.show();

      if(currentIndex == 40){
        currentIndex = 0;
        currentColour = CRGB(random(0,256),random(0,256),random(0,256));
        startFrequency = 100;
      }
    } else {
      display.clearDisplay();
      display.setTextSize(3);
      display.setCursor(0, 0);
      display.print(F("GAME"));
      display.setCursor(0, 25);
      display.print(F("OVER!"));
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setTextSize(3);
      display.setCursor(0, 0);
      display.print(F("Score:"));
      display.setCursor(0, 30);
      display.print(score,0);
      display.display();
      for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
       int noteDuration = 1000 / noteDurations[thisNote];
        tone(4, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(8);
      }
      delay(3000);
      score = 0;
      gameStarted = false;
      currentIndex = 0;
       timeLeft = 20.0;
    }
  } else {
    startFrequency = 100;
    //this is showing when the game is reset
    for (int i=0; i<40; i++){
        leds[i]=CRGB::Red;
    }   
    FastLED.show();

    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setCursor(0, 0);
    display.print(F("Winter"));
    display.setCursor(0, 15);
    display.print(F("Fair"));
    display.setCursor(0, 30);
    display.print(F("2024"));
    display.setCursor(0, 45);
    display.setTextSize(1);
    display.print(F("Press to start"));
    display.display();

    // //print some message or show an animation
    buttonPressed = digitalRead(BUTTON);
    if (buttonPressed == 0) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(3);
      display.print(F("Ready?"));
      display.display();
      delay(500);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("3"));
      display.display();
      tone(4, 500);
      delay(500);
      noTone(4);
      delay(500);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("2"));
      display.display();
      tone(4, 500);
      delay(500);
      noTone(4);
      delay(500);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("1"));
      display.display();
      tone(4, 500);
      delay(500);
      noTone(4);
      delay(500);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("GO!"));
      display.display();
      tone(4, 1500);
      delay(500);
      noTone(4);
      delay(500);
      gameStarted = true;
      startTime = millis();
      timer = millis();
    }
  }
}
