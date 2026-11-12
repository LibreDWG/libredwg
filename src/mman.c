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
 * mman.c: minimal mmap(2) compatibility layer for Windows.
 * Implements the subset of the API LibreDWG uses:
 *   mmap()   map a file via CreateFileMapping()/MapViewOfFile()
 *   munmap() UnmapViewOfFile()
 *   msync()  FlushViewOfFile(), a no-op for private (write-copy) views
 *
 * A regular file may be mapped PROT_WRITE | MAP_PRIVATE (read with a private
 * copy).  Reads are served from the page cache without a heap copy, and the
 * few bytes the DXF reader appends at the end of the buffer (a trailing
 * newline / NUL) or that the writer writes into the overshoot stay in the
 * private copy and never touch the input file.
 *
 * The caller may close the FILE* (hence the underlying handle) while the
 * mapping is still alive (dat_unmap_file() does exactly that), so we keep
 * our own reference to the file.  A view cannot exceed 4 GiB on Windows,
 * bigger files have to fall back to the buffered read path.
 */

#include "config.h"
#include "mman.h"


/* File size via the fd, or -1 on error. Avoids struct stat differences. */
long long
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
#if defined(_WIN32) && !defined(HAVE_SYS_MMAN_H)

#  include <windows.h>
#  include <stdio.h>
#  include <io.h> // _get_osfhandle
#  include <errno.h>

struct dwg_mman_entry
{
  void *view;
  HANDLE hmap;
  HANDLE hfile;
  int private_map; // MAP_PRIVATE: writes and msync() do not reach the file
  void *reserved;  // VirtualAlloc base (MEM_RELEASE after UnmapViewOfFile)
  size_t reserved_len;
};

/* We map at most a few views at any time (input + output). */
#  define DWG_MMAN_MAX 32
static struct dwg_mman_entry dwg_mman_entries[DWG_MMAN_MAX];
static int dwg_mman_count = 0;

static int
win32_to_errno (DWORD err)
{
  switch (err)
    {
    case ERROR_ACCESS_DENIED:
      return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    case ERROR_INVALID_PARAMETER:
    case ERROR_INVALID_HANDLE:
    case ERROR_FILE_NOT_FOUND:
      return EBADF;
    default:
      return EIO;
    }
}

void *
mmap (void *addr, size_t len, int prot, int flags, int fd,
      dwg_mmap_off_t offset)
{
  HANDLE hfile, hfile2, hmap;
  DWORD protect, access;
  ULARGE_INTEGER maxsize;
  void *base, *view;
  (void)addr;
  if (!len || len > 0xFFFFFFFF)
    {
      errno = len ? EOVERFLOW : EINVAL;
      return MAP_FAILED;
    }
  hfile = (HANDLE)_get_osfhandle (fd);
  if (hfile == INVALID_HANDLE_VALUE)
    {
      errno = EBADF;
      return MAP_FAILED;
    }
  if (!DuplicateHandle (GetCurrentProcess (), hfile, GetCurrentProcess (),
                        &hfile2, 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
      errno = win32_to_errno (GetLastError ());
      return MAP_FAILED;
    }
  protect = (prot & PROT_WRITE)
                ? ((flags & MAP_PRIVATE) ? PAGE_WRITECOPY : PAGE_READWRITE)
                : PAGE_READONLY;
  /* CreateFileMapping fails with ERROR_NOT_ENOUGH_MEMORY when the section
     size exceeds the file size for PAGE_WRITECOPY/PAGE_READWRITE.  Cap the
     section at the file size; the view may still be larger via
     MapViewOfFileEx into reserved address space. */
  {
    LARGE_INTEGER fsize;
    if (GetFileSizeEx (hfile2, &fsize) && fsize.QuadPart > 0
        && (ULONGLONG)fsize.QuadPart < (ULONGLONG)len)
      maxsize.QuadPart = (ULONGLONG)fsize.QuadPart;
    else
      maxsize.QuadPart = len;
  }
  hmap = CreateFileMapping (hfile2, NULL, protect, maxsize.HighPart,
                            maxsize.LowPart, NULL);
  if (!hmap)
    {
      fprintf (stderr, "CreateFileMapping failed: %lu\n", GetLastError ());
      errno = win32_to_errno (GetLastError ());
      CloseHandle (hfile2);
      return MAP_FAILED;
    }
  access = (prot & PROT_WRITE)
               ? ((flags & MAP_PRIVATE) ? FILE_MAP_COPY : FILE_MAP_WRITE)
               : FILE_MAP_READ;
  /* A file view cannot extend past EOF, but callers may legally touch the
     bytes beyond it (the read path maps size+64KiB for its append overshoot,
     and the writer grows the chain).  Reserve the full range first and map
     into it; pages past EOF fault in as zero and COW writes stay private. */
  base = VirtualAlloc (NULL, len, MEM_RESERVE, PAGE_READWRITE);
  if (!base)
    {
      errno = ENOMEM;
      CloseHandle (hmap);
      CloseHandle (hfile2);
      return MAP_FAILED;
    }
  view = MapViewOfFileEx (hmap, access, (DWORD)((UINT64)offset >> 32),
                          (DWORD)offset, (DWORD)len, base);
  if (!view)
    {
      DWORD err = GetLastError ();
      fprintf (stderr, "MapViewOfFileEx failed: %lu\n", err);
      VirtualFree (base, 0, MEM_RELEASE);
      errno = win32_to_errno (err);
      CloseHandle (hmap);
      CloseHandle (hfile2);
      return MAP_FAILED;
    }
  if (dwg_mman_count >= DWG_MMAN_MAX)
    {
      UnmapViewOfFile (view);
      VirtualFree (base, 0, MEM_RELEASE);
      CloseHandle (hmap);
      CloseHandle (hfile2);
      errno = ENOMEM;
      return MAP_FAILED;
    }
  dwg_mman_entries[dwg_mman_count].view = view;
  dwg_mman_entries[dwg_mman_count].hmap = hmap;
  dwg_mman_entries[dwg_mman_count].hfile = hfile2;
  dwg_mman_entries[dwg_mman_count].private_map = (flags & MAP_PRIVATE) ? 1 : 0;
  dwg_mman_entries[dwg_mman_count].reserved = base;
  dwg_mman_entries[dwg_mman_count].reserved_len = len;
  dwg_mman_count++;
  return view;
}

int
munmap (void *addr, size_t len)
{
  int i;
  (void)len; // Windows always unmaps the whole view
  for (i = 0; i < dwg_mman_count; i++)
    {
      if (dwg_mman_entries[i].view == addr)
        {
          if (!UnmapViewOfFile (addr))
            {
              errno = win32_to_errno (GetLastError ());
              return -1;
            }
          if (dwg_mman_entries[i].reserved)
            {
              VirtualFree (dwg_mman_entries[i].reserved,
                           dwg_mman_entries[i].reserved_len, MEM_DECOMMIT);
              VirtualFree (dwg_mman_entries[i].reserved, 0, MEM_RELEASE);
            }
          CloseHandle (dwg_mman_entries[i].hmap);
          dwg_mman_entries[i] = dwg_mman_entries[--dwg_mman_count];
          return 0;
        }
    }
  errno = EINVAL;
  return -1;
}

int
msync (void *addr, size_t len, int flags)
{
  int i;
  (void)len;
  (void)flags;
  for (i = 0; i < dwg_mman_count; i++)
    {
      if (dwg_mman_entries[i].view == addr)
        {
          if (dwg_mman_entries[i].private_map)
            return 0; // private (COW) maps are never written back
          if (!FlushViewOfFile (addr, 0))
            {
              errno = win32_to_errno (GetLastError ());
              return -1;
            }
          return 0;
        }
    }
  errno = EINVAL;
  return -1;
}

#endif // !HAVE_SYS_MMAN_H && defined(_WIN32)
