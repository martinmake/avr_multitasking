#ifndef _MULTITASKING_PREEMPTIVE_LED_H_
#define _MULTITASKING_PREEMPTIVE_LED_H_

#include <standard/standard.h>

class Led
{
	private:
		Bit m_port;

	public:
		Led(Pin led);
		~Led();

		Led& operator=(uint8_t state);
};

extern Led led_err;

#endif
