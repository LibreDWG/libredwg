import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export enum DwgTextGenerationFlag {
  NONE = 0,
  MIRRORED_X = 2,
  MIRRORED_Y = 4
}

export enum DwgTextHorizontalAlign {
  LEFT = 0,
  CENTER = 1,
  RIGHT = 2,
  ALIGNED = 3,
  MIDDLE = 4,
  FIT = 5
}

export enum DwgTextVerticalAlign {
  BASELINE = 0,
  BOTTOM = 1,
  MIDDLE = 2,
  TOP = 3
}

export interface DwgTextBase {
  /**
   * Text value
   */
  text: string
  /**
   * Thickness (optional; default = 0)
   */
  thickness: number
  /**
   * First alignment point (in OCS)
   */
  startPoint: DwgPoint2D
  /**
   * Second alignment point (in OCS) (optional)
   * This value is meaningful only if the justification is anything other than baseline/left
   */
  endPoint: DwgPoint2D
  /**
   * Text height
   */
  textHeight: number
  /**
   * Text rotation (optional; default = 0)
   */
  rotation: number
  /**
   * Relative X scale factor—width (optional; default = 1)
   * This value is also adjusted when fit-type text is used
   */
  xScale: number
  /**
   * Oblique angle (optional; default = 0)
   */
  obliqueAngle: number
  /**
   * Text style name (optional, default = STANDARD)
   */
  styleName: 'STANDARD' | string
  /**
   * Text generation flags (optional, default = 0):
   * - 2: Text is backward (mirrored in X)
   * - 4: Text is upside down (mirrored in Y)
   */
  generationFlag: DwgTextGenerationFlag
  /**
   * Horizontal text justification type (optional, default = 0) integer codes (not bit-coded):
   * - 0: Left
   * - 1: Center
   * - 2: Right
   * - 3: Aligned (if vertical alignment = 0)
   * - 4: Middle (if vertical alignment = 0)
   * - 5: Fit (if vertical alignment = 0)
   */
  halign: DwgTextHorizontalAlign
  /**
   * Vertical text justification type (optional, default = 0): integer codes (not bit-coded):
   * - 0: Baseline
   * - 1: Bottom
   * - 2: Middle
   * - 3: Top
   */
  valign: DwgTextVerticalAlign
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection: DwgPoint3D
}

export interface DwgTextEntity extends DwgEntity, DwgTextBase {
  /**
   * Entity type
   */
  type: 'TEXT'
}
