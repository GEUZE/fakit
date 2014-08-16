#include <stdlib.h>
extern "C" void * __dso_handle;
void * __dso_handle = 0;
void * operator new (size_t size) { return malloc(size); }
void operator delete (void * mem) { free(mem); }
