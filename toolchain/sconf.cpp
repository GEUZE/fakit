#include <sconf.h>
#include <sciopta.h>
extern "C" {
sc_module_cb_t * sc_modules[SC_MAX_MODULE];
#if SC_MAX_CONNECTOR > 0
sc_pcb_t * sc_connectors[SC_MAX_CONNECTOR];
#endif
}
