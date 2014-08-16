#ifndef __RTOSPP_MESSAGE_HPP
#define __RTOSPP_MESSAGE_HPP

#include <cstdlib>

typedef uint32_t MessageId;

class Process;

/// A minimal message to be passed between tasks
struct Message {
	// Allow Process to touch Messages privates
	friend class Process;

private:
	// The next message in the queue, NULL if this
	// is the last message
	Message *next = NULL;

	// Do not allow copies of this object
	Message(const Message&)        = delete;
	void operator=(const Message&) = delete;

public:
	MessageId id;

	// The process that send this message
	Process *sender   = NULL;

	/// Construct a new message
	Message(MessageId id) : id(id) {}

	/// Use the FreeRTOS safe functions to allocate
	void* operator new(size_t size) {
		return pvPortMalloc(size);
	}

	/// Use the FreeRTOS safe function to free
	void operator delete(void *msg) {
		vPortFree(msg);
	}
};

/// The template to inherit to implement your own messages
template<MessageId message_id>
struct BasicMessage : Message {
	static MessageId const ID = message_id;
	BasicMessage() : Message(ID) {}
};

template<typename SpecificMessage>
SpecificMessage * message_cast(Message * message) {
	return message->id == SpecificMessage::ID ? static_cast<SpecificMessage *>(message) : 0;
}

#endif
