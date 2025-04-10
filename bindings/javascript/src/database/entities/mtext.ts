import { DwgPoint3D } from '../common'
import { DwgAttachmentPoint } from './dimension'
import { DwgEntity } from './entity'

export declare enum DwgMTextDrawingDirection {
  LEFT_TO_RIGHT = 1,
  TOP_TO_BOTTOM = 3,
  BY_STYLE = 5
}

export interface DwgMTextEntity extends DwgEntity {
  type: 'MTEXT'
  insertionPoint: DwgPoint3D
  height: number
  width: number
  attachmentPoint: DwgAttachmentPoint
  drawingDirection: DwgMTextDrawingDirection
  text: string
  styleName: string
  extrusionDirection: DwgPoint3D
  direction: DwgPoint3D
  rotation: number
  lineSpacingStyle: number
  lineSpacing: number
  backgroundFill: number
  // backgroundColor: number
  fillBoxScale: number
  backgroundFillColor: number
  backgroundFillTransparency: number
  columnType: number
  // columnCount: number
  columnFlowReversed: number
  columnAutoHeight: number
  columnWidth: number
  columnGutter: number
  columnHeightCount: number
  columnHeights: number[]
}
