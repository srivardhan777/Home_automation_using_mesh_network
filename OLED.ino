#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi Credentials
#define   MESH_PREFIX     "Hotspot"
#define   MESH_PASSWORD   "0987654321"
#define   MESH_PORT       5555

// Necessary for OLED Display
#define OLED_RESET LED_BUILTIN //4
Adafruit_SSD1306 display(OLED_RESET);

bool button1_status = 0;
bool button2_status = 0;

#define Button1 D4
#define Button2 D5

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;


void sendMessage() ;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() 
{
  
// Reading Status of Pushbutton
  if (digitalRead(Button1) == LOW)
    button1_status = !button1_status;//(Initially at 0 i.e. button1_status = 0)
  if (digitalRead(Button2) == LOW)
    button2_status = !button2_status;//(Initially at 0 i.e. button1_status = 0)

// Serializing in JSON Format 
  DynamicJsonDocument doc(1024);
  doc["Relay1"] = button1_status;
  doc["Relay2"] = button2_status;
  String msg ;
  serializeJson(doc, msg);
  mesh.sendBroadcast( msg );
  Serial.println(msg);
  taskSendMessage.setInterval((TASK_SECOND * 1));
}
// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {

  String json;
  DynamicJsonDocument doc(1024);
  json = msg.c_str();
  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

   String Temp = doc["TEMP"];
   String Hum = doc["HUM"];
   String Gas = doc["GAS"];

   
   

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 0);
  if(Temp != "null"){
  display.print("Temp - ");
  display.print(Temp.substring(0,2));}
  display.setCursor(15, 20);
  if(Hum != "null"){
  display.print("Humi - ");
  display.print(Hum.substring(0,2));}
  display.setCursor(15, 40);
  if(Gas != "null"){
  display.print("Gas - ");
  display.print(Gas);}
  display.display();
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 25);
  display.println("Home Automation");
  display.setTextColor(WHITE); // 'inverted' text
  display.display();

  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
