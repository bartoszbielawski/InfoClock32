# InfoClock32
A smaller brother of the InfoClock with ESP32 and an OLED display

_The code is under development, many things can and will change!_

## How to install...

1. Download Visual Studio Code (or other editor that supports platform.io)
2. Download platform.io plugin for the editor,
3. Checkout the project and the dependencies,
4. Upload filesystem (`platformio target -r uploadfs`),
5. Upload the program itself,

## Configuration...

1. Power on the device and wait for it to set up an access point,
2. Connect to the access point and go to http://192.168.4.1
3. Open configuration editor,
4. Set `wifi.essid` and `wifi.password` and other variables,
5. Save - the device will reboot
6. That's it!

