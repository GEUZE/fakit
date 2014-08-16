#include <sciopta++>
#include <LPC177x_8x.h>
#include <stdio.h>
#include "../datamanager/datamanager.hpp"
#include <math.h>

extern "C" {
	extern uint32_t PeripheralClock;
}

extern Process adc;

namespace Adc {

	const uint8_t n_max_channels = 8;
	DataManager::M::Updater * updaters[n_max_channels];

	struct TimeoutMessage : BasicMessage<0xADC0> {};

	Ticks interval = Milliseconds(500);
	uint32_t const clockrate = 1000000;

        inline void waitMicro(unsigned int d){
            for( uint32_t volatile i=0; i<d*1000; ++i ) {
            }
        }

	void initialize() {

		// Enable clock power
		LPC_SC->PCONP |= 1 << 12;

		// Pin configuration
		LPC_IOCON->P0_23 = 1; // ADC[0]
		LPC_IOCON->P0_24 = 1; // ADC[1]
		LPC_IOCON->P0_25 = 1; // ADC[2]
		LPC_IOCON->P0_26 = 1; // ADC[3]
		LPC_IOCON->P0_12 = 3; // ADC[6]
		LPC_IOCON->P0_13 = 3; // ADC[7]

		// Setup control register
		uint32_t clkdiv = PeripheralClock / clockrate;
		LPC_ADC->CR = (1 << 0) | // SEL ADC[0]
		              (1 << 1) | // SEL ADC[1]
		              (1 << 2) | // SEL ADC[2]
		              (1 << 3) | // SEL ADC[3]
		              (1 << 6) | // SEL ADC[6]
		              (1 << 7) | // SEL ADC[7]
		         (clkdiv << 8) | // CLKDIV
		             (1 << 21);  // Operation mode

		// Only enable interrupts for channels 4 and 5
		LPC_ADC->INTEN = (1 << 0) | // SEL ADC[0]
                                 (1 << 1) | // SEL ADC[1]
                                 (1 << 2) | // SEL ADC[2]
                                 (1 << 3) | // SEL ADC[3]
                                 (1 << 6) | // SEL ADC[6]
                                 (1 << 7) ; // SEL ADC[7]

		// Enable interruptes
		NVIC_EnableIRQ(ADC_IRQn);



	}
        
        uint8_t Position(uint16_t smpl){
            // Button readout in range from 0 to 4096 
            // Measured values at each knob position are:
            // 1:   290
            // 2:   671
            // 3:   1029
            // 4:   1368
            // 5:   1714
            // 6:   2053
            // 7:   2348
            // 8:   2719
            // 9:   3090
            // 10:  3423
            // 11:  3732
            // 12:  4095
            const uint16_t staticPos[12] = {290,671,1029,1368,1714,2053,2348,2719,3090,3423,3732,4095};

            // Sets the margin for the buttonpositions
            int margin = 100;
            int pos;

            // Compare measured value to static knob position values
            for (int i=0; i<12; i++){
                if ((smpl >= (staticPos[i] - margin)) && (smpl < (staticPos[i] + margin))){
                    pos = i + 1; 
                    return pos;
                }
            }
         
        }

	struct Interrupt : BasicMessage<0xADC2> {
		uint8_t channel;
		uint16_t sample;
		Interrupt(uint8_t channel, uint16_t sample) : channel(channel), sample(sample) {}
	};

	// The analog channels we want to sample, 1 bit per channel.
	const uint8_t channels = (1 << 0) |
							 (1 << 1) |
							 (1 << 2) |
							 (1 << 3) |
							 (1 << 6) |
							 (1 << 7);

	// Keeps track of which channels are converted so far.
	uint8_t burst_progress = 0x00;

	void int_main(int src) {
		if (src == -1) { // Initialize
			initialize();
		} else if (src == 0) { // Actual interrupt
			// Every channel will generate one interrupt when it is done converting.
			// Determine which channel has generated this interrupt, get the sample for that channel,
			// and send it to the adc process.
			for (size_t i = 0; i < n_max_channels; ++i) {
				uint32_t data = LPC_ADC->DR[i];
				if (data & (1 << 31)) { // Check DONE bit for channel 'i'.
					// Get the sample.
					uint16_t sample = (data >> 4) & 0xFFF;
					Message::send(Interrupt(i, Position(sample)), adc);
					// Add this channel to our burst progress.
					burst_progress |= (1 << i);
					// If burst_progess contains all our channels, we're done!
					if ((burst_progress & channels) == channels) {
						// Disable burst mode.
						LPC_ADC->CR &= ~(1 << 16);
						burst_progress = 0;
					}
					return;
				}
			}
		}
	}

	void read() {
		LPC_ADC->CR &= ~(( 0x7 << 24) | // Clear START
		                 (0xFF <<  0)); // Reset channels

		LPC_ADC->CR |= (1 <<  0) | // Read channel 0
		               (1 <<  1) | // Read channel 1
		               (1 <<  2) | // Read channel 2
		               (1 <<  3) | // Read channel 3
		               (1 <<  6) | // Read channel 6
		               (1 <<  7) | // Read channel 7
		               (1 << 16);  // Enable burst mode
	}

	void main() {
                /*
                 * Button lay-out --------
                 *   [1]            [4]
                 *   [3][2]      [5][6]
                 *------------------------ */

		Message::send(DataManager::M::CreateChannel("knob-1 ", sizeof(uint8_t), 0), dataManager);
		Message::send(DataManager::M::CreateChannel("knob-2 ", sizeof(uint8_t), 1), dataManager);
		Message::send(DataManager::M::CreateChannel("knob-3 ", sizeof(uint8_t), 7), dataManager);
		Message::send(DataManager::M::CreateChannel("knob-4 ", sizeof(uint8_t), 2), dataManager);
		Message::send(DataManager::M::CreateChannel("knob-5 ", sizeof(uint8_t), 3), dataManager);
		Message::send(DataManager::M::CreateChannel("knob-6 ", sizeof(uint8_t), 6), dataManager);

		Timeout timeout;
		timeout.reset(TimeoutMessage());
		timeout.start(interval);

		while (true) {
			Message * m = Message::receive();
			if (DataManager::M::Updater * r = message_cast<DataManager::M::Updater>(m)) {
				updaters[r->external_handle] = r;
			} else if (Interrupt * t = message_cast<Interrupt>(m)) {
				uint8_t channel = t->channel;
				uint8_t sample = t->sample;
				if (updaters[channel]) {
					updaters[channel]->data<uint8_t>()[0] = sample;
					Message::send(updaters[channel], dataManager);
				}
				delete t;
			} else if (TimeoutMessage * t = message_cast<TimeoutMessage>(m)) {
				timeout.reset(t);
				timeout.start(interval);
				read();
			} else {
				// TODO: wut?
			}
		}

	}

}

Process adc_int = Process::createStaticInterrupt("adcI", Adc::int_main, 1024, 38);
Process adc     = Process::createStatic         ("adc" , Adc::    main, 1024, 25);
