These are unit tests for Shuttle-Tracker-Node using GoogleTest - Google Testing and Mocking Framework
// https://github.com/google/googletest

You need to install GoogleTest in order to run these unit tests.

TelemetryManager testing:

+FakeStream to simulate and capture UART I/O.
+TestableTelemetry subclass exposing key internals for direct testing.
+Three tests covering:
	+Parsing +CMTI → sending AT+CMGR=…
	+Handling “get location” → issuing an SMS
	+JSON formatting and HTTP PUT path for Firebase
