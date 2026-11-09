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
#  define DWG_HAVE_MMAP 1
#else
#  if defined(_WIN32)
#    include <stddef.h>
#    include <stdio.h> // off_t
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
void *mmap (void *addr, size_t len, int prot, int flags, int fd, off_t offset);
int munmap (void *addr, size_t len);
int msync (void *addr, size_t len, int flags);
#    define DWG_HAVE_MMAP 1
#  endif
# endif
#endif // DWG_MMAN_H
