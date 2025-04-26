import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgArcEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'ARC'
  /**
   * Thickness (optional; default = 0)
   */
  thickness: number
  /**
   * Center point (in OCS)
   */
  center: DwgPoint3D
  /**
   * Radius
   */
  radius: number
  /**
   * Start angle
   */
  startAngle: number
  /**
   * End angle
   */
  endAngle: number
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection: DwgPoint3D
}
