#include <stdint.h>
#include <Arduino.h>

#ifndef _MODBUSREGBANK
#define _MODBUSREGBANK

#include "modbus.h"
//#include <Wprogram.h>


struct modbusDigReg
{
	word address;
	byte value;

	modbusDigReg *next;
};

struct modbusAnaReg
{
	word address;
	word value;

	modbusAnaReg *next;
};

class modbusRegBank
{
	public:

		modbusRegBank(void);
		
		void add(word);
		word get(word);
		void set(word, word);
				
	private:
		void * search(word);
		
		modbusDigReg	*_digRegs,
						*_lastDigReg;
							
		modbusAnaReg	*_anaRegs,
						*_lastAnaReg;
};
#endif
