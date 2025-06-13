#include "TelemetryManager.h"

TelemetryManager::TelemetryManager(uint8_t simRx, uint8_t simTx,
                                   uint8_t gpsRx, uint8_t gpsTx,
                                   const char* phone)
  : simSerial(1), gpsSerial(2), phoneNumber(phone) {
  // configure UART pins
  simSerial.begin(9600, SERIAL_8N1, simRx, simTx);
  gpsSerial.begin(9600, SERIAL_8N1, gpsRx, gpsTx);
}

void TelemetryManager::begin() {
  // SIM initialization
  simSerial.println("AT");             waitForResponse("OK", 2000);
  simSerial.println("AT+CMGF=1");      waitForResponse("OK", 2000);
  simSerial.println("AT+CNMI=2,1,0,0,0"); // forward SMS
  waitForResponse("OK", 2000);
}

void TelemetryManager::handle() {
  // 1) Feed GPS data
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
  // 2) Feed SIM data and parse lines
  while (simSerial.available()) {
    char c = simSerial.read();
    smsBuffer += c;
    if (c == '\n') {
      processLine(smsBuffer);
      smsBuffer.clear();
    }
  }
  delay(5000);
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
    // cleanup
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
