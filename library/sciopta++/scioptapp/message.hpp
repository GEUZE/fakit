/// \file
/// Provides the Message class.

#include <sciopta.h>

#ifndef __SPP_MESSAGE_HPP
#define __SPP_MESSAGE_HPP

#include "pool.hpp"
#include "ticks.hpp"
#include "process.hpp"

union sc_msg {
	sc_msgid_t id;
};

/**
 * \class MessageId
 * \brief A Sciopta message ID.
 */
typedef sc_msgid_t MessageId;

/// A Sciopta message.
/**
 * \warning Messages may not have virtual member functions, and can therefore not have a deconstructor.
 *          Do not use anything inside a message that needs to be deconstructed, as no deconstructor will be called when deleting a Message.
 */
struct Message {
	
	/// The ID of the message.
	MessageId id;
	
	/// Craete a message with the specified ID.
	Message(MessageId id) : id(id) {}
	
	/// Allocate a new Sciopta message.
	/**
	 * \param size The number of bytes to allocate.
	 * \param timeout The time to wait when there is currently not enough memory left.
	 * \param pool The pool to allocate the message in.
	 */
	void * operator new (size_t size, Ticks timeout = Ticks::fatal_on_timeout, Pool pool = Pool::process_default) {
		return sc_msgAlloc(size, 0, pool, timeout);
	}
	
	/// Free a Sciopta message.
	/**
	 * \param message The message to free.
	 */
	void operator delete (void * message) {
		sc_msgFree(reinterpret_cast<sc_msg_t *>(&message));
	}
	
	/// Get the sender of this message.
	Process sender() const {
		sc_msg_t m = const_cast<sc_msg_t>(reinterpret_cast<sc_msg const *>(this));
		return sc_msgSndGet(&m);
	}
	
	/// Get the owner of this message.
	Process owner() const {
		sc_msg_t m = const_cast<sc_msg_t>(reinterpret_cast<sc_msg const *>(this));
		return sc_msgOwnerGet(&m);
	}
	
	/// Claim the ownership of this message.
	void acquire() const {
		sc_msg_t m = const_cast<sc_msg_t>(reinterpret_cast<sc_msg const *>(this));
		return sc_msgAcquire(&m);
	}
	
	/// Send a message to a process.
	template<typename SpecificMessage>
	static void send(SpecificMessage * & message, Process target) {
		Message * & m = const_cast<Message * &>(static_cast<Message * const &>(message));
		sc_msgTx(reinterpret_cast<sc_msg_t *>(&m), target.id, 0);
		message = 0;
	}
	
	/// Send a message to a process.
	template<typename SpecificMessage>
	static void send(SpecificMessage message, Process target) {
		SpecificMessage * m = new SpecificMessage(message);
		send(m, target);
	}
	
	/// Send a message to a process from any process.
	template<typename SpecificMessage>
	static void send(SpecificMessage * & message, Process target, Process sender) {
		Message * & m = const_cast<Message * &>(static_cast<Message * const &>(message));
		sc_msgTxAlias(reinterpret_cast<sc_msg_t *>(&m), target.id, 0, sender.id);
		message = 0;
	}
	
	/// Send a message to a process.
	template<typename SpecificMessage>
	static void send(SpecificMessage message, Process target, Process sender) {
		SpecificMessage * m = new SpecificMessage(message);
		send(m, target, sender);
	}
	
	/// Receive a message.
	static Message * receive(Ticks timeout = Ticks::endless_timeout) {
		return reinterpret_cast<Message *>(sc_msgRx(timeout, SC_MSGRX_ALL, SC_MSGRX_MSGID));
	}
	
	/// Receive a specific message.
	template<typename SpecificMessage>
	static SpecificMessage * receive(Ticks timeout = Ticks::endless_timeout) {
		sc_msgid_t ids[2] = {SpecificMessage::ID, 0};
		return reinterpret_cast<SpecificMessage *>(sc_msgRx(timeout, ids, SC_MSGRX_MSGID));
	}
	
};

/// Try to cast a message to a specific type of message.
/**
 * \tparam SpecificMessage The type of message to cast the message to.
 * \return The message (casted) when the id matches SpecificMessage::ID, 0 otherwise.
 */
template<typename SpecificMessage>
SpecificMessage * message_cast(Message * message) {
	return message->id == SpecificMessage::ID ? static_cast<SpecificMessage *>(message) : 0;
}

/// A basic (empty) message, to inherit from when making your own message class.
template<MessageId message_id>
struct BasicMessage : Message {
	
	static MessageId const ID = message_id;
	
	BasicMessage() : Message(ID) {}
	
};

#endif
