import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgLWPolylineEntity extends DwgEntity {
  type: 'LWPOLYLINE'
  /*
   * DXF group code: 70
   *
   * Polyline flag (bit-coded); default is 0:
   * - 1 extrusion
   * - 2 thickness
   * - 4 constwidth
   * - 8 elevation
   * - 16 num_bulges
   * - 32 has_widths
   * - 128 plinegen
   * - 512: closed
   * - 1024 vertexidcount
   *
   * ⚠️ **Warning:** The meaning of this field is different from the original meaning in DXF!
   */
  flag: number
  numberOfVertices: number
  constantWidth?: number
  elevation: number
  thickness: number
  extrusionDirection: DwgPoint3D
  vertices: DwgLWPolylineVertex[]
}

export interface DwgLWPolylineVertex extends DwgPoint2D {
  id: number
  startWidth?: number
  endWidth?: number
  bulge: number
}
