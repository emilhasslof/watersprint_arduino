#include "PinChangeInterrupt.h"

int FLOW_PINS[] = {A1, 2, A3, A2};
int INDICATOR_PINS[] = {5, 9, A4, A5};
int PUMP_SIGNAL = A0;
int RELAY_PINS[] = {4, 8, 6, 7};
int pulse_counts[] = {0, 0, 0, 0};
int stable_countdowns[] = {13, 12, 11, 10};
int pump_countdown = 5;
bool blink = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 0; i < 4; i++){
    pinMode(FLOW_PINS[i], INPUT_PULLUP);
    pinMode(INDICATOR_PINS[i], OUTPUT);
    pinMode(RELAY_PINS[i], OUTPUT);
    pinMode(PUMP_SIGNAL, INPUT);
  }
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[0]), incrementCount0, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[1]), incrementCount1, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[2]), incrementCount2, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[3]), incrementCount3, RISING);

}

void loop() {
  blink = !blink;
  for (int i = 0; i < 4; i++){
    pulse_counts[i] = 0;
  }
  delay(2000);

  //Debug
  Serial.print("Pulse counts: ");
  for(int i = 0; i < 4; i++){
    Serial.print(pulse_counts[i]);
    Serial.print(" ");
  }
  Serial.println();
  

  if(digitalRead(PUMP_SIGNAL) == LOW) {
    for(int i = 0; i < 4; i++) {
      digitalWrite(RELAY_PINS[i], LOW);
      digitalWrite(INDICATOR_PINS[i], LOW);
      stable_countdowns[i] = 5 + (3 - i);
      pump_countdown = 5;
    }
  } else {
    pump_countdown = max(0, pump_countdown - 1);
  }

  if(pump_countdown > 0) {
    return;
  }

  int max_count = 0;
  for(int i = 0; i < 4; i++){
    if(pulse_counts[i] > max_count) {
      max_count = pulse_counts[i];
    }
  }

  for(int i = 0; i < 4; i++){
    // Based on some quick testing, 7 pulses in 2 seconds is approximately 0.5 liters per minute
    bool absolute_ok = pulse_counts[i] > 7;
    bool relative_ok = (max_count / (float)pulse_counts[i]) < 1.3;

    if(relative_ok){
      digitalWrite(INDICATOR_PINS[i], HIGH);
    } else {
      if(blink) {
        digitalWrite(INDICATOR_PINS[i], HIGH);
      } else {
        digitalWrite(INDICATOR_PINS[i], LOW);
      }
    }

    if(absolute_ok){
      stable_countdowns[i] = max(0, stable_countdowns[i] - 1);
    } else {
      stable_countdowns[i] = 10 + (3 - i);
      digitalWrite(INDICATOR_PINS[i], LOW);
    }

    if(stable_countdowns[i] == 0) {
      digitalWrite(RELAY_PINS[i], HIGH);
    } else {
      digitalWrite(RELAY_PINS[i], LOW);
    }
  }
}

/*
  for(int i = 0; i < 4; i++){
    if (channelIsStable(pulse_counts, i)){
      stable_countdowns[i] = max(0, stable_countdowns[i] - 1);
      digitalWrite(INDICATOR_PINS[i], HIGH);
    } else {
      digitalWrite(INDICATOR_PINS[i], LOW);
      stable_countdowns[i] = 10 + (3 - i);
    }

    if(stable_countdowns[i] == 0) {
      digitalWrite(RELAY_PINS[i], HIGH);
    } else {
      digitalWrite(RELAY_PINS[i], LOW);
    }
  }
*/
/*
bool channelIsStable(int pulse_counts[], int channel_idx){
  // Based on some quick and dirty testing, 7 pulses in 2 seconds is approximately 0.5 liters per minute
  bool absolute_ok = pulse_counts[channel_idx] > 7;

  float max_count = 0;
  for(int i = 0; i < 4; i++) {
    max_count = max((float)pulse_counts[i], max_count);
  }

  bool relative_ok = (max_count / (float)pulse_counts[channel_idx]) < 1.5;
  return (relative_ok && absolute_ok);
}
*/


void incrementCount0() { pulse_counts[0]++; }
void incrementCount1() { pulse_counts[1]++; }
void incrementCount2() { pulse_counts[2]++; }
void incrementCount3() { pulse_counts[3]++; }
