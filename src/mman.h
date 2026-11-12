/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2025 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * mman.h: portable access to the mmap(2) API.
 * On POSIX this just includes <sys/mman.h>.
 * On Windows we provide the two calls we need (mmap, munmap, plus msync)
 * as a small compat implementation in mman.c on top of
 * CreateFileMapping()/MapViewOfFile().
 * DWG_HAVE_MMAP is defined if either is available.
 */

#ifndef DWG_MMAN_H
#define DWG_MMAN_H

#ifdef HAVE_SYS_MMAN_H
#  include <sys/mman.h>
#  include <sys/stat.h>
#  define DWG_HAVE_MMAP 1
#else
#  if defined(_WIN32)
#    include <stddef.h>
#    include <stdio.h> // size_t
#    include <fcntl.h>
#    include <io.h> // _fileno, _chsize_s
#    include <sys/stat.h>
#    include <sys/types.h>
#    ifndef _MSC_VER
#      include <unistd.h> // fileno, ftruncate
#    endif
#    ifndef PROT_NONE
#      define PROT_NONE 0x0
#    endif
#    ifndef PROT_READ
#      define PROT_READ 0x1
#    endif
#    ifndef PROT_WRITE
#      define PROT_WRITE 0x2
#    endif
#    ifndef PROT_EXEC
#      define PROT_EXEC 0x4
#    endif
#    ifndef MAP_SHARED
#      define MAP_SHARED 0x01
#    endif
#    ifndef MAP_PRIVATE
#      define MAP_PRIVATE 0x02
#    endif
#    ifndef MAP_FAILED
#      define MAP_FAILED ((void *)-1)
#    endif
#    ifndef MS_ASYNC
#      define MS_ASYNC 1
#    endif
#    ifndef MS_SYNC
#      define MS_SYNC 2
#    endif
/* off_t does not exist on MSVC (and is 32-bit on 32-bit MinGW): use a
   fixed 64-bit type like the LONGLONG offset MapViewOfFile wants. */
typedef long long dwg_mmap_off_t;
#    ifdef _MSC_VER
#      define fileno _fileno
#      ifndef ftruncate
static inline int
ftruncate (int fd, long long length)
{
  return _chsize_s (fd, length);
}
#      endif
#    endif
void *mmap (void *addr, size_t len, int prot, int flags, int fd,
            dwg_mmap_off_t offset);
int munmap (void *addr, size_t len);
int msync (void *addr, size_t len, int flags);
#    define DWG_HAVE_MMAP 1
#  endif
#endif

#ifndef _WIN32
#  include <sys/stat.h>
#  include <unistd.h>
#endif
/* File size via the fd, or -1 on error. Avoids struct stat differences. */
static inline long long
dwg_fstat_size (int fd)
{
#ifdef _MSC_VER
  struct _stat st;
  if (_fstat (fd, &st))
    return -1;
#else
  struct stat st;
  if (fstat (fd, &st))
    return -1;
#endif
  return (long long)st.st_size;
}

#endif // DWG_MMAN_H
