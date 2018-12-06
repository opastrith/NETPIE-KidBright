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
#define LM73_ADDR 0x4D
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();

float currentTemp,currentHumid,currentLight,currentMoisture;



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

///////////read temperature/////////////////

float readTemperature() {
  Wire1.beginTransmission(LM73_ADDR);
  Wire1.write(0x00); // Temperature Data Register
  Wire1.endTransmission();
  
  uint8_t count = Wire1.requestFrom(LM73_ADDR, 2);
  float temp = 0.0;
  if (count == 2) {
    byte buff[2];
    buff[0] = Wire1.read();
    buff[1] = Wire1.read();
    temp += (int)(buff[0]<<1);
    if (buff[1]&0b10000000) temp += 1.0;
    if (buff[1]&0b01000000) temp += 0.5;
    if (buff[1]&0b00100000) temp += 0.25;
    if (buff[0]&0b10000000) temp *= -1.0;
  }
  return temp;
}




///show Moisture and Light  ///////////////

void MeasureTempLightMoisture(){
 
 //   while(1){
      matrix.setTextSize(1);
      matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
      matrix.setTextColor(LED_ON);
      matrix.setRotation(1);
      float temp = readTemperature();
      Serial.print("Temp:");Serial.println(temp);
      ///Read light ///////////////////////////////
      
    long  analog_value = analogRead(36);
       Serial.print("Light:");Serial.println(analog_value);
      if(analog_value > 1000){
        analog_value = 1000;
      }
      float ll = (1000 - analog_value)*100/1000;
       currentLight =ll;
       analog_value = analogRead(39);
       Serial.print("Moisture:");Serial.println(analog_value);
      analog_value =  analog_value*3600/4095;
      float moisture= analog_value*100/3600;
       currentMoisture =  moisture;
      
   for (int8_t x=7; x>=-126; x--) {
        matrix.clear();
        matrix.setCursor(x,0);
        matrix.print("MT: " + String(moisture) +" Light = " + String(ll) + " %");
        matrix.writeDisplay();
        delay(30);
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
int timer = 0;
////////////NETPIE setting ////////////////////////
MicroGear microgear(client);


#define APPID   "TEINproject"
#define KEY     "8UR29KW3wDY00Pt"
#define SECRET  "XhUPo6SEzH4oc5RJ3cISr1cOB"
#define ALIAS   "A02"           // แทนที่ด้วยหมายเลขของท่าน เช่น "A01"
#define NEIGHBOR "A01"             // ชื่ออุปกรณ์ของเพื่อน เช่น "A02"
#define FEEDID   "TEINfeed"           
#define FEEDAPI  "7bolhRaYdxnUHnb9od7gszEz5DjFDG1O"   
//-------------------------------------------------------------

#define LEDSTATETOPIC "/ledstate/" ALIAS      // topic ที่ต้องการ publish ส่งสถานะ led ในที่นี้จะเป็น /ledstate/{ชื่อ alias ตัวเอง}
#define DHTDATATOPIC "/dht/" ALIAS            // topic ที่ต้องการ publish ส่งข้อมูล dht ในที่นี่จะเป็น /dht/{ชื่อ alias ตัวเอง}

#define BUTTONPIN1  16                         // pin ที่ต่อกับปุ่ม Flash บนบอร์ด NodeMCU
#define LEDPIN     12                // pin ที่ต่อกับไฟ LED บนบอร์ด NodeMCU

int currentLEDState = 1;      // ให้เริ่มต้นเป็น OFF หมายเหตุ ไฟ LED บนบอร์ดต่อแบบ active-low
int lastLEDState = 0;
int currentButtonState = 1;   // หมายเหตุ ปุ่ม flash ต่อเข้ากับ GPIO0 แบบ pull-up
int lastButtonState = 0;
long lastTimeWriteFeed = 0;

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
  dht.begin(); // initialize โมดูล DHT
   netpieshow("Starting..Lab04");

 
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
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
}

void loop() {
    if (microgear.connected()) {
        microgear.loop();
        MeasureTempLightMoisture();
        if(currentLEDState != lastLEDState){
          microgear.publish(LEDSTATETOPIC, currentLEDState);  // LEDSTATETOPIC ถูก define ไว้ข้างบน
          lastLEDState = currentLEDState;
        }

        if (digitalRead(BUTTONPIN1)==LOW) currentButtonState = 0;
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
            currentTemp=temp;
            currentHumid=humid;
             
            String datastring = (String)currentTemp+","+(String)currentHumid+","+(String)currentLight+","+(String)currentMoisture;
            Serial.print("Sending --> ");
            Serial.println(datastring);
            microgear.publish(DHTDATATOPIC,datastring);   // DHTDATATOPIC ถูก define ไว้ข้างบน
   
            
          }
        }
      /////////////////////////////////////////////////////////////////

        if(millis()-lastTimeWriteFeed > 15000){
          lastTimeWriteFeed = millis();
          if(humid!=0 && temp!=0){
             String feeddata = "{\"Humid\":";
            feeddata += String(currentHumid) ;
            feeddata += ", \"Temp\":";
            feeddata += String(currentTemp);
            feeddata += ", \"Light\":";
            feeddata += String(currentLight);
            feeddata += ", \"Moisture\":";
            feeddata += String(currentMoisture);
            feeddata += "}"; 
            Serial.print("Write Feed --> ");
            Serial.println(feeddata);
            
           
             netpieshow("Send to FEED->..."); 
            //microgear.writeFeed(FEEDID,feeddata);
            microgear.writeFeed(FEEDID,feeddata,FEEDAPI);
          }
        }
         
         
       


      
          
    }
    else {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID); 
    }
}
