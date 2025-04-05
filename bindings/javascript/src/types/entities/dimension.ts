import { DwgPoint3D } from "../common";
import { DwgEntity } from "./entity";

export declare enum DimensionType {
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

export declare enum AttachmentPoint {
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

export declare enum DimensionTextLineSpacing {
  AtLeast = 1,
  Exact = 2
}

export declare enum DimensionTextVertical {
  Center = 0,
  Above = 1,
  Outside = 2,
  JIS = 3,
  Below = 4
}

export declare enum DimensionZeroSuppression {
  Feet = 0,
  None = 1,
  Inch = 2,
  FeetAndInch = 3,
  Leading = 4,
  Trailing = 8,
  LeadingAndTrailing = 12
}

export declare enum DimensionZeroSuppressionAngular {
  None = 0,
  Leading = 1,
  Trailing = 2,
  LeadingAndTrailing = 3
}

export declare enum DimensionTextHorizontal {
  Center = 0,
  Left = 1,
  Right = 2,
  OverFirst = 3,
  OverSecond = 4
}

export declare enum DimensionToleranceTextVertical {
  Bottom = 0,
  Center = 1,
  Top = 2
}

export interface DwgDimensionEntityCommon extends DwgEntity {
  subclassMarker: string;
  handle: string;
  version: string;
  name: string;
  definitionPoint: DwgPoint3D;
  textPoint: DwgPoint3D;
  dimensionType: DimensionType;
  attachmentPoint: AttachmentPoint;
  textLineSpacingStyle?: DimensionTextLineSpacing;
  textLineSpacingFactor?: number;
  measurement?: number;
  text?: string;
  textRotation?: number;
  ocsRotation?: number;
  extrusionDirection?: DwgPoint3D;
  styleName: string;
}

export interface DwgAlignedDimensionEntity extends DwgDimensionEntityCommon {
  insertionPoint?: DwgPoint3D;
  subDefinitionPoint1: DwgPoint3D;
  subDefinitionPoint2: DwgPoint3D;
  rotationAngle: number;
  obliqueAngle: number;
}

export interface DwgAngularDimensionEntity extends DwgDimensionEntityCommon {
  subDefinitionPoint1: DwgPoint3D;
  subDefinitionPoint2: DwgPoint3D;
  centerPoint: DwgPoint3D;
  arcPoint: DwgPoint3D;
}

export interface DwgOrdinateDimensionEntity extends DwgDimensionEntityCommon {
  subDefinitionPoint1: DwgPoint3D;
  subDefinitionPoint2: DwgPoint3D;
}

export interface DwgRadialDiameterDimensionEntity extends DwgDimensionEntityCommon {
  centerPoint: DwgPoint3D;
  leaderLength: number;
}

export type DimensionEntity = DwgDimensionEntityCommon & (Partial<DwgAlignedDimensionEntity> | Partial<DwgAngularDimensionEntity> | Partial<DwgOrdinateDimensionEntity> | Partial<DwgRadialDiameterDimensionEntity>);
