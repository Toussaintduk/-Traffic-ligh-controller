# 🚦 Four-Way Traffic Light Controller with Pedestrian Crossings (AVR)

## 📘 Background

Traffic light controllers are **critical embedded systems** that manage traffic flow and ensure pedestrian safety at intersections.  
This project focuses on **designing and implementing** a four-way traffic light controller with pedestrian crossings using **AVR microcontrollers**, applying key concepts such as **I/O operations, timers, and interrupts**.

This work was completed as a **class assignment** under the supervision of  
 **Prof. Kizito NKURIKIYEYEZU**  
for the **EEE 3261 – Microcontroller and Embedded Systems** module  
in the **Electrical Power Engineering**  
at the **University of Rwanda**.

---

## 🧰 Hardware Setup

### 🔧 Components Used
| Component | Function |
|------------|-----------|
| **ATmega16/32** | Main microcontroller |
| **LEDs (12 total)** | Traffic lights for four directions (N, E, S, W) |
| **Pedestrian LEDs (4 pairs)** | Walk/Don’t Walk indicators |
| **Push Buttons (4)** | Pedestrian crossing request inputs |
| **Buzzer** | Audible pedestrian warning |
| **7-Segment Display** | Countdown timer display |

### 🧩 Port Configuration
- **North LEDs:** PB0–PB2 (R, Y, G)  
- **East LEDs:** PB3–PB5 (R, Y, G)  
- **South LEDs:** PC0–PC2 (R, Y, G)  
- **West LEDs:** PC3–PC5 (R, Y, G)  
- **Pedestrian LEDs:** PD0–PD3  
- **Buttons:** PD4–PD7 (with pull-ups)  
- **Buzzer:** PA0  
- **7-Segment Display:** PA1–PA7  

---

## ⚙️ System Features

### 1. Basic Traffic Light Control
- Standard **Green → Yellow → Red** sequence  
- **North/South** and **East/West** directions operate as synchronized pairs  
- Timing parameters:  
  - Green: 20s  
  - Yellow: 5s  
  - All-Red: 2s  

### 2. Pedestrian Crossing System
- Pedestrians press buttons to request crossing  
- System completes the current vehicle phase before activating pedestrian mode  
- **Countdown timer** displays remaining crossing time  
- **Buzzer** activates during crossing phase  
- **Crossing time:** 10 seconds (final 3 seconds have faster beeps)

---

## 🔄 Operating Modes

| Mode | Description |
|------|-------------|
| **Normal Mode** | Standard 20-second green phase |
| **Rush Hour Mode** | 30-second green for main road (N/S) |
| **Night Mode** | Flashing yellow for main road, flashing red for side road |
| **Emergency Mode** | Activated via special input; all red except for emergency direction |

---

## ⏱ Implementation Requirements

- **Timer1** provides a precise 1-second interrupt-driven time base  
- **Non-blocking timing** (no `_delay_ms()` functions)  
- Designed using a **finite state machine (FSM)**  
- **Direct port manipulation** for LED and button control  
- **Interrupt-based pedestrian requests** with proper debouncing  
- Real-time **visual and audible feedback** during operation  

---

## 💡 Display & Buzzer Logic

### 7-Segment Display
- Displays countdown for current phase  
- Flashes during final 3 seconds  
- Indicates special patterns for modes and emergencies  

### Buzzer
- Regular beeps during pedestrian crossing  
- Faster beeps during the final 3 seconds  
- Unique tones for mode changes and emergency override  

---

## 🧠 Safety Features

- **Fail-safe operation:** prevents conflicting green signals  
- **All-red safety interval** between direction changes  
- **Pedestrian safety lockout:** no “Walk” signal during vehicle green  
- **Watchdog timer** for automatic system recovery  
- **Safe mode fallback** during detected faults  

---

## 🧪 Testing & Validation

### ✅ Functional Testing
- Verified timing accuracy and sequence logic  
- Tested pedestrian requests under various traffic scenarios  
- Validated Rush Hour, Night, and Emergency modes  

### 🧩 Safety Testing
- Confirmed absence of conflicting signals  
- Verified all-red transitions and pedestrian safety  
- Simulated fault recovery using watchdog functionality  

### 📊 Documentation
- Complete **state machine diagram**  
- **Timing chart** and **truth table** for signal logic  
- User guide describing system operation and setup  

---

## 🖼️ Simulation Preview
*(To be added)*  
Include Proteus simulation screenshots showing:
- The four-way intersection layout  
- Traffic light operation sequence  
- Pedestrian crossing activation with countdown and buzzer  

---

## 🧑‍💻 Author

**Dukundimana Toussaint**  
🎓 Electrical Power Engineering & Data Science Student  
📍 University of Rwanda  
💬 Passionate about Embedded Systems, IoT, and AI for Sustainable Development  

---

## 👨‍🏫 Supervised by

**Prof. Kizito NKURIKIYEYEZU**  
Lecturer, Department of Electrical Power Engineering  
**University of Rwanda – College of Science and Technology**

---

> “Safety through intelligent embedded systems — one intersection at a time.”
