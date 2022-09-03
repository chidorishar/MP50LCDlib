#include "MP50LCD.h"

MP50LCD::MP50LCD(byte dataP, byte clockP, byte latchP) : dataPin(dataP), clockPin(clockP), latchPin(latchP)
{	
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	digitalWrite(clockPin, LOW);
	digitalWrite(latchPin, LOW);
	descBlocks[13] = -128;
}

char MP50LCD::getSymbolCode(const char & ch, bool hasPoint)
{
	char res = 0;

	switch (ch)
	{
	case '0':
		res = -67;
		break;
	case '1':
		res = -120;
		break;
	case '2':
		res = -27;
		break;
	case '3':
		res = -23;
		break;
	case '4':
		res = -40;
		break;
	case '5':
		res = 121;
		break;
	case '6':
		res = 125;
		break;
	case '7':
		res = -88;
		break;
	case '8':
		res = -3;
		break;
	case '9':
		res = -7;
		break;
	case 'A':
		res = U | UL | UR | M | BL | BR;
		break;
	case 'D':
		res = -67;
		break;
	case 'E':
		res = 117;
		break;
	case 'L':
		res = 21;
		break;
	case 'I':
		res = UR | BR;
		break;
	case 'H':
		res = UL | UR | M | BL | BR;
		break;
	case 'O':
		res = -67;
		break;
	case 'P':
		res = U | UL | UR | M | BL;
		break;
	case 'R':
		res = BL | M;
		break;
	case 'S':
		res = U | UL | M | BR | B;
		break;
	case '_':
		res = B;
		break;
	case '-':
		res = M;
		break;
	case '[':
		res = U | UL | BL | B;
		break;
	case ']':
		res = U | UR | BR | B;
		break;
	}
	if (hasPoint)
		res |= 2;

	return res;
}

void MP50LCD::operator<< (String s)
{
	ClearBlocksData();
	//setup output data
	if (s.charAt(0) == '-' || s.charAt(0) == '+')
	{
		if(s.charAt(0) == '-')
			descBlocks[10] = 2;
		s.remove(0, 1);
	}
	byte len = s.length() >= 10 ? 10 : s.length(); //can display only 10 symbols on main line
	s.toUpperCase(); //can display only capital 
	for (byte i = 0; i < len; i++)
	{
		if(s.charAt(0) == '.')
			descBlocks[len - 1 - i] = getSymbolCode(' ', true);
		else if (s.charAt(1) == '.')
		{
			descBlocks[len - 1 - i] = getSymbolCode(s.charAt(0), true);
			s.remove(1, 1);
		}
		else
			descBlocks[len - 1 - i] = getSymbolCode(s.charAt(0));
		s.remove(0, 1);
	}
	UpdateDisplay();
}

void MP50LCD::registerTick()
{
	digitalWrite(clockPin, LOW);
	delayMicroseconds(250);
}

void MP50LCD::registerTack()
{
	delayMicroseconds(250);
	digitalWrite(clockPin, HIGH);
	delayMicroseconds(250);
}

//output data to display
void MP50LCD::UpdateDisplay()
{	
	digitalWrite(latchPin, HIGH);
	delay(1);
	for (byte b = 0; b < 13; b++) //blocks enumeration
	{
		if (b == 6) //specified middle bits
		{
			for (byte t = 0; t < 12; t++)
			{
				registerTick();
				if (t < 4)
					digitalWrite(dataPin, bitRead(descBlocks[6], t));
				else if (t < 8 && t >= 4)
					digitalWrite(dataPin, LOW);
				else
					digitalWrite(dataPin, bitRead(descBlocks[6], t - 4));
				registerTack();
			}
			continue;
		}
		for (byte s = 0; s < 8; s++) //segments enumeration
		{
			registerTick();
			digitalWrite(dataPin, bitRead(descBlocks[b], s));
			registerTack();
		}
		if (b == 12) //specified end bits
			for (byte t = 4; t < 8; t++)
			{
				registerTick();
				digitalWrite(dataPin, bitRead(descBlocks[13], t));
				registerTack();
			}
	}
	digitalWrite(latchPin, LOW);
	delay(4);
	digitalWrite(latchPin, HIGH);
}