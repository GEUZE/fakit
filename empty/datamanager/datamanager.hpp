#include <string.h>
#include <stdint.h>

#include <sciopta++>

#ifndef DATAMANAGER_HPP__
#define DATAMANAGER_HPP__

// The data manager... it manages data. :)
// The idea is that a data source can register itself to
// feed a 'data channel' to the data manager. Something
// that wants some specific data can ask te data manager
// to connect it to such a channel. The data manager keeps
// track of the time at which the last update occured.

// A channel is identified by its name: A string with a
// maximum of 7 characters.

// Processes that work with the data manager can have
// the following roles:
//   - Provider
//      A provider registers a channel and sends updates
//      whenever the data updates.
//   - Reader
//      A reader can ask the data manager for the latest
//      data in some channel.
//   - Listener
//      A listener subscribes itself to a channel and will
//      receive all updates to this channel without
//      explicitly asking for each one.

// If a listener wants to subscribe to a channel that
// isn't created by a provider yet, it's called a 'hoper';
// It is hoping for the channel to be created in the
// future.

extern Process dataManager;

namespace DataManager {
	
	class Channel;
	class Listener;
	
	typedef Channel * ChannelHandle;
	
	namespace M {
		
		// Create a channel with the given name and size.
		// The 'external_handle' can be used to identify this request on your side,
		// An 'Updater' will be sent back.
		struct CreateChannel : BasicMessage<0xDA01> {
			char name[8];
			size_t size;
			uint32_t external_handle;
			CreateChannel(const char * name, size_t size, uint32_t e = 0) : size(size), external_handle(e) {
				strncpy(this->name, name, 8);
			}
		};
		
		// Request a handle to a channel, or optiomally become a 'hoper' if it doesn't exist yet.
		// The 'external_handle' can be used to identify this request on your side.
		// You will get the same message back, with 'handle' filled in.
		// If 'wait' is false and the channel doesn't exist yet, you will get a message back
		// with 'handle' set to 0 (null).
		// If 'wait' is true, 'handle' is guaranteed to be valid when the message is returned.
		struct OpenChannel : BasicMessage<0xDA02> {
			char name[8];
			size_t size;
			uint32_t external_handle;
			bool wait;
			ChannelHandle handle;
			OpenChannel(char const * name, size_t size, uint32_t e = 0, bool wait = true) : size(size), external_handle(e), wait(wait) {
				strncpy(this->name, name, 8);
			}
		};
		
		// Request handles to multiple channels.
		// The 'external_handle' can be used to identify this request on your side.
		// You will get the same message back, with 'handles' array filled in.
		// 'ok' will be set to true if all channels existed and the entire 'handles' array is filled.
		// Otherwise, 'ok' is false and some values in 'handles' are 0 (null).
		struct OpenChannels : BasicMessage<0xDA03> {
			char const * const * names; // Zero (null, not "") terminated array of names.
			size_t const * sizes; // The sizes of the channels.
			ChannelHandle * handles; // Where to store the handles.
			uint32_t external_handle;
			bool ok;
			OpenChannels(char const * const * names, size_t const * sizes, ChannelHandle * handles, uint32_t e = 0) : names(names), sizes(sizes), handles(handles), external_handle(e) {}
		};
		
		// Same as 'OpenChannel', except that you will receive all updates without having to ask for them.
		struct ListenChannel : BasicMessage<0xDA04> {
			char name[8];
			size_t size;
			uint32_t external_handle;
			ListenChannel(char const * name, size_t size, uint32_t e = 0) : size(size), external_handle(e) {
				strncpy(this->name, name, 8);
			}
		};
		
		// The 'Updater'.
		// This message will be allocated by the data manager (after you ask it to create a channel).
		// It is then sent back and forth between the data source and the data manager to send updates.
		struct Updater : BasicMessage<0xDA05> {
			static void * operator new (size_t size, size_t extra_size) {
				return Message::operator new(size + extra_size);
			}
			ChannelHandle const handle;
			size_t const size;
			uint32_t external_handle;
			protected: uint8_t data_[];
			public:
			template<typename T> T       * data()       { return reinterpret_cast<T       *>(data_); }
			template<typename T> T const * data() const { return reinterpret_cast<T const *>(data_); }
			Updater(ChannelHandle handle, size_t size, uint32_t e = 0) : handle(handle), size(size), external_handle(e) {}
		};
		
		// Read data from a channel.
		// The data will be stored at 'data' and you will
		// get the exact same message back, with 'time' filled in.
		struct Read : BasicMessage<0xDA06> {
			ChannelHandle channel;
			void * data;
			Time time;
			Read(ChannelHandle c, void * d) : channel(c), data(d), time(0) {}
		};
		
		// Read data from multiple channels.
		// The data will be stored at 'data' and you will
		// get the exact same message back, with 'time' filled in.
		struct MultiRead : BasicMessage<0xDA07> {
			ChannelHandle const * channels; // List of channels, may contain 0 (null) handles (those are simply skipped).
			size_t n_channels; // The length of the list.
			void * const * data; // May be 0 (null) or may contain 0 (null) pointers.
			Time * times; // May be 0 (null)
			MultiRead(ChannelHandle const * c, size_t n, void * const * d, Time * t) : channels(c), n_channels(n), data(d), times(t) {}
		};
		
		// The 'Notifier'.
		// This message will be allocated by the data manager (after you ask it to create and listen to a channel).
		// It is then sent back and forth between the data manager and the listener..
		struct Notifier : BasicMessage<0xDA08> {
			Listener * listener;
			static void * operator new (size_t size, size_t extra_size) {
				return Message::operator new(size + extra_size);
			}
			uint32_t external_handle;
			Time time;
			protected: uint8_t data_[];
			public:
			template<typename T> T       * data()       { return reinterpret_cast<T       *>(data_); }
			template<typename T> T const * data() const { return reinterpret_cast<T const *>(data_); }
			Notifier(Listener * listener, uint32_t e = 0) : listener(listener), external_handle(e) {}
		};
		
		// Get access to the complete list of channels with the data and timestamps.
		// You will get a 'ChannelsList' message back, which you have to return
		// within a millisecond. If you don't, the data manager will rape you in
		// the ass and kill you.
		struct GetChannelsList : BasicMessage<0xDA09> {
			bool send_back;
			GetChannelsList(bool send_back = false) : send_back(send_back) {}
		};
		
		struct ChannelsList : BasicMessage<0xDA0A> {
			Channel * channels; // See channel.hpp
			uint16_t n_channels;
		};
		
		// Register a process to receive updates of all channels.
		// Newly created channels will be automatically added to the list of stalked channels.
		// For every newly created channel, a StalkChannel message is sent back to the stalking process.
		struct Stalker : BasicMessage<0xDA0B> {};

		// This message is send back to any stalker processes when a new channel is created.
		// It contains information about the channel such that the stalker can keep track
		// of what channels it is stalking.
		struct StalkedChannel : BasicMessage<0xDA0C> {
			char name[8];
			size_t size;
			uint32_t external_handle;
			StalkedChannel(char const * name, size_t size, uint32_t external_handle)
				: size(size), external_handle(external_handle) {
				strncpy(this->name, name, 8);
			}
		};
	}
	
}

#endif
