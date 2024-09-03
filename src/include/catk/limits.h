#ifndef __LIMITS_H
#define __LIMITS_H

#define DEVNAME_MAX	50	    /* device name length in mount table */
#define ARG_MAX		32	      /* length (in pages) of argv+env in 'execve' */
#define CHILD_MAX	64	      /* simultaneous processes per real user ID */
#define LINK_MAX	255	      /* maximum number of links to a file */
#define MAX_CANON	255	      /* bytes in a terminal canonical input queue */
#define MAX_INPUT	255	      /* bytes for which space will be available in a terminal input queue */
#define NGROUPS_MAX	32	    /* simultaneous supplementary group IDs */
#define OPEN_MAX	256	      /* files one process can have opened at once */

#ifndef FD_SETSIZE
#define FD_SETSIZE OPEN_MAX /* descriptors that a process may examine with 'pselect' or 'select' */
#endif

#define NAME_MAX	255	      /* bytes in a filename */
#define PATH_MAX	1024    	/* bytes in a pathname */
#define PIPE_BUF	4096	    /* bytes than can be written atomically to a pipe (cannot be bigger than PAGE_SIZE) */
#define UIO_MAXIOV	16	    /* maximum number of scatter/gather elements that can be processed in one call */

#endif
