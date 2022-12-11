
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <WiFiUdp.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PULSE_INPUT = A0;
const int PULSE_BLINK = 2;    
const int PULSE_FADE = 5;
const int THRESHOLD = 550;   

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor;

//network credentials
const char* ssid = "";
const char* password = "";
// Firebase project API Key
#define API_KEY ""

// Insert Authorized Email and Corresponding Password 
#define USER_EMAIL ""
#define USER_PASSWORD ""

// RTDB URLefine the RTDB URL
#define DATABASE_URL ""
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;
// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child node for heart rate data
String hrPath = "/heartRate";
String parentPath;
FirebaseJson json;

unsigned long previousMillis = 0;    // will store last time HR was updated
const long interval = 10000;  

void setup() {
  Serial.begin(115200);

  // Configure the PulseSensor manager.
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);
  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    //fail handle
    for(;;) {
      // Flash the led to show things didn't work.
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(50);
    }
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  //Firebase setup
  // Assign the api key 
  config.api_key = API_KEY;
  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  // Assign the RTDB URL 
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
    // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;
    // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);
  // Getting the user UID
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
//set database path
 databasePath = "/UsersData/" + uid + "/HeartRate";
}

void loop() {
    //track time
  unsigned long currentMillis = millis();
  //wait interval 
  if (currentMillis - previousMillis >= interval) {
    //get BPM
    if (pulseSensor.sawNewSample()) {
      if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
       int myBPM = pulseSensor.getBeatsPerMinute(); 
      if (pulseSensor.sawStartOfBeat()) {
         Serial.println("♥  A HeartBeat Happened ! "); 
          Serial.print("BPM: ");                        
          Serial.println(myBPM);     
           parentPath= databasePath;
    //set json with BPM to be uploaded to firebase
    json.set(hrPath.c_str(), String(myBPM));
    //update database
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    delay(1000);
      }
    }}}}
