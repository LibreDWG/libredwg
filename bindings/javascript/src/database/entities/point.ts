import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgPointEntity extends DwgEntity {
  type: 'POINT'
  position: DwgPoint3D
  thickness: number
  extrusionDirection: DwgPoint3D
  angle: number
}
