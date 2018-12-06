/*  NETPIE ESP32 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <WiFi.h>
#include <MicroGear.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#include <DHT.h>

#define BUZZER_PIN 13
#define WIFI_LED 2
#define IOT_LED 12
#define USB_IO 25
#define BT_LED 23
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();

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





//const char* ssid     = "DSB";
//const char* password = "I0t@NT$B0x";

///////////////WIFI setting/////////////////////


const char* ssid     = "Goyangle";
const char* password = "pi3thnop";
WiFiClient client;

int last_chat_time = 0;

////////////NETPIE setting ////////////////////////
MicroGear microgear(client);


#define APPID   "TEINproject"
#define KEY     "8UR29KW3wDY00Pt"
#define SECRET  "XhUPo6SEzH4oc5RJ3cISr1cOB"
#define ALIAS   "A02"           // แทนที่ด้วยหมายเลขของท่าน เช่น "A01"
#define NEIGHBOR "A01"             // ชื่ออุปกรณ์ของเพื่อน เช่น "A02"

//-------------------------------------------------------------

#define LEDSTATETOPIC "/ledstate/" ALIAS      // topic ที่ต้องการ publish ส่งสถานะ led ในที่นี้จะเป็น /ledstate/{ชื่อ alias ตัวเอง}
#define DHTDATATOPIC "/dht/" ALIAS            // topic ที่ต้องการ publish ส่งข้อมูล dht ในที่นี่จะเป็น /dht/{ชื่อ alias ตัวเอง}

#define BUTTONPIN1  16                         // pin ที่ต่อกับปุ่ม Flash บนบอร์ด NodeMCU
#define LEDPIN     12                // pin ที่ต่อกับไฟ LED บนบอร์ด NodeMCU

int currentLEDState = 1;      // ให้เริ่มต้นเป็น OFF หมายเหตุ ไฟ LED บนบอร์ดต่อแบบ active-low
int lastLEDState = 0;
int currentButtonState = 1;   // หมายเหตุ ปุ่ม flash ต่อเข้ากับ GPIO0 แบบ pull-up
int lastButtonState = 0;


#define DHTTYPE DHT22
#define DHTPIN 19
DHT dht(DHTPIN, DHTTYPE);


float humid = 0;     // ค่าความชื้น
float temp  = 0;     // ค่าอุณหภูมิ

long lastDHTRead = 0;
long lastDHTPublish = 0;

long lastTimeFeed = 0;




void updateLED(int state) {
    currentLEDState = state;

    // ไฟ LED บน NodeMCU เป็น active-low จะติดก็ต่อเมื่อส่งค่า LOW ไปให้ LEDPIN
    if (currentLEDState == 1) digitalWrite(LEDPIN, LOW); // LED ON
    else  digitalWrite(LEDPIN, HIGH); // LED OFF
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

   netpieshow("Starting....Lab03");

 
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(9600);
    Serial.println("Starting...");
    dht.begin(); // initialize โมดูล DHT

    // กำหนดชนิดของ PIN (ขาI/O) เช่น INPUT, OUTPUT เป็นต้น
    pinMode(LEDPIN, OUTPUT);          // LED pin mode กำหนดค่า
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
    digitalWrite(WIFI_LED, LOW);
    digitalWrite(IOT_LED, HIGH);
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
    digitalWrite(WIFI_LED, LOW);
    digitalWrite(IOT_LED, LOW);
    netpieshow("NETPIE connected"); 

}

void loop() {
    if (microgear.connected()) {
        microgear.loop();

        if(currentLEDState != lastLEDState){
          microgear.publish(LEDSTATETOPIC, currentLEDState);  // LEDSTATETOPIC ถูก define ไว้ข้างบน
          lastLEDState = currentLEDState;
        }

        if (digitalRead(BUTTONPIN1)==HIGH) currentButtonState = 0;
        else currentButtonState = 1;

        if(currentButtonState != lastButtonState){
          microgear.chat(NEIGHBOR, currentButtonState);
          lastButtonState = currentButtonState;
        }

        // เซนเซอร์​ DHT อ่านถี่เกินไปไม่ได้ จะให้ค่า error เลยต้องเช็คเวลาครั้งสุดท้ายที่อ่านค่า
        // ว่าทิ้งช่วงนานพอหรือยัง ในที่นี้ตั้งไว้ 2 วินาที ก
        if(millis() - lastDHTRead > 2000){
          humid = dht.readHumidity();     // อ่านค่าความชื้น
          temp  = dht.readTemperature();  // อ่านค่าอุณหภูมิ
          lastDHTRead = millis();
          
          Serial.print("Humid: "); Serial.print(humid); Serial.print(" %, ");
          Serial.print("Temp: "); Serial.print(temp); Serial.println(" °C ");
           
          // ตรวจสอบค่า humid และ temp เป็นตัวเลขหรือไม่
          if (isnan(humid) || isnan(temp)) {
            Serial.println("Failed to read from DHT sensor!");
             netpieshow("DHT sensor ERROR...");
          }
          else{
            // เตรียมสตริงในรูปแบบ "{humid},{temp}"
            netpieshow("Tem:"+String(temp)+" C "+"Hum:"+String(humid)+"%"); 
            String datastring = (String)humid+","+(String)temp;
            Serial.print("Sending --> ");
            Serial.println(datastring);
            microgear.publish(DHTDATATOPIC,datastring);   // DHTDATATOPIC ถูก define ไว้ข้างบน
             
          }

            String datastring = (String)humid+","+(String)temp;
            Serial.print("Sending --> ");
            Serial.println(datastring);
            microgear.publish(DHTDATATOPIC,datastring);   // DHTDATATOPIC ถูก define ไว้ข้างบน
          
        }  
    }
    else {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID); 
    }
}
