#include <sciopta.h>

#ifndef PRIORITY_HPP
#define PRIORITY_HPP

class Priority {
	
	protected:
		sc_prio_t priority;
		
	public:
		Priority(sc_prio_t priority) : priority(priority) {}
		
		operator sc_prio_t () const {
			return priority;
		}
		
};

#endif
