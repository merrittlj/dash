#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <sys/stat.h>


extern int _write(int fd, char *ptr, int len);

extern int _fstat(int fd, struct stat *st);

extern void *_sbrk(int incr);

extern int _close(int fd);

extern int _isatty(int fd);

extern int _read(int fd, char *ptr, int len);

extern int _lseek(int fd, int ptr, int dir);

#endif
