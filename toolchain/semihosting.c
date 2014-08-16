struct stat;
int _fstat (__attribute__((unused))int fd, __attribute__((unused))struct stat * buf) { return 0; }
int _close (__attribute__((unused))int fd) { return 0; }
int _isatty(__attribute__((unused))int fd) { return 0; }
int _lseek (__attribute__((unused))int fd, __attribute__((unused))int offset, __attribute__((unused))int whence) { return 0; }
int _read  (__attribute__((unused))int fd, __attribute__((unused))void * buf, __attribute__((unused))unsigned int length) { return 0; }
int _write (__attribute__((unused))int fd, __attribute__((unused))void const * buf, unsigned int length) { return length; }
