
// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// network credentials
const char* ssid = "";
const char* password = "";
//Firebase project API Key
#define API_KEY ""
//Authorized Email and Corresponding Password
#define USER_EMAIL "
#define USER_PASSWORD ""
// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL ""
#define DHTPIN 5     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);
// current temperature & humidity varaibles 
float t = 0.0;
float h = 0.0;
// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
// Variable to save USER UID
String uid;
// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String humPath = "/humidity";
// Parent Node (to be updated in every loop)
String parentPath;
FirebaseJson json;
unsigned long previousMillis = 0;    // will store last time DHT was updated
const long interval = 10000;  // Updates DHT readings every 10 seconds


void setup(){
  Serial.begin(115200);
  //start DHT
  dht.begin();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  //Firebase setup
  config.api_key = API_KEY;
  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
    // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;
    // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);
  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');`
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  //set path for values obtained
 databasePath = "/UsersData/" + uid + "/DHT";
}
 
void loop(){  
 //get time
   unsigned long currentMillis = millis();
   //wait time interval 
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Read temperature as Celsius 
    float newT = dht.readTemperature();
    if (isnan(newT)) { //fail handle 
      Serial.println("Failed to read from DHT sensor!");
    }
    else { 
      t = newT;
      Serial.print("Temperature: ");
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    if (isnan(newH)) { //fail handle
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.print("Humidity: ");
      Serial.println(h);
    }
    //firebase
    parentPath= databasePath;
    //json set for humidity and temp values
    json.set(tempPath.c_str(), String(t));
    json.set(humPath.c_str(), String(h));
    //update firebase
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
