// Temperature -> Fan control (D13) using Steinhart-Hart or Beta equation
// Wiring assumption:
// 5V --- R_PULLUP (10k) --- A0 --- NTC (10k @ 25C) --- GND

const int sensorPin = A0;
const int fanPin = 13;

const float VCC = 5.0;
const float R_PULLUP = 10000.0; // 10k pull-up resistor

// --- Steinhart-Hart coefficients (example values for a common 10k NTC) ---
// Replace these A,B,C with the ones from your thermistor datasheet for best accuracy
const double SH_A = 0.001129148;
const double SH_B = 0.000234125;
const double SH_C = 0.0000000876741; // 8.76741e-08

// --- Beta model parameters (alternative) ---
const double B_COEFF = 3950.0;  // Beta value (typical 3950)
const double R0 = 10000.0;      // Resistance at T0 (10k @ 25C)
const double T0 = 25.0 + 273.15; // Reference temp in Kelvin

// Choose calculation method: true = Steinhart-Hart, false = Beta
const bool USE_STEINHART = true;

// Smoothing/averaging
const int NUM_SAMPLES = 8;
const unsigned long SAMPLE_DELAY_MS = 10; // ms between samples for averaging

// Soft-PWM parameters
const unsigned int PWM_CYCLES = 200; // cycles per print interval (adjustable)
const unsigned int PWM_PERIOD_US = 10000; // period per cycle in microseconds (10ms -> 100Hz)

void setup() {
  pinMode(fanPin, OUTPUT);
  Serial.begin(9600);
  delay(200);
  Serial.println("Starting Temp->Fan (Steinhart-Hart / Beta) demo");
}

void loop() {
  // ---------- Read and average ADC ----------
  unsigned long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; ++i) {
    sum += analogRead(sensorPin);
    delay(SAMPLE_DELAY_MS);
  }
  float adc = float(sum) / NUM_SAMPLES; // 0 - 1023

  // ---------- Convert ADC -> voltage ----------
  float vout = adc * (VCC / 1023.0);

  // Guard against divide-by-zero
  if (vout <= 0.0001) vout = 0.0001;
  if (vout >= VCC - 0.0001) vout = VCC - 0.0001;

  // ---------- Compute thermistor resistance ----------
  // Divider: Vout = Vcc * Rth / (Rpull + Rth)
  // => Rth = Rpull * Vout / (Vcc - Vout)
  double rTherm = R_PULLUP * (double(vout) / (VCC - double(vout)));

  // ---------- Compute temperature ----------
  double temperatureC = 0.0;
  if (USE_STEINHART) {
    // Steinhart-Hart: 1/T = A + B*ln(R) + C*(ln(R))^3
    double lnR = log(rTherm);
    double invT = SH_A + SH_B * lnR + SH_C * lnR * lnR * lnR;
    double tempK = 1.0 / invT;
    temperatureC = tempK - 273.15;
  } else {
    // Beta formula: 1/T = 1/T0 + 1/B * ln(R/R0)
    double tempK = 1.0 / ( (1.0 / T0) + (1.0 / B_COEFF) * log(rTherm / R0) );
    temperatureC = tempK - 273.15;
  }

  // ---------- Control fan (D13) ----------
  // thresholds: <25 off, 25-29.999 -> 50% (simulated), >=30 -> 100%
  if (temperatureC < 25.0) {
    digitalWrite(fanPin, LOW);
    Serial.print("Temp: ");
    Serial.print(temperatureC, 2);
    Serial.println(" °C | Fan: OFF");
    delay(1000); // update rate while OFF

  } else if (temperatureC < 30.0) {
    // Simulate 50% duty software PWM on D13
    Serial.print("Temp: ");
    Serial.print(temperatureC, 2);
    Serial.print(" °C | Fan: 50% (software PWM)\n");

    // we implement PWM_CYCLES cycles of (on/off) with 50% duty
    unsigned int on_us = PWM_PERIOD_US / 2;
    unsigned int off_us = PWM_PERIOD_US - on_us;
    for (unsigned int c = 0; c < PWM_CYCLES; ++c) {
      digitalWrite(fanPin, HIGH);
      delayMicroseconds(on_us);
      digitalWrite(fanPin, LOW);
      delayMicroseconds(off_us);
    }

  } else {
    digitalWrite(fanPin, HIGH);
    Serial.print("Temp: ");
    Serial.print(temperatureC, 2);
    Serial.println(" °C | Fan: 100%");
    delay(1000); // update rate while full ON
  }
}
