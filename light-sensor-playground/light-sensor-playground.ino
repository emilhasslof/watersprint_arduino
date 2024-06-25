int LIGHT_SENSOR = A3;
int NBR_MEASURES = 1000;

bool do_measure = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LIGHT_SENSOR, INPUT);
}

void loop() {
  if (!light_on()) {
    return;
  }


  float max = measurePeak(NBR_MEASURES);
  Serial.print("Max: ");
  Serial.println(max);

}

bool light_on(){
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

float measurePeak(int num) {
  int start_time = millis();
  float max = 0;
  int reading = 0;
  float v_out = 0;
  for(int i = 0; i < num; i++) {
    reading = analogRead(LIGHT_SENSOR);
    v_out = reading * 5.0 / 1024;
    if(v_out > max) { max = v_out; }
    //Serial.print(String(v_out) + " : ");
  }
  int end_time = millis();
  int elapsed_time = end_time - start_time;
  //Serial.println("Measure period: " + String(elapsed_time) + "ms");
  return max;
}
