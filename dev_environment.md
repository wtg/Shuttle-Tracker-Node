# Shuttle Tracker Node dev environment setup.

## Clone the repository. 
    Clone the repository from https://github.com/wtg/Shuttle-Tracker-Node
## Installing Arduino CLI
   Get Arduino CLI from [here](https://arduino.github.io/arduino-cli/0.21/installation/) 
   Download and install the exe, or if you have a package manager you can install using that if you would prefer.

### Configuration
    > arduino-cli config init
    > arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/packkage_esp32_index.json

## Windows 

### Drivers
    1. Download driver from the [driver page](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
    2. Extract zip file 
    3. Use Windows Search to find and open the "Device manager" program.
    4. Connect an Esp32 to your computer over usb.
    5. Look for "CP2102N USb to UART bridge controller" under the "Other Devices" section.
    6. Right Click and select "Update Driver" -> "Browse my Computer". Navigate and select the unzipped driver you downloaded earlier. Make sure to select the parent folder not the "arm", "x64", or "x86" subfolders. Click next. It should tell you that the driver installed successfully. 
    7. Now you should see an entry under the Ports that looks like "Silicon Labs CP210x USB to UART Bridge". The port number will be at the end of the device name in parenthesis(ie. (COM9)).

### Compatability
    For compatability with our build system, we need to link the arduino-cli.exe to "arduino-cli" for WSL.
    > ln -s  $ARDUINO-CLI.EXE PATH$ /usr/bin/arduino-cli

## Linux
    User needs to be added to a group for access.
    > sudo usermod -a -G dialout <your_username>
    If you are Arch the group might be uucp instead.
    > sudo usermod -a -G uucp <your_username>
    If neither of these groups are the right one you can find it with this command.
    > ls -l <PORT>
    Output:
    > c--------- 1 root <GROUP> 188, 0 Mar  3 17:19 /dev/ttyUSB0
    > sudo usermod -a -G <GROUP> <your_username>
    Restart, and you should be able to run the arduino commands. 

    If it still is not working you might need to run this command to directly give permissions to interact with the port.
    > sudo chmod a+rw <PORT> 


 
