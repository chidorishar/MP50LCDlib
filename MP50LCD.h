#ifndef _MP50LCDLIB_
#define _MP50LCDLIB_

#include <Arduino.h>


class MP50LCD
{
private:	
	byte dataPin;
	byte clockPin;
	byte latchPin;
	char descBlocks[14] = {0};
	char getSymbolCode(const char & ch, bool hasPoint = false);
	inline void ClearBlocksData()
	{
		for (byte i = 0; i < 13; i++)
			descBlocks[i] = 0;
	}
	inline void registerTick();
	inline void registerTack();
	void UpdateDisplay();
public:
	//display blocks descriptors
	enum {B = 1, PNT = 2, BL = 4, BR = 8, UL = 16, U = 32, M = 64, UR = 128};

	MP50LCD(byte dataP, byte clockP, byte latchP);
	void operator<< (String s);
	inline void operator<< (const char* s)
	{
		operator<<(String(s));
	}
};

#endif