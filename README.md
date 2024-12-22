# WiFiPowerSocketControl

[![CI](https://github.com/matthias-bs/WiFiPowerSocketControl/actions/workflows/CI.yml/badge.svg)](https://github.com/matthias-bs/BresserWeatherSensorReceiver/actions/workflows/CI.yml)[![License: MIT](https://img.shields.io/badge/license-MIT-green)](https://github.com/matthias-bs/WiFiPowerSocketControl/blob/main/LICENSE)
<!-- [![GitHub release](https://img.shields.io/github/release/matthias-bs/WiFiPowerSocketControl?maxAge=3600)](https://github.com/matthias-bs/WiFiPowerSocketControl/releases) -->


Control a WiFi power socket switch from an ESP32 used as access point

This example demonstrates how to connect a WiFi power socket switch to an ESP32 working
as a WiFi access point and as HTTP client. A button connected to the ESP32 toggles the
switch state variable, which is indicated by a LED connected to the ESP32. When the button is
pressed, the ESP32 sends an HTTP request to the power socket switch to change its state
accordingly.

Tested with [Delock 11826](https://www.delock.com/produkt/11826/merkmale.html)

You can implement your own logic to control the switch, e.g. based on a sensor value.

Multiple WiFi devices can connect to the ESP32's access point - this allows you to extend
this sketch to control multiple power socket switches.

> [!WARNING]  
> Assess the risks of controlling a power switch via a potentially unreliable and insecure WiFi connection.
> Read the [MIT License](LICENSE).

## Usage

1. The ESP32 provides a WiFi access point (SoftAP)
2. Configure your WiFi power socket switch to connect to the ESP32's access point
   (see SSID and password in the sketch)
3. Configure the URLs for the power socket switch in the sketch
4. If available, use the ESP32 dev board's user button, otherwise connect a button to a GPIO pin
5. If available, use the ESP32 dev board's built-in LED, otherwise connect a LED to a GPIO pin
6. Wait until the power socket switch has connected to the access point
7. When the button is pressed, the switch state variable and the LED state are toggled
   and an HTTP request is sent to the power socket switch to turn it on or off
