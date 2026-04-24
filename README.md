# Automatic Medicine Dispenser
> An Arduino-based smart medicine box that automatically dispenses medication at scheduled times, monitors storage conditions, and reminds users until medicine is taken.

---

## Project Overview

The Automatic Medicine Dispenser is a 3-compartment smart device designed to help patients take their medication on time. Each compartment opens automatically at a scheduled time (8AM, 2PM, 8PM), detects whether the medicine has been taken using IR sensors, and reminds the user with buzzer alerts every 5 seconds until the medicine is removed. The system also monitors temperature and humidity to ensure safe medicine storage, and uses an ultrasonic sensor to detect user proximity and activate the LCD display only when needed.

---

## Components Used

| Component | Quantity | Description |
|---|---|---|
| Arduino Uno | 1 | Microcontroller |
| Servo Motor (SG90) | 3 | Opens/closes each compartment |
| IR Sensor (FC-51) | 3 | Detects medicine presence |
| HC-SR04 Ultrasonic Sensor | 1 | Detects user proximity |
| DHT11 Temp & Humidity Sensor | 1 | Monitors storage conditions |
| DS1302 RTC Module | 1 | Real-time clock |
| 16x2 I2C LCD Display | 1 | Displays time and alerts |
| Buzzer | 1 | Audible reminders and alerts |
| Power Bank (5V 2A) | 1 | Powers the entire system via USB |
| Jumper Wires | — | Connections |
| Breadboard | 1 | Prototyping |

---

## Pin Configuration

| Component | Pin |
|---|---|
| Servo Morning | 7 |
| Servo Afternoon | 9 |
| Servo Night | 10 |
| IR Sensor Morning | 2 |
| IR Sensor Afternoon | 3 |
| IR Sensor Night | 8 |
| Buzzer | 11 |
| DHT11 Data | A1 |
| LCD SDA | A4 |
| LCD SCL | A5 |
| RTC DAT | 4 |
| RTC CLK | 5 |
| RTC RST | 6 |
| Ultrasonic TRIG | 13 |
| Ultrasonic ECHO | A0 |

---

## Circuit Diagram

<img width="867" height="673" alt="image" src="https://github.com/user-attachments/assets/0394f065-8e87-4f89-9ed6-ea48549773ab" />

---

## Working

### Normal Operation
1. System starts up, LCD displays "Medicine Box Ready!" and turns off
2. RTC module tracks real time continuously
3. Ultrasonic sensor detects if user is within 30cm — LCD turns on showing current time, turns off when user walks away
4. DHT11 continuously monitors temperature and humidity — triggers long beep + LCD warning if temp exceeds 34°C or humidity exceeds 60%, regardless of user proximity

### Medicine Dispensing
1. At **8:00 AM** → Morning compartment servo opens, single beep, LCD shows "Morning Time!"
2. IR sensor detects medicine presence — if medicine not taken, buzzer beeps 3 times every 5 seconds and LCD reminds user
3. Once medicine is removed (IR detects absence) → single beep, servo closes, LCD shows "Medicine Taken!"
4. Same process repeats at **2:00 PM** (Afternoon) and **8:00 PM** (Night)
5. All flags reset at midnight for the next day

### LCD States
| Situation | LCD Shows |
|---|---|
| No one nearby | LCD off (blank) |
| Person nearby | `Time: HH:MM:SS` / `Waiting...` |
| Medicine time | `Morning Time!` / `Box Opening...` |
| Medicine reminder | `Take your` / `Morning medicine!` |
| Medicine taken | `Medicine Taken!` / `Box Closed.` |
| Temp too high | `TEMP TOO HIGH!` / `xx.x C` + long beep |
| Humidity too high | `HUMIDITY HIGH!` / `xx.x %` + long beep |

---

## How to Run

### 1. Install Required Libraries
Open Arduino IDE → Sketch → Include Library → Manage Libraries and install:
- `LiquidCrystal I2C` by Frank de Brabander
- `Rtc by Makuna` (provides ThreeWire and RtcDS1302)
- `DHT sensor library` by Adafruit
- `Adafruit Unified Sensor` by Adafruit

### 2. Connect Components
Wire all components according to the pin configuration table above. Power the system using a 5V 2A power bank via the Arduino USB port.

### 3. Upload the Code
- Open `medicine_dispenser.ino` in Arduino IDE
- Select **Tools → Board → Arduino Uno**
- Select the correct COM port under **Tools → Port**
- Click **Upload**

### 4. Testing Mode
The code includes a software test clock for testing without waiting for actual scheduled times. Change these values in the code to trigger each compartment:

```cpp
int testHour   = 7;   // change to 7, 13, or 19
int testMinute = 59;
int testSecond = 55;  // triggers 5 seconds after upload
```

| Test | Set testHour to |
|---|---|
| Morning (8AM) | 7 |
| Afternoon (2PM) | 13 |
| Night (8PM) | 19 |

### 5. Switch to Real Time
After testing, replace in `loop()`:
```cpp
// remove test clock calculation and replace with:
RtcDateTime now = Rtc.GetDateTime();
int h = now.Hour();
int m = now.Minute();
int s = now.Second();
```

### 6. Power It
Connect a 5V 2A power bank to the Arduino USB port. The system will start automatically.

---

## Future Improvements

- **Emergency Override** — Physical buttons per compartment to manually open any box outside scheduled times
- **Missed Dose Tracking** — Log and display which doses were missed during the day
- **Medicine Level Detection** — Ultrasonic sensor inside each compartment to alert when medicine is running low and needs refilling
- **Mobile App Integration** — Bluetooth or WiFi module (ESP8266) to send alerts to caregiver's phone when medicine is not taken
- **Multiple Alarm Times** — Allow user to configure custom medicine schedules instead of fixed 8AM/2PM/8PM

---

## Libraries Used

| Library | Author |
|---|---|
| Servo.h | Arduino built-in |
| Wire.h | Arduino built-in |
| LiquidCrystal_I2C.h | Frank de Brabander |
| ThreeWire.h / RtcDS1302.h | Makuna |
| DHT.h | Adafruit |

---
## License

MIT License - see LICENSE file for details.
