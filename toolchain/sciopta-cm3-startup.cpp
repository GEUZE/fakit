#include <sciopta.h>
#include <ossys/errtxt.h>
#include <system_LPC177x_8x.h>
#include <stdio.h>

typedef void (*FunctionPointer)();

extern "C" FunctionPointer __init_array;
extern "C" FunctionPointer __init_array_end;

void targetSetup();

void __constructors() {
	for (FunctionPointer * x = &__init_array; x != &__init_array_end; ++x) (*x)();
}

extern "C" void TargetSetup() {
	targetSetup();
}

