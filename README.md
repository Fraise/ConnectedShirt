# connected_shirt
This repository contains source files for a connected shirt using Arduino Lilypad, providing fall detection.

# The shirt
It consists in a simple circuit around the Atmega 328p provided by the Arduino Lilypad board. It communicates through UDP protocol using the XBEE wireless connection. The microcontroller uses a simple scheduler to switch between tasks of measuring acceleration, blinking leds and sending packets.

# The server
It is a lightweight UDP/TCP multithreaded server. You can see the different measures through webpages. The server also sends acknowlegement to the shirt sending a fall detection packet, so the user knows the information has been received.
