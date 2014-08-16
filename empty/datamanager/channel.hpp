#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sciopta++>

#ifndef DATAMANAGER_CHANNEL_HPP__
#define DATAMANAGER_CHANNEL_HPP__

namespace DataManager {

	class Listener;
	class Hoper;

	// The channels are kept as a linked list.
	struct Channel {
		Channel * next;
		char name[8];
		size_t const size;
		bool hope; // If true, this channel is just a hope of some hoper, it isn't really created by some data source yet.
		Hoper * hopers;
		Listener * listeners;
		Time last_update;
		unsigned char data[];

		Channel(char const * name, size_t size, Channel * next, bool hope = false) :
			next(next), size(size), hope(hope), hopers(0), listeners(0), last_update(0)
		{
			strncpy(this->name, name, 8);
			memset(data, size, 0);
		}

		void * operator new (size_t size, size_t extra_size) {
			return malloc(size + extra_size);
		}

	};

}

#endif

