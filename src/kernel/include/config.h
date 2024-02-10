#pragma once
#include "stdint.h"

static char * sys_arch = "X86/intel";
static char * sys_term = "80x25";
static char * sys_sesh = "/bin/k_sh";
static char * sys_ver = "0.072";
static char * sys_home = "/home/root";
static char * sys_name = "Catkernel";
static char * sys_user = "root";
static char * sys_mountpoint = "/";
static char * sys_pwd = "/";
extern char cmdline[256];
extern char loader[256];
extern uint32_t counter;
