#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <standard/standard.h>
#include <usart/usart.h>

#include "os.h"
#include "led.h"

Led led_err(Pin({PORTB, PB5}));
Led led_a(  Pin({PORTB, PB4}));
Led led_b(  Pin({PORTB, PB3}));
Led led_c(  Pin({PORTB, PB2}));

void thread_a(void)
{
	while (1) {
		Usart::sendc('A');
		led_a = 1;
		_delay_ms(100);
		led_a = 0;
		_delay_ms(100);
	}
}

void thread_b(void)
{
	while (1) {
		Usart::sendc('B');
		led_b = 1;
		_delay_ms(500);
		led_b = 0;
		_delay_ms(500);
	}
}

void thread_c(void)
{
	while (1) {
		Usart::sendc('C');
		led_c = 1;
		_delay_ms(1000);
		led_c = 0;
		_delay_ms(1000);
	}
}

inline void init(void)
{
	Usart::begin(TIO_BAUD, F_CPU);
	Usart::sendf(25, "CLOCK SPEED: %uMHz\n",  F_CPU / 1000000);
	Usart::sendf(25, "UART  SPEED: %uHz\n\n", TIO_BAUD       );

	os << thread_a;
	os << thread_b;
	os << thread_c;

	os.start();
}

int main(void)
{
	init();
}
