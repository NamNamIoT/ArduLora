#include <stdint.h>
#ifndef _MODBUSPROTOCOL
#define _MODBUSPROTOCOL

//Maximum device list for network
#define DEVMAX		10
//Maximum control register que size
#define QUEMAX		10
//Maximum serial wait in micro seconds
#define SERIALMAXDELAY	100
#define SERIALBAUD		9600
//the total silence time needed to signify an EOM or SOM in RTU mode

//Modbus function codes
#define READ_DO 	0x01
#define READ_DI		0x02
#define READ_AO 	0x03
#define READ_AI		0x04

#define WRITE_DO	0x05
#define WRITE_AO	0x06

#define RTU 		0x01
#define ASCII		0x02

#define MASTER		0x01
#define SLAVE		0x02

#define DO			0x00
#define DI			0x01
#define AI			0x03
#define AO			0x04

#endif