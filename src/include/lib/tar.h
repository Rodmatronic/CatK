#ifndef _TAR_H
#define _TAR_H

#include <stdint.h>

/* general definitions */
#define TAR_MAGIC        "ustar"
#define TAR_MAGIC_LEN    5
#define TAR_BLOCK_SIZE   512

/* type flag definitions */
#define TAR_NORMAL_FILE '0'
#define TAR_HARD_LINK   '1'
#define TAR_SYM_LINK    '2'
#define TAR_CHAR_DEV    '3'
#define TAR_BLOCK_DEV   '4'
#define TAR_DIRECTORY   '5'
#define TAR_FIFO        '6'

struct tar_inode
{
  char filename[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag[1];
};

#endif
