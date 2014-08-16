#include <sciopta.h>

#ifndef _SPP_PROCESS_HPP
#define _SPP_PROCESS_HPP

#include "ticks.hpp"
#include "pool.hpp"
#include "priority.hpp"
#include "processname.hpp"

class Process {
	
	protected:
		sc_pid_t id;
		
		Process(sc_pid_t id) : id(id) {}
		
		friend class Message;
		
	public:
		Process() : id(SC_ILLEGAL_PID) {}
		
		bool operator !            () const { return id != SC_ILLEGAL_PID           ; }
		     operator void const * () const { return id == SC_ILLEGAL_PID ? 0 : this; }
		
		bool operator == (Process p) const { return p.id == id; }
		bool operator != (Process p) const { return p.id != id; }
		
		/// Create a new priority process.
		static Process create(
			char const * name,
			void (*start)(),
			sc_bufsize_t stacksize,
			Priority priority,
			bool started = true,
			Ticks timeslice = 0,
			Pool pool = Pool::process_default
		) {
			return sc_procPrioCreate(
				name,
				start,
				stacksize,
				timeslice,
				priority,
				started ? SC_PDB_STATE_RUN : SC_PDB_STATE_STP,
				pool
			);
		}
		
		/// Create a new static priority process.
		static Process createStatic(
			char const * name,
			void (*start)(),
			sc_bufsize_t stacksize,
			Priority priority,
			bool started = true,
			Ticks timeslice = 0
		) {
			return sc_sysProcCreate(
				name,
				start,
				stacksize,
				timeslice,
				priority,
				started ? SC_PDB_STATE_RUN : SC_PDB_STATE_STP,
				0, // module id
				0, // pcb
				0, // stack
				SC_PROCPRIOCREATESTATIC
			);
		}
		
		/// Create a new timer process.
		static Process createTimer(
			char const * name,
			void (*start)(int),
			sc_bufsize_t stacksize,
			Ticks period,
			bool started = true,
			Ticks initial_delay = 0,
			Pool pool = Pool::process_default
		) {
			return sc_procTimCreate(
				name,
				start,
				stacksize,
				period,
				initial_delay,
				started ? SC_PDB_STATE_RUN : SC_PDB_STATE_STP,
				pool
			);
		}
		
		/// Create a new interrupt process.
		static Process createInterrupt(
			char const * name,
			void (*start)(int),
			sc_bufsize_t stacksize,
			int vector,
			Pool pool = Pool::process_default
		) {
			return sc_procIntCreate(
				name,
				start,
				stacksize,
				vector,
				0,
				0,
				pool
			);
		}
		
		/// Create a new static interrupt process.
		static Process createStaticInterrupt(
			char const * name,
			void (*start)(int),
			sc_bufsize_t stacksize,
			int vector,
			Pool pool = Pool::process_default
		) {
			return sc_sysProcCreate(
				name,
				(void (*)())start,
				stacksize,
				vector,
				0, // unused
				SC_PDB_STATE_RUN,
				0, // module id
				0, // pcb
				0, // stack
				SC_PROCINTCREATESTATIC
			);
		}
		
		/// Kill the process.
		void kill(bool clean = true) const {
			sc_procKill(id, clean ? 0 : SC_PROCKILL_KILL);
		}
		
		/// \name Process Deamon
		/// \{
		
		/// Register process deamon.
		static bool registerDeamon() {
			return sc_procDaemonRegister() == 0;
		}
		
		/// Unregister process deamon.
		static void unregisterDeamon() {
			sc_procDaemonUnregister();
		}
		
		/// \}
		
		/// Try to get a process by name, within time specified time.
		static Process getByName(char const * name, Ticks timeout = 0) {
			return sc_procIdGet(name, timeout);
		}
		
		/// The 'self' process.
		/**
		 * TODO: Extend documentation.
		 */
		static Process self() {
			return SC_CURRENT_PID;
		}
		
		/// Get the calling process.
		static Process current() {
			return sc_procIdGet(0, 0);
		}
		
		/// Get the name of the process.
		ProcessName name() const {
			return ProcessName(sc_procNameGet(id));
		}
		
		/// \name Observing
		/// \{
		
/* TODO
		/// Observe the process.
		void observe(ErrorMessage * message) const {
			sc_procObserve(message, id);
		}
		
		/// Stop a process from observing a process.
		void stopObserving(Process observed) const {
			sc_procUnobserve(observed, id);
		}
*/
		
		/// \}
		
		/// Check if a given process path is correctly formatted.
		static bool checkPath(char const * path) {
			return sc_procPathCheck(const_cast<char *>(path)) != 0;
		}
		
		/// Get the parent process.
		Process parent() const {
			return sc_procPpidGet(id);
		}
		
		/// Get the priority of the process.
		Priority priority() const {
			return sc_procPrioGet(id);
		}
		
		/// Set te priority of the calling process.
		static void setPriority(Priority priority) {
			sc_procPrioSet(priority);
		}
		
		/// Get the time slice of the process.
		Ticks timeSlice() const {
			return sc_procSliceGet(id);
		}
		
		/// Set the time slice of the process.
		void setTimeSlice(Ticks ticks) const {
			sc_procSliceSet(id, ticks);
		}
		
		/// Get the interrupt vector of the (interrupt) process.
		int interruptVector() const {
			return sc_procVectorGet(id);
		}
		
		/// \name Execution control
		/// \{
		
		/// Start the process.
		/**
		 * \note Starting a process that is already started is illegal.
		 */
		void start() const {
			sc_procStart(id);
		}
		
		/// Stop the process.
		/**
		 * \note Stopping a process that is already stopped is allowed.
		 *       For each call to stop(), an extra call of start() is required to start the process.
		 */
		void stop() const {
			sc_procStop(id);
		}
		
		/// Enable the wakeup of a timer or interrupt process.
		void enableWakeup() const {
			sc_procWakeupEnable();
		}
		
		/// Disable the wakeup of a timer or interrupt process.
		void disableWakeup() const {
			sc_procWakeupDisable();
		}
		/// \}
		
		/// Yield the CPU to the next ready process withtin the current process's priority group.
		static void yield() {
			sc_procYield();
		}
		
		/// \name Scheduler
		/// \{
		
		static void lockScheduler() {
			sc_procSchedLock();
		}
		
		static void unlockScheduler() {
			sc_procSchedUnlock();
		}
		
		/// \}
		
};

#endif
