#include <multiboot2.h>
#include <catk/kernel.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <stdint.h>

char * obtain_cmdline(uint32_t addr)
{
  struct multiboot_tag_string * string_tag = (struct multiboot_tag_string *)multiboot2_locate_tag(addr, MULTIBOOT_TAG_TYPE_CMDLINE);
  return string_tag->string;
}

char * get_cmdline_param_val(char * cmdline, char * name)
{
  char * p, * p2, c;
  size_t namelen = 0, vallen = 0;

  if(!name || !*name || !*cmdline)
  {
    return NULL;
  }

  for(p = name; *p; p++, namelen++);

  p = cmdline;

  while(*p)
  {
    if(memcmp(name, p, namelen) == 0)
    {
      c = p[namelen];

      // param=val or param=
      if(c == '=')
      {
        p += namelen + 1;
        p2 = p;

        while(*p2 && !isspace(*p2) && *p2 != '\t')
        {
          p2++;
        }

        vallen = p2 - p;

        if(!(p2 = malloc(vallen + 1)))
        {
          return NULL;
        }

        memcpy(p2, p, vallen);
        p2[vallen] = '\0';

        return p2;
      }
      else
      {
        return NULL;
      }
    }

    p++;
  }

  return NULL;
}


int has_cmdline_param(char * cmdline, char * name)
{
  char * p, * p2, c;
  size_t namelen = 0;

  if(!name || !name[0] || !cmdline[0])
  {
    return 0;
  }

  for(p = name; *p; p++, namelen++);

  p = cmdline;

  while(*p)
  {
    if(!(p2 = strstr(p, name)))
    {
      return 0;
    }

    c = p2[namelen];

    if(c == '\0' || isspace(c) || c == '\t' || c == '\n' || c == '=')
    {
      return 1;
    }

    p = p2 + namelen;
  }

  return 0;
}
