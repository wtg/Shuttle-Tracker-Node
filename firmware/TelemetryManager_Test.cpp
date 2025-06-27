// Unit tests for TelemetryManager using GoogleTest

#include <gtest/gtest.h>
#include "TelemetryManager.h"

// Simple FakeStream to capture input/output
class FakeStream : public Stream {
public:
  std::string in, out;
  int available() override { return in.size(); }
  int read() override { char c = in.front(); in.erase(0,1); return c; }
  int peek() override { return in.empty() ? -1 : in.front(); }
  void flush() override {}
  size_t write(uint8_t c) override { out.push_back(c); return 1; }
  size_t write(const uint8_t* buf, size_t len) override { out.append((char*)buf, len); return len; }
};

class TestableTelemetry : public TelemetryManager {
public:
  // inheritance of constructor
  using TelemetryManager::TelemetryManager;
  using TelemetryManager::processLine;
  using TelemetryManager::sendLocationSMS;
  using TelemetryManager::sendSpeedSMS;
  using TelemetryManager::postToFirebase;
};

// Test fixture sets up a test instance with fakes
class TelemetryManagerTest : public ::testing::Test {
protected:
  FakeStream fakeSim, fakeGps;
  TestableTelemetry* tm;

  void SetUp() override {
    // Constructor arguments: SIM stream, GPS stream, phone, APN, user, pass, fbHost, fbAuth, fbPath, interval-ms
    tm = new TestableTelemetry(
      fakeSim, fakeGps,
      "+1234567890",
      "internet", "", "",
      "test.firebaseio.com", "secret", "/devices/test",
      1000
    );
    tm->begin();
  }

  void TearDown() override {
    delete tm;
  }
};

TEST_F(TelemetryManagerTest, CMTITriggersCMGR) {
  fakeSim.out.clear();
  fakeSim.in = "+CMTI: \"SM\",5\r\n";
  tm->handle();
  EXPECT_NE(fakeSim.out.find("AT+CMGR=5"), std::string::npos);
}

TEST_F(TelemetryManagerTest, GetLocationCommandSendsSMS) {
  // simulate valid GPS fix
  tm->gps.location = TinyGPSLocation(12.345678, 98.765432);

  // simulate incoming SMS with "get location"
  std::string hdr = "+CMGR: \"REC UNREAD\",\"+1234567890\"\r\n";
  fakeSim.in = hdr + "get location\r\n";

  fakeSim.out.clear();
  tm->processLine(hdr);
  tm->sendLocationSMS();

  EXPECT_NE(fakeSim.out.find("CMGS"), std::string::npos);
}

TEST_F(TelemetryManagerTest, PostToFirebaseFormatsCorrectJSON) {
  // simulate GPS fix
  tm->gps.location = TinyGPSLocation(1.230000, 4.560000);
  // capture HTTP payload via fakeSim (which underlies gsmClient)
  fakeSim.out.clear();
  tm->postToFirebase("{\"lat\":1.230000,\"lng\":4.560000}");
  std::string sent = fakeSim.out;
  EXPECT_NE(sent.find("PUT /devices/test.json?auth=secret"), std::string::npos);
  EXPECT_NE(sent.find("{\"lat\":1.230000,\"lng\":4.560000}"), std::string::npos);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
