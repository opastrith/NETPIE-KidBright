
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define BUZZER_PIN 13
#define WIFI_LED 2
#define IOT_LED 12
#define USB_IO 25
#define BT_LED 23





Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();

void setup() {
  //while (!Serial);
  Serial.begin(9600);
  Serial.println("16x8 LED Mini Matrix Test");

   pinMode(WIFI_LED, OUTPUT); 
  pinMode(IOT_LED, OUTPUT); 
  
  digitalWrite(WIFI_LED, HIGH);
  digitalWrite(IOT_LED, HIGH);

  
  matrix.begin(0x70);  // pass in the address
}


void loop() {
  
 
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  matrix.setRotation(1);
  Serial.println("IoT");

  
  digitalWrite(WIFI_LED, LOW);
  digitalWrite(IOT_LED, LOW);
  
  for (int8_t x=7; x>=-100; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("NETPIE Training");
    matrix.writeDisplay();
    delay(100);
  }
   
  digitalWrite(WIFI_LED, HIGH);
  digitalWrite(IOT_LED,  HIGH);
 delay(500);
  
}
