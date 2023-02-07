#include "Keyboard.h"

// pin map
#define JOY_X A3
#define JOY_Y A2
// #define JOY_SW A1
#define BTN_A 3
#define BTN_B 2

#define ANALOG_MAX 1023


volatile bool flag_A = false;
volatile bool flag_B = false;

void setup() {
  Serial.begin(9600);

  // set pin mode for joy stick
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  //set pin mode for push button
  // pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  // attach interrupt for push button
  attachInterrupt(digitalPinToInterrupt(BTN_A), onPushA, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_B), onPushB, FALLING);
}

void onPushA() {
  flag_A = true;
}

void onPushB() {
  flag_B = true;
}

void loop() {
  // check push buttons
  if (flag_A) {
    flag_A = false;
    Keyboard.write('A');
    Serial.println("A");
  }
  if (flag_B) {
    flag_B = false;
    Keyboard.write('B');
    Serial.println("B");
  }
  // move left
  if (analogRead(JOY_X) == ANALOG_MAX) {
    Keyboard.press(KEY_LEFT_ARROW);
  } else {
    Keyboard.release(KEY_LEFT_ARROW);
  }

  // move right
  if (analogRead(JOY_X) == 0) {
    Keyboard.press(KEY_RIGHT_ARROW);
  } else {
    Keyboard.release(KEY_RIGHT_ARROW);
  }

  // move up
  if (analogRead(JOY_Y) == ANALOG_MAX) {
    Keyboard.press(KEY_UP_ARROW);
  } else {
    Keyboard.release(KEY_UP_ARROW);
  }

  // move down
  if (analogRead(JOY_Y) == 0) {
    Keyboard.press(KEY_DOWN_ARROW);
  } else {
    Keyboard.release(KEY_DOWN_ARROW);
  }
}
