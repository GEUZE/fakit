/// \file
/// Provides the Module class.

#include <sciopta.h>

#ifndef __MODULE_HPP
#define __MODULE_HPP

#include "priority.hpp"
#include "pool.hpp"

/// A Sciopta module.
class Module {
	
	protected:
		sc_moduleid_t id;
		
		Module(sc_moduleid_t id) : id(id) {}
		
	public:
		Module() : id(SC_ILLEGAL_MID) {}
		
		bool operator !            () const { return id != SC_ILLEGAL_MID           ; }
		     operator const void * () const { return id == SC_ILLEGAL_MID ? 0 : this; }
		
		bool operator == (Module m) const { return m.id == id; }
		bool operator != (Module m) const { return m.id != id; }
		
		/// Create a new module.
		/**
		 * \param name The name of the module. with a maximum of 31 characters.
		 * \param init The initialisation function of the module. (ie. The entry point.)
		 * \param stacksize The size of the stack of the initialisation process, in bytes.
		 * \param priority The priority of the module.
		 * \param start The location of the module in memory.
		 * \param size The size of the module in memory.
		 *             (That is, 256 + stack per static process,
		 *                       the size of the message pools,
		 *                       the size of the initialized data,
		 *                       the number of configured hooks times four, and
		 *                       96 if friends are not used, or 112 if they are.
		 * \param initialized_data_size The size of the initialized data.
		 * \param max_pools The maximum number of pools inside the module.
		 * \param max_processes The maximum number of processes inside the module.
		 * \return The created module.
		 */
		static Module create(
			char const * name,
			void (*init)(),
			sc_bufsize_t stacksize,
			Priority priority,
			void * start,
			sc_modulesize_t size,
			sc_modulesize_t initialized_data_size,
			unsigned int max_pools,
			unsigned int max_processes
		) {
			return sc_moduleCreate(
				name,
				init,
				stacksize,
				priority,
				reinterpret_cast<char *>(start),
				size,
				initialized_data_size,
				max_pools,
				max_processes
			);
		}
		
		/// Kill the module.
		void kill(bool clean = true) {
			sc_moduleKill(id, clean ? 0 : SC_MODULEKILL_KILL);
		}
		
		/// Get the 'self' module.
		/**
		 * The 'self' module always refers to the module that uses it.
		 * If you want to give a reference to this module to another one, you should use Module::current() instead.
		 */
		static Module self() {
			return SC_CURRENT_MID;
		}
		
		/// Get the calling module.
		static Module current() {
			return sc_moduleIdGet(0);
		}
		
		/// \name Friends
		/// \{
		
		/// Add a friend to the calling module.
		static void addFriend(Module const m) {
			sc_moduleFriendAdd(m.id);
		}
		
		/// Remove a friend from the calling module.
		static void removeFriend(Module const m) {
			sc_moduleFriendRm(m.id);
		}
		
		/// Add all existing modules as a friend to the calling module.
		static void addAllAsFriend() {
			sc_moduleFriendAll();
		}
		
		/// Remove all friends from the calling module.
		static void removeAllFriends() {
			sc_moduleFriendNone();
		}
		
		/// Check whether a module is a friend of the calling module.
		static bool isFriend(Module const m) {
			return sc_moduleFriendGet(m.id) != 0;
		}
		
		/// \}
		
		/// Get a module by its name.
		static Module getByName(char const * name) {
			return Module(sc_moduleIdGet(name));
		}
		
		/// Get the name of the module.
		char const * name() const {
			return sc_moduleNameGet(id);
		}
		
};

#endif
