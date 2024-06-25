// Steinhart-Hart model coefficients for NTC thermistor NCP18XH103J03RB
// Calculated using https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
// resistance-temperature pairs taken from datasheet https://www.mouser.se/datasheet/2/281/r44e-522712.pdf
float A = 0.9017477480 * pow(10, -3);
float B = 2.489190310 * pow(10, -4);
float C = 2.043213857 * pow(10, -7);

double inputVoltage = 5;
int R1 = 10000; // Resistance of R1 in ohms

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

// Switch from 0-index to 1-index to match markings on the tubes  
  double temp_1 = readTemp(A0);
  double temp_2 = readTemp(A1);
  double temp_3 = readTemp(A2);
  double temp_4 = readTemp(A3);
  double temp_5 = readTemp(A4);
  double temp_6 = readTemp(A5);
  int minutes_since_start = millis() / 60000;


  Serial.println(
    "T1 = " + String(round(temp_1)) + ", " +
    "T2 = " + String(round(temp_2)) + ", " +
    "T3 = " + String(round(temp_3)) + ", " +
    "T4 = " + String(round(temp_4)) + ", " +
    "T5 = " + String(round(temp_5)) + ", " +
    "T6 = " + String(round(temp_6)) + ", " + 
    "Minutes since start = " + minutes_since_start
  );


  delay(60000);
}

// Reads specified pin and calculates temperature
double readTemp(int pin){
  double VOut = analogReadVoltage(pin);
  int R2 = ntcResistance(inputVoltage, VOut, R1);
  return tempFromResistance(R2);
}

double analogReadVoltage(int pin) {
  double reading = analogRead(pin);
  double voltage = (reading / 1024) * 5;
  return voltage;
}

// Calculates resistance of ntc-thermistor in a circuit set up as follows:
// https://upload.wikimedia.org/wikipedia/commons/2/21/Resistive_divider2.svg
// where the ntc-thermistor is R2
int ntcResistance(double VIn, double VOut, int R1){
  return R1 / ((VIn/VOut) - 1);
}

// Calculates temperature in celsius using the Steinhart-Hart equation
double tempFromResistance(int res){
  double log_r = log(res);
  double log_r3 = log_r * log_r * log_r;
  return 1.0 / (A + B * log_r + C * log_r3) - 273.15;
}


