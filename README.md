# 🚦 Four-Way Traffic Light Controller with Pedestrian Crossings (AVR)

## 📘 Background

Traffic light controllers are **critical embedded systems** that manage traffic flow and ensure pedestrian safety at intersections.  
This project involves **designing and implementing** a four-way traffic light controller with pedestrian crossings using **AVR microcontrollers**, applying concepts of **I/O operations, timers, and interrupts**.  

Developed as part of **EEE 3261 — Microcontroller & Embedded Systems** at the **University of Rwanda**, this lab exercise integrates both **hardware simulation** (Proteus) and **embedded C programming** (AVR-GCC).

---

## 🧰 Hardware Setup

### 🔧 Components Used
| Component | Function |
|------------|-----------|
| **ATmega16/32** | Main microcontroller |
| **LEDs (12 total)** | Traffic lights for 4 directions (N, E, S, W) |
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
- **North/South** and **East/West** operate as synchronized pairs  
- Timing:  
  - Green: 20s  
  - Yellow: 5s  
  - All-Red: 2s  

### 2. Pedestrian Crossing System
- Pedestrian requests via push buttons  
- System completes current traffic phase before activating pedestrian crossing  
- **Countdown timer** (7-segment display) shows crossing time  
- **Buzzer** activates during crossing phase  
- **Crossing time:** 10s  
- Last 3 seconds use **faster beeping** to indicate "hurry up"

---

## 🔄 Operating Modes

| Mode | Description |
|------|-------------|
| **Normal Mode** | Standard 20s green cycle |
| **Rush Hour Mode** | 30s extended green for main road (N/S) |
| **Night Mode** | Flashing yellow (main) and flashing red (side) |
| **Emergency Mode** | Activated via special button; all red except emergency lane |

---

## ⏱ Implementation Requirements

- **Timer1** used for precise 1-second base (interrupt-driven, no delay functions)  
- Fully **non-blocking timing** using interrupts  
- Designed as a **finite state machine (FSM)** for clear logic flow  
- **Direct port manipulation** for efficiency  
- **Button debouncing** and interrupt-based pedestrian request handling  
- Visual and audible feedback for all user interactions  

---

## 💡 Display & Buzzer Logic

### 7-Segment Display
- Shows countdown for current green/pedestrian phase  
- Updates every second  
- Flashes during last 3 seconds  
- Displays special patterns for system modes and emergency alerts  

### Buzzer
- Beeps regularly during pedestrian crossing  
- Increases frequency in final 3 seconds  
- Plays unique patterns for mode changes and emergencies  

---

## 🧠 Safety Features

- **Fail-safe operation:** prevents conflicting green signals  
- **All-red safety phase** between transitions  
- **Pedestrian safety lockout:** prevents walk signal during vehicle green  
- **Watchdog timer** for system reset in case of fault  
- **Safe mode fallback** if system errors occur  

---

## 🧪 Testing & Validation

### ✅ Functional Testing
- Verify all traffic sequences and timings  
- Test pedestrian button response under various traffic conditions  
- Validate special modes (rush hour, night, emergency)

### 🧩 Safety Testing
- Confirm no conflicting green signals  
- Ensure all-red intervals function correctly  
- Test system recovery from simulated faults  

### 📊 Documentation
- Complete **state machine diagram**  
- **Timing chart** and **truth table** for signal combinations  
- User manual explaining normal and emergency operation  

---

## 🧑‍💻 Author

**Dukundimana Toussaint**  
🎓 Electrical Power Engineering & Data Science Student  
📍 University of Rwanda  
💬 Passionate about Embedded Systems, IoT, and AI for sustainable development  

---

## 📜 License
This project is released under the **MIT License** — free for academic and educational use.

---

> “Safety through intelligent embedded systems — one intersection at a time.”  
