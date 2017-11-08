//
//  baton_base.h
//  
//
//  Created by Daniel Reeves on 7/15/15.
//
//

#ifndef _baton_base_h
#define _baton_base_h


//  SERIAL_DEBUG - format serial output (not sure if it works to remove this at this point)
#define SERIAL_OUTPUT


//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port
#define  SERIAL_PORT_SPEED  115200

//  Use microsd card if set, otherwise, use eeprom
#define  NETSETTINGS_ON_CARD


// EXPORT_TO_ETHERNET - send data over udp
#define EXPORT_TO_ETHERNET

// outgoing packet size
#define UDP_OUT_PACKET_SIZE 48

// EXPORT_MULTIPLE 
#define EXPORT_ACC_MULTIPLE 10.0
#define EXPORT_ANGLE_MULTIPLE 1000.0

// Locaiton of netsettings on eeprom (if using eeprom)
#define NETSETTINGS_EEPROM_OFFSET 0

#endif
