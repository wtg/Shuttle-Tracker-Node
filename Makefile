setup:
ifeq ("$(wildcard $(.port.txt))","")
	touch .port.txt
	echo "Type serial port into .port.txt"
endif

file := .port.txt
variable := $(shell cat ${file})

%:
    @: 

compile:

	arduino-cli compile -b esp32:esp32:esp32c3 $(filter-out $@,$(MAKECMDGOALS))
upload:
	arduino-cli upload -b esp32:esp32:esp32c3 -p $(variable) $(filter-out $@,$(MAKECMDGOALS))

monitor:
	arduino-cli monitor -p $(variable) -c baudrate=115200

compupmon:
	arduino-cli compile -b esp32:esp32:esp32c3 $(filter-out $@,$(MAKECMDGOALS))
	arduino-cli upload -b esp32:esp32:esp32c3 -p $(variable) $(filter-out $@,$(MAKECMDGOALS))
	arduino-cli monitor -p $(variable) -c baudrate=115200
