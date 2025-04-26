import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgToleranceEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'TOLERANCE'
  /**
   * Dimension style name
   */
  styleName: string
  /**
   * Insertion point (in WCS)
   */
  insertionPoint: DwgPoint3D
  /**
   * String representing the visual representation of the tolerance
   */
  text: string
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection: DwgPoint3D
  /**
   * X-axis direction vector (in WCS)
   */
  xAxisDirection: DwgPoint3D
}
