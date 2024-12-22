///////////////////////////////////////////////////////////////////////////////////////////////////
// WiFiPowerSocketControl.ino
//
// Control a WiFi power socket switch from an ESP32
//
// This example demonstrated how to connect a WiFi power socket switch to an ESP32 working
// as a WiFi access point and as HTTP client. A button connected to the ESP32 toggles the
// switch state variable, which is indicated by a LED connected to the ESP32. When the button is
// pressed, the ESP32 sends an HTTP request to the power socket switch to change its state
// (on or off).
//
// Tested with Delock 11826 (https://www.delock.com/produkt/11826/merkmale.html)
//
// You can implement your own logic to control the switch, e.g. based on a sensor value.
// Multiple WiFi devices can connect to the ESP32's access point - this allows you to extend
// this sketch to control multiple power socket switches.
//
// 1. The ESP32 provides a WiFi access point (SoftAP)
// 2. Configure your WiFi power socket switch to connect to the ESP32's access point
//    (see SSID and password below)
// 3. Configure the URLs for the power socket switch below
// 4. If available, use the ESP32 dev board's user button, otherwise connect a button to a GPIO pin
// 5. If available, use the ESP32 dev board's variable, otherwise connect a LED to a GPIO pin
// 6. When the button is pressed, the switch state variable and the LED state are toggled
//    and an HTTP request is sent to the power socket switch to turn it on or off
//
// https://github.com/matthias-bs/WiFiPowerSocketControl
//
// created: 12/2024
//
//
// MIT License
//
// Copyright (c) 2024 Matthias Prinke
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// History:
// 20241221 Created
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include <HTTPClient.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#endif

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

// Set these to the URLs for the WiFi power socket switch
// For Delock 11826, replace delock-3844 with the hostname of your device
#define URL_ON "http://delock-3844.local/cm?cmnd=Power%20on"
#define URL_OFF "http://delock-3844.local/cm?cmnd=Power%20off"
// With Username / Password
// #define "http://delock-XXXX.local/cm?&user=admin&password=put_password_here&cmnd=Power%20on"
// #define "http://delock-XXXX.local/cm?&user=admin&password=put_password_here&cmnd=Power%20off"

const int buttonPin = 0;             /// GPIO pin connected to the button
volatile bool buttonPressed = false; /// Flag to indicate that the button was pressed
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; /// Debounce time in milliseconds
bool switchState = false;               /// Current state of the switch

/**
 * @brief Handle button press interrupt
 *
 */
void IRAM_ATTR handleButtonPress()
{
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) > debounceDelay)
  {
    buttonPressed = true;
    lastDebounceTime = currentTime;
  }
}

/**
 * @brief Send an HTTP request to the WiFi power socket switch
 *
 * @param url
 * @return int 0: switch is OFF, 1: switch is ON, -1: unknown state
 */
int sendRequest(const char *url)
{
  HTTPClient http;
  int res = -1;

  Serial.print("[HTTP] begin...\n");
  // configure target server and url
  // http.begin("url", ca); //HTTPS
  http.begin(url); // HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);
      if (payload.indexOf("\"POWER\":\"ON\"") != -1)
      {
        res = 1;
      }
      else if (payload.indexOf("\"POWER\":\"OFF\"") != -1)
      {
        res = 0;
      }
      else
      {
        res = -1;
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  return res;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password))
  {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  pinMode(buttonPin, INPUT_PULLUP);                                              // Set button pin as input with internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING); // Attach interrupt to button pin
}

void loop()
{
  if (buttonPressed)
  {
    int res = -1;
    buttonPressed = false;
    switchState = !switchState;
    digitalWrite(LED_BUILTIN, switchState);
    if (switchState)
    {
      Serial.println("Turning the switch ON");
      res = sendRequest(URL_ON);
    }
    else
    {
      Serial.println("Turning the switch OFF");
      res = sendRequest(URL_OFF);
    }
    if (res == 1)
    {
      Serial.println("Switch is ON");
    }
    else if (res == 0)
    {
      Serial.println("Switch is OFF");
    }
    else
    {
      Serial.println("Switch state unknown");
    }
  }
}
