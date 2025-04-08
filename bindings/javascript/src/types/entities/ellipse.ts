import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgEllipseEntity extends DwgEntity {
  type: 'ELLIPSE'
  center: DwgPoint3D
  majorAxisEndPoint: DwgPoint3D
  extrusionDirection: DwgPoint3D
  axisRatio: number
  startAngle: number
  endAngle: number
}
