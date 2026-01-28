# 🛴 Smart Electric Scooter Safety System

<img src="https://res.cloudinary.com/db0qkzn6a/image/upload/v1769618329/gitdocs/user_38kuGUTnJD7r23NEbRjPSnVelVV/images/ybkvsdr52xqcbfaqy9b8.png" alt="Scooter Safety System Demo" width="100%">

An embedded safety control system for electric scooters featuring automatic obstacle detection, emergency braking, and smart charging management using PIC16F877A microcontroller.

---

## 🎯 Overview

This project implements a comprehensive safety system for electric scooters with the following capabilities:
- **Automatic emergency braking** when obstacles are detected
- **PWM-based motor control** with smooth acceleration/deceleration
- **Smart charging system** with timer and status indicators
- **LCD display** for real-time system feedback
- **Security mode** to enable/disable safety features
- **Interactive 3D web demo** for portfolio presentation

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| **Obstacle Detection** | Ultrasonic sensor triggers automatic braking when distance < 30cm |
| **Motor Control** | PWM speed regulation with forward/reverse direction |
| **LCD Display** | 16x2 display showing speed, status, and alerts |
| **Safety Toggle** | Enable/disable security features via button |
| **Charging Mode** | Automated charging with timer and LED indicators |
| **Visual Alerts** | Red/green LEDs for status feedback |

---

## 🎮 Quick Start — Try the Demo

### Interactive 3D Web Preview
**Open in browser:**
   ```
   https://rayenouerghui.github.io/securite-trotinnete/
   ```

## 🛠️ Hardware Requirements

### Components
- **Microcontroller:** PIC16F877A @ 20MHz
- **Display:** 16x2 LCD (HD44780)
- **Motor Driver:** L293D or equivalent
- **Sensors:** 
  - Ultrasonic distance sensor (ADC channel 1)
  - Potentiometer for throttle (ADC channel 0)
- **Indicators:** Red and green LEDs
- **Buttons:** 3x push buttons (security, charge, history)
- **Power Supply:** 5V DC

### Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| **RA0 (AN0)** | Throttle Input | Potentiometer for speed control |
| **RA1 (AN1)** | Distance Sensor | Obstacle detection input |
| **RB0** | Security Button | Toggle safety features on/off |
| **RB1** | Brake LED | Indicator during emergency braking |
| **RB3** | Green LED | System ready / charged status |
| **RB4** | Charge Button | Enter charging mode |
| **RB5** | History Button | Display system history |
| **RB7** | Red LED | Charging / alert indicator |
| **RC0-RC7** | LCD Interface | RS, EN, D4-D7 connections |
| **RD0, RD1** | Motor Direction | Forward/reverse control signals |
| **PWM1** | Motor Speed | PWM output for speed control |

<img src="systeme photo.png" alt="Hardware Setup" width="600">

---

## 💻 Firmware Setup

### Prerequisites
- **mikroC PRO for PIC** (recommended compiler)
- **PICkit or compatible programmer** for flashing
- **Proteus ISIS** (optional, for simulation)

### Build Instructions

1. **Open mikroC PRO for PIC**
2. **Create new project:**
   - Select device: **PIC16F877A**
   - Set clock frequency: **20 MHz**
3. **Add source file:** `trottinette.c`
4. **Configure libraries:**
   - Enable ADC library
   - Enable LCD library
   - Enable PWM library
5. **Build project** (Ctrl+F9)
6. **Flash the `.hex` file** to PIC16F877A using your programmer

### Proteus Simulation (Optional)
- Open `trottinette.DSN` in Proteus ISIS
- Load the compiled `.hex` file into the PIC16F877A component
- Run simulation to test before hardware deployment

---

## 🚀 How It Works

### System Operation

**1. Startup**
- System initializes LCD, ADC, PWM, and interrupts
- Display shows "SYSTEME PRET" (System Ready)
- Green LED turns on

**2. Normal Mode**
- Throttle position controls motor speed (0-100%)
- Speed changes smoothly using adaptive acceleration curves
- LCD displays current speed and status
- Distance sensor continuously monitors for obstacles

**3. Security Mode (Toggle via RB0)**
- **Security ON:** Automatic emergency braking enabled
- **Security OFF:** Manual control only
- LCD displays "SEC ON" or "SEC OFF"

**4. Emergency Braking**
- Triggered when obstacle distance < 30cm (ADC value < 300)
- Motor speed ramps down rapidly
- Red LED and brake indicator activate
- LCD shows "OBSTACLE!" and "FREINAGE!" (Braking)
- System waits until obstacle clears before resuming

**5. Charging Mode (Activated via RB4)**
- Press RB4 to enter charging mode
- Red LED blinks during charging
- Press RB4 four times to start charging timer
- LCD shows "CHARGING" → "TIMER" → "CHARGED"
- Green LED illuminates when fully charged

### Speed Control Algorithm

The system uses a smart speed adjustment algorithm:
- **Deadzone:** 48-52% throttle position = stop
- **Reverse:** <48% throttle = reverse direction
- **Forward:** >52% throttle = forward direction
- **Adaptive acceleration:** Speed change rate varies based on current speed
  - Faster changes at low speeds
  - Smoother changes at high speeds

---

## 📊 System Behavior

### LCD Messages

| Message | Meaning |
|---------|---------|
| `SYSTEME PRET` | System initialized and ready |
| `SEC ON` / `SEC OFF` | Security mode status |
| `CHARGING` | Charging mode active |
| `TIMER` | Timer-based charging started |
| `CHARGED` | Charging complete |
| `OBSTACLE !` | Obstacle detected |
| `FREINAGE !` | Emergency braking in progress |

### LED Indicators

| LED | Color | Status |
|-----|-------|--------|
| RB3 | Green | System ready / Fully charged |
| RB7 | Red | Charging / Alert |
| RB1 | - | Emergency braking active |

---

## 📁 Repository Structure

```
securite-trotinnete/
│
├── index.html              # 3D web demo interface
├── scooter.glb             # 3D scooter model
├── trottinette.c           # Main firmware (PIC16F877A)
├── trottinette.DSN         # Proteus schematic
├── trottinette.mcppi       # mikroC project file
├── systeme photo.png       # Hardware photo
└── README.md               # This file
```

---

## 🔧 Configuration & Customization

### Adjust Obstacle Detection Threshold
```c
// In trottinette.c
if (distance_value < 300) {  // Change 300 to your preferred threshold
    Emergency_Stop();
}
```

### Modify Acceleration Curve
```c
// Adjust speed_change values for different acceleration profiles
if (current_speed < 20) {
    speed_change = 5;  // Faster acceleration at low speeds
} else if (current_speed < 50) {
    speed_change = 3;  // Medium acceleration
} else {
    speed_change = 1;  // Smooth acceleration at high speeds
}
```

### Change Charging Timer Duration
```c
// Modify Timer0 prescaler and initial value
TMR0 = 0;              // Starting value
OPTION_REG = 0x07;     // Prescaler setting
```

---

## 🐛 Troubleshooting

### Web Demo Issues

**Problem:** 3D model doesn't load
- **Solution:** Use a local HTTP server (not `file://` protocol)
- **Command:** `python -m http.server 8000`

### Firmware Compilation Issues

**Problem:** LCD/PWM functions not found
- **Solution:** Enable required libraries in mikroC:
  - Project → Edit Project → Check "ADC", "LCD", "PWM"

**Problem:** PWM not working
- **Solution:** Verify PWM pin configuration and frequency settings

### Hardware Issues

**Problem:** Buttons not responding
- **Solution:** Check RB port pull-up resistors
- **Code check:** Ensure `NOT_RBPU_bit = 0;` (enables pull-ups)

**Problem:** LCD shows garbage characters
- **Solution:** Verify LCD connections (RC0-RC7) and contrast adjustment

**Problem:** Motor doesn't respond
- **Solution:** Check PWM output pin, motor driver connections, and power supply

---

## 🎓 Technical Details

### Interrupt System
- **RBIF:** Port B change interrupt for button detection
- **TMR0IF:** Timer0 interrupt for charging timer
- **Debouncing:** Implemented in interrupt handler to prevent false triggers

### ADC Configuration
- **Resolution:** 10-bit (0-1023)
- **Channels used:** AN0 (throttle), AN1 (distance)
- **Conversion time:** ~20 μs

### PWM Configuration
- **Frequency:** 5 kHz (configurable)
- **Resolution:** 8-bit (0-255)
- **Duty cycle:** Calculated as: `(current_speed * 255) / 100`

---
## 📄 License

This project is currently unlicensed. Please contact the repository owner for usage permissions.

To make this project open-source, consider adding a license file (MIT, Apache 2.0, GPL, etc.).

---

## 👨‍💻 Author

**Rayen Ouerghui**

- GitHub: [@rayenouerghui](https://github.com/rayenouerghui)
- Email: [medrayenouerghui@gmail.com]

---

## 🙏 Acknowledgments
- Microchip Technology for PIC16F877A documentation
- MikroElektronika for mikroC PRO compiler
- Labcenter Electronics for Proteus ISIS
---
## 📞 Support
If you have questions or need help:
- Open an issue on GitHub
- Check the troubleshooting section above
- Review the Proteus simulation file for circuit reference

---

<div align="center">

### ⭐ If you find this project useful, please give it a star!

**Made with ❤️ for safer e-mobility**

</div>
