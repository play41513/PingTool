# PingTool
 Network card ping test.

## Program

### PingTool v2.8
- Notices :Copyright(c) 2023 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Added 2.5G network card test.
	- Hidden "Static IP" functionality.

### PingTool v2.7
- Notices :Copyright(c) 2021 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Debug.
### PingTool v2.6
- Notices :Copyright(c) 2021 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Optimization: Improved determination of USB speed.

### PingTool v2.5
- Notices :Copyright(c) 2021 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Fixed the error in extracting USB PVID for v2.4.

### PingTool v2.4
- Notices :Copyright(c) 2021 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Resolved the issue of unable to obtain USB-Type for some VPID.
	- Prevented some screens from flickering.

### PingTool v2.3
- Notices :Copyright(c) 2021 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Resolved the problem of immediate program failure to determine USB speed when some network cards are freshly inserted, program now continuously fetches speed for up to five seconds if unable to retrieve immediately.
	- Added network card mutual ping function (check under the Start button).
	- Added USB 2.0 network card test (check under the Start button).

### PingTool v2.2
- Notices :Copyright(c) 2020 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Fixed the issue of misjudgment of Realtek network card IC USB3.0.

### PingTool v2.1
- Notices :Copyright(c) 2020 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Modified the work order information input screen.
	- Compatible with ASIX product testing.
	- ASIX product USB2.0 and USB3.0 testing.
	- UI changes for ASIX product testing.
	- Added network card name comparison function.
	- BarcodeMAC functionality corrected.

### PingTool v2.0.0-14
- Notices :Copyright(c) 2019 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Added work order information input screen.
	- Modified LOG file location.
	- Added USB LOG storage functionality.

### PingTool v2.0.0-13
- Notices :Copyright(c) 2019 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Added setting field: MAC automatic matching function.
	- LOG file field changes.

### PingTool v2.0.0-12
- Notices :Copyright(c) 2019 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Resolved UI display issues.

### PingTool v2.0.0-11
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Some products restart halfway through the network card test due to Displaylink driver updates. The program has been optimized accordingly.

### PingTool v2.0.0-10
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Added VID for new product IA-UED6.
	- Changed packet size to 32 bytes.
	- Network speed test: Directly obtain system network speed, no longer check response time for large ping packets.

### PingTool v2.0.0-9
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Modified ZeroMAC functionality: virtual MAC is now a random code.

### PingTool v2.0.0-8
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Modified program for some WiFi network cards with different names causing the changes in version 2.0.0-7 to be ineffective.

### PingTool v2.0.0-7
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Program modification for Z270 motherboard WiFi network card issue.

### PingTool v2.0.0-6
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Added MAC, Network Card Name comparison function (double-click on the field in the test result window).

### PingTool v2.0.0-5
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- IP determination time changed to 20 seconds.

### PingTool v2.0.0-4
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Some computers on the 3F production line caused the program to become unresponsive
	. Program structure has been adjusted to reduce program resource consumption.
	- IP determination time canceled.
	- Program now clears the screen and returns to the initial screen when the network card is disconnected while obtaining IP.

### PingTool v2.0.0-3
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- For some products on the 4F production line, connection time is too long, IP determination time changed to 30 seconds.

### PingTool v2.0.0-2
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Fixed the issue of the program not responding on some computers.

### PingTool v2.0.0-1
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :	
	- Fixed the issue where checking the ZeroMAC option caused subsequent product MACs to be 00249B000000.

### PingTool v2.0.0
- Notices :Copyright(c) 2018 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :
	- Network speed test now only tests 1000M.
	- Added test: Check if the device is on USB3.0.
### PingTool v1.0.0
- Notices :Copyright(c) 2017 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :	
	- Network card ping test.

### PingTool UCD4
- Notices :Copyright(c) 2019 Leno
- Compiler :Embarcadero RAD Studio 2010 Version 14.0.3615.26342
- OS :Windows 8 ver6.3 Build 9600
- Desc :	
	- DS-UCD4 ping test.