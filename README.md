# Arduino Temperature-Controlled Fan Simulation

## Project Overview
This project simulates a **temperature-controlled fan** using an **Arduino Uno**, a **10k NTC thermistor**, and an **LED** to represent the fan.  

**Fan behavior:**
- Temp < 25°C → Fan OFF  
- 25°C ≤ Temp < 30°C → Fan 50% speed (software PWM)  
- Temp ≥ 30°C → Fan 100% ON  

The **Serial Monitor** shows live temperature and fan status.

---

## Components
- Arduino Uno  
- 10 kΩ NTC thermistor  
- 10 kΩ resistor  
- LED (to simulate fan)  
- 220 Ω resistor  
- Jumper wires  

---

## Wiring Diagram

   +5V
    |
   10kΩ
    |
    |----> A0 (Analog Input)
    |
  [NTC Thermistor]
    |
   GND

## Code
- File: `TempFanControl.ino`  
- Reads temperature using **Steinhart-Hart equation**.  
- Controls LED on D13 based on temperature.  
- Prints temperature and fan state to Serial Monitor at **9600 baud**.

---

## How to Run

### On Real Arduino
1. Connect the circuit.  
2. Open `TempFanControl.ino` in Arduino IDE.  
3. Select **Board → Arduino Uno** and the correct **COM port**.  
4. Click **Upload**.  
5. Open Serial Monitor to see temperature and fan state.  

### On Tinkercad Simulation
1. Create a circuit with Arduino Uno, resistor, thermistor, and LED.  
2. Paste the code into the Text Editor.  
3. Start simulation.  
4. Adjust thermistor resistance to simulate different temperatures.

---

## Notes
- D13 does not support hardware PWM, so 50% fan speed is simulated via software.  
- For accurate readings, replace Steinhart-Hart coefficients with your thermistor datasheet values.
