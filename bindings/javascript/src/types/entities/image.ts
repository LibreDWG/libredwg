import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgImageFlags {
  ShowImage = 1,
  ShowImageWhenNotAlignedWithScreen = 2,
  UseClippingBoundary = 4,
  TransparencyIsOn = 8
}

export declare enum DwgImageClippingBoundaryType {
  Rectangular = 1,
  Polygonal = 2
}

export declare enum DwgImageClipMode {
  Outside = 0,
  Inside = 1
}

export interface DwgImageEntity extends DwgEntity {
  type: 'IMAGE'
  version: number
  position: DwgPoint3D
  uPixel: DwgPoint3D
  vPixel: DwgPoint3D
  imageSize: DwgPoint3D
  imageDefHandle: string
  flags: DwgImageFlags
  clipping: number
  brightness: number
  contrast: number
  fade: number
  imageDefReactorHandle: string
  clippingBoundaryType: DwgImageClippingBoundaryType
  countBoundaryPoints: number
  clippingBoundaryPath: DwgPoint3D[]
  clipMode: DwgImageClipMode
}
