cmake_minimum_required(VERSION 3.10)
project(FirmwareTests)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ../)

# OTA update manager tests
add_executable(OTAUpdateManagerTest
  OTAUpdateManager_Test.cpp
  ../OTAUpdateManager.cpp
)
target_link_libraries(OTAUpdateManagerTest
  ${GTEST_LIBRARIES}
  pthread
)

# Telemetry manager tests
add_executable(TelemetryManagerTest
  TelemetryManager_Test.cpp
  ../TelemetryManager.cpp
)
target_link_libraries(TelemetryManagerTest
  ${GTEST_LIBRARIES}
  pthread
)
