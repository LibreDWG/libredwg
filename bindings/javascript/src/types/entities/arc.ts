import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgArcEntity extends DwgEntity {
  type: 'ARC'
  thickness: number
  center: DwgPoint3D
  radius: number
  startAngle: number
  endAngle: number
  extrusionDirection: DwgPoint3D
}
