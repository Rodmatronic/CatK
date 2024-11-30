#ifndef __PARAMS_H
#define __PARAMS_H

char * obtain_cmdline(void);
char * get_cmdline_param_val(char * cmdline, char * name);
int has_cmdline_param(char * cmdline, char * name);

#endif
