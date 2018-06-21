/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010, 2018 Free Software Foundation, Inc.            */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg.vapi: vapi bindings
 * written by Reini Urban
 */

namespace LibreDWG {

  [CCode (cname="Dwg_Bitcode_TimeBLL", cprefix="Dwg_Bitcode_", has_type_id = false)]
  public struct TimeBLL {
    public uint32 days;
    public uint32 ms;
    public double value;
  }

  [CCode (cprefix="DWG_")]
  public enum Error {
      NOERR = 0,
      /* sorted by severity */
      ERR_WRONGCRC         = 1,
      ERR_NOTYETSUPPORTED  = 2,
      ERR_UNHANDLEDCLASS   = 4,
      ERR_INVALIDTYPE      = 8,
      ERR_INVALIDHANDLE    = 16,
      ERR_INVALIDEED       = 32,
      ERR_VALUEOUTOFBOUNDS = 64,
      /* -- critical errors: --- */
      ERR_CRITIAL          = 128,
      ERR_CLASSESNOTFOUND  = 128,
      ERR_SECTIONNOTFOUND  = 256,
      ERR_PAGENOTFOUND     = 512,
      ERR_INTERNALERROR    = 1024,
      ERR_INVALIDDWG       = 2048,
      ERR_IOERROR          = 4096,
      ERR_OUTOFMEM         = 8192,
  }

  //[Compact]
  //[CCode (cheader_filename="dwg.h", cname="dwg", free_function="dwg_free", cprefix="dwg_")]
  //public class Dwg {
    //public int add_object (Dwg_Data *dwg);
    //public void free_object (Dwg_Object *dwg);
    //public void free (Dwg_Data *dwg);
  //}

}
