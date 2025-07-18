// OTAUpdateManager_Test.cpp
#include <gtest/gtest.h>
#include "../OTAUpdateManager.h"

// A Fake subclass to override network behavior
class FakeOTA : public OTAUpdateManager {
public:
    FakeOTA(const char* fwUrl, const char* verUrl, const char* ca)
      : OTAUpdateManager(fwUrl, verUrl, ca), fetchCalled(false), otaCalled(false), shouldFetch(false) {}

    // Override to simulate version check
    bool fetchLatestVersion() override {
        fetchCalled = true;
        return shouldFetch;
    }
    // Override to simulate OTA execution
    void performOTA() override {
        otaCalled = true;
    }

    // Allow toggling behavior
    void setShouldFetch(bool v) { shouldFetch = v; }
    // Expose checkAndUpdate for tests
    using OTAUpdateManager::checkAndUpdate;

    bool fetchCalled;
    bool otaCalled;
private:
    bool shouldFetch;
};

TEST(OTAUpdateManager, DoesNotFetchAgainAfterFirstCheck) {
    FakeOTA fake("fw.bin", "version.txt", "CA");
    // First call: should fetch
    fake.setShouldFetch(true);
    fake.checkAndUpdate();
    EXPECT_TRUE(fake.fetchCalled);
    EXPECT_TRUE(fake.otaCalled);

    // Reset flags
    fake.fetchCalled = false;
    fake.otaCalled = false;

    // Second call: hasChecked is true, so no further fetch or OTA
    fake.checkAndUpdate();
    EXPECT_FALSE(fake.fetchCalled);
    EXPECT_FALSE(fake.otaCalled);
}

TEST(OTAUpdateManager, SkipsOTAWhenNoNewVersion) {
    FakeOTA fake("fw.bin", "version.txt", "CA");
    // Simulate no new version available
    fake.setShouldFetch(false);
    fake.checkAndUpdate();
    EXPECT_TRUE(fake.fetchCalled);
    EXPECT_FALSE(fake.otaCalled);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
