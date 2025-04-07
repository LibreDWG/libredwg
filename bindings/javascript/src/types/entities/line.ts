import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgLineEntity extends DwgEntity {
  type: 'LINE'
  thickness: number
  startPoint: DwgPoint3D
  endPoint: DwgPoint3D
  extrusionDirection: DwgPoint3D
}
