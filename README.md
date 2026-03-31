# Microprocessors on Zynq-7000 (Zybo Z7)

## Overview

This repository contains a collection of embedded systems projects developed on the **Zybo Z7 (Zynq-7000)** platform. These projects demonstrate core concepts in microprocessor systems, including hardware interfacing, interrupt-driven programming, and real-time control.

The work focuses on building modular, scalable embedded applications that integrate software running on the ARM processor with hardware peripherals on the board.

## Projects

### 1. Traffic Control System

**Location:** `Traffic_Control/`

A real-time embedded system that simulates a traffic light controller using LEDs, switches, and optional sensor input.

#### Key Features
- State-machine-based traffic light control (Red, Yellow, Green)
- GPIO-based LED control
- Button and switch interaction
- Modular hardware abstraction (LED, ADC, IO)
- Platform initialization using Zynq processing system

#### Concepts Demonstrated
- Embedded C programming
- Hardware abstraction layers
- Real-time system design
- Peripheral interfacing (GPIO, ADC)


### 2. Dual UART Interrupt System

**Location:** `dual_uart_interrupt_system/` *(formerly Module5)*

An interrupt-driven communication system that bridges two UART interfaces and enables real-time data transfer and packet-based messaging.

#### Key Features
- Dual UART communication (UART0 ↔ UART1)
- Interrupt-driven receive handling using GIC
- Structured packet protocol (`ping_t`)
- Button-triggered events (ping, update, exit)
- LED feedback for system state
- Echo and forwarding between UART channels

#### Concepts Demonstrated
- Interrupt-driven programming
- Serial communication (UART)
- Embedded communication protocols
- Event-driven system design
- Hardware-software co-design


## Hardware Platform: Zybo Z7 (Zynq-7000)

All projects run on the **Zybo Z7 development board**, which features the **Xilinx Zynq-7000 SoC**.

### Key Architecture

- **Processing System (PS):**
  - Dual-core ARM Cortex-A9
  - Runs embedded C applications

- **Programmable Logic (PL):**
  - FPGA fabric for custom hardware design


### Onboard Features Used

- UART (serial communication)
- GPIO (buttons, switches, LEDs)
- Interrupt controller (GIC)
- Analog input (XADC, optional)


## Repository Structure

Microprocessors/
│
├── Traffic_Control/ # Traffic light control system
│
├── dual_uart_interrupt_system/ # UART + interrupt-based system
│
├── USART_Integration/ # (Development workspace / Vitis project files)
│
└── README.md # This file


## Development Environment

- **Language:** C
- **Tools:**
  - Xilinx Vivado (hardware design)
  - Xilinx Vitis (software development)
- **Target Platform:** Zybo Z7 (Zynq-7000)


## Learning Outcomes

Across these projects, the following skills were developed:

- Embedded systems programming in C
- Interrupt-driven architecture
- Serial communication (UART)
- Hardware abstraction and modular design
- Real-time system behavior
- Debugging using UART and LEDs



## Author

**Tahjae Jackson**  
Dartmouth College — Computer Science & Engineering  



## Notes

This repository is part of a microprocessors / embedded systems workflow using the Zynq platform and is intended for academic and educational use.