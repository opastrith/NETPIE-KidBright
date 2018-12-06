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




//const char* ssid     = "DSB";
//const char* password = "I0t@NT$B0x";

///////////////WIFI setting/////////////////////


const char* ssid     = "Goyangle";
const char* password = "pi3thnop";
WiFiClient client;

int last_chat_time = 0;

////////////NETPIE setting ////////////////////////


#define APPID   "TEINproject"
#define KEY     "8UR29KW3wDY00Pt"
#define SECRET  "XhUPo6SEzH4oc5RJ3cISr1cOB"
#define ALIAS   "Basic"
   
////////////////////////////////////////////////////////////////////


MicroGear microgear(client);

// เมื่อมี message ใหม่เข้ามา จะเรียก function นี้
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

// เมื่อ connect netpie สำเร็จ จะเรียก function นี้
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    // ตั้ง alias ของ microgear นี้เป็น ALIAS
    microgear.setAlias(ALIAS);
}


void setup() {
      Wire1.begin(4, 5);
 
 pinMode(WIFI_LED, OUTPUT); 
  pinMode(IOT_LED, OUTPUT); 

  pinMode(BT_LED, OUTPUT); 
  digitalWrite(WIFI_LED, HIGH);
  digitalWrite(IOT_LED, HIGH);
 
  digitalWrite(BT_LED, HIGH);
 
  matrix.begin(0x70);  // pass in the address

   netpieshow("Starting..Lab01");

 
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(9600);
    Serial.println("Starting...");
    
  

 
     int count=0;
    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
            count++;
             matrix.clear();
             matrix.setCursor(0,0);
             count = count+1;
             matrix.print(String(count)+"..");
             matrix.writeDisplay();
        }
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(WIFI_LED, LOW);
    
    
    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
    netpieshow("NETPIE connected..");
   digitalWrite(IOT_LED, LOW);
}

void loop() {
    // check NETPIE connecting 
    if (microgear.connected()) {
        digitalWrite(IOT_LED, LOW);
        // เรียก function นี้เป็นระยะๆ ไม่เช่นนั้น connection จะหลุด
        microgear.loop();

        if (millis() - last_chat_time >= 1000) {
            Serial.println("Send chat message >>>");

            // chat หา device ชื่อ ALIAS ซึ่งก็คือชื่อของตัวเอง
            microgear.chat(ALIAS,"Hello..");
            last_chat_time = millis();
            netpieshow("Hello from NETPIE");
            
        } 
    }
    else {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID);
        netpieshow("reconnect");
        digitalWrite(IOT_LED, HIGH);
    }
}
