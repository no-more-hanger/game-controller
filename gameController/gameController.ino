#include "Keyboard.h"

// pin map
#define JOY_X A3
#define JOY_Y A2
// #define JOY_SW A1
#define BTN_A 3
#define BTN_B 2
#define PEDAL 7

#define ANALOG_MAX 1023

// pedal
#define RPM_STD 60
#define MIN_CNT_INTERVAL 50    // 회전 최소 주기
#define MIN_RPM_INTERVAL 5000  // RPM 계산 최소 주기

// KEY CODE
#define ASCII_ZERO 48      // 0 ~ 9 단계
#define ASCII_C 67         // 10 ~ 33 단계
#define MAX_LEVEL 33       // 최대 속도 레벨
#define LEVEL_INTERVAL 10  // (임시) 속도 레벨 간 간격

#define DEBUG  // comment on production

// interrupt flag
volatile bool flag_A;
volatile bool flag_B;
bool flag_pedal;

// pedal
int rotation_cnt;  // 회전수

// pedal sensor - time value [ms]
unsigned long prev_cnt_timestamp;  //직전 회전한 시간
unsigned long prev_rpm_timestamp;  //직전 계산한 시간

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  // set pin mode for joy stick
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  //set pin mode for push button
  // pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  // set pin mode for IR sensor
  pinMode(PEDAL, INPUT);

  // initialize variables
  flag_A = false;
  flag_B = false;
  flag_pedal = false;

  prev_cnt_timestamp = millis();
  prev_rpm_timestamp = prev_cnt_timestamp;

  rotation_cnt = 0;

  // attach interrupt for push button
  attachInterrupt(digitalPinToInterrupt(BTN_A), onPushA, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_B), onPushB, FALLING);
}

// interrupt routine
void onPushA() {
  flag_A = true;
}

void onPushB() {
  flag_B = true;
}

void readButtons() {
  if (flag_A) {
    flag_A = false;
    Keyboard.write('A');

#ifdef DEBUG
    Serial.println("A");
#endif
  }
  if (flag_B) {
    flag_B = false;
    Keyboard.write('B');

#ifdef DEBUG
    Serial.println("B");
#endif
  }
}

void readJoyStick() {
  // move left
  if (analogRead(JOY_X) == ANALOG_MAX) {
    Keyboard.press(KEY_LEFT_ARROW);
#ifdef DEBUG
    Serial.println("Left");
#endif
  } else {
    Keyboard.release(KEY_LEFT_ARROW);
  }

  // move right
  if (analogRead(JOY_X) == 0) {
    Keyboard.press(KEY_RIGHT_ARROW);
#ifdef DEBUG
    Serial.println("Right");
#endif
  } else {
    Keyboard.release(KEY_RIGHT_ARROW);
  }

  // move up
  if (analogRead(JOY_Y) == ANALOG_MAX) {
    Keyboard.press(KEY_UP_ARROW);
#ifdef DEBUG
    Serial.println("Up");
#endif

  } else {
    Keyboard.release(KEY_UP_ARROW);
  }

  // move down
  if (analogRead(JOY_Y) == 0) {
    Keyboard.press(KEY_DOWN_ARROW);
#ifdef DEBUG
    Serial.println("Down");
#endif
  } else {
    Keyboard.release(KEY_DOWN_ARROW);
  }
}

void readPedal() {
  // check minimun time interval
  unsigned long current_timestamp = millis();
  bool pedal = digitalRead(PEDAL);

  if (current_timestamp - prev_cnt_timestamp < MIN_CNT_INTERVAL) {
    return;
  }
  prev_cnt_timestamp = current_timestamp;

  if (!flag_pedal && pedal == LOW) {
    flag_pedal = true;
    rotation_cnt++;
#ifdef DEBUG
    Serial.print("\t rotation_cnt: ");
    Serial.print(rotation_cnt);
#endif
  } else if (pedal == HIGH) {
    flag_pedal = false;
  }
}

void calculateRPM() {
  unsigned long current_timestamp = millis();
  if (current_timestamp - prev_rpm_timestamp < MIN_RPM_INTERVAL) {
    return;
  }

  int rpm = (int)(rotation_cnt * RPM_STD * 1000.0 / ((float)(current_timestamp - prev_rpm_timestamp)));
  rotation_cnt = 0;
  prev_rpm_timestamp = current_timestamp;
#ifdef DEBUG
  Serial.print("\t rpm: ");
  Serial.print(rpm);
#endif
  RPMToKeyCode(rpm);
}

void RPMToKeyCode(int rpm) {
  if (rpm == 0) {  // 정지 상태
    return;
  }

  int level = rpm / LEVEL_INTERVAL;
  int ascii;
  if (level < 10) {  // 0단계 ~ 9단계
    ascii = ASCII_ZERO + level;
  } else if (level < MAX_LEVEL) {  // 10단계 ~ 32단계
    ascii = ASCII_C + level - 10;
  } else {  // 33단계
    ascii = ASCII_C + MAX_LEVEL - 10;
  }

#ifdef DEBUG
  Serial.print("\t level: ");
  Serial.print(level);

  Serial.print("\t ascii: ");
  Serial.print((char)ascii);
#endif

  Keyboard.write(ascii);
}

void loop() {
  // check push buttons
  readButtons();

  // check joystick
  readJoyStick();

  // check pedal
  readPedal();

  // calculate rpm
  calculateRPM();
}
