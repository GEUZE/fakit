/*SOC -*-c++-*-
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03070CL48                                                   **
** +Revision: 1.97 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** SDD API                                                          **
**********************************************************************
EOC*/

#ifndef _GDD_SDD_H_
#define _GDD_SDD_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>

#include <sdd/sdd.msg>

  /**
   * To free correctly an object description. It does first a release and then 
   * frees up the description.
   * \param[in]  self    Object description 
   */
  void sdd_objFree (sdd_obj_t NEARPTR NEARPTR self);
  /** 
   * Duplicate an object description. 
   * \param[in]  self    Object description 
   * \return             NULL on error else the duplicated object descriptor. 
   */
  sdd_obj_t NEARPTR sdd_objDup (const sdd_obj_t NEARPTR self);
  /**
   * Get last set time of object.
   * \param[in]  self    Object description 
   * \param[in]  data    date time
   * \return             -1 on error. 
   */
  int sdd_objTimeGet (const sdd_obj_t NEARPTR self, __u32 *data);
  /**
   * Set time of object.
   * \param[in]  self    Object description 
   * \param[in]  data    date time
   * \return             -1 on error. 
   */
  int sdd_objTimeSet (const sdd_obj_t NEARPTR self, __u32 data);
  /**
   * Get size of object, this could be cache size or file size or any other
   * size a object could have.
   * \param[in]  self    Object description 
   * \param[out] free    Free space
   * \param[out] total   Total available space
   * \return             -1 on error. 
   */
  int sdd_objSizeGet (const sdd_obj_t NEARPTR self, sdd_size_t * size);
  /**
   * Get description object from a process.
   * \param[in]  pid     Process id
   * \param[in]  name    Name of object if multiple are hold 
   * \return             NULL on error.
   */
  sdd_obj_t NEARPTR sdd_objDesc (sc_pid_t pid, const char *name);
  /**
   * Get object tag info
   * \param[in]  self    Object description
   * \param[in]  tag     Requested Tag
   * \param[out] data    Requested Data
   * \param[in]  size    Size of the data holder
   * \return             -1 on error
   */
  int sdd_objTagGet (const sdd_obj_t NEARPTR self, 
		     sc_tag_t tag, 
		     void *data,
                     size_t size);
  /**
   * Set object tag info
   * \param[in]  self    Object description
   * \param[in]  tag     Tag which will be manipulated
   * \param[in]  data    Data
   * \param[in]  size    Size of the data holder
   * \return             -1 on error
   */
  int sdd_objTagSet (const sdd_obj_t NEARPTR self, 
		     sc_tag_t tag, 
		     const void *data,
                     size_t size);
  /**
   * Add an object description to a manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  object  Object description
   * \return             -1 on error
   */
  int sdd_manAdd (const sdd_obj_t NEARPTR self, 
		  sdd_obj_t NEARPTR NEARPTR object);
  /**
   * Remove an object description from a manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  ref     Object description of the reference
   * \param[in]  size    Size of the expected object description
   * \return             -1 on error
   */
  int sdd_manRm (const sdd_obj_t NEARPTR self, 
		 const sdd_obj_t NEARPTR ref, 
		 __uint size);
  /**
   * Get an object description from a manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  ref     Object description of the reference
   * \param[in]  size    Size of the expected object description
   * \return             NULL on error else the requested object descriptor
   */
  sdd_obj_t NEARPTR sdd_manGet (sdd_obj_t NEARPTR self, sdd_obj_t NEARPTR ref,
                                int size);
    
  /**
   * Get the first object description from a manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  size    Size of the expected object description
   * \return             NULL on error else the requested object descriptor
   */
  sdd_obj_t NEARPTR sdd_manGetFirst (const sdd_obj_t NEARPTR self, __uint size);
  /**
   * Get an object description starting from the reference description from a 
   * manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  ref     Object description of the reference
   * \param[in]  size    Size of the expected object description
   * \return             NULL on error else the requested object descriptor
   */
  sdd_obj_t NEARPTR sdd_manGetNext (const sdd_obj_t NEARPTR self,
                                    const sdd_obj_t NEARPTR ref, __uint size);
  /**
   * Notify of object adds. 
   * manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  name    Object name on which we are waiting on.
   * \param[in]  tmo     How long we want at max waiting on.    
   * \return             -1 on error else 0 
   */
  int sdd_manNotifyAdd (const sdd_obj_t NEARPTR man, const char *name, 
                        sc_ticks_t tmo);
  /**
   * Notify of object removes. 
   * manager.
   * \param[in]  self    Object description of the manager
   * \param[in]  name    Object name on which we are waiting on.
   * \param[in]  tmo     How long we want at max waiting on.    
   * \return             -1 on error else 0 
   */
  int sdd_manNotifyRm (const sdd_obj_t NEARPTR man, const char *name, 
                       sc_ticks_t tmo);

  /**
   * Open an object of type SDD_DEV_TYPE with the specified flags.
   * \param[in]  self    Object description of the device
   * \param[in]  flags   O_RDONLY, O_WRONLY, O_RDWR, O_CREATE, O_TRUNC
   * \return             -1 on error
   */
  int sdd_devOpen (sdd_obj_t NEARPTR self, flags_t flags);
  /**
   * Close an object of type SDD_DEV_TYPE.
   * \param[in]  self    Object description of the device
   * \return             -1 on error
   */
  int sdd_devClose (const sdd_obj_t NEARPTR self);
  /**
   * Read from an SDD_DEV_TYPE object
   * \param[in]  self    Object description of the device
   * \param[in]  buf     Memory to which the read should write to
   * \param[in]  size    Size of bytes which the user wants to read
   * \return             -1 on error else number of bytes read
   */
  ssize_t sdd_devRead (const sdd_obj_t NEARPTR self, __u8 * buf, ssize_t size);
  /**
   * Asynchron read from a object of type SDD_DEV_TYPE. The user then has to 
   * get a message with id SDD_DEV_READ_REPLY to get the data.
   * \param[in]  self    Object description of the device
   * \param[in]  size    Size of bytes which the user wants to read
   * \return             -1 on error
   */
  int sdd_devAread (const sdd_obj_t NEARPTR self, ssize_t size);
  /**
   * Write to an SDD_DEV_TYPE object.
   * \param[in]  self    Object description of the device
   * \param[in]  buf     Memory to which the read should write to
   * \param[in]  size    Size of bytes which the user wants to read
   * \return             -1 on error else number of bytes written
   */
  ssize_t sdd_devWrite (const sdd_obj_t NEARPTR self, const __u8 * buf,
                        ssize_t size);
  /**
   * Asynchron write to an SDD_DEV_TYPE object. The user then has to wait for
   * a message with id SDD_DEV_WRITE_REPLY.
   * \param[in]  self    Object description of the device
   * \param[in]  buf     Memory to which the read should write to
   * \param[in]  size    Size of bytes which the user wants to write
   * \return             -1 don error
   */
  int sdd_devAwrite (const sdd_obj_t NEARPTR self, 
		     const char *buf, ssize_t size);
  /**
   * This function should be replaced with the tag system. It is only here for
   * old software which are not allready ported to the newer system.
   */
  int sdd_devIoctl (const sdd_obj_t NEARPTR self, unsigned int cmd,
                    unsigned long arg);
  /**
   * Seek in a SDD_FILE_TYPE object.
   * \param[in]  self     Object description of the file
   * \param[in]  off      Offset of bytes to seek
   * \param[in]  whence   SEEK_SET, SEEK_CUR, SEEK_END
   * \return              -1 on error else the new offset
   */
  off_t sdd_fileSeek (const sdd_obj_t NEARPTR self, off_t off, int whence);
  /**
   * Set the new file size (truncation or expantion)
   * \param[in]  self     Object description of the file
   * \param[in]  size     The new size of the file
   * \return              -1 on error else the new size
   * \todo                This function is not implemented yet
   */
  ssize_t sdd_fileResize (const sdd_obj_t NEARPTR self, size_t size);
  /**
   * Open an object of type SDD_NET_TYPE. It does tell the network driver 
   * where to send the received data.
   * This function are normaly used by protocols.
   * \param[in]  self     Object description of the network driver
   * \param[in]  proto    The procotol id to be able to route back the answer 
   *                      message if a proxy is in between, else 0.
   * \param[in]  pid      The pid to which the network driver should send the 
   *                      data (proxy in between for example SCP_link).
   *                      if not specified the caller is will receive the data
   * \return              -1 don error
   */
  int sdd_netOpen (sdd_obj_t NEARPTR self, __u32 proto, sc_pid_t link);
  /**
   * Close an object of type SDD_NET_TYPE. 
   * This function are normaly used by protocols.
   * \param[in]  self     Object description of the network driver
   * \return              -1 don error
   */
  int sdd_netClose (const sdd_obj_t NEARPTR self);

  /**
   * Creates a new object description. Only for internal use.
   */
  sdd_obj_t NEARPTR sdd_objNew (const char *name, 
				sc_pid_t controller,
                                sc_pid_t sender, 
				sc_pid_t receiver,
                                void NEARPTR handle, 
				sc_poolid_t plid,
                                sc_ticks_t tmo);
  /**
   * Open an SDD_DEV_TYPE device on a specified path.
   * \param[in]  root      Root manager object description, start for the path 
   *                       evaluation
   * \param[in]  path      Path to the device
   * \param[in]  flags     The same like for the sdd_devOpen function
   * \return               Object description of the device specified in the 
   *                       path
   */
  sdd_obj_t NEARPTR sdd_devOpenByName (const sdd_obj_t NEARPTR root,
                                       const char *path, flags_t flags);
  /**
   * Set a process as root manager. Get an object description of this root 
   * manager.
   * \param[in]  process   Process name (relativ or fully qualified)
   * \param[in]  name      Name the object descriptor should get
   * \param[in]  plid      Pool for memory allocation
   * \param[in]  tmo       How long to try to get the object descriptor
   * \return               NULL on error else object description for the root 
   *                       manager
   */
  sdd_obj_t NEARPTR sdd_manGetRoot (const char *process, const char *name,
                                    sc_poolid_t plid, sc_ticks_t tmo);
  /**
   * Get an object by name from a manager (name != path).
   * \param[in]  self      Object descripion of the manager
   * \param[in]  name      Name of the object descriptor
   * \return               NULL on error else object description
   */
  sdd_obj_t NEARPTR sdd_manGetByName (const sdd_obj_t NEARPTR self,
                                      const char *name);

  sdd_obj_t NEARPTR 
  sdd_manGetByRelPath (const sdd_obj_t NEARPTR self, const char *path);

  /**
   * Get an object by path from a manager (name != path).
   * \param[in]  self      Object descripion of the manager
   * \param[in]  name      Name of the object descriptor
   * \return               NULL on error else object description
   */
  sdd_obj_t NEARPTR sdd_manGetByPath (const sdd_obj_t NEARPTR self,
                                      const char *path);
  /**
   * Resolve a path and return last manager in the path.
   * \param[in]  self      Starting manager object description
   * \param[in]  path      Path to the object.
   * \param[out] last      Gives back the rest of the path starting from the
   *                       last manager found in the path
   * \return               NULL on error else objct description of the last
   *                       manager in the path
   */
  sdd_obj_t NEARPTR sdd_objResolve (const sdd_obj_t NEARPTR self, 
				    const char *path,
                                    const char **last);
  /**
   * If you use this function for netbufs send to a protocol, you need to set 
   * the header to  68 and the tail should not be zero for connection oriented 
   * protocols like TCP else you could get a very poor network performance and 
   * a big memory consumtion.
   * \param[in]  head      Size of the header in the netbuf
   * \param[in]  data      Size of the data in the netbuf
   * \param[in]  tail      Size of the tail
   * \param[in]  plid      Pool for memory allocation
   * \param[in]  tmo       Timeout for allocation 
   * \return               A new netbuf
   */
  sdd_netbuf_t NEARPTR sdd_netbufAlloc (size_t head, size_t data,
                                        size_t tail, sc_poolid_t plid,
                                        sc_ticks_t tmo);
  /**
   * To free a netbuf it just checks the pointer and does the casting for
   * sc_msgFree.
   * \param[in]  buffer    A netbuf
   */
  void sdd_netbufFree (sdd_netbuf_t NEARPTR NEARPTR buffer);
  /**
   * To copy a netbuf's data and alloc a new header and tail.
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   * \param[in]  newHeadRoom The new header space for the copy
   * \param[in]  newTailRoom The new tail space for the copy
   * \param[in]  plid      Pool for memory allocation
   * \param[in]  tmo       Timeout for allocation
   * \return               A copy of buffer
   */
  sdd_netbuf_t NEARPTR sdd_netbufCopy (const sdd_netbuf_t NEARPTR buffer,
                                       ssize_t newHeadRoom,
                                       ssize_t newTailRoom, sc_poolid_t plid,
                                       sc_ticks_t tmo);
  /**
   * Reallocation of a netbuf.
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[out] buffer    A netbuf, will be replaced with a new one
   * \param[in]  newDataroom The new size of data
   * \param[in]  tmo       timeout for allocation (we use the same pool like 
   *                       for buffer)
   */
  void sdd_netbufRealloc (sdd_netbuf_t NEARPTR NEARPTR buffer,
                          ssize_t newDataRoom, sc_ticks_t tmo);
  /**
   * Pull away from data -> add this to header
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   * \param[in]  len       Number of bytes which should be pulled
   */
  void sdd_netbufPull (sdd_netbuf_t NEARPTR buffer, ssize_t len);
  /**
   * Trim the data -> add this to tail
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   * \param[in]  len       Number of bytes which should be trimmed 
   */
  void sdd_netbufTrim (sdd_netbuf_t NEARPTR buffer, size_t len);
  /**
   * Push bytes to data -> remove this from head 
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   * \param[in]  len       Number of bytes which should be pushed 
   */
  void sdd_netbufPush (sdd_netbuf_t NEARPTR buffer, ssize_t len);
  /**
   * Put bytes to data -> remove this from tail 
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   * \param[in]  len       Number of bytes which should be putted 
   */
  void sdd_netbufPut (sdd_netbuf_t NEARPTR buffer, ssize_t len);
  /**
   * Move the cur pointer 
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   * \param[in]  len       Number of bytes which should be moved 
   */
  void sdd_netbufCur (sdd_netbuf_t NEARPTR buffer, ssize_t len);
  /**
   * Set cur pointer to data
   * This functions are mainly used for protocol stacks and should not be used
   * in any application.
   * \param[in]  buffer    A netbuf
   */
  void sdd_netbufCurReset (sdd_netbuf_t NEARPTR buffer);

  sdd_netbuf_t NEARPTR sdd_netbufDup(const sdd_netbuf_t NEARPTR buffer,
				     sc_poolid_t plid,
				     sc_ticks_t tmo);

#ifdef __cplusplus
}
#endif
#endif
