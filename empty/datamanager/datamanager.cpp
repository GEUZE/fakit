#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sciopta++>

#include <LPC177x_8x.h>

#include "datamanager.hpp"
#include "channel.hpp"

// Please see the header file datamanager.hpp for more details.

namespace DataManager {
	
	// The channels are kept as a linked list.
	Channel * channels = 0;
	uint16_t n_channels = 0;
	
	// The message for (Get)ChannelsList.
	M::ChannelsList * channels_list_message = 0;
	
	// A listener.
	// The message will be sent to the listener when there is
	// an update. Listeners are stored as a linked list.
	struct Listener {
		M::Notifier * notifier;
		Process listener;
		Listener * next;
		
		Listener(size_t size, uint32_t e, Process p, Listener * next) :
			notifier(new (size) M::Notifier(this, e)), listener(p), next(next) {}
		
	};
	
	// A hoper.
	// Some process that is hoping for a channel to be created.
	// The hopers are stored as a linked list.
	struct Hoper {
		M::OpenChannel * hoper;
		Hoper * next;
		
		Hoper(M::OpenChannel * m, Hoper * next) :
			hoper(m), next(next) {}
		
		void * operator new (size_t size) {
			return malloc(size);
		}
		
		void operator delete (void * m) {
			free(m);
		}
		
	};
	
	// A stalker.
	// Some process that wants to get all data
	// of all channels going through the datamanager.
	struct Stalker {
		Process process;
		uint32_t last_external_handle; // Will be automatically incremented for each new channel.
		Stalker * next;

		Stalker(Process process, Stalker * next)
			: process(process), last_external_handle(0), next(next) {};
	};
	Stalker * stalkers = 0;

	// Get a channel by its name.
	Channel * find_channel(char const * name, size_t size) {
		for (Channel * c = channels; c; c = c->next) {
			if (strncmp(name, c->name, 8) == 0) {
				if (c->size != size) {
					// TODO: Error, size mismatch!
					return 0;
				}
				return c;
			}
		}
		return 0;
	}
	
	// The data manager main function.
	void main() {
		channels_list_message = new M::ChannelsList;
		while (true) {
			Message * r = Message::receive();
			
			if (M::CreateChannel * m = message_cast<M::CreateChannel>(r)) {
				if (Channel * c = find_channel(m->name, m->size)) {
					if (c->hope) {
						// If the channel is being hoped for, tell the
						// hopers that time has come; All their dreams will
						// come true, their long hoped for channel is finally created!
						Hoper * next;
						for (Hoper * h = c->hopers; h; h = next) {
							Message::send(h->hoper, h->hoper->sender());
							next = h->next;
							delete h; // Also, delete the hopers list directly.
						}
						c->hopers = 0;
						c->hope = false; // The dream comes true;
						// And give the creator its updater.
						M::Updater * x = new (m->size) M::Updater(c, m->size, m->external_handle);
						Message::send(x, m->sender());
					} else {
						// TODO: Error, already exists!
					}
				} else {
					// It didn't exist yet, create it!
					c = channels = new (m->size) Channel(m->name, m->size, channels);
					n_channels++;
					// And give the creator its updater.
					M::Updater * x = new (m->size) M::Updater(c, m->size, m->external_handle);
					Message::send(x, m->sender());
				}
				// Subscribe all stalkers to this channel.
				for (Stalker * stalker = stalkers; stalker; stalker = stalker->next) {
					// Notify the stalker that this channel is added to his stalk list.
					uint32_t external_handle = stalker->last_external_handle++;
					M::StalkedChannel * s = new M::StalkedChannel(m->name, m->size, external_handle);
					Message::send(s, stalker->process);
					// Subscribe the stalker to this channel.
					M::ListenChannel * l = new M::ListenChannel(m->name, m->size, external_handle);
					Message::send(l, dataManager, stalker->process);
				}
				delete m;

			} else if (M::OpenChannel * m = message_cast<M::OpenChannel>(r)) {
				if (Channel * c = find_channel(m->name, m->size)) {
					if (c->hope) {
						if (m->wait) {
							// If the channel is already being hoped for by others:
							// Hope together! Unite the hopers, together we stand strong.
							c->hopers = new Hoper(m, c->hopers);
						} else {
							// Or if the opener doesn't want to wait, tell it the bad news.
							m->handle = 0;
							Message::send(m, m->sender());
						}
					} else {
						// If the channel already existed for real, tell the opener
						// the good news.
						m->handle = c;
						Message::send(m, m->sender());
					}
				} else {
					if (m->wait) {
						// If it didn't exist, start the dream and hope for it.
						c = channels = new (m->size) Channel(m->name, m->size, channels, true);
						n_channels++;
						c->hopers = new Hoper(m, 0);
					} else {
						// Or if the opener doesn't want to wait, tell it the bad news.
						m->handle = 0;
						Message::send(m, m->sender());
					}
				}
				
			} else if (M::OpenChannels * m = message_cast<M::OpenChannels>(r)) {
				char const * const * n = m->names;
				size_t const * s = m->sizes;
				ChannelHandle * h = m->handles;
				m->ok = true;
				while (char const * name = *n++) {
					size_t size = *s++;
					if (!(*h++ = find_channel(name, size))) m->ok = false;
				}
				Message::send(m, m->sender());
				
			} else if (M::ListenChannel * m = message_cast<M::ListenChannel>(r)) {
				Channel * c = find_channel(m->name, m->size);
				// If it didn't exist, hope it'll be created.
				if (!c) {
					c = channels = new (m->size) Channel(m->name, m->size, channels, true);
					n_channels++;
				}
				c->listeners = new Listener(m->size, m->external_handle, m->sender(), c->listeners);
				delete m;
				
			} else if (M::Read * m = message_cast<M::Read>(r)) {
				// Copy the data to wherever it should go, fill in the time, and send it back.
				memcpy(m->data, m->channel->data, m->channel->size);
				m->time = m->channel->last_update;
				Message::send(m, m->sender());
				
			} else if (M::MultiRead * m = message_cast<M::MultiRead>(r)) {
				// Copy the data to wherever it should go, fill in the times, and send it back.
				Channel * const * c = m->channels;
				void * const * d = m->data;
				Time * t = m->times;
				size_t n = m->n_channels;
				while (n--) {
					if (Channel * channel = *c++) {
						if (d && *d) memcpy(*d++, channel->data, channel->size);
						if (t) *t++ = channel->last_update;
					} else {
						++d;
						++t;
					}
				}
				Message::send(m, m->sender());
				
			} else if (M::Updater * m = message_cast<M::Updater>(r)) {
				// Copy the data to the channel and update the time.
				memcpy(m->handle->data, m->data<uint8_t>(), m->handle->size);
				m->handle->last_update = Time::now();
				// And notify the listeners.
				for (Listener * l = m->handle->listeners; l; l = l->next) {
					// Okay, not ALL listeners.
					// Skip the ones that are still processing the previous update.
					if (!l->notifier) continue;
					memcpy(l->notifier->data<uint8_t>(), m->handle->data, m->handle->size);
					l->notifier->time = m->handle->last_update;
					Message::send(l->notifier, l->listener);
				}
				// And send the message back, we're done with it.
				Message::send(m, m->sender());
				
			} else if (M::Notifier * m = message_cast<M::Notifier>(r)) {
				// Apparently, a process is done processing an update.
				// Put it back so we can annoy the process again with more updates.
				m->listener->notifier = m;
				
			} else if (M::GetChannelsList * m = message_cast<M::GetChannelsList>(r)) {
				channels_list_message->channels = channels;
				channels_list_message->n_channels = n_channels;
				Message::send(channels_list_message, m->sender());
				channels_list_message = Message::receive<M::ChannelsList>(Milliseconds(1) + 1);
				if (channels_list_message) {
					if (m->send_back) Message::send(m, m->sender());
					else delete m;
				} else {
					// Claim back the channels list by killing the process
					// and creating a new channels list message.
					Process s = m->sender();
					delete m;
					s.kill();
					channels_list_message = new M::ChannelsList;
				}
				
			} else if (M::Stalker * m = message_cast<M::Stalker>(r)) {
				Stalker * s = stalkers = new Stalker(m->sender(), stalkers);
				// Subscribe to all already existing channels.
				for (Channel * c = channels; c; c = c->next) {
					uint32_t external_handle = s->last_external_handle++;
					M::StalkedChannel * sc = new M::StalkedChannel(c->name, c->size, external_handle);
					Message::send(sc, s->process);
					// Subscribe the stalker to this channel.
					M::ListenChannel * l = new M::ListenChannel(c->name, c->size, external_handle);
					Message::send(l, dataManager, s->process);
				}

			} else {
				// TODO: Handle unknown message.
				delete r;
			}
			
		}
	}
	
}

Process dataManager = Process::createStatic("DM", DataManager::main, 4*1024, 10);

