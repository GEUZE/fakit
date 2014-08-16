#include <stdint.h>

#include <sciopta++>

#ifndef CAN_HPP__
#define CAN_HPP__

extern Process can;

namespace Can {
	
	namespace M {
		
		// Sender of a CAN message.
		// You'll get the message back when the message is transferred to the hardware buffer.
		// However, if you put send_back to 'false', the CAN process will just delete the message after processing instead.
		struct Sender : BasicMessage<0xCA01> {
			uint8_t bus;
			uint16_t can_id;
			uint8_t length;
			uint8_t data[8];
			Sender * next;
			bool send_back;
			Sender(uint8_t bus, uint16_t can_id, uint8_t length, uint8_t const * data = 0, bool send_back = true)
				: bus(bus), can_id(can_id), length(length), next(0), send_back(send_back) {
				if (data) for (size_t i = 0; i < length && i < 8; ++i) this->data[i] = data[i];
			}
		};
		
		// Send a CAN message.
		// This is just a 'Sender' with 'send_back' set to 'false' by default.
		struct Send : Sender {
			Send(uint8_t bus, uint16_t can_id, uint8_t length, uint8_t const * data = 0)
				: Sender(bus, can_id, length, data, false) {}
		};
		
		// Listen on the specified bus for ids between id_from and id_to, inclusive.
		struct Listen : BasicMessage<0xCA02> {
			uint8_t bus;
			uint16_t id_from;
			uint16_t id_to;
			Process listener;
			Listen(uint8_t bus, uint16_t id_from, uint16_t id_to, Process listener)
				: bus(bus), id_from(id_from), id_to(id_to), listener(listener) {}
		};
		
		// A can message was received.
		struct Received : BasicMessage<0xCA03> {
			uint8_t bus;
			uint16_t can_id;
			uint8_t length;
			uint8_t data[8];
			Received(uint8_t bus, uint16_t can_id, uint8_t length, uint8_t const * data)
				: bus(bus), can_id(can_id), length(length) {
				for (size_t i = 0; i < length && i < 8; ++i) this->data[i] = data[i];
			}
		};
		
	}
	
}

#endif
