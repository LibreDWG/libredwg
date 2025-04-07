import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgCircleEntity extends DwgEntity {
  type: 'CIRCLE'
  thickness: number
  center: DwgPoint3D
  radius: number
  extrusionDirection: DwgPoint3D
}
