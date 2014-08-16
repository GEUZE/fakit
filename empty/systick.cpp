#include <sciopta++>
#include <LPC177x_8x.h>
#include <system_LPC177x_8x.h>

#ifndef CNF_TICKLENGTH
#define CNF_TICKLENGTH 1000
#endif

void SCI_sysTick(int src) {
	if (src == 0) {
		sc_tick();
	} else if (src == -1) {
		SysTick_Config(SystemCoreClock / CNF_TICKLENGTH);
		NVIC_SetPriority(SysTick_IRQn, 0xFF);
		sc_tickLength(CNF_TICKLENGTH);
	}
}

Process sysTick = Process::createStaticInterrupt(
	"systick",
	SCI_sysTick,
	1024,
	15
);

