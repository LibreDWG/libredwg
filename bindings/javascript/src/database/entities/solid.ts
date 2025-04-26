import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgSolidEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'SOLID'
  /**
   * Corner1
   */
  corner1: DwgPoint2D
  /**
   * Corner2
   */
  corner2: DwgPoint2D
  /**
   * Corner3
   */
  corner3: DwgPoint2D
  /**
   * Corner4. If only three corners are entered to define the SOLID,
   * then the fourth corner coordinate is the same as the third.
   */
  corner4?: DwgPoint2D
  /**
   * Thickness (optional; default = 0)
   */
  thickness: number
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection: DwgPoint3D
}
