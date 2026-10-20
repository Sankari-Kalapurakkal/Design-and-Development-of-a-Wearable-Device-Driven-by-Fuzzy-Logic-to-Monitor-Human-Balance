# Design-and-Development-of-a-Wearable-Device-Driven-by-Fuzzy-Logic-to-Monitor-Human-Balance
### Embedded C++ Firmware for On-Chip Edge Computing and Fall Risk Analytics

This repository houses the core production firmware developed for my MSc Dissertation in Electrical and Electronic Engineering at Sheffield Hallam University. The system interfaces an MPU6050 MEMS Inertial Measurement Unit (IMU)** over I2C to capture physical balance anomalies and computes mathematical risk sets directly on the edge microcontroller using an integrated Mamdani-style Fuzzy Inference Engine.

---

🔬 Project Overview & Performance Metrics

This project optimized a wireless, accelerometer-based wearable device strategically placed on the waist for real-time human balance monitoring and fall prevention. The system was validated under both stationary and dynamic motion states, achieving robust clinical performance benchmarks:

* Overall Success Rate:90% accuracy in predicting true positive and negative fall conditions.
* Sensitivity / Recall:100% (1.0), ensuring zero false negatives (critical for elderly safety applications).
* Precision:80% (0.8) positive predictive value.
* F1 Score:0.88889, confirming balanced model performance.

📐 Calibrated Orientation Mapping
The system maps precise boundary conditions to mathematically isolate fall vectors from daily human activities:
Fall Event Triggers (Forward/Backward Motion): Deployed a strict system threshold set at 25. Fall events captured an orientation window where the Roll angle ranged between 32.71° to 35.51°, and the Pitch angle fell between -91.95° to -26.95°.
Normal Non-Fall Activities: Verified clean baselines where the Roll angle spanned 0° to 64.99° and the Pitch angle ranged from -123.08° to -94°.

---

 Mathematical & Structural Architecture

The device performs high-speed loop routines divided into four primary processing stages:

1. Kinematic Vector Extraction: Raw digital values are parsed from the 16-bit registers of the MPU6050 accelerometer and converted to angular vectors using mathematical trigonometric relations.
2. Static Posture Deadband Filtering: An optimized acceleration gate checks static windows (`0.5 < AccX < 1.20`) to lock steady positions to a strict zero-reference, preventing false triggers while resting or standing.
3. Fuzzification Layer: Translates real-time angular metrics into triangular membership profiles representing linguistic categories (Low, Medium, High).
4. Defuzzification: Solves parallel antecedent rules utilizing a centroid-based singleton deduction logic mapped against the output domain to output crisp risk indicators.

---

🛠️ Hardware Requirements & Pinout

| Components       | Connection Type    | Microcontroller Pin | MPU6050 Pin |

| MCU Platform     | I2C Hardware Bus   | `A5 (SCL)`          | `SCL` |
| MEMS Sensor      | I2C Hardware Bus   | `A4 (SDA)`          | `SDA` |
| Power Supply     | Common Rail VCC    | `3.3V / 5V`         | `VCC` |
| Ground Reference | Common Ground Rail | `GND`               | `GND` |

---

💻 How to Flash and Validate

1. Prerequisites: Activate the standard `Wire.h` communications environment within your local IDE.
2. Sensor Calibration: Keep the sensor completely level on power-up for 200 execution loop cycles. The system automatically executes `calculate_IMU_error()` to map local drift matrix values.
3. Baud Rate: Connect your system serial interface at `19200 Baud` to stream live Roll, Pitch, and crisp Fall Risk value determinations.

Note: Base IMU hardware register parsing structures were adapted from open-source MPU6050 implementation documentation, with custom localized calibration matrices and the entire Fuzzy Logic core inference architecture uniquely engineered for this research application.
