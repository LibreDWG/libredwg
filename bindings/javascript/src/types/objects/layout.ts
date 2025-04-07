import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgCommonObject } from './common'

export interface DwgLayoutObject extends DwgCommonObject {
  layoutName: string
  controlFlag: number
  tabOrder: number
  minLimit: DwgPoint2D
  maxLimit: DwgPoint2D
  insertionPoint: DwgPoint3D
  minExtent: DwgPoint3D
  maxExtent: DwgPoint3D
  elevation: number
  ucsOrigin: DwgPoint3D
  ucsXAxis: DwgPoint3D
  ucsYAxis: DwgPoint3D
  orthographicType: number
  paperSpaceTableId: string
  viewportId: string
  namedUcsId?: string
  orthographicUcsId?: string
  shadePlotId: string
}
