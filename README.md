# Arduino
A collection of Arduino sketches and libraries

## Sketches
###baton_BNO055
Utilize the BNO055 to compute gestural and pointing data and send over UDP. Also receives control commands over UDP.

## General Use Libraries
### Wire_custom
Fixes a hang bug in the base Arduino library.

### SD_Network
Save, access, and set networking configurations to an SD card

### Ethernet-NODNS
Clone of base Arduino Ethernet library, but with all DNS stripped. Saves LOTS of memory.

### serialCon
Paced and controllable serial communication. Provides an extendible base class for providing single character commands and multi-character data delivery. Provides several utility functions. 

### LEDCon, LEDMatrixCon, LEDPalettesCon
Extensions of serialCon to control individually addressable RGB LED matrices and strings over serial communication. 


PENDING: break into separate repos for the different libraries.
