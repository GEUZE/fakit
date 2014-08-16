#ifndef __RTOSPP_TIMER_HPP
#define __RTOSPP_TIMER_HPP

#if configUSE_TIMERS != 1
	#error RTOS++ needs timers to be enabled to provide the timer class
#endif

#include <cstdio>

#include <timers.h>

/// A rtos++ timer
/**
 * This class wraps around the FreeRTOS software timer
 * functionality.
 */
class Timer {
	// The handle for the FreeRTOS timer
	xTimerHandle timer_handle;

	// The message to send back when the timer expires
	Message * message;
	// The process to send the message to when the timer expires
	Process * process;

	/// The callback function
	/**
	 * This functions gets called from the FreeRTOS timer process
	 * when the timer expires. In the callback a message to the
	 * original process is then sent.
	 */
	static void callback( xTimerHandle expired_timer ) {
		Timer * timer_object = reinterpret_cast<Timer *>(pvTimerGetTimerID(expired_timer));

		// The Process::send function sets the original message * to NULL,
		// use this ugly hack to fix that.
		Message * tmp = timer_object->message;
		timer_object->process->send( timer_object->process, tmp );
	}

public:
	// Don't allow copies of this object
	Timer(const Timer&)          = delete;
	void operator=(const Timer&) = delete;

	/// Create a new timer
	/**
	 * \param name A name that could be used later to identify this timer.
	 * \param tick_to_wait The number of ticks to wait before triggering this timer.
	 * \param auto_repeat If this timer automatically starts again.
	 */
	Timer( const char * name, unsigned int ticks_to_wait, bool auto_repeat=false) {
		timer_handle = xTimerCreate(
			(const signed char *) name,
			ticks_to_wait,
			auto_repeat ? pdTRUE : pdFALSE,
			reinterpret_cast<void *>(this),
			&callback
		);

		configASSERT( timer_handle != NULL );
	}

	/// Delete this timer
	~Timer() {
		portBASE_TYPE tmp = xTimerDelete(timer_handle,0);
		configASSERT( tmp == pdPASS);
		// Suppress unused variable warning for tmp
		(void) tmp;
	}

	/// Set the parameters for this timer without starting the timer
	/**
	 * Set the process to wake and the message to send without starting
	 * the actual timer. The reset function can be called on this timer
	 * after this function has set up the timer.
	 * \param process_to_wake The process to wake when this timer expires.
	 * \param message_to_send The message to send when this timer expires.
	 */
	inline void set_parameters( Process * process_to_wake, Message * message_to_send ) {
		process = process_to_wake;
		message = message_to_send;
	}

	/// Start this timer
	/**
	 * \param process_to_wake The process to wake when this timer expires.
	 * \param message_to_send The message to send when the timer expires.
	 * \param[optional] block_time The number of ticks to wait for the timer queue to unblock.
	 */
	inline void start(Process * process_to_wake, Message * message_to_send, unsigned int block_time = 0) {
		process = process_to_wake;
		message = message_to_send;

		portBASE_TYPE tmp = xTimerStart(timer_handle, block_time);
		configASSERT( tmp == pdPASS );
		// Suppress unused variable warning for tmp
		(void) tmp;
	}

	/// Stop this timer
	/**
	 * \param[optional] block_time The number of ticks to wait for the timer queue to unblock.
	 */
	inline void stop(unsigned int block_time = 0) {
		portBASE_TYPE tmp = xTimerStop(timer_handle, block_time);
		configASSERT( tmp == pdPASS );
		// Suppress unused variable warning for tmp
		(void) tmp;
	}

	/// Reset this timer
	/**
	 * \param[optional] process_to_wake The process to wake when this timer expires.
	 * \param[optional] message_to_send The message to send when the timer expires.
	 * \param[optional] block_time The number of ticks to wait for the timer queue to unblock.
	 */
	inline void reset(Process * process_to_wake = NULL, Message * message_to_send = NULL, unsigned int block_time = 0) {
		if( process_to_wake != NULL ) {
			process = process_to_wake;
		}
		if( message_to_send != NULL ) {
			message = message_to_send;
		}

		portBASE_TYPE tmp = xTimerReset(timer_handle, block_time);
		configASSERT( tmp == pdPASS );
		// Suppress unused variable warning for tmp
		(void) tmp;
	}

	/// Change the period of this timer
	/**
	 * \param new_ticks_to_wait The new number of ticks to wait before expiring.
	 * \param[optional] block_time The number of ticks to wait for the timer queue to unblock.
	 */
	inline void change_period(unsigned int new_ticks_to_wait, unsigned int block_time = 0) {
		portBASE_TYPE tmp = xTimerChangePeriod( timer_handle, new_ticks_to_wait, block_time );
		configASSERT( tmp == pdPASS );
		// Suppress unused variable warning for tmp
		(void) tmp;
	}

	/// Return if this timer is currently active
	/**
	 * \return If this timer is currently counting.
	 */
	inline bool is_active() {
		return xTimerIsTimerActive(timer_handle) != pdFALSE;
	}
};

#endif
