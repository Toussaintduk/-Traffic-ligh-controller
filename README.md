# 🚦 Four-Way Traffic Light Controller with Pedestrian Crossings (AVR)

## 📘 Background

Traffic light controllers are **critical embedded systems** that manage traffic flow and ensure pedestrian safety at intersections.  
This project demonstrates the **design and implementation of a four-way traffic light controller** with pedestrian crossings using an **AVR microcontroller (ATmega324P)**.  
It applies key embedded concepts including **I/O operations, timers, and interrupts**.

This work was completed as a **class assignment** under the supervision of  
👨‍🏫 **Prof. Kizito NKURIKIYEYEZU**  
for the **EEE 3261 – Microcontroller and Embedded Systems** module  
in the **Electrical Power Engineering Department**  
at the **University of Rwanda**.

---

## 🧰 Hardware Setup

### 🔧 Components Used
| Component | Function |
|------------|-----------|
| **ATmega324P** | Main microcontroller |
| **LEDs (12 total)** | Traffic lights for four directions (N, E, S, W) |
| **Pedestrian LEDs (4 pairs)** | Walk/Don’t-Walk indicators |
| **Push Buttons (4)** | Pedestrian crossing request inputs |
| **Buzzer** | Audible pedestrian warning |
| **7-Segment Display** | Countdown timer display |

### 🧩 Port Configuration
- **North LEDs:** PB0–PB2 (R, Y, G)  
- **East LEDs:** PB3–PB5 (R, Y, G)  
- **South LEDs:** PC0–PC2 (R, Y, G)  
- **West LEDs:** PC3–PC5 (R, Y, G)  
- **Pedestrian LEDs:** PD0–PD3  
- **Push Buttons:** PD4–PD7 (with pull-ups)  
- **Buzzer:** PA0  
- **7-Segment Display:** PA1–PA7  

---

## ⚙️ System Features

### 🚦 Traffic Light Sequence
- Implements a **standard traffic cycle**: Green → Yellow → Red  
- **North/South** and **East/West** directions operate as synchronized pairs  
- Timing:  
  - Green phase: 20 seconds  
  - Yellow phase: 5 seconds  
  - All-red safety phase: 2 seconds  

### 🚶 Pedestrian Crossing
- Pedestrians can press a button to request crossing  
- Crossing phase starts **after current traffic cycle completes**  
- Pedestrian LEDs show “Walk” and “Don’t Walk”  
- **Countdown timer** (7-segment display) shows remaining time  
- **Buzzer** sounds during crossing, faster beeps during the last 3 seconds  

---

## 🔄 Operating Modes

| Mode | Description |
|------|-------------|
| **Normal Mode** | Standard 20-second green light sequence |
| **Rush Hour Mode** | Extends green light to 30 seconds on main road (N/S) |
| **Night Mode** | Flashing yellow for main road, red for side road |
| **Emergency Mode** | Gives green to one direction only, all others red |

---

## 🧠 Implementation Requirements
- **CPU Clock Frequency:** `1000000UL (1 MHz)`  
- **Timer1** used to generate 1-second time base via interrupts  
- **Non-blocking timing** — no `_delay_ms()`  
- **State Machine Design:** clean state transitions and fail-safe sequencing  
- **Direct I/O operations:** via AVR port manipulation  
- **Interrupt-driven button inputs** for pedestrian calls  
- **Debouncing** implemented for all buttons  
- **Visual and audible** pedestrian feedback  

---

## 🧪 Software & Simulation Setup

### 🧰 Tools Used
| Tool | Purpose |
|------|----------|
| **Atmel Studio 7** | Writing, compiling, and building HEX file |
| **Proteus 8 Professional** | Circuit simulation and testing |

### ⚙️ CPU Clock Configuration
In your Atmel Studio code, define the system clock as:
```c
#define F_CPU 1000000UL
