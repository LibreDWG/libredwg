import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgDimensionType {
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

export declare enum DwgAttachmentPoint {
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

export declare enum DwgDimensionTextLineSpacing {
  AtLeast = 1,
  Exact = 2
}

export declare enum DwgDimensionTextVertical {
  Center = 0,
  Above = 1,
  Outside = 2,
  JIS = 3,
  Below = 4
}

export declare enum DwgDimensionZeroSuppression {
  Feet = 0,
  None = 1,
  Inch = 2,
  FeetAndInch = 3,
  Leading = 4,
  Trailing = 8,
  LeadingAndTrailing = 12
}

export declare enum DwgDimensionZeroSuppressionAngular {
  None = 0,
  Leading = 1,
  Trailing = 2,
  LeadingAndTrailing = 3
}

export declare enum DwgDimensionTextHorizontal {
  Center = 0,
  Left = 1,
  Right = 2,
  OverFirst = 3,
  OverSecond = 4
}

export declare enum DwgDimensionToleranceTextVertical {
  Bottom = 0,
  Center = 1,
  Top = 2
}

export interface DwgDimensionEntityCommon extends DwgEntity {
  type: 'DIMENSION'
  subclassMarker: string
  version: number
  /**
   * DXF group code: 2
   * Name of the block that contains the entities that make up the dimension picture
   */
  name: string
  definitionPoint: DwgPoint3D
  textPoint: DwgPoint2D
  dimensionType: DwgDimensionType
  attachmentPoint: DwgAttachmentPoint
  textLineSpacingStyle?: DwgDimensionTextLineSpacing
  textLineSpacingFactor?: number
  measurement?: number
  text?: string
  textRotation?: number
  ocsRotation?: number
  extrusionDirection?: DwgPoint3D
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
  subclassMarker: 'AcDb3PointAngularDimension'
  subDefinitionPoint1: DwgPoint3D
  subDefinitionPoint2: DwgPoint3D
  centerPoint: DwgPoint3D
  arcPoint: DwgPoint3D
}

export interface DwgOrdinateDimensionEntity extends DwgDimensionEntityCommon {
  subclassMarker: 'AcDbOrdinateDimension'
  subDefinitionPoint1: DwgPoint3D
  subDefinitionPoint2: DwgPoint3D
}

export interface DwgRadialDiameterDimensionEntity
  extends DwgDimensionEntityCommon {
  subclassMarker: 'AcDbRadialDimension' | 'AcDbDiametricDimension'
  centerPoint: DwgPoint3D
  leaderLength: number
}

export type DwgDimensionEntity = DwgDimensionEntityCommon &
  (
    | Partial<DwgAlignedDimensionEntity>
    | Partial<DwgAngularDimensionEntity>
    | Partial<DwgOrdinateDimensionEntity>
    | Partial<DwgRadialDiameterDimensionEntity>
  )
