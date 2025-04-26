import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgEllipseEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'ELLIPSE'
  /**
   * Center point (in WCS)
   */
  center: DwgPoint3D
  /**
   * Endpoint of major axis, relative to the center (in WCS)
   */
  majorAxisEndPoint: DwgPoint3D
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection: DwgPoint3D
  /**
   * Ratio of minor axis to major axis
   */
  axisRatio: number
  /**
   * Start parameter (this value is 0.0 for a full ellipse)
   */
  startAngle: number
  /**
   * End parameter (this value is 2pi for a full ellipse)
   */
  endAngle: number
}
