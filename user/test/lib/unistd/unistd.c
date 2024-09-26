#include "unistd.h"

int write(int fd, const char * buf, int count) {
  int ret;
  /* '=a' means return the value in eax, 'a' means to input something into eax */ 
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x03), "b"(fd), "c"(buf), "d"(count));
  /* to understand this, basically, the middle part of the register name
   * isnt just junk, but kind of like an identifier.
   *
   * eAx
   * eBx
   * eCx
   * eDx
   *
   * Inline assembly lets us use these middle letters to specify which registers we want to use.
   * For example:
   *
   * uint32_t eax = 0xcafebabe;
   * asm volatile("int 0x80" :: "a"(eax));
   *
   * ^ ^ ^
   * That inputs EAX to the inline assembly function
   *
   */
  return ret;
}

int read(int fd, void * buf, int count) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x02), "b"(fd), "c"(buf), "d"(count));
  return ret;
}

int open(const char * pathname, int flags, uint16_t mode) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x04), "b"(pathname), "c"(flags), "d"(mode));
  return ret;
}

int fork(void) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x05));
  return ret;
}

int access(const char * pathname, int mode) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x06), "b"(pathname), "c"(mode));
  return ret;
}

void _exit(int error_code) {
  asm volatile("int $0x80" :: "a"(0x01), "b"(error_code));
}

int wait(int * status) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x07), "b"(status));
  return ret;
}

int chdir(const char * pathname) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x08), "b"(pathname));
  return ret;
}

int execv(const char * pathname, char * argv[]) {
  int ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(0x09), "b"(pathname), "c"(argv), "d"(NULL));
  return ret;
}

void beep(int ms) {
  asm volatile("int $0x80" :: "a"(0x0a), "b"(ms));
}
