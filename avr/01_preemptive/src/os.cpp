#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include <usart/usart.h>

#include "os.h"
#include "led.h"

#define save_context() \
	asm volatile( \
			"push	r0                    \n\t" \
			"in  	r0, __SREG__          \n\t" \
			"push	r0                    \n\t" \
			"push	r1                    \n\t" \
			"push	r2                    \n\t" \
			"push	r3                    \n\t" \
			"push	r4                    \n\t" \
			"push	r5                    \n\t" \
			"push	r6                    \n\t" \
			"push	r7                    \n\t" \
			"push	r8                    \n\t" \
			"push	r9                    \n\t" \
			"push	r10                   \n\t" \
			"push	r11                   \n\t" \
			"push	r12                   \n\t" \
			"push	r13                   \n\t" \
			"push	r14                   \n\t" \
			"push	r15                   \n\t" \
			"push	r16                   \n\t" \
			"push	r17                   \n\t" \
			"push	r18                   \n\t" \
			"push	r19                   \n\t" \
			"push	r20                   \n\t" \
			"push	r21                   \n\t" \
			"push	r22                   \n\t" \
			"push	r23                   \n\t" \
			"push	r24                   \n\t" \
			"push	r25                   \n\t" \
			"push	r26                   \n\t" \
			"push	r27                   \n\t" \
			"push	r28                   \n\t" \
			"push	r29                   \n\t" \
			"push	r30                   \n\t" \
			"push	r31                   \n\t" \
			"in	r28, __SP_L__         \n\t" \
			"in	r29, __SP_H__         \n\t" \
			"sts	current_stack+0, r28  \n\t" \
			"sts	current_stack+1, r29  \n\t" \
			);

#define restore_context() \
	asm volatile( \
			"lds	r28, current_stack+0  \n\t" \
			"lds	r29, current_stack+1  \n\t" \
			"out	__SP_L__, r28         \n\t" \
			"out	__SP_H__, r29         \n\t" \
			"pop	r31                   \n\t" \
			"pop	r30                   \n\t" \
			"pop	r29                   \n\t" \
			"pop	r28                   \n\t" \
			"pop	r27                   \n\t" \
			"pop	r26                   \n\t" \
			"pop	r25                   \n\t" \
			"pop	r24                   \n\t" \
			"pop	r23                   \n\t" \
			"pop	r22                   \n\t" \
			"pop	r21                   \n\t" \
			"pop	r20                   \n\t" \
			"pop	r19                   \n\t" \
			"pop	r18                   \n\t" \
			"pop	r17                   \n\t" \
			"pop	r16                   \n\t" \
			"pop	r15                   \n\t" \
			"pop	r14                   \n\t" \
			"pop	r13                   \n\t" \
			"pop	r12                   \n\t" \
			"pop	r11                   \n\t" \
			"pop	r10                   \n\t" \
			"pop	r9                    \n\t" \
			"pop	r8                    \n\t" \
			"pop	r7                    \n\t" \
			"pop	r6                    \n\t" \
			"pop	r5                    \n\t" \
			"pop	r4                    \n\t" \
			"pop	r3                    \n\t" \
			"pop	r2                    \n\t" \
			"pop	r1                    \n\t" \
			"pop	r0                    \n\t" \
			"out	__SREG__, r0          \n\t" \
			"pop	r0                    \n\t" \
			"reti                         \n\t" \
			);

volatile uint16_t current_stack;

NanoKernel os;

NanoKernel::NanoKernel()
{
	atomic_start();

	current_stack  = 0;
	current_thread = 0;
	thread_count   = 0;

	TCCR2A = (1 << WGM21);                            // CTC
	TCCR2B = (1 << CS22) | (0 << CS21) | (1 << CS20); // F_CPU/128
	OCR2A = F_CPU / 128 / OS_TICK_RATE - 1;
	TIMSK2 = (1 << OCIE1A);
}

NanoKernel::~NanoKernel()
{
	led_err = 1;
}

void NanoKernel::start()
{
	current_thread = 0;
	current_stack = tcbs[current_thread].sp;

	atomic_end();
	restore_context();
}

void NanoKernel::operator<<(thread_func func)
{
	current_thread = thread_count;
	thread_count++;

	resize_tcb_arr();

	// clear stack
	memset((void*) tcbs[current_thread].stack, 0, OS_STACK_SIZE * sizeof(uint8_t));

	// set stack pointer
	tcbs[current_thread].sp = (uint16_t) &tcbs[current_thread].stack[OS_STACK_END - OS_CONTEXT_SIZE];

	// set program counter
	tcbs[current_thread].stack[OS_STACK_END - 0] = (uint16_t) func;
	tcbs[current_thread].stack[OS_STACK_END - 1] = (uint16_t) func >> 8;

	Usart::sendf(30, "THREAD ADDED (%d)\n", thread_count             );
	Usart::sendf(30, " - FUNC ADDR: %p\n",  func                     );
	Usart::sendf(30, " - STACK PTR: %p\n",  tcbs[thread_count - 1].sp);
}

inline void NanoKernel::resize_tcb_arr()
{
	volatile THREAD_CONTROL_BLOCK* new_arr = new THREAD_CONTROL_BLOCK[thread_count];
	memcpy((void*) new_arr, (void*) tcbs, (thread_count - 1) * sizeof(THREAD_CONTROL_BLOCK));

	delete[] tcbs;
	tcbs = new_arr;
}

ISR(TIMER2_COMPA_vect, ISR_NAKED)
{
	// push context on the stack
	save_context();

	if (!os.is_atomic()) {
		os.save_stack_ptr();
		os.rotate_threads();
		os.load_stack_ptr();
	}

	// pull context from the restored stack
	restore_context();
}
