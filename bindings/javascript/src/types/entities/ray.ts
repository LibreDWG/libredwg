import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgRayEntity extends DwgEntity {
  type: 'RAY'
  firstPoint: DwgPoint3D
  unitDirection: DwgPoint3D
}
