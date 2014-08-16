#ifndef __RTOSPP_PROCESS_HPP
#define __RTOSPP_PROCESS_HPP

// Define TESTING if you want to do invariant checking
//#define TESTING

// Generate a nice warning so we don't forget to turn this off
#ifdef TESTING
	#warning RTOS++ TESTING enabled, expect slower message passing
#endif

#include "semaphore.hpp"

#include <task.h>

/**
 * FreeRTOS uses priorities between 0 and configMAX_PRIORITIES-1
 * where higher means more important.
 */
namespace Priorities {
	const uint8_t Idle      = tskIDLE_PRIORITY;
	const uint8_t Low       = 1;
	const uint8_t Medium    = 2;
	const uint8_t High      = 3;
	const uint8_t Driver    = 4;
	const uint8_t Interrupt = 5;
}

/// A rtos++ task
/**
 * This class wraps around all the FreeRTOS structures
 * and implements a mailbox mechanism.
 * To use this class make a function that returns void and
 * accepts an Process*. You can create a Process with this
 * function that passes itself to the function so you can
 * use the functions.
 */
class Process {
	// The FreeRTOS handle for this task
	xTaskHandle task_handle;

	// The start and end pointer of the queue
	Message * queue_start = NULL;
	Message * queue_end   = NULL;
#ifdef TESTING
	// The number of messages in the queue, this
	// is only used to check the queue invariant.
	uint32_t num_messages = 0;
#endif

	// If this process is waiting for a new message
	bool waiting = false;

	// The function pointer to the function we want to
	// run in this process
	void (*process_function)(Process*);

	// These functions are used to start the process
	static void runner( void * process );
	void run();

	/// Add a message to the message queue from another Process
	/**
	 * \param msg The message to send to this process
	 * \param timeout The maximum time to wait to complete the sending
	 *
	 * \return If the sending of the message was succesfull
	 */
	bool sendTo( Message * msg );

public:
	/// Send a message to from this process to another
	/**
	 * The pointer passed to this function gets set to NULL which this
	 * function achieves by taking a reference to the pointer to the Message.
	 * This function is a template because the C++ compiler cannot automaticly
	 * cast the message to a reference pointer which required a cast at every
	 * place this function was used, because of type inference this solution
	 * does not cause an error. More at:
	 * http://stackoverflow.com/questions/898955/reference-to-pointers-and-c-polymorphism
	 *
	 * \param to The Process to send this message to
	 * \param msg A reference to a pointer to the message pointer
	 * \param timeout How long to block to send this message
	 *
	 * \return If the sending was successful
	 */
	template<typename T>
	bool send(Process * to, T * & msg) {
		// Set the sender of the message
		msg->sender = this;

		bool response = to->sendTo(msg);

		// Set the variable in the process to NULL
		// just to be sure that they don't keep
		// working with this message.
		msg = NULL;

		return response;
	}

	// Don't allow copies of this object
	Process(const Process&)        = delete;
	void operator=(const Process&) = delete;

	/// Create a new process
	/**
	 * \param procFunc The function that this process executes
	 * \param processName The name of this process
	 * \param[optional] trace_identifer The identifier on the trace pins
	 * \param[optional] priority The priority of this process
	 * \param[optional] stackDepth The size of the stack for this process in bytes
	 */
	Process(
		void (*procFunc)(Process*),
		const portCHAR *processName,
		uint32_t trace_identifier       = 0,
		unsigned portBASE_TYPE priority = Priorities::Medium,
		unsigned portSHORT stackDepth   = configMINIMAL_STACK_SIZE
	);

	/// Receive a message for this process
	/**
	 * Return the next received message for this process.
	 * If there is no message in the queue wait for it.
	 * After receiving a message via this method the receiver
	 * has ownership over this message and is responsible for
	 * freeing up the memory or passing the message on.
	 * \param block If the process has to block to wait for a
	 *              message if there isn't any available.
	 */
	Message * receive(bool block = true);

#ifdef TESTING
	/// Check the validity of the queue pointers
	/**
	 * Check that there are num_messages messages in the
	 * queue and that the next pointer of each of those
	 * messages and the start queue and end queue pointer
	 * are correctly set.
	 */
	void assert_queue_invariant();
#endif

#if INCLUDE_vTaskDelay==1
	/// Delay the current process
	/**
	 * \param ticksToDelay The amount of ticks to wait
	 */
	inline void delay(const portTickType ticks_to_delay) const {
		vTaskDelay(ticks_to_delay);
	}
#endif

#if INCLUDE_vTaskDelayUntil==1
	/// Delay the current process to wake every x ticks
	/**
	 * \param ticksToDelay The amount of ticks since the previous wake
	 *                     to delay this task.
	 */
	inline void delay_until(const portTickType ticks_to_delay) const {
		static portTickType last_awake = 0;
		if( last_awake == 0 )
			last_awake = xTaskGetTickCount();
		vTaskDelayUntil(&last_awake, ticks_to_delay);
	}
#endif

#if INCLUDE_vTaskSuspend==1
	/// Resume this process
	inline void resume() {
		vTaskResume(task_handle);
	}

	/// Suspend this process
	inline void suspend() {
		vTaskSuspend(task_handle);
	}

#if INCLUDE_xTaskResumeFromISR==1
	/// Resume this process from a ISR
	/**
	 * \return If a context switch from the ISR is needed
	 */
	inline bool resume_from_isr() {
		return xTaskResumeFromISR(task_handle) == pdTRUE;
	}
#endif
#endif

#if INCLUDE_vTaskPrioritySet==1
	/// Set the priority of this task
	/**
	 * \param new_priority The new priority for this task
	 */
	inline void set_priority(unsigned portBASE_TYPE new_priority) {
		vTaskPrioritySet(task_handle, new_priority);
	}
#endif

#if INCLUDE_uxTaskPriorityGet==1
	/// Get the priority of this task
	/**
	 * \return The current priority of this process
	 */
	inline unsigned portBASE_TYPE get_priority() const {
		return uxTaskPriorityGet(task_handle);
	}
#endif

#if INCLUDE_eTaskGetState==1
	/// Get the state of this task
	inline eTaskState get_state() const {
		return eTaskGetState(task_handle);
	}
#endif

#if INCLUDE_pcTaskGetTaskName==1
	/// Get the name of this task
	/**
	 * \return The name of this task
	 */
	inline const char * get_name() const {
		return pcTaskGetTaskName(task_handle);
	}
#endif

#if INCLUDE_vTaskDelete==1
	/// Delete the task in the destructor
	~Process() {
		vTaskDelete(task_handle);
	}
#endif

#if configUSE_APPLICATION_TASK_TAG==1
	/// Set this tasks tag
	inline void set_tag(pdTASK_HOOK_CODE tag) {
		vTaskSetApplicationTaskTag(task_handle, tag);
	}

	/// Get this tasks tag
	inline pdTASK_HOOK_CODE get_tag() const {
		return xTaskGetApplicationTaskTag(task_handle);
	}

	/// Call the function pointer in this tasks tag
	inline portBASE_TYPE call_tag(void * parameter) {
		return xTaskCallApplicationTaskHook(task_handle, parameter);
	}
#endif

#if INCLUDE_uxTaskGetStackHighWaterMark==1
	/// Get info on how close the stack got to overflowing
	/**
	 * \return The amount of words that were still available
	 *         on the stack at the point that the most stack
	 *         of this task was allocated.
	 */
	inline unsigned portBASE_TYPE get_stack_high_water_mark() {
		return uxTaskGetStackHighWaterMark(task_handle);
	}
#endif

	/// Start the task scheduler
	static void start_scheduler();
};

#endif
