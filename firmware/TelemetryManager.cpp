#include "TelemetryManager.h"

// Adjust RX buffer if needed before TinyGSM include
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 256
#endif

TelemetryManager::TelemetryManager(
  uint8_t simRx, uint8_t simTx,
  uint8_t gpsRx, uint8_t gpsTx,
  const char* phone,
  const char* apn_, const char* user_, const char* pass_,
  const char* fbHost_, const char* fbAuth_, const char* fbPath_,
  unsigned long fbIntervalMs
) :
  simSerial(1),
  gpsSerial(2),
  phoneNumber(phone),
  modem(simSerial),
  clientSecure(modem, 0),
  httpClient(clientSecure, fbHost_, 443),
  apn(apn_), gprsUser(user_), gprsPass(pass_),
  fbHost(fbHost_), fbAuth(fbAuth_), fbPath(fbPath_),
  fbInterval(fbIntervalMs), lastFbMillis(0)
{
    // Initialize UART pins
    simSerial.begin(9600, SERIAL_8N1, simRx, simTx);
    gpsSerial.begin(9600, SERIAL_8N1, gpsRx, gpsTx);
}

void TelemetryManager::begin() {
  // SIM configuration
  simSerial.println("AT");             waitForResponse("OK", 2000);
  simSerial.println("AT+CMGF=1");      waitForResponse("OK", 2000);
  simSerial.println("AT+CNMI=2,1,0,0,0"); waitForResponse("OK", 2000);

  // Modem restart & HTTP timeout
  modem.restart();
  httpClient.setHttpResponseTimeout(90 * 1000);

  Serial.println("TelemetryManager initialized.");
}

void TelemetryManager::handle() {
  // 1) Feed GPS
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
  // 2) Feed SIM and process SMS lines
  while (simSerial.available()) {
    char c = simSerial.read();
    smsBuffer += c;
    if (c == '\n') {
      processLine(smsBuffer);
      smsBuffer.clear();
    }
  }
  // 3) Periodic Firebase reporting
  unsigned long now = millis();
  if (now - lastFbMillis >= fbInterval) {
    lastFbMillis = now;
    if (connectGPRS()) {
      // build JSON
      if (gps.location.isValid()) {
        String data = "{";
        data += "\"lat\":" + String(gps.location.lat(), 6) + ",";
        data += "\"lng\":" + String(gps.location.lng(), 6);
        data += "}";
        postToFirebase(data);
      }
      // clean up
      modem.gprsDisconnect();
    }
  }
}

bool TelemetryManager::waitForResponse(const char* target, uint32_t timeout_ms) {
  uint32_t start = millis();
  String resp;
  while (millis() - start < timeout_ms) {
    while (simSerial.available()) {
      resp += char(simSerial.read());
      if (resp.indexOf(target) >= 0) return true;
    }
  }
  return false;
}

String TelemetryManager::readNextLine() {
  String s;
  uint32_t start = millis();
  while (millis() - start < 2000) {
    if (simSerial.available()) {
      char c = simSerial.read();
      s += c;
      if (c == '\n') break;
    }
  }
  return s;
}

void TelemetryManager::processLine(const String& line) {
  if (line.startsWith("+CMTI:")) {
    int idx = line.substring(line.indexOf(',')+1).toInt();
    simSerial.printf("AT+CMGR=%d\r\n", idx);
  }
  else if (line.startsWith("+CMGR:")) {
    String header = line;
    String body   = readNextLine();
    // extract sender
    int q1 = header.indexOf('"');
    int q2 = header.indexOf('"', q1+1);
    int q3 = header.indexOf('"', q2+1);
    int q4 = header.indexOf('"', q3+1);
    String sender = header.substring(q3+1, q4);

    body.trim();
    if (sender == phoneNumber) {
      if (body.equalsIgnoreCase("get location")) sendLocationSMS();
      else if (body.equalsIgnoreCase("get speed"))    sendSpeedSMS();
    }
    // cleanup SMS
    simSerial.println("AT+CMGD=1,4");
    waitForResponse("OK", 2000);
  }
}

void TelemetryManager::sendLocationSMS() {
  if (!gps.location.isValid()) return;
  String msg = String("http://maps.google.com/maps?q=loc:")
               + String(gps.location.lat(), 6) + ","
               + String(gps.location.lng(), 6);
  simSerial.println("AT+CMGF=1");              waitForResponse("OK", 2000);
  simSerial.printf("AT+CMGS=\"%s\"\r\n", phoneNumber);
  if (!waitForResponse(">", 5000)) return;
  simSerial.print(msg);
  simSerial.write(0x1A);
  waitForResponse("OK", 10000);
}

void TelemetryManager::sendSpeedSMS() {
  if (!gps.speed.isValid()) return;
  String msg = String("Speed km/h: ") + String(gps.speed.kmph(), 2);
  simSerial.println("AT+CMGF=1");              waitForResponse("OK", 2000);
  simSerial.printf("AT+CMGS=\"%s\"\r\n", phoneNumber);
  if (!waitForResponse(">", 5000)) return;
  simSerial.print(msg);
  simSerial.write(0x1A);
  waitForResponse("OK", 10000);
}

bool TelemetryManager::connectGPRS() {
  return modem.gprsConnect(apn, gprsUser, gprsPass);
}

void TelemetryManager::postToFirebase(const String& data) {
  // build full path
  String url = String(fbPath);
  if (!url.startsWith("/")) url = "/" + url;
  url += ".json?auth=" + String(fbAuth);

  // HTTP PATCH (using PUT for entire path)
  httpClient.put(url.c_str(), "application/json", data);
  int status = httpClient.responseStatusCode();
  String resp  = httpClient.responseBody();
  Serial.printf("Firebase [%d]: %s\n", status, resp.c_str());

  if (!httpClient.connected()) {
    httpClient.stop();
  }
}
