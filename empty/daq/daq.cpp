#include <sciopta++>
#include <stdio.h>

#include <vector>

#include "../datamanager/datamanager.hpp"
#include "../can/can.hpp"

namespace Daq {

	// TODO Many lookups, use other data structure for this.
	struct Sensornode {
		uint16_t can_id;
		char name[8];
		size_t length;
		bool enabled;
		uint32_t handle;
		Sensornode * next;

		Sensornode(uint16_t can_id, const char * name, size_t length, uint32_t handle, Sensornode * next)
			: can_id(can_id), length(length), enabled(false), handle(handle),  next(next)
		{ strncpy(this->name, name, 8); };

		// Send data.
		void send_data(const uint8_t * data) const {
			if (!enabled) return;
			Can::M::Send * s = new Can::M::Send(0, can_id + 1, length);
			if (!s) return;
			memcpy(s->data, data, length);
			Message::send(s, can);
		}

		// ECU commands.
		void   enable()       { enabled = true ; }
		void  disable()       { enabled = false; }
		void announce() const {
			size_t name_length = strlen(name);
			// TODO Optimize this by saving a Can::M::Sender per sensornode.
			Can::M::Send * send = new Can::M::Send(0, can_id, 1 + name_length);
			if (!send) return;
			for (size_t i = 0; i < 7; ++ i) {
				send->data[i + 1] = name[i];
			}
			send->data[0] = length;
			Message::send(send, can);
		}

		static void announce_all();
		static void   enable_all();
		static void  disable_all();
	};

	// Linked list of all added sensornodes.
	Sensornode * sensornodes;

	void Sensornode::announce_all() { for (const Sensornode * sn = sensornodes; sn; sn = sn->next) sn->announce(); }
	void Sensornode::  enable_all() { for (      Sensornode * sn = sensornodes; sn; sn = sn->next) sn->  enable(); }
	void Sensornode:: disable_all() { for (      Sensornode * sn = sensornodes; sn; sn = sn->next) sn-> disable(); }

	inline uint16_t get_new_can_id() {
		static uint16_t can_id = 0x200;
		return can_id += 2;
	}

	void add_sensornode(const char * name, uint32_t handle, size_t size) {
		sensornodes = new Sensornode(get_new_can_id(), name, size, handle, sensornodes);
	}

	const Sensornode * find_by_handle(uint32_t handle) {
		for (Sensornode * sn = sensornodes; sn; sn = sn->next) {
			if (sn->handle == handle) return sn;
		}
		return NULL;
	}
	Sensornode * find_by_can_id(uint16_t can_id) {
		for (Sensornode * sn = sensornodes; sn; sn = sn->next) {
			if (sn->can_id == can_id) return sn;
		}
		return NULL;
	}

	inline void enable_sensornode(Sensornode * sn) {
		sn->enable();
	}

	void main() {

		// Register yourself as a stalker.
		// You will receive all new data of all channels in the datamanager.
		Message::send(DataManager::M::Stalker(), dataManager);

		// Listen to sensornode commands from the ECU.
		Message::send(Can::M::Listen(0, 0x100, 0x100, Process::current()), can);

		while (true) {
			Message * m = Message::receive();
			if (DataManager::M::StalkedChannel * c = message_cast<DataManager::M::StalkedChannel>(m)) {
				if (c->size <= 8) add_sensornode(c->name, c->external_handle, c->size);
				delete c;
				sensornodes->announce();
			} else if (DataManager::M::Notifier * n = message_cast<DataManager::M::Notifier>(m)) {
				find_by_handle(n->external_handle)->send_data(n->data<const uint8_t>());
				Message::send(n, dataManager);
			} else if (Can::M::Received * r = message_cast<Can::M::Received>(m)) {
				// Handle ECU commands.
				uint16_t address = 0;
				if (r->length == 3) address = (r->data[2] << 8 & 0xFF00) | (r->data[1] & 0xFF);
				if ((address >= 0x300 || address < 0x200) && address != 0) continue;
				switch (r->data[0]) {
					case 0: if (address == 0) Sensornode::announce_all(); else { Sensornode * sn = find_by_can_id(address); if (sn) sn->announce(); }  break;
					case 1: if (address == 0) Sensornode::  enable_all(); else { Sensornode * sn = find_by_can_id(address); if (sn) sn->  enable(); }  break;
					case 2: if (address == 0) Sensornode:: disable_all(); else { Sensornode * sn = find_by_can_id(address); if (sn) sn-> disable(); }  break;
					default: break;
				}
				delete r;
			} else {
				delete m;
			}
		}

	}

}

Process daq = Process::createStatic("daq", Daq::main, 1024, 20);
