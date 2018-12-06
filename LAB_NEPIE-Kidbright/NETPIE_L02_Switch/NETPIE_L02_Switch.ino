/*  NETPIE ESP32 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <WiFi.h>
#include <MicroGear.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();


#define BUZZER_PIN 13
#define WIFI_LED 2
#define IOT_LED 12
#define USB_IO 25
#define BT_LED 23

void netpieshow(String text){
 
 //   while(1){
      matrix.setTextSize(1);
      matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
      matrix.setTextColor(LED_ON);
      matrix.setRotation(1);
     
      for (int8_t x=7; x>=-126; x--) {
        matrix.clear();
        matrix.setCursor(x,0);
        matrix.print(text);
        matrix.writeDisplay();
        delay(50);
      }
     
  //  }
}

void textshow(String text){
 
 //   while(1){
      matrix.setTextSize(1);
      matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
      matrix.setTextColor(LED_ON);
      matrix.setRotation(1);
     matrix.clear();
        matrix.setCursor(0,0);
        matrix.print(text);
        matrix.writeDisplay();
        
     
     
  //  }
}




const char* ssid     = "DSB";
const char* password = "I0t@NT$B0x";

///////////////WIFI setting/////////////////////


//const char* ssid     = "Goyangle";
//const char* password = "pi3thnop";
WiFiClient client;

int last_chat_time = 0;

////////////NETPIE setting ////////////////////////
MicroGear microgear(client);


#define APPID   "TEINproject"
#define KEY     "8UR29KW3wDY00Pt"
#define SECRET  "XhUPo6SEzH4oc5RJ3cISr1cOB"
#define ALIAS   "A02"           // แทนที่ด้วยหมายเลขของท่าน เช่น "A01"
#define NEIGHBOR "A01"             // ชื่ออุปกรณ์ของเพื่อน เช่น "A02"


#define BUTTONPIN1  16                         // pin ที่ต่อกับปุ่ม Flash บนบอร์ด NodeMCU
#define LEDPIN     12                // pin ที่ต่อกับไฟ LED บนบอร์ด NodeMCU

int currentLEDState = 1;      // ให้เริ่มต้นเป็น OFF หมายเหตุ ไฟ LED บนบอร์ดต่อแบบ active-low
int lastLEDState = 0;
int currentButtonState = 1;   // หมายเหตุ ปุ่ม flash ต่อเข้ากับ GPIO0 แบบ pull-up
int lastButtonState = 0;


void updateLED(int state) {
    
    if(state==1 && currentLEDState == 0){
        currentLEDState = 1;
        digitalWrite(LEDPIN, LOW); // LED ON
        
    }
    else if (state==0 && currentLEDState == 1) {
        currentLEDState = 0;
        digitalWrite(LEDPIN, HIGH); // LED OFF
    }
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);

    if (*(char *)msg == '0') updateLED(0);
    else if (*(char *)msg == '1') updateLED(1);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

   
////////////////////////////////////////////////////////////////////



void setup() {
     Wire1.begin(4, 5);
 
 pinMode(WIFI_LED, OUTPUT); 
  pinMode(IOT_LED, OUTPUT); 
  pinMode(USB_IO, OUTPUT); 
  pinMode(BT_LED, OUTPUT); 
  digitalWrite(WIFI_LED, HIGH);
  digitalWrite(IOT_LED, HIGH);
  digitalWrite(USB_IO, HIGH);
  digitalWrite(BT_LED, HIGH);
  
  matrix.begin(0x70);  // pass in the address

    netpieshow("Starting..Lab02");



  

    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(9600);
    Serial.println("Starting...");

    // กำหนดชนิดของ PIN (ขาI/O) เช่น INPUT, OUTPUT เป็นต้น
    pinMode(LEDPIN, OUTPUT); // LED pin mode กำหนดค่า

    pinMode (BUTTONPIN1, INPUT);
    pinMode(BUTTONPIN1, INPUT_PULLUP);  // Pull up to 3.3V on input - some buttons already have this done
    updateLED(currentLEDState);

     int i=0;
    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
            i++;
            textshow(String(i)+"..");
        }
        
    }
    
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
     digitalWrite(WIFI_LED, LOW);
  digitalWrite(IOT_LED, HIGH);
    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
   netpieshow("NETPIE connected"); 
    digitalWrite(WIFI_LED, LOW);
    digitalWrite(IOT_LED, LOW);
}

void loop() {
    if (microgear.connected()) {
        microgear.loop();
         textshow(String(ALIAS));
        if(currentLEDState != lastLEDState){
          microgear.publish("/LEDstate", currentLEDState);
          lastLEDState = currentLEDState;
          netpieshow("LEDState:"+String(currentLEDState));
        }

        if (digitalRead(BUTTONPIN1)==HIGH)
        {currentButtonState = 0;
        
        // Serial.println("BUTTON pressed");
        }
        else 
        {currentButtonState = 1;
        Serial.println("BUTTON pressed");
        } 
        
        if(currentButtonState != lastButtonState){
          ///SEND currentButtonState to NEIGHBOR 
          microgear.chat(NEIGHBOR, currentButtonState);
          
          lastButtonState = currentButtonState;
          netpieshow("ButtonState:"+String(currentButtonState));
        }
        
    }
    else {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID); 
    }
}
