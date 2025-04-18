import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'
import { DwgImageClipMode, DwgImageClippingBoundaryType, DwgImageFlags } from './image'

export interface DwgWipeoutEntity  extends DwgEntity {
  type: 'WIPEOUT'
  version: number
  position: DwgPoint3D
  uPixel: DwgPoint3D
  vPixel: DwgPoint3D
  imageSize: DwgPoint2D
  imageDefHandle: number
  flags: DwgImageFlags
  clipping: number
  brightness: number
  contrast: number
  fade: number
  imageDefReactorHandle: number
  clippingBoundaryType: DwgImageClippingBoundaryType
  countBoundaryPoints: number
  clippingBoundaryPath: DwgPoint3D[]
  clipMode: DwgImageClipMode
}

