#include "painlessMesh.h"
#include <ArduinoJson.h>

// WiFi Credentials
#define   MESH_PREFIX     "Hotspot"
#define   MESH_PASSWORD   "0987654321"
#define   MESH_PORT       5555

//Pin Declaration
#define Relay2 D1

bool relay2_status = 0;


Scheduler userScheduler;
painlessMesh  mesh;


void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage()
{
  // Serializing in JSON Format
  DynamicJsonDocument doc(1024);
  doc["GAS"] = analogRead(A0);
  String msg ;
  serializeJson(doc, msg);
  mesh.sendBroadcast(msg);
  Serial.println(msg);
  taskSendMessage.setInterval((TASK_SECOND * 10));
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg )
{
  //Deserializing
  String json;
  DynamicJsonDocument doc(1024);
  json = msg.c_str();
  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

  relay2_status = doc["Relay2"];
  digitalWrite(Relay2, relay2_status);

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

  pinMode(Relay2, OUTPUT);
  pinMode(A0, INPUT);

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
