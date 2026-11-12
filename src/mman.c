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
 * copy).  Reads are served from the page cache without a heap copy, and
 * writes to the private view never touch the input file.
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
  int heap;        // malloc+read fallback (view not file-backed)
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
  HANDLE hfile, hfile2 = NULL, hmap = NULL;
  DWORD protect, access;
  ULARGE_INTEGER maxsize;
  LARGE_INTEGER fsize;
  void *view;
  int heap = 0;
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
  if (!GetFileSizeEx (hfile, &fsize))
    {
      errno = win32_to_errno (GetLastError ());
      return MAP_FAILED;
    }
  /* Windows file views cannot extend past EOF: CreateFileMapping rejects a
     section larger than the file, and MapViewOfFile rejects a view larger
     than the section.  Oversized private mappings use a private heap copy. */
  if ((flags & MAP_PRIVATE) && (LONGLONG)len > fsize.QuadPart)
    {
      view = calloc (1, len);
      if (!view)
        {
          errno = ENOMEM;
          return MAP_FAILED;
        }
      heap = 1;
      { LARGE_INTEGER off; off.QuadPart = offset;
        SetFilePointerEx (hfile, off, NULL, FILE_BEGIN); }
      {
        DWORD rd = 0;
        LONGLONG avail = fsize.QuadPart - (LONGLONG)offset;
        DWORD to_read = avail > (LONGLONG)len ? (DWORD)len
                            : avail > 0 ? (DWORD)avail
                                        : 0;
        if (to_read && !ReadFile (hfile, view, to_read, &rd, NULL))
          {
            DWORD err = GetLastError ();
            free (view);
            errno = win32_to_errno (err);
            return MAP_FAILED;
          }
      }
      goto track;
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
  maxsize.QuadPart = len;
  hmap = CreateFileMapping (hfile2, NULL, protect, maxsize.HighPart,
                            maxsize.LowPart, NULL);
  if (!hmap)
    {
      errno = win32_to_errno (GetLastError ());
      CloseHandle (hfile2);
      return MAP_FAILED;
    }
  access = (prot & PROT_WRITE)
               ? ((flags & MAP_PRIVATE) ? FILE_MAP_COPY : FILE_MAP_WRITE)
               : FILE_MAP_READ;
  /* MapViewOfFileEx requires the base to be aligned to the allocation
     granularity (64KiB); VirtualAlloc does not guarantee that for an
     arbitrary size.  MapViewOfFile lets Windows pick the address. */
  view = MapViewOfFile (hmap, access, (DWORD)((UINT64)offset >> 32),
                        (DWORD)offset, (DWORD)len);
  if (!view)
    {
      DWORD err = GetLastError ();
      errno = win32_to_errno (err);
      CloseHandle (hmap);
      CloseHandle (hfile2);
      return MAP_FAILED;
    }
track:
  if (dwg_mman_count >= DWG_MMAN_MAX)
    {
      if (heap)
        free (view);
      else
        {
          UnmapViewOfFile (view);
        }
      if (hmap)
        CloseHandle (hmap);
      if (hfile2)
        CloseHandle (hfile2);
      errno = ENOMEM;
      return MAP_FAILED;
    }
  dwg_mman_entries[dwg_mman_count].view = view;
  dwg_mman_entries[dwg_mman_count].hmap = hmap;
  dwg_mman_entries[dwg_mman_count].hfile = hfile2;
  dwg_mman_entries[dwg_mman_count].private_map = (flags & MAP_PRIVATE) ? 1 : 0;
  dwg_mman_entries[dwg_mman_count].heap = heap;
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
          if (dwg_mman_entries[i].heap)
            free (addr);
          else
            {
              if (!UnmapViewOfFile (addr))
                {
                  errno = win32_to_errno (GetLastError ());
                  return -1;
                }
            }
          if (dwg_mman_entries[i].hmap)
            CloseHandle (dwg_mman_entries[i].hmap);
          if (dwg_mman_entries[i].hfile)
            CloseHandle (dwg_mman_entries[i].hfile);
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
