int AL_VD = A0;
int AH_VD = A5;
int LIGHT_SENSOR = A3;
float v_in = 5;

int R1 = 12000;
float RESISTANCE_BREAKPOINT = 250; // Active low ok if resistance < 2000 kOhm
int NBR_LIGHT_MEASURES = 1000;
float LIGHT_BREAKPOINT = 0.32; // Find good value for this
// Light values:
// < 280 complete fail
// < 320 save for analysis
// > 320 OK

bool ah_ok = false;
bool al_ok = false;
bool light_ok = false;
bool do_measure = true;

void setup() {
    Serial.begin(9600);
    pinMode(AL_VD, INPUT);
    pinMode(AH_VD, INPUT);
    pinMode(LIGHT_SENSOR, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    analogReference(EXTERNAL);
}

void loop() {
  al_ok = false;
  ah_ok = false;
  light_ok = false;

  if (!lightOn()) {
    do_measure = true;
    return;
  } else if (!do_measure) {
    return;
  } 

  do_measure = false;

  //Light Sensor
  float light_peak = measurePeak(NBR_LIGHT_MEASURES);
  //Serial.println("LIGHT = " + String(light_peak, 2));
  if(light_peak >= LIGHT_BREAKPOINT) {
    light_ok = true;
  }

  // Active Low
  int al_reading = analogRead(AL_VD);
  float al_v_out = al_reading * 5.0 / 1024; // Convert to voltage
  //Serial.println(v_out);
  float R2 = R1 / ((v_in / al_v_out) - 1);
  //Serial.print("LOW  = ");
  //Serial.println(R2);
  if(R2 < RESISTANCE_BREAKPOINT && R2 > 100) {
    al_ok = true;
  }

  delay(5);

  //Active High
  int ah_reading = analogRead(AH_VD);
  float ah_v_out = ah_reading * 5.0 / 1024;
  //Serial.println("HIGH = " + String(v_out, 2));
  if(ah_v_out > 4.45) {
    ah_ok = true;
  }
  delay(5);

  if(al_ok && ah_ok && light_ok) {
    Serial.print("All OK! Light: ");
    Serial.println(String(light_peak, 3));
    return;
  } else if (al_ok && ah_ok && !light_ok) {
    Serial.print("---------- SIGNAL OK, LIGHT FAIL! ----------: ");
    Serial.println(String(light_peak, 3));
    return;
  }
  Serial.print("---------- FAIL! ----------");
  if(!al_ok) {
    Serial.print(" , active low R2 = " + String(R2));
  } 
  if(!ah_ok) {
    Serial.print(" , active high = " + String(ah_v_out));
  } 
  if (!light_ok) {
    Serial.print(" , light = " + String(light_peak, 3));
  }
  Serial.println();
}


float measurePeak(int num) {
  float max = 0;
  int reading = 0;
  float v_out = 0;
  for(int i = 0; i < num; i++) {
    reading = analogRead(LIGHT_SENSOR);
    v_out = reading * 5.0 / 1024;
    if(v_out > max) { max = v_out; }
    delay(1);
  }
  return max;
}

bool lightOn() {
  float max = 0;
  float val = 0;
  for(int i = 0; i < 5; i++){
    val = analogRead(LIGHT_SENSOR);
    if (val > max) {
      max = val;
    }
    delay(2);
  }
  return max > 0;
}

void blink() {
  for(int i = 0; i < 20; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(25);
    digitalWrite(LED_BUILTIN, LOW);
    delay(25);
  }
}
// Resistance AL: open | closed = 12276012.00 | 94.49