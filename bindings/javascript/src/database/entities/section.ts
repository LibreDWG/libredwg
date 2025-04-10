import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgSectionEntity extends DwgEntity {
  type: 'SECTION'
  state: number
  flag: number
  name: string
  verticalDirection: DwgPoint3D
  topHeight: number
  bottomHeight: number
  indicatorTransparency: number
  indicatorColor: number
  numberOfVertices: number
  vertices: DwgPoint3D[]
  numberOfBackLineVertices: number
  backLineVertices: DwgPoint3D[]
  geometrySettingHardId: string
}
