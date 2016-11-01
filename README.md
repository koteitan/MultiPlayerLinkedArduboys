# MultiPlayerLinkedArduboys
Please make symbolic links to /server/raspi3/serial0 and serial1 
from your serial ports. 
for example:

    cd /server/raspi3/
    ls /dev/serial/by-path/
    # check your serial port here...
    ln -s /dev/serial/by-path/pci-0000:00:1d.7-usb-0:1.4:1.0-port0
    ln -s /dev/serial/by-path/pci-0000:00:1d.7-usb-0:1.2:1.0-port0
    gcc -o server server.c
    ./server

## Reference
- https://www.arduino.cc/en/Serial/ReadBytes
- https://www.arduino.cc/en/Serial/write
- https://www.arduino.cc/en/Serial/SetTimeout
- http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/
