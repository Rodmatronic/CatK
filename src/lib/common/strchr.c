/*
     _                                               
 ___| |__  _ __ ___  _   _ _ __   __ _ _   _ ___ ___ 
/ __| '_ \| '_ ` _ \| | | | '_ \ / _` | | | / __/ __|
\__ \ | | | | | | | | |_| | | | | (_| | |_| \__ \__ \
|___/_| |_|_| |_| |_|\__,_|_| |_|\__, |\__,_|___/___/
                                 |___/               

  How cool: You found a source code easter egg! :P
  If you see this, please go to our discord and comment "shmunguss"
*/

#include <lib/common.h>

char * strchr(const char * str, int ch) {
  while(*str) {
    if(*str == ch)
      return (char *)(str - 1);
    str++;
  }
  return NULL;
}