# Dual UART Interrupt System (Zybo Z7)

## Overview

This module implements an **interrupt-driven dual UART communication system** on the **Zybo Z7 (Zynq-7000)** platform. It enables real-time data transfer between two UART interfaces while integrating GPIO-based user input through buttons and switches.

The system demonstrates key embedded systems concepts including:
- Interrupt handling
- Serial communication (UART)
- Event-driven programming
- Hardware-software interaction

---

## Key Features

- 📡 **Dual UART Communication**
  - UART0 (9600 baud)
  - UART1 (115200 baud)
  - Bidirectional message forwarding

- ⚡ **Interrupt-Driven Architecture**
  - UART receive interrupts handled via GIC
  - Non-blocking communication

- 🔁 **Ping Protocol**
  - Structured packet communication using `ping_t`
  - Validates communication integrity

- 🎛️ **GPIO Interaction**
  - Buttons trigger system actions
  - Switches control LED states

- 💡 **LED Feedback**
  - Visual indication of system state and events

---

## System Components

### 1. UART Interfaces

Two UART peripherals are configured:

| UART | Baud Rate | Purpose |
|------|----------|--------|
| UART0 | 9600 | External communication / input |
| UART1 | 115200 | Debug output / monitoring |

---

### 2. Interrupt Handlers

#### `handler0` (UART0)
- Receives incoming data
- Forwards data to UART1

#### `handler1` (UART1)
- Echoes received characters
- Handles newline formatting (`\r → \n`)

---

### 3. Ping Protocol

```c
typedef struct {
    int type;
    int id;
} ping_t;
