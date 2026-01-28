<img src="https://res.cloudinary.com/db0qkzn6a/image/upload/v1769618329/gitdocs/user_38kuGUTnJD7r23NEbRjPSnVelVV/images/ybkvsdr52xqcbfaqy9b8.png" alt="Système de Sécurité Trottinette — demo" width="100%">

# Système de Sécurité — Trottinette (PIC16F877A)

Compact embedded security system and demo for an electric scooter (trottinette).  
This repository contains:
- Firmware for a PIC16F877A microcontroller (trottinette.c).
- A browser-based 3D preview / demo (index.html + scooter.glb).
- Photos and assets (systeme photo.png, screenshots).
- Proteus/design files (trottinette.DSN, trottinette.mcppi).

> The firmware is written in a mikroC / CCS-like style (uses Lcd_Init, PWM1_Init, PWM1_Set_Duty, sbit declarations). Use the appropriate compiler (mikroC Pro for PIC is recommended) and an ICSP programmer (PICkit, etc.) to flash the PIC.

---

## Features
- Emergency braking on obstacle detection (distance sensor + ADC).
- Motor control with PWM and direction outputs.
- Throttle reading from a potentiometer (ADC).
- Security toggle button (enable/disable safety features).
- Charging mode with default delay or timed charge (button-driven).
- LCD status messages (16x2).
- LEDs for status and charging feedback.
- Interrupt-driven button handling and timer-based charging.

---

## Quick Demo — Run the 3D preview (index.html)

The HTML demo uses the `model-viewer` web component and the included `scooter.glb` model.

1. Clone or download the repository.
2. It's best to run a local HTTP server (some browsers block local `file://` requests for model assets):
   - Python 3:
     - From the repo folder run:
       - `python -m http.server 8000`
     - Open: `http://localhost:8000/index.html`
   - Or use any static file server (live-server, http-server, etc.).
3. The page will load the 3D scooter model and show the HUD overlay.

---

## Firmware — Build & Flash

Recommended: mikroC Pro for PIC (functions in the code match mikroC conventions). If you prefer another toolchain (MPLAB X + XC8), you will need to adapt helper calls and pin declarations.

Basic steps with mikroC:
1. Open a new Project in mikroC for PIC.
2. Add `trottinette.c` as the main source.
3. Configure the device: PIC16F877A.
4. Set oscillator and configuration bits as required by your hardware.
5. Build the project.
6. Use your programmer (PICkit, ICD, etc.) to flash the generated hex file.

Notes:
- The code uses library calls like `Lcd_Init()`, `ADC_Read()`, `PWM1_Set_Duty()`. Make sure those libraries are enabled or available in your toolchain.
- If you use MPLAB/XC8, replace or implement the LCD and PWM helper functions accordingly.

---

## Hardware / Pin Mapping (from trottinette.c)

Below is a concise mapping derived directly from the source file. Confirm with your schematic before wiring.

- ADC:
  - AN0 (ADC channel 0) — Throttle / accelerator (accel_value) -> RA0 (ADC_Read(0))
  - AN1 (ADC channel 1) — Distance / obstacle sensor (distance_value) -> RA1 (ADC_Read(1))

- Buttons (PORTB change interrupt):
  - RB0 — Security toggle button (press toggles `security`).
  - RB4 — Charge button (press sequences: enter charge, press 4 times to skip default delay and start timer).
  - RB5 — History button (show history on LCD)

- LEDs:
  - RB3 — Green/status LED (turned on when charged)
  - RB7 — Red LED (charging / alert)
  - RB1 — Used during braking as an indicator

- LCD (16x2) connected to PORTC:
  - RC0 — LCD_RS
  - RC1 — LCD_EN
  - RC4 — LCD_D4
  - RC5 — LCD_D5
  - RC6 — LCD_D6
  - RC7 — LCD_D7

- Motor / Direction / PWM:
  - RD0, RD1 — motor direction control (RD0/RD1 set together to indicate forward/back/stop)
  - PWM1 — motor speed control (PWM1_Init and PWM1_Set_Duty used). The PWM duty is set as percent -> 255 scale in code.

- Timer / Interrupts:
  - Timer0 used as a charge timer (OPTION_REG and TMR0 configured). RB change interrupt (RBIF) used for button detection.

System photo (hardware / wiring reference):
<img src="systeme photo.png" alt="Photo du système - trottinette" width="600">

---

## How the Firmware Works (summary)

Main modes and logic (see `trottinette.c` for the complete implementation):

- Startup:
  - ADC configured, TRIS registers set, PWM started, LCD initialized, Timer0 interrupt prepared.
  - RB change interrupts enabled (RBIE), with debouncing logic in the interrupt handler.

- Security toggle:
  - RB0 toggles `security` (on/off). When security is enabled, a close obstacle (distance ADC < 300) triggers an emergency stop.

- Normal operation:
  - Throttle read from ADC channel 0 -> `speed_percent`.
  - Deadzone around 50% is used: if throttle <48% it's treated as backward (target 100% reverse), >52% forward, otherwise stop (target 0).
  - A multi-step algorithm adjusts `current_speed` smoothly using variable speed_change values depending on the current speed band.
  - PWM duty set via `PWM1_Set_Duty(current_speed * 255 / 100)`.

- Emergency Stop:
  - If an obstacle is detected (ADC distance < 300) and `security` is enabled, `Emergency_Stop()` is called.
  - This function ramps down speed rapidly, sets red LED and RB1, shows messages on LCD and waits until the obstacle is cleared.

- Charging mode:
  - RB4 enters charging mode. A default short flashing delay occurs; pressing RB4 four times during the default delay skips the delay and starts a timer mode (using Timer0 interrupts).
  - When charging completes, LEDs and LCD indicate "CHARGED".

- Interrupts:
  - RBIF detects button changes (RB4 and RB5 handled).
  - Timer0IF used to detect elapsed charge time.

---

## Typical LCD messages
- "SYSTEME PRET" — boot ready
- "SEC ON" / "SEC OFF" — security status
- "CHARGING" — charging mode entered
- "TIMER" — timer mode started
- "CHARGED" — charge complete
- "OBSTACLE !" / "FREINAGE !" — emergency braking

---

## Files in this repository
- index.html — Web demo using <model-viewer> and the 3D model.
- scooter.glb — 3D model used by the demo.
- trottinette.c — Main firmware source (PIC16F877A).
- trottinette.DSN — Proteus design file (schematic/simulation).
- trottinette.mcppi — Proteus project or related file.
- systeme photo.png — Photo of the physical system (used above).
- Screenshot 2025-12-16 100008.png — UI screenshot / additional preview.

---

## Troubleshooting
- Model doesn’t load in browser:
  - Serve via HTTP (python -m http.server) instead of opening the file directly.
  - Confirm scooter.glb is in the same folder as index.html.
- PWM or LCD functions missing at compile time:
  - Ensure you enable/include the hardware libraries in mikroC or implement equivalent drivers if using another toolchain.
- Buttons not detected / debouncing issues:
  - Check pull-up/pull-down resistor configuration for PORTB and NOT_RBPU_bit. The code sets NOT_RBPU_bit = 0 (enable RB pull-ups).
- Proteus simulation:
  - Use the .DSN file to simulate the circuit in Proteus (if available) and verify pin mapping before flashing hardware.

---

## Extending / Customizing
- Adjust ADC thresholds (distance_value < 300) to suit your sensor.
- Tune speed_change bands for a different acceleration curve.
- Replace Lcd functions and PWM helper calls if using a different compiler.
- Add telemetry (UART or BLE) for remote monitoring.

---

## Contributing
- Suggestions, bug reports, and pull requests are welcome.
- If you modify the firmware for another MCU or toolchain, please add a short HOWTO and updated wiring.

---

## License
No license file included. If you want this project to be open-source, add a LICENSE (MIT, Apache-2.0, etc.) to the repository. Otherwise contact the repository owner for license details.

---

If you'd like, I can:
- Add a short Proteus wiring diagram explanation taken from trottinette.DSN,
- Convert the firmware to an MPLAB XC8-compatible example,
- Produce a simple schematic PDF based on the pin mapping above.