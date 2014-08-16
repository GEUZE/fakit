#include <stdio.h>

#include <sciopta.h>
#include <ossys/errtxt.h>

#include <LPC177x_8x.h>

#include <sciopta++>

extern "C" void SystemInit();

char module_memory[30 * 1024];

void __constructors();

int errorHook(sc_errcode_t err, sc_extra_t iptr, int user, sc_pcb_t const * pcb) {
	// Store the error in non-volatile memory,
	// so the DAQ can append it to the log after rebooting.
	LPC_RTC->GPREG0 = 0xBAD00000 + (user ? 1 : 0);
	LPC_RTC->GPREG1 = err;
	LPC_RTC->GPREG2 = iptr;
	LPC_RTC->GPREG3 = (pcb->name[0] << 24) | (pcb->name[1] << 16) | (pcb->name[2] << 8) | pcb->name[3];
	LPC_RTC->GPREG4 = Time::now();

	printf("\033[1;31m[ FATAL SCIOPTA %s ERROR ]\033[m\n", user ? "USER" : "SYSTEM");
	printf("Error code: %08x\n", static_cast<unsigned int>(err));
	printf("Extra code: %08x\n", static_cast<unsigned int>(iptr));
	if (!user) {
		char const * function = func_txt[err>>24];
		char const * error    =  err_txt[(err>>12) & 0xfff];
		printf("Function: %s\nError: %s\n", function, error);
	}
	if (pcb) {
		printf("Process ID: %08x\n", static_cast<unsigned int>(pcb->pid));
		printf("Process: %s\n", pcb->name);
		printf("Source location: %s:%d\n", pcb->cfile, pcb->cline);
	}
	asm volatile ("bkpt");
	return 0;
}

extern "C" void sc_kerneld();

void defaultModule() {
	sc_procSchedLock();
	{
		// Define various maximum sizes of messages in the message pool
		static const sc_bufsize_t bufsizes[8]= {
			8, 12, 24, 32, 64, 96, 128, 512
		};
		// Create the (only) message pool
		sc_sysPoolCreate(
				0,          // start address
				0x2000,     // size
				8,          // #buffer sizes
				bufsizes,   // buffer sizes
				"default",  // name
				0x0         // module id
		);
	}
	// Create the init process
	sc_sysProcCreate(
		"sc_kerneld", // name
		sc_kerneld, // function
		1024, // function stack size
		0, // ticks
		4, // priority
		SC_PDB_STATE_RUN, // after creation, put this process in the 'ready' state
		0, // module ID
		0, // process control block
		0, // what's this? // FIXME
		SC_PROCPRIOCREATESTATIC // function code
	);
	__constructors();
	sc_procSchedUnlock();
	sc_procPrioSet(32);
	while (true);
}

void targetSetup() {
	SystemInit();
	sc_miscErrorHookRegister(errorHook);
	// Create the (only) module
	sc_sysModuleCreate(
		"ABC", // Name
		defaultModule, // Main function
		1024, // Main function stack size
		0, // Priority
		module_memory, // Start
		sizeof(module_memory), // Size
		0, // Init size
		1, // Pools
		32 // Processes
	);
}

