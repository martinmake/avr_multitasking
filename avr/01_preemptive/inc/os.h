#ifndef _MULTITASKING_OS_H_
#define _MULTITASKING_OS_H_

#include <inttypes.h>

#define OS_TICK_RATE    ((uint16_t) 1000              )
#define OS_STACK_SIZE   ((uint8_t ) 128               )
#define OS_STACK_END    ((uint8_t ) OS_STACK_SIZE - 1 )
#define OS_CONTEXT_SIZE ((uint8_t ) 32 + 3            )

typedef void (*thread_func)(void);

extern volatile uint16_t current_stack;

class NanoKernel
{
	private:
		typedef struct thread_control_block
		{
			uint16_t sp;
			uint8_t  stack[OS_STACK_SIZE];
		} THREAD_CONTROL_BLOCK;

		volatile uint16_t              current_thread;
		volatile THREAD_CONTROL_BLOCK* tcbs;
		uint16_t                       thread_count;

		bool m_context_switch_enabled;

	public:
		NanoKernel();
		~NanoKernel();

		void operator<<(thread_func func);
		void start();

		inline void save_stack_ptr(void) { tcbs[current_thread].sp = current_stack;              }
		inline void rotate_threads(void) { current_thread = (current_thread + 1) % thread_count; }
		inline void load_stack_ptr(void) { current_stack = tcbs[current_thread].sp;              }

		inline void atomic_start(void) { m_context_switch_enabled = false;  }
		inline void atomic_end(void)   { m_context_switch_enabled = true;   }
		inline bool is_atomic(void)    { return !m_context_switch_enabled;  }

	private:
		inline void resize_tcb_arr(void);
};

extern NanoKernel os;

#endif
