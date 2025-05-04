import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export enum DwgDimensionType {
  Rotated = 0,
  Aligned = 1,
  Angular = 2,
  Diameter = 3,
  Radius = 4,
  Angular3Point = 5,
  Ordinate = 6,
  ReferenceIsExclusive = 32,
  IsOrdinateXTypeFlag = 64,
  IsCustomTextPositionFlag = 128
}

export enum DwgAttachmentPoint {
  TopLeft = 1,
  TopCenter = 2,
  TopRight = 3,
  MiddleLeft = 4,
  MiddleCenter = 5,
  MiddleRight = 6,
  BottomLeft = 7,
  BottomCenter = 8,
  BottomRight = 9
}

export enum DwgDimensionTextLineSpacing {
  AtLeast = 1,
  Exact = 2
}

export enum DwgDimensionTextVertical {
  Center = 0,
  Above = 1,
  Outside = 2,
  JIS = 3,
  Below = 4
}

export enum DwgDimensionZeroSuppression {
  Feet = 0,
  None = 1,
  Inch = 2,
  FeetAndInch = 3,
  Leading = 4,
  Trailing = 8,
  LeadingAndTrailing = 12
}

export enum DwgDimensionZeroSuppressionAngular {
  None = 0,
  Leading = 1,
  Trailing = 2,
  LeadingAndTrailing = 3
}

export enum DwgDimensionTextHorizontal {
  Center = 0,
  Left = 1,
  Right = 2,
  OverFirst = 3,
  OverSecond = 4
}

export enum DwgDimensionToleranceTextVertical {
  Bottom = 0,
  Center = 1,
  Top = 2
}

export interface DwgDimensionEntityCommon extends DwgEntity {
  type: 'DIMENSION'
  subclassMarker: string
  /**
   * Version number:
   * - 0: 2010
   */
  version: number
  /**
   * Name of the block that contains the entities that make up the dimension picture
   */
  name: string
  /**
   * Definition point (in WCS)
   */
  definitionPoint: DwgPoint3D
  /**
   * Middle point of dimension text (in OCS)
   */
  textPoint: DwgPoint2D
  /**
   * Dimension type: Values 0-6 are integer values that represent the dimension type.
   * Values 32, 64, and 128 are bit values, which are added to the integer values
   * (value 32 is always set in R13 and later releases)
   * - 0: Rotated, horizontal, or vertical
   * - 1: Aligned
   * - 2: Angular
   * - 3: Diameter
   * - 4: Radius
   * - 5: Angular 3-point
   * - 6: Ordinate
   * - 32: Indicates that the block reference (group code 2) is referenced by this dimension only
   * - 64: Ordinate type. This is a bit value (bit 7) used only with integer value 6. If set,
   * ordinate is X-type; if not set, ordinate is Y-type.
   * - 128: This is a bit value (bit 8) added to the other group 70 values if the dimension text
   * has been positioned at a user-defined location rather than at the default location.
   */
  dimensionType: DwgDimensionType
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
   * Dimension text line-spacing style (optional):
   * - 1 (or missing): At least (taller characters will override)
   * - 2: Exact (taller characters will not override)
   */
  textLineSpacingStyle?: DwgDimensionTextLineSpacing
  /**
   * Dimension text-line spacing factor (optional):
   * Percentage of default (3-on-5) line spacing to be applied. Valid values range from 0.25 to 4.00
   */
  textLineSpacingFactor?: number
  /**
   * Actual measurement (optional; read-only value)
   */
  measurement?: number
  /**
   * Dimension text explicitly entered by the user. Optional; default is the measurement. If null
   * or “<>”, the dimension measurement is drawn as the text, if ““ (one blank space), the text is
   * suppressed. Anything else is drawn as the text
   */
  text?: string
  /**
   * The rotation angle of the dimension text away from its default orientation (the direction of
   * the dimension line) (optional)
   */
  textRotation?: number
  /**
   * The horizontal direction for the dimension entity. The dimension entity determines the orientation
   * of dimension text and lines for horizontal, vertical, and rotated linear dimensions
   * This group value is the negative of the angle between the OCS X axis and the UCS X axis. It is
   * always in the XY plane of the OCS.
   */
  ocsRotation?: number
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection?: DwgPoint3D
  /**
   * Dimension style name
   */
  styleName: string
}

export interface DwgAlignedDimensionEntity extends DwgDimensionEntityCommon {
  subclassMarker: 'AcDbAlignedDimension' | 'AcDbRotatedDimension'
  insertionPoint?: DwgPoint2D
  subDefinitionPoint1: DwgPoint3D
  subDefinitionPoint2: DwgPoint3D
  rotationAngle: number
  obliqueAngle: number
}

export interface DwgAngularDimensionEntity extends DwgDimensionEntityCommon {
  /**
   * 	Subclass marker (AcDb3PointAngularDimension)
   */
  subclassMarker: 'AcDb3PointAngularDimension'
  /**
   * Definition point for linear and angular dimensions (in WCS)
   */
  subDefinitionPoint1: DwgPoint3D
  /**
   * Definition point for linear and angular dimensions (in WCS)
   */
  subDefinitionPoint2: DwgPoint3D
  /**
   * Definition point for diameter, radius, and angular dimensions (in WCS)
   */
  centerPoint: DwgPoint3D
  /**
   * Point defining dimension arc for angular dimensions (in OCS)
   */
  arcPoint: DwgPoint3D
}

export interface DwgOrdinateDimensionEntity extends DwgDimensionEntityCommon {
  /**
   * Subclass marker (AcDbOrdinateDimension)
   */
  subclassMarker: 'AcDbOrdinateDimension'
  /**
   * Definition point for linear and angular dimensions (in WCS)
   */
  subDefinitionPoint1: DwgPoint3D
  /**
   * Definition point for linear and angular dimensions (in WCS)
   */
  subDefinitionPoint2: DwgPoint3D
}

export interface DwgRadialDiameterDimensionEntity
  extends DwgDimensionEntityCommon {
  /**
   * Subclass marker (AcDbRadialDimension or AcDbDiametricDimension)
   */
  subclassMarker: 'AcDbRadialDimension' | 'AcDbDiametricDimension'
  /**
   * Definition point for diameter, radius, and angular dimensions (in WCS)
   */
  centerPoint: DwgPoint3D
  /**
   * Leader length for radius and diameter dimensions
   */
  leaderLength: number
}

export type DwgDimensionEntity = DwgDimensionEntityCommon &
  (
    | Partial<DwgAlignedDimensionEntity>
    | Partial<DwgAngularDimensionEntity>
    | Partial<DwgOrdinateDimensionEntity>
    | Partial<DwgRadialDiameterDimensionEntity>
  )
