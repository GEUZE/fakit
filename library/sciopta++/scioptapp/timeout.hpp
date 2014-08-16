/// \file
/// Provides the Timeout class.

#include <sciopta.h>

#ifndef __SPP_TIMEOUT_HPP
#define __SPP_TIMEOUT_HPP

#include "message.hpp"

class Timeout {

	protected:
		sc_tmoid_t id;
		Message * message;

	public:
		Timeout() : id(0), message(0) {}

		template<typename SpecificMessage>
		Timeout(SpecificMessage * & m) : id(0), message(m) {
			m = 0;
		}

		template<typename SpecificMessage>
		Timeout(SpecificMessage m) : id(0), message(new SpecificMessage(m)) {}

		/// Start the timeout.
		/**
		 * \returns true if the timeout was started, false if there is no message available (probably because it already started).
		 *
		 * When you receive the message, call .reset().
		 * This class won't automagically know when you receive the message.
		 */
		bool start(Ticks timeout) {
			if (!message) return false;
			id = sc_tmoAdd(timeout, reinterpret_cast<sc_msg_t *>(&message));
			message = 0;
			return true;
		}

		/// Cancel the timeout.
		/**
		 * \returns true if the timeout was canceled, false if it wasn't active.
		 */
		bool cancel() {
			if (!active()) return false;
			message = reinterpret_cast<Message *>(sc_tmoRm(&id));
			return true;
		}

		/// Reset the timeout.
		void reset() {
			message = 0;
			id = 0;
		}

		/// Reset the timeout.
		/**
		 * \param m The message to use next time the timeout is started.
		 */
		template<typename SpecificMessage>
		void reset(SpecificMessage * & m) {
			message = m;
			m = 0;
			id = 0;
		}

		/// Reset the timeout.
		/**
		 * \param m The message to use next time the timeout is started.
		 */
		template<typename SpecificMessage>
		void reset(SpecificMessage m) {
			message = new SpecificMessage(m);
			id = 0;
		}

		/// Check if the timeout is active.
		/**
		 * Note that if you don't call .reset() when you receive the message back,
		 * .active() will still return true.
		 */
		bool active() {
			return id;
		}

	private:
		Timeout & operator = (Timeout const &);
		Timeout(Timeout const &);

};

#endif
