#include <stdint.h>

#include <sciopta++>

#include <LPC177x_8x.h>

#include "can.hpp"

extern "C" {
	extern uint32_t PeripheralClock;
}

extern Process can;

namespace Can {
	
	// This is the 'listener tree'.
	// A binary search tree of CAN ID ranges,
	// sorted primary on 'bus' and secondary on the ID range.
	struct Listener {
		Listener * left;
		Listener * right;
		uint8_t bus;
		uint32_t id_from;
		uint32_t id_to;
		Process listener;
	} * listeners = 0;
	
	// Processes an incoming message and sends it to the main can process to be dispatched.
	void message_received(uint8_t bus) {
		LPC_CAN_TypeDef * can = bus == 0 ? LPC_CAN1 : LPC_CAN2;
		
		if ((can->RFS & (3 << 30)) == 0) { // 11-bit ID data frame.
			uint16_t can_id = can->RID;
			uint8_t length = (can->RFS >> 16) & 0x0F;
			uint8_t data[] = {
				(can->RDA      ) & 0xFF,
				(can->RDA >>  8) & 0xFF,
				(can->RDA >> 16) & 0xFF,
				(can->RDA >> 24) & 0xFF,
				(can->RDB      ) & 0xFF,
				(can->RDB >>  8) & 0xFF,
				(can->RDB >> 16) & 0xFF,
				(can->RDB >> 24) & 0xFF
			};
			Message::send(M::Received(bus, can_id, length, data), ::can);
		} else {
			// TODO: Handle 29-bit ID frames and remote frames.
		}
		
		can->CMR = 0x04; // Command: Release the receive buffer.
	}
	
	// There are two seperate 'send queues' for CAN1 and CAN2.
	// They are circular linked lists. The root pointer points to the
	// youngest message (to be processed last). send_queue[x]->next is
	// the oldest message (to be processed first). send_queue[x] is 0
	// when the queue is empty.
	M::Sender * send_queue[2] = {0, 0};
	
	// Try to send the next message.
	// That is, send the next message if both a message is in the queue
	// and the CAN buffer is available.
	// Only the first CAN buffer is used, as we have our own buffering
	// system. (One that can store more than 3 message.)
	void try_send(uint8_t bus) {
		// Only send a message if the first hardware buffer (the only
		// hardware buffer we use) is empty.
		LPC_CAN_TypeDef * can = bus == 0 ? LPC_CAN1 : LPC_CAN2;
		if ((can->SR & (1 << 2)) && send_queue[bus]) {
			M::Sender * m = send_queue[bus]->next;
			// Send the message.
			can->TFI1 = m->length << 16;
			can->TID1 = m->can_id;
			can->TDA1 = m->data[0] | (m->data[1] << 8) | (m->data[2] << 16) | (m->data[3] << 24);
			can->TDB1 = m->data[4] | (m->data[5] << 8) | (m->data[6] << 16) | (m->data[7] << 24);
			can->CMR = 0x21; // Command: Start transmission from buffer 1
			// Pop the message from the queue.
			if (send_queue[bus] == m) send_queue[bus] = 0;
			else                      send_queue[bus]->next = m->next;
			if (m->send_back) {
				// And send it back to the process that sent the message if that was requested.
				Message::send(m, m->sender());
			} else {
				// Otherwise, just delete it.
				delete m;
			}
		}
	}
	
	// This writes the ID ranges from a 'listener tree' in the right order
	// to the acceptance filter.
	size_t write_acceptance_filter_table(Listener * l, size_t index = 0) {
		if (!l) return index;
		index = write_acceptance_filter_table(l->left, index);
		uint8_t bus = l->bus & 7;
		uint16_t from = l->id_from & 0x7FF;
		uint16_t to   = l->id_to   & 0x7FF;
		uint32_t entry = (bus << 29) | (from << 16) | (bus << 13) | to;
		LPC_CANAF_RAM->mask[index++] = entry;
		index = write_acceptance_filter_table(l->right, index);
		return index;
	}
	
	// This will update to acceptance filter to accept exactly the IDs for
	// which processes are actually listening.
	void update_acceptance_filter() {
		LPC_CANAF->AFMR = 0x01; // Temporarily turn acceptance filter off.
		size_t entries = write_acceptance_filter_table(listeners);
		LPC_CANAF->SFF_sa     = 0;
		LPC_CANAF->SFF_GRP_sa = 0;
		LPC_CANAF->EFF_sa     = entries * 4;
		LPC_CANAF->EFF_GRP_sa = entries * 4;
		LPC_CANAF->ENDofTable = entries * 4;
		LPC_CANAF->AFMR = 0x00; // Turn the acceptanfe filter back on again.
	}
	
	// Set the CAN bitrate.
	void set_bitrate(uint8_t bus, uint32_t speed) {
		LPC_CAN_TypeDef * can = bus == 0 ? LPC_CAN1 : LPC_CAN2;
		// See datasheet of LPC177x_8x for details.
		uint32_t x = PeripheralClock / speed;
		uint8_t nt;
		for (nt = 24; nt > 0 && x % nt; nt -= 2);
		uint16_t brp = x / nt - 1;
		nt -= 1;
		uint8_t tseg1 = nt - nt / 3 - 1;
		uint8_t tseg2 =      nt / 3 - 1;
		can->MOD = 1; // Reset mode
		uint8_t sjw = 3; // Maximum value.
		can->BTR = (tseg2 << 20) | (tseg1 << 16) | (sjw << 14) | brp;
		can->MOD = 0; // Operation mode
	}
	
	struct Interrupt : BasicMessage<0xCAEE> {
		uint32_t icr1, icr2;
		Interrupt(uint32_t icr1, uint32_t icr2) : icr1(icr1), icr2(icr2) {}
	};
	
	void int_main(int src) {
		if (src == 0) {
			while (true) {
				if      (LPC_CAN1->SR & 1) message_received(0);
				else if (LPC_CAN2->SR & 1) message_received(1);
				else break;
			}
			// The ICR register is reset when read.
			uint32_t icr1 = LPC_CAN1->ICR;
			uint32_t icr2 = LPC_CAN2->ICR;
			// Reset the bus when an error occurs.
			if (icr1 & (1 << 2)) { LPC_CAN1->GSR &= ~0xFFFF0000; LPC_CAN1->MOD &= ~1; }
			if (icr2 & (1 << 2)) { LPC_CAN2->GSR &= ~0xFFFF0000; LPC_CAN2->MOD &= ~1; }
			// Notify the can process.
			Message::send(Interrupt(icr1, icr2), can);
		}
	}
	
	void main() {
		
		// Pin configurations.
		// TODO
		LPC_IOCON->P0_0 = 1; // CAN RD1
		LPC_IOCON->P0_1 = 1; // CAN TD1
		// CAN2
		
		// Enable the power for the CAN peripherals.
		LPC_SC->PCONP |= 3 << 13;
		
		// Set the speed.
		set_bitrate(0, 1000000);
		// CAN2
		
		// Enable the CAN interrupts.
		// RIE: Receive interrupt.
		// TIE1: Transmit interrupt for the first buffer.
		// EIE: Error interrupt.
		LPC_CAN1->IER = 0x07;
		// CAN2
		
		// Enable the interrupt.
		NVIC_EnableIRQ(CAN_IRQn);
		
		while (true) {
			Message * r = Message::receive();
			if (Interrupt * m = message_cast<Interrupt>(r)) {
				
				// Process received messages (if any).
				while (true) {
					if      (LPC_CAN1->SR & 1) message_received(0);
					else if (LPC_CAN2->SR & 1) message_received(1);
					else break;
				}
				
				// When a buffer is available again, send the next message, if any.
				if (m->icr1 & (1 << 1)) try_send(0);
				if (m->icr2 & (1 << 1)) try_send(1);
				
				delete m;
				
			} else if (M::Received * m = message_cast<M::Received>(r)) {
				// Dispatch it to the right process.
				Listener * l = listeners;
				while (l) {
					if      (l->bus     > m->bus   ) l = l->left ;
					else if (l->bus     < m->bus   ) l = l->right;
					else if (l->id_from > m->can_id) l = l->left ;
					else if (l->id_to   < m->can_id) l = l->right;
					else break;
				}
				if (l) Message::send(m, l->listener);
				else delete m;

			} else if (M::Listen * m = message_cast<M::Listen>(r)) {
				
				// Add the listener in the binary search tree.
				Listener * * l = &listeners;
				while (l && *l) {
					if      ((*l)->bus     > m->bus    ) l = &((*l)->left );
					else if ((*l)->bus     < m->bus    ) l = &((*l)->right);
					else if ((*l)->id_from > m->id_to  ) l = &((*l)->left );
					else if ((*l)->id_to   < m->id_from) l = &((*l)->right);
					else l = 0;
				}
				if (l) {
					*l = new Listener;
					(*l)->left     = 0;
					(*l)->right    = 0;
					(*l)->bus      = m->bus     ;
					(*l)->id_from  = m->id_from ;
					(*l)->id_to    = m->id_to   ;
					(*l)->listener = m->listener;
					
					// Update the acceptance filter to accept the messages for this new listener as well.
					update_acceptance_filter();
				} else {
					// TODO: Fail!
					// The range overlaps with an existing one.
				}
				
				delete m;
				
			} else if (M::Sender * m = message_cast<M::Sender>(r)) {
				
				// Place the message in the send queue . . .
				M::Sender * & q = send_queue[m->bus];
				if (q) {
					m->next = q->next;
					q->next = m;
				} else {
					m->next = m;
				}
				q = m;
				
				// . . . and try to send it directly.
				try_send(m->bus);
				
			} else {
				
				// TODO: Wut?
				delete m;
				
			}
		}
	}
	
}

Process can_int = Process::createStaticInterrupt("CanInt", Can::int_main, 1024, 41);
Process can     = Process::createStatic         ("Can"   , Can::    main, 1024,  1);

