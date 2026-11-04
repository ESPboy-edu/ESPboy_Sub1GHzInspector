# CC1101 debug service tool v1.0.1 pre-release for SmartRC-CC1101-Driver-Lib v.2.5.1

Note: if you have any suggestions, feel free to share them under "issues".

---------------------------------------------
Description
---------------------------------------------
With this tool, you can register entries of the CC1101 during operation
read and change. You can switch to send or receive.
Set the transmission strength and change or call up the frequency.
And so on...
Or you use advanced futures only for your sketch.


For: https://github.com/LSatan/SmartRC-CC1101-Driver-Lib


---------------------------------------------
Instructions and commands for service only!
(advanced features)
---------------------------------------------
add at the beginning of your sketch:

#include <cc1101_debug_service.h>

add in the setup of your sketch(all optional):

cc1101_debug.setChsp(250.00);	//Set cannel spacing from min 25.39khz to max 405.45khz directly like SmartRF Studio.

cc1101_debug.setRxbw(200.00);	//Set receive bandwidth from min 58.03khz to max 812.5khz directly like SmartRF Studio.

cc1101_debug.setDRate(100.00);	//Set Data rate from min 0.02kBaud to max 1621.83kBaud directly like SmartRF Studio.

cc1101_debug.setDeviation(47.00);	//Set Deviation from min 1.58khz to max 380.85khz directly like SmartRF Studio.

cc1101_debug.load_slot(0);		//loads register entries previously saved in the eeprom. For example, they were created with debug / service. There are 6 slots from 0 to 5.

cc1101_debug.save_slot(0);		//save the register entries to eeprom to slot0. There are 6 slots from 0 to 5. Note: not useful here!


Note:		This service function uses the least storage space.

---------------------------------------------
Preview service (advanced features)
---------------------------------------------

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/install_example_service.png"/>

---------------------------------------------
Commands for debug/service
---------------------------------------------
Enter in the serial monitor:


reg 		 (Shows all settings of the register.)

ver 		 (Displays the cc1101 version number.)

tx 		 (puts the cc1101 in transmit mode.)

rx 		 (puts the cc1101 in receive mode.)

pa=10 		 (sets the transmission strength to the set value.)

mhz? 		 (shows the current frequency and the associated register entries.)

mhz=433.92 	 (sets the specified frequency and then displays it.)

rxbw?		 (shows the current receive bandwidth and the associated register entries.)

rxbw=200.00	 (sets the specified receive bandwidth and then displays it.)

drate?		 (shows the current Data rate and the associated register entries.)

drate=100.00	 (sets the specified Data rate and then displays it.)

chsp?		 (shows the current channel spacing and the associated register entries.)

chsp=250.00	 (sets the specified channel spacing and then displays it.)

chan?		 (shows the current channel in decimal.)

chan=5		 (sets the specified channel in decimal.)

dev?		 (shows the current Deviation and the associated register entries.)

dev=10.00	 (sets the specified Deviation and then displays it.)

save?		 (shows free and used memory slots from the eeprom.)

save=0		 (saves the current entries on the selected slot in the eeprom. Slot 0 - 5.)

load=0		 (loads stored entries from the selected slot in the eeprom. Slot 0 - 5.)


Register entry +? Example:

MDMCFG0? 	 (shows the current value for this entry.)

Register entry + hex value Example:

MDMCFG0=FF 	 (sets the hex value FF for this entry.)


---------------------------------------------
add instructions debug / service
---------------------------------------------
CC1101 debug service tool for SmartRC-CC1101-Driver-Lib from version V2.3.3 and newer.

add at the beginning of your sketch:

#include <cc1101_debug_service.h>

add at the beginning of the loop:

cc1101_debug.debug ();


Finished!


Note:		This debug / service function uses the most storage space.

---------------------------------------------
Preview debug / service
---------------------------------------------

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/install_example.png"/>

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/start_example.png"/>

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/commands_example.png"/>
