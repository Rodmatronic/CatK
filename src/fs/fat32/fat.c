#include <catk/fat32.h>
#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <config.h>
#include <lib/common.h>
#include <lib/ctype.h>

static int fat32_lba_start = 0;

static struct filesystem * fat32 = NULL;
static struct boot_sector * bs = NULL;
struct fs_operations fat32_ops;
struct file_operations fat32_fops;
struct fs_info * fat32_fsinfo = NULL;

static bool fsinfo_verify_signatures(void)
{
  printk("FSINFO sig1: 0x%08x\n", fat32_fsinfo->signature1);
  printk("FSINFO sig2: 0x%08x\n", fat32_fsinfo->signature2);
  printk("FSINFO sig3: 0x%08x\n", fat32_fsinfo->signature3);
  return (fat32_fsinfo->signature1 == FAT32_FSINFO_SIG1 && fat32_fsinfo->signature2 == FAT32_FSINFO_SIG2 && fat32_fsinfo->signature3 == FAT32_FSINFO_SIG3);
}

int fat32_mount(struct filesystem * fs, struct device * blkdev)
{
  fat32_lba_start = blkdev->fops->firstpart();
  fat32 = fs;
  uint8_t buffer[512];
  blkdev->fops->lseek(NULL, 0, SEEK_SET);
  blkdev->fops->read(NULL, buffer, 1);
  bs = (struct boot_sector *)buffer;
  if(bs->signature != BOOT_SECTOR_SIG)
  {
    printk("Invalid bootsector signature: 0x%04x\n", bs->signature);
    return -EINVAL;
  }
  blkdev->fops->lseek(NULL, bs->fat32_rec.fsinfo_sect_num, SEEK_SET);
  blkdev->fops->read(NULL, buffer, 1);
  fat32_fsinfo = (struct fs_info *)buffer; /* convert data into fsinfo struct */
  if(!fsinfo_verify_signatures())
  {
    printk("FAT32: FSINFO verification failed\n");
    return -EINVAL;
  }
  return 0;
}

int fat32_init(void)
{
  return register_filesystem("FAT32", &fat32_ops, &fat32_fops, FS_MOUNT_DISK);
}

struct fs_operations fat32_ops = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  fat32_mount
};

struct file_operations fat32_fops = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};
