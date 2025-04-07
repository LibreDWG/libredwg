import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgXlineEntity extends DwgEntity {
  type: 'XLINE'
  firstPoint: DwgPoint3D
  unitDirection: DwgPoint3D
}
