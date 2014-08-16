#ifndef __RTOSPP_SEMAPHORE_HPP
#define __RTOSPP_SEMAPHORE_HPP

#include <task.h>
#include <queue.h>
#include <semphr.h>

namespace Semaphore {
	namespace {
		enum SemaphoreType {
			BINARY,
			MUTEX
		};
	}

	/// A wrapper around a FreeRTOS semaphore
	/**
	 * \tparam type Which semaphore to create
	 */
	template<SemaphoreType type>
	class BasicSemaphore {
		xSemaphoreHandle semaphoreHandle = NULL;

	public:
		/// Construct a semaphore
		BasicSemaphore() {
			switch( type ) {
				case BINARY: vSemaphoreCreateBinary( semaphoreHandle ); take(0); break;
				case MUTEX:  semaphoreHandle = xSemaphoreCreateMutex(); break;
			}
			configASSERT( semaphoreHandle != NULL );
		}

		/// Delete this semaphore
		~BasicSemaphore() {
			vSemaphoreDelete( semaphoreHandle );
		}

		/// Try to take this semaphore
		/**
		 * \param timeout The maximum amount of ticks to wait
		 * \return If the semaphore was succesfully taken
		 */
		inline bool take(const portTickType timeout = portMAX_DELAY) {
			return xSemaphoreTake( semaphoreHandle, timeout ) == pdTRUE;
		}

		/// Give this semaphore
		/**
		 * Give this semaphore from a process.
		 * \return If the semaphore was succesfully given
		 */
		inline bool give() {
			return xSemaphoreGive( semaphoreHandle ) == pdTRUE;
		}

		/// Give this semaphore from an ISR
		/**
		 * \return If the semaphore was succesfully given
		 */
		inline bool give_from_isr(signed portBASE_TYPE * task_awoken = NULL) {
			switch( type ) {
				case BINARY:
				{
					bool success = xSemaphoreGiveFromISR( semaphoreHandle, task_awoken ) == pdTRUE;
					return success;
				}
				case MUTEX: configASSERT(false);
			}
		}
	};

	/// A binary semaphore
	/**
	 * A binary semaphore is used for synchronization
	 * between tasks, where a task or interrupt gives
	 * the semaphore and another tasks waits for it.
	 */
	typedef BasicSemaphore<BINARY> Binary;

	/// A mutex semaphore
	/**
	 * A mutex achieves mutual exclusion between tasks,
	 * a taken mutex has to be returned by the same task.
	 * Therefor a mutex cannot be given from an ISR.
	 * The difference between a binary semaphore and a
	 * mutex is that a mutex uses priority inheritance,
	 * where is a high priority task is waiting for a
	 * low priority task the low priority task temporarily
	 * gets the priority of the waiting task untill it
	 * releases the mutex.
	 */
	typedef BasicSemaphore<MUTEX>  Mutex;
}

#endif
