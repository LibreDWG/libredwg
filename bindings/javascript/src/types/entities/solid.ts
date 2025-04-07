import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgSolidEntity extends DwgEntity {
  type: 'SOLID'
  points: DwgPoint3D[]
  thickness: number
  extrusionDirection: DwgPoint3D
}
