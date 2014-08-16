/**
 * Some code of the rtos++ processes are shared between
 * all tasks and are to big to inline each time. This
 * file should be compiled with every project using rtos++.
 * The block comments describing a function are in the
 * process.hpp header file.
 *
 * @author Harmjan Treep <mail@harmjantreep.nl>
 */

#include <rtos++>

bool Process::sendTo( Message * msg ) {
#ifdef TESTING
	configASSERT( msg->next == NULL );
	// Make sure msg isn't in the queue already
	Message * current = queue_start;
	for( uint32_t i=0; i<num_messages; ++i ) {
		configASSERT( current != msg );
		current = current->next;
	}
	configASSERT( current == NULL );
	assert_queue_invariant();
#endif

	// If this is the first message in the queue
	if( queue_start == NULL ) {
		queue_start = msg;
	} else { // Else add this message to the end of the queue
		queue_end->next = msg;
	}
	queue_end = msg;

#ifdef TESTING
	++num_messages;

	assert_queue_invariant();
#endif

	if( waiting ) {
		// If the task is waiting for a message it is suspended.
		// Resume the task preferably without task switching.
		this->resume_from_isr();
	}

	return true;
}

void Process::runner( void * process ) {
	((Process*) process)->run();
}

void Process::run() {
	(*process_function)(this);
}

Process::Process(
	void (*procFunc)(Process*),
	const portCHAR *processName,
	uint32_t trace_identifier,
	unsigned portBASE_TYPE priority,
	unsigned portSHORT stackDepth
)
{
	// Create the task in FreeRTOS
	xTaskCreate(
		&Process::runner,
		(const signed char*) processName,
		stackDepth,
		this,
		priority,
		&task_handle
	);

	// Set the tag to the trace identifier
	vTaskSetApplicationTaskTag( task_handle, (pdTASK_HOOK_CODE) trace_identifier );

	// Store function to call when this process starts
	process_function = procFunc;

#ifdef TESTING
	assert_queue_invariant();
#endif
}

Message * Process::receive(bool block) {
#ifdef TESTING
	assert_queue_invariant();
#endif

	// If there is no message in the queue suspend this task
	if( queue_start == NULL ) {
		// If we don't want to wait return NULL
		if( !block ) {
			return NULL;
		}

		waiting = true;
		// Suspend the task while waiting for a message. This is
		// way faster than a semaphore and the task can only get
		// unblocked by 1 event, receiving a message so this still
		// works. This can fail if pre-emption is enabled since
		// than a message can be added to the queue after the
		// queue_start==NULL check.
		suspend();
		waiting = false;
	}

	// Get the first element out of the queue
	Message *element = queue_start;
	// Update the queue to the next element
	if( queue_start->next == NULL ) {
		queue_start = NULL;
		queue_end   = NULL;
	} else {
		queue_start = queue_start->next;
	}

#ifdef TESTING
	// Decrease the number of messages in the queue
	--num_messages;

	assert_queue_invariant();
#endif

	// Reinitialize the queue members
	element->next = NULL;

	// Return the retrieved message from the queue
	return element;
}

#ifdef TESTING
void Process::assert_queue_invariant() {
	if( num_messages == 0 ) {
		configASSERT( queue_start == NULL );
		configASSERT( queue_end == NULL );
	} else {
		Message * current = queue_start;

		// Check if the first num_messages-1 messages have
		// a correctly set next pointer and iterate over them.
		for( uint32_t i=0; i<num_messages-1; ++i ) {
			configASSERT( current->next != NULL );
			current = current->next;
		}

		// Check that this is the last element in this queue
		configASSERT( queue_end == current );
		// Check that the next pointer of this node is NULL
		configASSERT( current->next == NULL );
	}
}
#endif

void Process::start_scheduler() {
	NVIC_SetPriorityGrouping( 0 );

	// Setup the GPIO ports for the trace functionality
	// Currently using the port 2 first 8 pins
	// Also see the FreeRTOSConfig.h file for the trace
	// macros
	LPC_GPIO2->DIR |= 0xFF;
	LPC_GPIO2->CLR = 0xFF;

	// Start the actual FreeRTOS scheduler
	vTaskStartScheduler();
}
