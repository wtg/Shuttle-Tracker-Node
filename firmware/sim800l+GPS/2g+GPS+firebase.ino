#define TINY_GSM_MODEM_SIM800

//Increase RX buffer
#define TINY_GSM_RX_BUFFER 256

#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
#include <ArduinoHttpClient.h> //https://github.com/arduino-libraries/ArduinoHttpClient

const char FIREBASE_HOST[]  = "esp32-gps-firebase-6df57-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH  = "";  //Contact tranh4@rpi.edu for database secret code
const String FIREBASE_PATH  = "/";
const int SSL_PORT          = 443;

char apn[]  = "internet";
char user[] = ""; //contact tranh4@rpi.edu for APN infos
char pass[] = ""; //contact tranh4@rpi.edu for APN infos

#define rxPin 4
#define txPin 2
HardwareSerial sim800(1);
TinyGsm modem(sim800);

#define RXD2 16
#define TXD2 17
HardwareSerial neogps(2);
TinyGPSPlus gps;

TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;
long interval = 10000;

void setup() {
  Serial.begin(115200);
  Serial.println("esp32 serial initialize");
  
  sim800.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800L serial initialize");

  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("neogps serial initialize");
  delay(3000);
  
  //Restart takes quite some time
  //To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  
  // Unlock SIM card with a PIN
  //modem.simUnlock("1234");
  
  http_client.setHttpResponseTimeout(90 * 1000); //^0 secs timeout
}

void loop() {
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(1000);
    return;
  }
  Serial.println(" OK");
  
  http_client.connect(FIREBASE_HOST, SSL_PORT);
  
  while (true) {
    if (!http_client.connected()) {
      Serial.println();
      http_client.stop();// Shutdown
      Serial.println("HTTP  not connect");
      break;
    }
    else{
      gps_loop();
    }
  }
}

void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http) {
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); 
  
  String url;
  if (path[0] != '/') {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;
  Serial.print("POST:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);
  
  String contentType = "application/json";
  http->put(url, contentType, data);
  

  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("Response: ");
  Serial.println(response);

  if (!http->connected()) {
    Serial.println();
    http->stop();// Shutdown
    Serial.println("HTTP POST disconnected");
  }
}


void gps_loop()
{
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }

  if(true){
  newData = false;
  
  String latitude, longitude;

  
  latitude = String(gps.location.lat(), 6); // Latitude in degrees (double)
  longitude = String(gps.location.lng(), 6); // Longitude in degrees (double)
  
  
  Serial.print("Latitude= "); 
  Serial.print(latitude);
  Serial.print(" Longitude= "); 
  Serial.println(longitude);
      
  String gpsData = "{";
  gpsData += "\"lat\":" + latitude + ",";
  gpsData += "\"lng\":" + longitude + "";
  gpsData += "}";
  
  PostToFirebase("PATCH", FIREBASE_PATH, gpsData, &http_client);
  

  }
}