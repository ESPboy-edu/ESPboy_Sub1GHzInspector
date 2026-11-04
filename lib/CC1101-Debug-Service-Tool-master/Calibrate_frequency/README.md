# CC1101 Calibrate_frequency.ino


---------------------------------------------
Description
---------------------------------------------
Tool for calibrating the frequency bands.
With some cc1101 versions, the set frequency differs from the actual frequency. In this case, you can use frequency calibration. The frequency is then adjusted with every change. Please note that slight deviations are possible despite calibration.

For: https://github.com/LSatan/SmartRC-CC1101-Driver-Lib

---------------------------------------------
Instructions
---------------------------------------------
Example for 315 mhz band.

Enter the frequency band in the serial monitor that you want to calibrate. In the example 1 for 315 MHz. 

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl1.png"/>

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl2.png"/>


The cc1101 now starts to send a click..click .. signal for 2 seconds every 4 seconds. so we're looking at 300 MHz in the SDR.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl3.png"/>

Now we coordinate the signal.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl4.png"/>

And enter the determined signal in the serial monitor.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl5.png"/>

After entering, the transmission frequency changes.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl6.png"/>

Now we go back to our SDR program and set it to 348MHz.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl7.png"/>

Adjust the signal again.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl8.png"/>

And enter the determined signal in the serial monitor.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl9.png"/>

Now the offset has been determined and you can copy the marked line into your sketches in the setup area.

<img src="https://github.com/LSatan/CC1101-Debug-Service-Tool/blob/master/img/cl10.png"/>

Now you can start the process for other frequency bands.
