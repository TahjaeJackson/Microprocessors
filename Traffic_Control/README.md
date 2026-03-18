# Microprocessors: Traffic Control System on Zynq-7000 (Zybo Z7)

## Overview

This project implements a **traffic control system** on the **Zybo Z7 (Zynq-7000)** development board. It combines low-level embedded C programming with hardware-software integration to simulate and control traffic light behavior using peripherals such as LEDs, timers, ADCs, and communication interfaces.

The system demonstrates how a processor interacts with hardware components in a real-time environment, making it a practical application of microprocessor systems and embedded design principles.


## Objectives

- Design a modular embedded system for traffic control
- Interface with hardware peripherals (GPIO, ADC, timers)
- Implement real-time control logic in C
- Integrate multiple software and hardware modules
- Understand hardware-software co-design using the Zynq platform


## System Architecture

The project is organized into modular components that interact through well-defined interfaces:

Traffic_Control/
│
├── main.c # Entry point of the application
├── platform.c/.h # Platform initialization (Zynq setup)
├── platform_config.h # Hardware configuration
│
├── led.c/.h # LED control (traffic lights)
├── adc.c/.h # Analog input handling (e.g., sensors)
├── io.c/.h # General I/O abstraction
├── servo.c/.h # Servo motor control (optional/extension)
│
├── gic.c/.h # Interrupt controller (GIC)
│
├── lscript.ld # Linker script
├── Xilinx.spec # Build configuration
└── README.md


---


## Key Components

### 1. Traffic Control Logic (`main.c`)
- Implements the state machine for traffic lights
- Controls transitions between:
  - Red
  - Yellow
  - Green
- May incorporate timing or sensor-based decision-making

---

### 2. LED Module (`led.c / led.h`)
- Controls onboard LEDs representing traffic lights
- Abstracts GPIO operations
- Enables clean separation between logic and hardware

---

### 3. ADC Module (`adc.c / adc.h`)
- Reads analog signals (e.g., simulated traffic density)
- Converts sensor input into usable digital values
- Can influence traffic timing dynamically

---

### 4. I/O Module (`io.c / io.h`)
- Handles low-level input/output operations
- Provides reusable interface for hardware communication

---

### 5. Interrupt System (`gic.c / gic.h`)
- Configures the **Generic Interrupt Controller (GIC)**
- Enables responsive, event-driven behavior
- Useful for:
  - Timers
  - External signals
  - Sensor triggers

---

### 6. Platform Layer (`platform.c / platform.h`)
- Initializes hardware and runtime environment
- Sets up:
  - UART (for debugging)
  - Memory
  - Processor configuration

---

## Zybo Z7 Board Overview

The **Zybo Z7** is a development board based on the **Xilinx Zynq-7000 SoC**, which integrates:

### Processing System (PS)
- Dual-core ARM Cortex-A9 processor
- Runs embedded C applications
- Handles high-level control logic

### Programmable Logic (PL)
- FPGA fabric
- Enables custom hardware acceleration and digital design

---

### Key Features of the Zybo Z7

- ARM + FPGA hybrid architecture
- Onboard LEDs, switches, and buttons
- Analog inputs (via XADC)
- UART for serial communication
- GPIO interfaces
- PWM support for motors/servos

---

### Why Zybo Z7 for This Project?

- Combines **software flexibility** (ARM CPU) with **hardware control** (FPGA)
- Ideal for real-time systems like traffic control
- Supports scalable designs:
  - Pure software implementation (current project)
  - Hardware-accelerated extensions (future work)

---

## How It Works

1. The system initializes the platform and hardware modules
2. The traffic controller enters a loop (or interrupt-driven flow)
3. LEDs simulate traffic lights
4. Timing or sensor inputs determine state transitions
5. Optional modules (ADC, servo) enhance realism

---

## Build & Run Instructions

### Prerequisites
- Xilinx Vivado / Vitis installed
- Zybo Z7 board connected

### Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/<your-username>/Microprocessors.git
   cd Microprocessors/Traffic_Control
