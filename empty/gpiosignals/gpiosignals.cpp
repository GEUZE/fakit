#include <sciopta++>
#include <LPC177x_8x.h>
#include <stdio.h>
#include "../datamanager/datamanager.hpp"

extern Process gpioSignals;

namespace GpioSignals {

	struct Interrupt : BasicMessage<0x6661> {
		uint32_t falling;
		uint32_t rising;
		Interrupt(uint32_t f, uint32_t r) : falling(f), rising(r) {}
	};

	void int_main(int src) {
		if (src == 0) {
			Message::send(Interrupt(LPC_GPIOINT->IO0IntStatF, LPC_GPIOINT->IO0IntStatR), gpioSignals);
			LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;
		}
	}

	DataManager::M::Updater * updaters[9] = {};

	void send_update(size_t id, uint8_t data = 1) {
		while (!updaters[id]) {
			DataManager::M::Updater * m = Message::receive<DataManager::M::Updater>();
			updaters[m->external_handle] = m;
		}
		if (updaters[id]->size == 1) updaters[id]->data<uint8_t>()[0] = data;
		Message::send(updaters[id], dataManager);
	}

	void send_updates(uint32_t falling, uint32_t rising) {
		if ( rising & (1 << 27)) {
                    send_update(0, 0);
                }
		if ( rising & (1 << 28)) {
                    send_update(1, 0);
                }
		if (falling & (1 << 27)) {
                    send_update(0, 1);
                }
		if (falling & (1 << 28)) {
                    send_update(1, 1);
                }
	}

	void main() {

		LPC_IOCON->P0_27 = 40; // GPIO with pull-down
		LPC_IOCON->P0_28 = 40; // GPIO with pull-down

		LPC_GPIOINT->IO0IntEnR =
			(1 << 27) |
			(1 << 28);
		LPC_GPIOINT->IO0IntEnF =
			(1 << 27) |
			(1 << 28);

		NVIC_EnableIRQ(GPIO_IRQn);

		Message::send(DataManager::M::CreateChannel("bt-strt", 1, 1), dataManager);
		Message::send(DataManager::M::CreateChannel("bt-chrg", 1, 0), dataManager);

		Ticks::sleep(Milliseconds(1500));

		{ // Initial values
			uint32_t pins = LPC_GPIO0->PIN;
			send_updates(~pins, pins);
		}

		while (true) {
			Message * r = Message::receive();
			if (DataManager::M::Updater * m = message_cast<DataManager::M::Updater>(r)) {
				updaters[m->external_handle] = m;
			} else if (Interrupt * m = message_cast<Interrupt>(r)) {
				send_updates(m->falling, m->rising);
                                delete m;
			} else {
				// TODO: wut?
				delete m;
			}
		}

	}

}

Process gpioSignals_int = Process::createStaticInterrupt("gpiosigI", GpioSignals::int_main, 1024, 54);
Process gpioSignals     = Process::createStatic         ("gpiosig" , GpioSignals::    main, 1024,  1);
