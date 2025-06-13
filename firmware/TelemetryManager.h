// TelemetryManager.h
#ifndef TELEMETRY_MANAGER_H
#define TELEMETRY_MANAGER_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Arduino.h>

class TelemetryManager {
public:
    // constructor: SIM RX/TX, GPS RX/TX, phone number
    TelemetryManager(uint8_t simRx, uint8_t simTx, uint8_t gpsRx, uint8_t gpsTx, const char* phone);
    void begin();        // init serials and modules
    void handle();       // call in loop()

private:
    HardwareSerial simSerial;
    HardwareSerial gpsSerial;
    TinyGPSPlus   gps;
    const char*   phoneNumber;
    String        smsBuffer;

    bool   waitForResponse(const char* target, uint32_t timeout_ms);
    String readNextLine();
    void   processLine(const String& line);
    void   sendLocationSMS();
    void   sendSpeedSMS();
};

#endif // TELEMETRY_MANAGER_H