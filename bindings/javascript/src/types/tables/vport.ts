import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgCommonTableEntry } from './table'

export interface DwgVPortTableEntry extends DwgCommonTableEntry {
  name: string
  standardFlag: number
  lowerLeftCorner: DwgPoint2D
  upperRightCorner: DwgPoint2D
  center: DwgPoint2D
  snapBasePoint: DwgPoint2D
  snapSpacing: DwgPoint2D
  gridSpacing: DwgPoint2D
  viewDirectionFromTarget: DwgPoint3D
  viewTarget: DwgPoint3D
  lensLength: number
  frontClippingPlane: number
  backClippingPlane: number
  viewHeight: number
  snapRotationAngle: number
  viewTwistAngle: number
  circleSides: number
  frozenLayers: string[]
  styleSheet: string
  renderMode: number
  viewMode: number
  ucsIconSetting: number
  ucsOrigin: DwgPoint3D
  ucsXAxis: DwgPoint3D
  ucsYAxis: DwgPoint3D
  orthographicType: number
  elevation: number
  shadePlotSetting: number
  majorGridLines: number
  backgroundObjectId?: string
  shadePlotObjectId?: string
  visualStyleObjectId?: string
  isDefaultLightingOn: boolean
  defaultLightingType: number
  brightness: number
  contrast: number
  ambientColor?: number
}
