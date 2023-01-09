# Ultrasound Pen and Tracking System

## Introduction
The aim of the project is to capture the position of a stylus on any two-dimensional Cartesian plane and obtain its coordinates in software. This project primarily uses the principles of ultrasound waves and their propagation through air to determine the coordinates of the stylus. The project also uses peripherals such as Wide timers, PWMs, I2C and GPIOs in the TM4C123GH6PM microcontroller on the TI Tiva C launchpad board.

## Hardware
The project implements included the development of the Stylus and the Receiver circuit
### Stylus
The Stylus is a simple electronic circuit enclosed in a 3D-printed case. Electronics include
* IC: 556 dual timer
* Resistors: 1.8kΩ, 1.5kΩ, 470Ω, 33Ω
* Diodes: IN914
* Capacitors: 1µF, 0.1µF
* IR emitter
#### Ultrasound emitter
* Supply: 9V DC
* Switch

#### Schematic
![Alt text](README_Images/image15.png?raw=true "")

Figure 1: Schematic of Stylus

![Alt text](README_Images/image5.png?raw=true "")

Figure 2a: Stylus

![Alt text](README_Images/image12.png?raw=true "")

Figure 2b: Stylus Circuit

### Receiver
The receiver circuit consists of the following components:
* Sensors: Ultrasound Receivers, IR Receiver
* IC: TLC072, MCP6544, RedBoard
* Resistors: 470Ω, 4.7kΩ, 10kΩ, 47kΩ, 470kΩ
* Capacitors: 1µF, 0.1µF
* Diodes: IN914
* Buzzer

#### Block Diagram
Figure 3 shows the block diagram of the receiver circuit
![Alt text](README_Images/image13.jpg?raw=true "")

Figure 3: Receiver circuit block diagram

![Alt text](README_Images/image2.png?raw=true "")

Figure 4: Receiver Circuit

## Operation
When the user presses the button on the Stylus, an IR pulse and a train of Ultrasound pulses are emitted. The IR pulse (an electromagnetic wave) reaches the receiver circuit first and thereby, triggers a falling edge interrupt on IR_IN.

The interrupt service routine of the IR pulse, ir_interrupt_handler,will start three wide timers - WTIMER0A, WTIMER0B and WTIMER1A, each configured as shown in Table 1.

![Alt text](README_Images/table1.png?raw=true "")

Table 1: Timer Configuration

When the Ultrasound receivers detect an input, timer interrupts are triggered and will be handled by their respective service routines - sA_interrupt_handler, sB_interrupt_handler, sC_interrupt_handler. Within each of the timer interrupt handlers, the timer register value at the instance of interrupt trigger is read and stored in an array - g_timer_n_FIFO, where n denotes the respective array for each timer-receiver pair. Additionally, the ISR also resets the timer value register, clears the interrupt flag, and sets one flag indicating the reception of an Ultrasound signal.

The above steps are elucidated in Figure 5.

![Alt text](README_Images/image3.jpg?raw=true "")

Figure 5: Signal detection and Interrupt handling

### Triangulation
The raw timer values in the g_timer_n_FIFO arrays are converted to manipulable distance (metric, mm) values by the following formula:

![Alt text](README_Images/image17.png?raw=true "")

Where,
40e6 = System clock speed
1000 = Convert to mm
343 = Speed of sound in air at standard temperature and pressure

The distance so calculated shall be the distance of the source of the ultrasound (position of the Stylus) from the Ultrasound sensors.

![Alt text](README_Images/image9.jpg?raw=true "")

Figure 6: Triangulation

From Figure 6, applying principles of overlapping triangles and solving for linear equation of two variables, we obtain the coordinates x and y of the Stylus as:

The coordinates of the three sensors shall be configured by the user and stored in the EEPROM.

### General Configurations
![Alt text](README_Images/table2.png?raw=true "")

Table 2: GPIO Pins Configuration

![Alt text](README_Images/table3.png?raw=true "")

Table 3: PWM Configurations

## Outputs
Table 4 lists the supported commands in software

![Alt text](README_Images/table4.png?raw=true "")

Table 4: Commands and their sample outputs
Screengrabs of outputs without stylus input
