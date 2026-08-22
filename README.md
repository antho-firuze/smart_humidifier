# Smart Humidifier

An ESP/Arduino-based smart humidifier controller featuring a built-in web server for real-time monitoring and control.

---

## Features

* **Web Interface**: Integrated HTTP server serving an interactive dashboard (`index_page.h`).
* **Automated Control**: Smart humidity threshold regulation and scheduling.
* **Wi-Fi Connectivity**: Easy setup and remote control across your local network.
* **CLI/Arduino CLI Ready**: Configured with `sketch.yaml` for straightforward compilation and deployment via Arduino CLI.

---

## File Overview

* `smart_humidifier.ino` – Main Arduino application containing sensor read loops, Wi-Fi connectivity, and state control.
* `index_page.h` – HTML/CSS/JS web dashboard embedded directly into microcontroller flash memory.
* `sketch.yaml` – Configuration profile for compilation using Arduino CLI.
* `.gitignore` – Git exclusion settings for temporary build files and hardware platform dependencies.

---

## Getting Started

### Prerequisites

* [Arduino IDE](https://www.arduino.cc/en/software) or [Arduino CLI](https://arduino.github.io/arduino-cli/)
* ESP8266 or ESP32 Board Support Package (BSP) installed in the IDE

### Installation & Flashing

1. Clone this repository:
   ```bash
   git clone [https://github.com/antho-firuze/smart_humidifier.git](https://github.com/antho-firuze/smart_humidifier.git)
   cd smart_humidifier


2. Open `smart_humidifier.ino` in the Arduino IDE or compile via Arduino CLI:
   ```bash
   arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 .

   ```


3. Update Wi-Fi credentials (SSID and Password) in `smart_humidifier.ino`.
4. Upload the sketch to your board and open the Serial Monitor (115200 baud) to view the assigned IP address.

---

## Usage

1. Connect your device (mobile or PC) to the same local network as the board.
2. Open a web browser and navigate to the IP address printed on the Serial Monitor.
3. Control power, view current relative humidity/temperature levels, and adjust target setpoints directly from the web interface.

---

## License

This project is open-source. See the repository options for licensing details.
