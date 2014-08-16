#include <sciopta.h>

#ifndef __SPP_TICKS_HPP
#define __SPP_TICKS_HPP

/// A point in time.
class Time {
	
	protected:
		sc_time_t time;
		
	public:
		Time(sc_time_t time = 0) : time(time) {}
		
		operator sc_time_t () const {
			return time;
		}
		
		static Time now() {
			return sc_tickGet();
		}
		
};

/// A number of (sciopta) ticks.
class Ticks {
	
	protected:
		sc_ticks_t ticks;
		
	public:
		Ticks(sc_ticks_t ticks) : ticks(ticks) {}
		
		operator sc_ticks_t () const {
			return ticks;
		}
		
		/// Sleep for the specified time.
		static void sleep(Ticks ticks) {
			sc_sleep(ticks);
		}
		
		/// Advance the kernel time with one tick.
		static void tick() {
			sc_tick();
		}
		
		/// Get the length of a tick, in microseconds.
		static unsigned int tickLength() {
			return sc_tickLength(0);
		}
		
		/// Set the length of a tick, in microseconds.
		static void setTickLength(unsigned int length) {
			sc_tickLength(length);
		}
		
		static sc_ticks_t const fatal_on_timeout = SC_FATAL_IF_TMO;
		static sc_ticks_t const  endless_timeout = SC_ENDLESS_TMO;
		
};

/// A number of Milliseconds.
/**
 * The useful thing of using this class is that it'll be automatically
 * converted to Ticks (using the current tick length) when needed.
 */
class Milliseconds {
	
	protected:
		unsigned int milliseconds;
		
	public:
		Milliseconds(unsigned int milliseconds) : milliseconds(milliseconds) {}
		
		operator unsigned int () const {
			return milliseconds;
		}
		
		Milliseconds(Ticks ticks) : milliseconds(sc_tickTick2Ms(ticks)) {};
		
		operator Ticks() {
			return sc_tickMs2Tick(milliseconds);
		}
		
};

#endif
