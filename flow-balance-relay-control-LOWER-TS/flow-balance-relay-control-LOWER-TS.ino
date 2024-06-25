#include "PinChangeInterrupt.h"

// pin definitions
int FLOW_PINS[] = {2, 3, 4, 5};
int POWER_TOGGLES = A0;
int TOGGLES_TEST_SIGNAL = 9;
int TOGGLE_PINS[] = {A4, A1, A5, A3};
int POWER_RELAY_PIN = 12;
int CLOSE_RELAY_PIN = 13;
int INDICATOR_PINS[] = {6, 8, 10, 7};
bool blink = true;

boolean channels_active[] = {true, true, true, true};
int pulse_counts[] = {0, 0, 0, 0};

void setup()
{
  Serial.begin(9600);

  // Set up pins
  pinMode(POWER_RELAY_PIN, OUTPUT);
  digitalWrite(POWER_RELAY_PIN, HIGH);
  pinMode(CLOSE_RELAY_PIN, OUTPUT);
  pinMode(POWER_TOGGLES, OUTPUT);
  digitalWrite(POWER_TOGGLES, LOW);
  //analogWrite(POWER_TOGGLES, 10);

  for (int i = 0; i < 4; i++)
  {
    pinMode(FLOW_PINS[i], INPUT_PULLUP);
    pinMode(TOGGLE_PINS[i], INPUT);
    pinMode(INDICATOR_PINS[i], OUTPUT);
  }
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[0]), incrementCount0, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[1]), incrementCount1, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[2]), incrementCount2, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(FLOW_PINS[3]), incrementCount3, RISING);
}

void loop()
{
  blink = !blink;
  updateChannelsActive();
  for (int i = 0; i < 4; i++)
  {
    pulse_counts[i] = 0;
  }
  delay(2000);

  int high = 0;
  int low = 999999;
  for (int i = 0; i < 4; i++)
  {
    if (channels_active[i])
    {
      if (pulse_counts[i] > high)
      {
        high = pulse_counts[i];
      }
      if (pulse_counts[i] < low)
      {
        low = pulse_counts[i];
      }
    }
  }
  bool all_ok = true;
  for(int i = 0; i < 4; i++)
  {
    bool absolute_ok = pulse_counts[i] > 7;
    bool relative_ok = (high / (float)pulse_counts[i]) < 1.3;
    
    if(relative_ok){
      digitalWrite(INDICATOR_PINS[i], HIGH);
    } else {
      if(blink) {
        digitalWrite(INDICATOR_PINS[i], HIGH);
      } else {
        digitalWrite(INDICATOR_PINS[i], LOW);
      }
    }

    if(!absolute_ok){
      digitalWrite(INDICATOR_PINS[i], LOW);
      if(channels_active[i])
      {
        all_ok = false;
      }
    }

    if(all_ok) 
    {
      digitalWrite(CLOSE_RELAY_PIN, LOW);
    } else 
    {
      digitalWrite(CLOSE_RELAY_PIN, HIGH);
    }
  }

  // Debugging
  
  Serial.println("Channels active: " +
                 String(channels_active[0]) + ", " +
                 String(channels_active[1]) + ", " +
                 String(channels_active[2]) + ", " +
                 String(channels_active[3]));
  Serial.println(String(pulse_counts[0]) + ", " + String(pulse_counts[1]) + ", " + String(pulse_counts[2]) + ", " + String(pulse_counts[3]));
  
}

void incrementCount0() { pulse_counts[0]++; }
void incrementCount1() { pulse_counts[1]++; }
void incrementCount2() { pulse_counts[2]++; }
void incrementCount3() { pulse_counts[3]++; }

void updateChannelsActive()
{
  // Test signal is connected via 1 MOhm resitor, so the test signal will be drowned out if cable is connected to 5VDC
  pinMode(TOGGLES_TEST_SIGNAL, OUTPUT);
  digitalWrite(TOGGLES_TEST_SIGNAL, HIGH);
  delay(10);
  for (int i = 0; i < 4; i++) {
    channels_active[i] = (digitalRead(TOGGLE_PINS[i]) == HIGH);
  }

  digitalWrite(TOGGLES_TEST_SIGNAL, LOW);
  delay(10);
  for (int i = 0; i < 4; i++) {
    if(channels_active[i] && digitalRead(TOGGLE_PINS[i] == LOW)) {
      // Disconnected cable detected
      channels_active[i] = false;
    } else {
      channels_active[i] = true;
    }
  }
  // test signal is deactivated between updates
  pinMode(TOGGLES_TEST_SIGNAL, INPUT);
}


