#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Custom_AHT21.h"
#include "Custom_ENS160.h"

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// pin and I2C configurations
#define SDA_PIN 8
#define SCL_PIN 9
#define SCREEN_WIDTH 128     // OLED display width,  in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define I2C_FREQUENCY 100000 // I2C frequency in Hz (100 kHz)

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

long lastMsg = 0;

I2cInterface i2c;

CustomAHT21 aht21;
CustomENS160 ens160;
void initializeDebugging()
{
  Serial.begin(9600);
  if (DEBUG == 1)
  {
    ens160.ens.enableDebugging(Serial);
  }
}

void initializeI2C()
{
  Wire.begin(SDA_PIN, SCL_PIN, I2C_FREQUENCY);
  i2c.begin(Wire, ENS160_I2C_ADDRESS);
}

void initializeDisplay()
{
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  {
    Serial.println(F("failed to start SSD1306 OLED"));
  }

  // delay(2000);         // wait two seconds for initializing
  oled.clearDisplay(); // clear display

  // oled.setTextSize(1);      // set text size
  oled.setTextColor(WHITE); // set text color
}
void initializeSensors()
{
  // initialize ENS160 sensor
  debugln("[initializeSensors] Initializing ENS160.");
  ens160.begin(&i2c);
  debugln("[initializeSensors] ENS160 initialized successfully.");

  // initialize AHT21 sensor
  debug("[initializeSensors] Initializing AHT21.");
  aht21.begin();
  debugln("[initializeSensors] AHT21 initialized successfully.");
}

void setup()
{
  debugln("Starting setup...");

  initializeDebugging();
  initializeI2C();
  initializeSensors();
  initializeDisplay();

  debugln("Setup complete.");
}

void loop()
{

  AHT21Data aht21d = aht21.read();
  String aht21log = "TEMP:" + String(aht21d.temp) + " HUM:" + String((int)aht21d.humidity) + "%";

  ENS160Data ens160d = ens160.read();
  String ens160log_tvoc = "TVOC:" + String((int)ens160d.tvoc);
  String ens160log_eco2 = "ECO2:" + String((int)ens160d.eco2);

  debugln(aht21log.c_str());
  debugln(ens160log_tvoc.c_str());
  debugln(ens160log_eco2.c_str());
  oled.clearDisplay();
  oled.setTextSize(1); // set text size
  oled.setCursor(0, 8);
  oled.println(aht21log.c_str());
  oled.setTextSize(2); // set text size
  oled.setCursor(0, 20);
  oled.println(ens160log_tvoc.c_str());
  oled.setCursor(0, 40);
  oled.println(ens160log_eco2.c_str());
  oled.display();
  delay(1000);
}