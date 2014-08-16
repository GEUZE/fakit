// This file is part of Sciopta++.
// See COPYING for license details.

/// \file
/// Provides the Pool class.

#include <sciopta.h>

#ifndef __POOL_HPP
#define __POOL_HPP

/// A Sciopta message pool.
/**
 * Only buffers (ie. messages) of predefined sizes can be allocated in a pool.
 * If a message of a not prededfined size is allocated, the smallest larger size is used instead.
 */
class Pool {
	
	protected:
		sc_poolid_t id;
		
		friend class Message;
		friend class Process;
		
		operator sc_poolid_t () const {
			return id;
		}
		
	public:
		Pool() : id(SC_ILLEGAL_POOLID) {}
		
		Pool(sc_poolid_t id) : id(id) {}
		
		bool operator !            () const { return id != SC_ILLEGAL_POOLID           ; }
		     operator const void * () const { return id == SC_ILLEGAL_POOLID ? 0 : this; }
		
		bool operator == (Pool p) const { return p.id == id; }
		bool operator != (Pool p) const { return p.id != id; }
		
		/// Create a new pool in the current module.
		/**
		 * \param name The name of the pool, with a maximum of 31 characters.
		 * \param size The size of the pool.
		 *             (Including the size of the control block, which is 68 + n * 20,
		 *              where n is the maximum number of message buffer sizes for the whole system.
		 *              Add another n * 20 if process or message statistics are used.)
		 * \param n_buffer_sizes The number of buffer sizes (either 4, 8 or 16, and not higher that the system wide maximum.)
		 * \param buffer_sizes The buffer sizes, in ascending order.
		 * \param start The address to create the pool at. If omitted, the next available address in the module is used.
		 * \return The created pool.
		 */
		static Pool create(
			char const * name,
			sc_plsize_t size,
			size_t n_buffer_sizes,
			sc_bufsize_t const * buffer_sizes,
			void * start = 0
		) {
			return sc_poolCreate(
				reinterpret_cast<char *>(start),
				size,
				n_buffer_sizes,
				const_cast<sc_bufsize_t *>(buffer_sizes),
				name
			);
		}
		
		/// Kill the pool.
		/**
		 * \warning A pool can only be killed if all the messages in it are already freed.
		 * \note Every process inside a module can kill a pool.
		 */
		void kill() {
			sc_poolKill(id);
		}
		
		/// Reset the pool.
		/**
		 * \warning A pool can only be reset if all the messages in it are already freed.
		 * \note Every process inside a module can kill a pool.
		 */
		void reset() {
			sc_poolReset(id);
		}
		
		/// \name Default pool
		/// \{
		
		/// The default pool of the process.
		/**
		 * To be used when allocating a message.
		 */
		static sc_poolid_t const process_default = SC_DEFAULT_POOL;
		
		/// The invalid value of a Pool object.
		/**
		 * You can check the validness of a Pool like a boolean:
		 * \code
		 * if (!my_pool) panic();
		 * \endcode
		 */
		static sc_poolid_t const invalid = SC_ILLEGAL_POOLID;
		
		/// Set the pool as the process's default pool.
		/**
		 * The default pool is used to allocate messages in when using <code>Pool::default</code>
		 * and when the kernel has to copy a message from another module.
		 * 
		 * The default default pool is the first pool of the module.
		 */
		void setProcessDefault() {
			sc_poolDefault(id);
		}
		
		static Pool getProcessDefault() {
			return sc_poolIdGet(0);
		}
		
		/// \}
		
		/// Get a pool by its name.
		/**
		 * \param name The name of the pool.
		 * \return The pool if it exists, <code>Pool::invalid</code> otherwise.
		 */
		static Pool getByName(char const * name) {
			return sc_poolIdGet(name);
		}
		
};

#endif
