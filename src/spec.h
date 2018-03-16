/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#ifdef IS_ENCODER
#define ENCODER if (1)
#define DECODER if (0)
#endif

#ifdef IS_DECODER
#define ENCODER if (0)
#define DECODER if (1)
#undef IF_ENCODE_FROM_EARLIER
#define IF_ENCODE_FROM_EARLIER if (0)
#endif

#ifdef IS_PRINT
#define ENCODER if (0)
#define DECODER if (0)
#undef IF_ENCODE_FROM_EARLIER
#define IF_ENCODE_FROM_EARLIER if (0)
#endif
