#include <sciopta.h>

#ifndef __PROCESSNAME_HPP
#define __PROCESSNAME_HPP

/// The name of a process.
/**
 * Includes the target and module name.
 */
class ProcessName {
	
	public:
		char  target[SC_MODULE_NAME_SIZE + 1];
		char  module[SC_MODULE_NAME_SIZE + 1];
		char process[SC_PROC_NAME_SIZE   + 1];
		
	protected:
		ProcessName(sc_msg_t p) {
			sc_procNameGetMsgReply_t * n = reinterpret_cast<sc_procNameGetMsgReply_t *>(p);
			for(unsigned int i = 0; i <= SC_MODULE_NAME_SIZE; i++) if (!(target [i] = n->target [i])) break;
			for(unsigned int i = 0; i <= SC_MODULE_NAME_SIZE; i++) if (!(module [i] = n->module [i])) break;
			for(unsigned int i = 0; i <= SC_PROC_NAME_SIZE  ; i++) if (!(process[i] = n->process[i])) break;
			sc_msgFree(&p); //TODO: Use Message instead.
		}
		
		friend class Process;
		
};

#endif
