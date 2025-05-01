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
  /**
   * Insertion point
   */
  insertionPoint: DwgPoint3D
  /**
   * Nominal (initial) text height
   */
  textHeight: number
  /**
   * Reference rectangle height. No DXF group code
   */
  rectHeight?: number
  /**
   * Reference rectangle width
   */
  rectWidth: number
  /**
   * Horizontal width of the characters that make up the mtext entity. This value will always be equal to
   * or less than the value of 'rectWidth' (read-only, ignored if supplied)
   */
  extentsWidth: number
  /**
   * Vertical height of the mtext entity (read-only, ignored if supplied)
   */
  extentsHeight: number
  /**
   * Attachment point:
   * - 1: Top left
   * - 2: Top center
   * - 3: Top right
   * - 4: Middle left
   * - 5: Middle center
   * - 6: Middle right
   * - 7: Bottom left
   * - 8: Bottom center
   * - 9: Bottom right
   */
  attachmentPoint: DwgAttachmentPoint
  /**
   * Drawing direction:
   * - 1: Left to right
   * - 3: Top to bottom
   * - 5: By style (the flow direction is inherited from the associated text style)
   */
  drawingDirection: DwgMTextDrawingDirection
  /**
   * Text string
   */
  text: string
  /**
   * Text style name (STANDARD if not provided) (optional)
   */
  styleName: string
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection?: DwgPoint3D
  /**
   * X-axis direction vector (in WCS)
   */
  direction: DwgPoint3D
  /**
   * Rotation angle in radians
   */
  rotation: number
  /**
   * Mtext line spacing style (optional):
   * - 1: At least (taller characters will override)
   * - 2: Exact (taller characters will not override)
   */
  lineSpacingStyle: number
  /**
   * Mtext line spacing factor (optional):
   * Percentage of default (3-on-5) line spacing to be applied. Valid values range from 0.25 to 4.00
   */
  lineSpacing: number
  /**
   * Background color (if RGB color)
   */
  backgroundFill: number
  // backgroundColor: number
  /**
   * Fill box scale (optional):
   * Determines how much border there is around the text.
   */
  fillBoxScale: number
  /**
   * Background fill color (optional):
   * Color to use for background fill when 'backgroundFill' is 1.
   */
  backgroundFillColor: number
  /**
   * Transparency of background fill color (not implemented)
   */
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
