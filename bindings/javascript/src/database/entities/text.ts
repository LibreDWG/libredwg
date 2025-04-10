import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgTextGenerationFlag {
  NONE = 0,
  MIRRORED_X = 2,
  MIRRORED_Y = 4
}

export declare enum DwgTextHorizontalAlign {
  LEFT = 0,
  CENTER = 1,
  RIGHT = 2,
  ALIGNED = 3,
  MIDDLE = 4,
  FIT = 5
}

export declare enum DwgTextVerticalAlign {
  BASELINE = 0,
  BOTTOM = 1,
  MIDDLE = 2,
  TOP = 3
}

export interface DwgTextEntity extends DwgEntity {
  type: 'TEXT'
  text: string
  thickness: number
  startPoint: DwgPoint2D
  endPoint: DwgPoint2D
  textHeight: number
  rotation: number
  xScale: number
  obliqueAngle: number
  styleName: string
  generationFlag: number
  halign: DwgTextHorizontalAlign
  valign: DwgTextVerticalAlign
  extrusionDirection: DwgPoint3D
}
