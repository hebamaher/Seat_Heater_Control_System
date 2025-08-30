# Seat Heater Control System

## Overview
This project implements a **seat heater control system** for the **driver** and **passenger** seats of a car.  

Each seat consists of:
1. A **button** to set the seat heating level.  
2. A **temperature sensor** to monitor seat temperature.  
3. A **heating element** controlled by variable intensity power.  
4. A **shared display screen** showing the system state for both seats.  

---

## System Features
1. **Heating Levels**
   - **Off** → Heater disabled.  
   - **Low** → Target: 25 °C.  
   - **Medium** → Target: 30 °C.  
   - **High** → Target: 35 °C.  

2. **Temperature Control**
   - Maintains temperature within ±2 °C of the target.  

3. **Diagnostics**
   - Sensor failure if readings are **outside 5 °C – 40 °C**.  
   - Failures are logged with timestamp.  

4. **User Interface**
   - All data is shown on the car’s screen.  

5. **Controls**
   - **2 buttons** in the center console → one for each seat.  
   - **1 button** on the steering wheel → controls driver seat heater for easy access.  

---

## Functionality
1. **Button Cycling**  
   - Initial state: **Off**.  
   - Each press cycles through → Off → Low → Medium → High → Off.  

2. **Heater Intensity Control**
   - ≥ 10 °C below target → **High intensity**.  
   - 5–10 °C below target → **Medium intensity**.  
   - 2–5 °C below target → **Low intensity**.  
   - Above target → Heater disabled.  
   - Heater re-enabled once temperature drops ≥ 2 °C below target.  

   🔧 **Simulation (using LEDs):**  
   - Green = Low intensity  
   - Blue = Medium intensity  
   - Cyan = High intensity  

3. **Temperature Sensor**
   - Connected via **ADC**.  
   - Uses LM35 sensor (or a potentiometer for testing).  
   - Mapping: `0–3.3V` → `0–45 °C` (valid range: 5–40 °C).  

4. **UART Display**
   - Displays current temperature, heating level, and heater state.  

5. **Error Handling**
   - Invalid sensor readings disable the heater.  
   - Red LED turns on to indicate failure.  
   - Red LED turns off when sensor returns to valid range.  

---

## Requirements
1. **Controller** → Implemented on **Tiva C**.  
2. **Software** → Built with **FreeRTOS**.  
   - At least **6 tasks**.  
   - Reusable task implementations for driver and passenger seat.  
   - Shared resources → protected with **mutexes**.  
   - Event-based tasks → handled using **event flags** or **semaphores**.  
   - Safe inter-task data sharing.  
   - Buttons → **edge-triggered interrupts** (not polling).  

3. **MCAL Modules** → Implement:
   - **GPIO**, **UART**, **GPTM**, **ADC**.  

4. **Diagnostics**
   - Store failures + timestamps (using GPTM).  
   - Log last heating level + timestamp.  

5. **Runtime Measurements**
   - Execution time per task.  
   - CPU load.  
   - Resource lock time per task (using **trace hook macros**).  
   - Accuracy: **0.1 ms** (validated with FreeRTOS runtime stats but manual measurements required).  

---

## Hardware Components
For each seat (driver & passenger):  
- Temperature sensor (**potentiometer for testing**)  
- **Red LED** → error reporting  
- **Heater** → simulated with **Green** + **Blue LEDs**  
- **Button** → heating level control  

Additional for driver seat:  
- **Steering wheel button** for easy heater control  

---

## System Summary
- **2 Seats (Driver + Passenger)**  
- **3 Buttons total**  
- **Temperature Sensor (or Potentiometer)**  
- **LEDs (Red, Green, Blue)** → simulate heater + errors  
- **FreeRTOS-based Controller** running on **Tiva C**  
