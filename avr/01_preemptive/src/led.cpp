#include "led.h"

Led::Led(Pin led)
	: m_port(led.port)
{
	led.dd.set();
}

Led::~Led()
{
}

Led& Led::operator=(uint8_t state)
{
	if (state)
		m_port.set();
	else
		m_port.clear();

	return *this;
}
