// TelemetryManager.h
#ifndef TELEMETRY_MANAGER_H
#define TELEMETRY_MANAGER_H

#define TINY_GSM_RX_BUFFER 256

#include <TinyGsmClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>

class TelemetryManager {
public:
    /**
     * @param simRx   ESP32 RX pin for SIM800L TX
     * @param simTx   ESP32 TX pin for SIM800L RX
     * @param gpsRx   ESP32 RX pin for NEO-6M TX
     * @param gpsTx   ESP32 TX pin for NEO-6M RX
     * @param phone   recipient phone number for SMS commands
     * @param apn     GPRS APN
     * @param user    GPRS username (often empty)
     * @param pass    GPRS password (often empty)
     * @param fbHost  Firebase RTDB host (no protocol)
     * @param fbAuth  Firebase database secret/Auth token
     * @param fbPath  Firebase JSON path prefix
     * @param fbIntervalMs reporting interval in ms
    */
    TelemetryManager(
      uint8_t simRx, uint8_t simTx,
      uint8_t gpsRx, uint8_t gpsTx,
      const char* phone,
      const char* apn, const char* user, const char* pass,
      const char* fbHost, const char* fbAuth, const char* fbPath,
      unsigned long fbIntervalMs = 10000UL
    );

    void begin();   // initialize SIM, GPS, and HTTP
    void handle();  // call periodically in loop()

private:
    // Serial/UART
    HardwareSerial simSerial;
    HardwareSerial gpsSerial;
    TinyGPSPlus   gps;
    const char*   phoneNumber;
    String        smsBuffer;

    // Firebase via GPRS
    TinyGsm        modem;
    TinyGsmClientSecure clientSecure;
    HttpClient     httpClient;
    const char*    apn;
    const char*    gprsUser;
    const char*    gprsPass;
    const char*    fbHost;
    const char*    fbAuth;
    const char*    fbPath;
    unsigned long  fbInterval;
    unsigned long  lastFbMillis;

    // Core helpers
    bool   waitForResponse(const char* target, uint32_t timeout_ms);
    String readNextLine();
    void   processLine(const String& line);
    void   sendLocationSMS();
    void   sendSpeedSMS();

    // Firebase methods
    bool   connectGPRS();
    void   postToFirebase(const String& data);
};

#endif // TELEMETRY_MANAGER_H