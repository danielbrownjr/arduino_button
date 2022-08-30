#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_SHT4x.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
char buffer[16];             // buffer for LCD display
int first, second;             // front and back of the decimal

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 0;    // the debounce time; increase if the output flickers

LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void sensor_init(){
  Serial.println("Adafruit SHT40 test");
  lcd.setCursor(0,1);
  lcd.print("SHT40 test");
  delay(500);
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT40");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  lcd.setCursor(0,1);
  lcd.print("SHT40 Found");
  Serial.println(sht4.readSerial(), HEX);
  delay(500);
  lcd.clear();
}

int floatToInt(float f){
  int i = (int)f;
  return i;
}

int floatToIntDecimal(float f){
  int i = (int)f;
  float trunc_f = f - i;
  return (int)(trunc_f * 100);
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  sensor_init();
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater()) {
     case SHT4X_NO_HEATER: 
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
       }
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);
}

void loop() {
  // read the state of the switch into a local variable:
  sensors_event_t humidity, temp;
  uint32_t timestamp = millis();

  int reading = digitalRead(buttonPin);
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  timestamp = millis() - timestamp; // calculate how long it took to get the data

  int temp_int = floatToInt(temp.temperature);
  int temp_dec = floatToIntDecimal(temp.temperature);
  int hum_int = floatToInt(humidity.relative_humidity);
  int hum_dec = floatToIntDecimal(humidity.relative_humidity);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);
  if (ledState == HIGH) {
    sprintf(buffer, "Humi: %d.%d %%", hum_int, hum_dec);
    delay(100);
  } else {
    sprintf(buffer, "Temp: %d.%d C", temp_int, temp_dec);
    delay(100);
  }
  
  lcd.setCursor(0,0);
  lcd.print(buffer);


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  //Serial.print(reading);
}