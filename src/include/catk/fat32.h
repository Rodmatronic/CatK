#ifndef __FAT32_H
#define __FAT32_H

#include <catk/fs.h>
#include <catk/device.h>
#include <catk/compiler.h>
#include <stdint.h>

struct boot_param_block
{
  uint8_t code_jmp[3];
  char oem_ident[8];
  uint16_t bytes_per_sect;
  uint8_t sect_per_clust;
  uint16_t reserved_sect;
  uint8_t num_fat;
  uint16_t num_root;
  uint16_t total_sectors;
  uint8_t media_descriptor;
  uint16_t num_sect_per_fat;
  uint16_t sect_per_track;
  uint16_t num_heads;
  uint32_t num_hidden_sectors;
  uint32_t large_sect;
}_packed_;

struct fat32_ext_record
{
  uint32_t fat_size;
  uint16_t flags;
  uint16_t fat_version;
  uint32_t root_clust;
  uint16_t fsinfo_sect_num;
  uint16_t boot_sect_backup;
  uint8_t reserved1[12];
  uint8_t drive_num;
  uint8_t reserved2;
  uint8_t signature;
  uint32_t volume_id;
  char volume_label[8];
}_packed_;

struct boot_sector
{
  struct boot_param_block bpb;
  struct fat32_ext_record fat32_rec;
  uint8_t boot_code[431];
  uint16_t signature;
}_packed_;

struct fs_info
{
  uint32_t signature1;
  uint8_t reserved1[480];
  uint32_t signature2;
  uint32_t last_free_clust;
  uint32_t clust_avail;
  uint8_t reserved2[12];
  uint32_t signature3;
}_packed_;

struct fat32_priv_data
{
  uint8_t fizzbuzz;
};

#define FAT32_FIRST_CLUSTER   2
/* all signatures are reversed due to difference in endianess */
#define FAT32_FSINFO_SIG1     0x41615252
#define FAT32_FSINFO_SIG2     0x61417272
#define FAT32_FSINFO_SIG3     0xaa550000
#define BOOT_SECTOR_SIG       0xaa55

struct device;
struct filesystem;

int fat32_init(void);

#endif
