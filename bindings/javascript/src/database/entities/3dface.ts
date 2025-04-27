import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface Dwg3dFaceEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: '3DFACE'
  /**
   * First corner (in WCS)
   */
  corner1: DwgPoint3D
  /**
   * Second corner (in WCS)
   */
  corner2: DwgPoint3D
  /**
   * Third corner (in WCS)
   */
  corner3: DwgPoint3D
  /**
   * Fourth corner (in WCS). If only three corners are entered, this is the same as
   * the third corner
   */
  corner4?: DwgPoint3D
  /**
   * Invisible edge flags (optional; default = 0):
   * - 1: First edge is invisible
   * - 2: Second edge is invisible
   * - 4: Third edge is invisible
   * - 8: Fourth edge is invisible
   */
  flag: number
}
