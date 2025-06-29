#include "modbusRegBank.h"
#include <stdlib.h>

modbusRegBank::modbusRegBank(void)
{
	_digRegs		= 0;
	_lastDigReg		= 0;
	_anaRegs		= 0;
	_lastAnaReg		= 0;
}


void modbusRegBank::add(word addr)
{
	if(addr<20000)
	{
		modbusDigReg *temp;

		temp = (modbusDigReg *) malloc(sizeof(modbusDigReg));
		temp->address = addr;
		temp->value		= 0;
		temp->next		= 0;

		if(_digRegs == 0)
		{
			_digRegs = temp;
			_lastDigReg = _digRegs;
		}
		else
		{
			//Assign the last register's next pointer to temp;
			_lastDigReg->next = temp;
			//then make temp the last register in the list.
			_lastDigReg = temp;
		}
	}	
	else
	{
		modbusAnaReg *temp;

		temp = (modbusAnaReg *) malloc(sizeof(modbusAnaReg));
		temp->address = addr;
		temp->value = 0;
		temp->next = 0;

		if(_anaRegs == 0)
		{
			_anaRegs = temp;
			_lastAnaReg = _anaRegs;
		}
		else
		{
			_lastAnaReg->next = temp;
			_lastAnaReg = temp;
		}
	}
}

word modbusRegBank::get(word addr)
{
	if(addr < 20000)
	{
		modbusDigReg * regPtr;
		regPtr = (modbusDigReg *) this->search(addr);
		if(regPtr)
			return(regPtr->value);
		else
			return(NULL);	
	}
	else
	{
		modbusAnaReg * regPtr;
		regPtr = (modbusAnaReg *) this->search(addr);
		if(regPtr)
			return(regPtr->value);
		else
			return(NULL);	
	}
}

void modbusRegBank::set(word addr, word value)
{
	//for digital data
	if(addr < 20000)
	{
		modbusDigReg * regPtr;
		//search for the register address
		regPtr = (modbusDigReg *) this->search(addr);
		//if a pointer was returned the set the register value to true if value is non zero
		if(regPtr)
			if(value)
				regPtr->value = 0xFF;
			else
				regPtr->value = 0x00;
	}
	else
	{
		modbusAnaReg * regPtr;
		//search for the register address
		regPtr = (modbusAnaReg *) this->search(addr);
		//if found then assign the register value to the new value.
		if(regPtr)
			regPtr->value = value;
	}
}

void * modbusRegBank::search(word addr)
{
	//if the requested address is 0-19999 
	//use a digital register pointer assigned to the first digital register
	//else use a analog register pointer assigned the first analog register

	if(addr < 20000)
	{
		modbusDigReg *regPtr = _digRegs;

		//if there is no register configured, bail
		if(regPtr == 0)
			return(0);

		//scan through the linked list until the end of the list or the register is found.
		//return the pointer.
		do
		{
			if(regPtr->address == addr)
				return(regPtr);
			regPtr = regPtr->next;
		}
		while(regPtr);
	}
	else
	{
		modbusAnaReg *regPtr = _anaRegs;

		//if there is no register configured, bail
		if(regPtr == 0)
			return(0);

		//scan through the linked list until the end of the list or the register is found.
		//return the pointer.
		do
		{
			if(regPtr->address == addr)
				return(regPtr);
			regPtr = regPtr->next;
		}
		while(regPtr);
	}
	return(0);
}


