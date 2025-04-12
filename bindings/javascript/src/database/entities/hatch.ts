import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export enum DwgHatchSolidFill {
  PatternFill = 0,
  SolidFill = 1
}

export enum DwgHatchAssociativity {
  NonAssociative = 0, // For MPolygon LacksSolidFill
  Associative = 1
}

export enum DwgHatchStyle {
  Normal = 0, // Odd parity area
  Outer = 1, // Outermost area
  Ignore = 2
}

export enum DwgHatchPatternType {
  UserDefined = 0,
  Predefined = 1,
  Custom = 2
}

export enum DwgHatchBoundaryAnnotation {
  NotAnnotated = 0,
  Annotated = 1
}

export enum DwgHatchGradientFlag {
  Solid = 0,
  Gradient = 1
}

export enum DwgHatchGradientColorFlag {
  TwoColor = 0,
  OneColor = 1
}

export enum DwgBoundaryPathTypeFlag {
  Default = 0,
  External = 1,
  Polyline = 2,
  Derived = 4,
  Textbox = 8,
  Outermost = 16
}

export enum DwgBoundaryPathEdgeType {
  Line = 1,
  Circular = 2,
  Elliptic = 3,
  Spline = 4
}

interface DwgBoundaryPathBase {
  boundaryPathTypeFlag: number
  // numberOfSourceBoundaryObjects: number
  // sourceBoundaryObjects: string[]
}

export interface DwgPolylineBoundaryPath extends DwgBoundaryPathBase {
  hasBulge: boolean
  isClosed: boolean
  numberOfVertices: number
  vertices: (DwgPoint2D & {
    bulge: number
  })[]
}

export interface DwgEdgeBoundaryPath<EdgeType extends DwgBoundaryPathEdge>
  extends DwgBoundaryPathBase {
  numberOfEdges: number
  edges: EdgeType[]
}

interface DwgBoundaryPathEdgeCommon {
  type: DwgBoundaryPathEdgeType
}

export type DwgBoundaryPath =
  | DwgPolylineBoundaryPath
  | DwgEdgeBoundaryPath<DwgBoundaryPathEdge>

export interface DwgLineEdge extends DwgBoundaryPathEdgeCommon {
  start: DwgPoint2D
  end: DwgPoint2D
}

export interface DwgArcEdge extends DwgBoundaryPathEdgeCommon {
  center: DwgPoint2D
  radius: number
  startAngle: number
  endAngle: number
  isCCW?: boolean
}

export interface DwgEllipseEdge extends DwgBoundaryPathEdgeCommon {
  center: DwgPoint2D
  end: DwgPoint2D
  lengthOfMinorAxis: number
  startAngle: number
  endAngle: number
  isCCW?: boolean
}

export interface DwgSplineEdge extends DwgBoundaryPathEdgeCommon {
  degree: number
  isPeriodic?: boolean
  numberOfKnots: number
  numberOfControlPoints: number
  knots: number[]
  controlPoints: (DwgPoint2D & {
    weight?: number
  })[]
  numberOfFitData: number
  fitDatum: DwgPoint2D[]
  startTangent: DwgPoint2D
  endTangent: DwgPoint2D
}

export type DwgBoundaryPathEdge =
  | DwgLineEdge
  | DwgArcEdge
  | DwgEllipseEdge
  | DwgSplineEdge

export interface DwgHatchDefinitionLine {
  angle: number
  base: DwgPoint2D
  offset: DwgPoint2D
  numberOfDashLengths: number
  dashLengths: number[]
}

export interface DwgHatchEntityBase extends DwgEntity {
  type: 'HATCH'
  // elevationPoint: DwgPoint3D
  extrusionDirection?: DwgPoint3D
  patternName: string
  solidFill: DwgHatchSolidFill
  // patternFillColor: number
  associativity: DwgHatchAssociativity
  numberOfBoundaryPaths: number
  boundaryPaths: DwgBoundaryPath[]
  hatchStyle: DwgHatchStyle
  patternType: DwgHatchPatternType
  patternAngle?: number
  patternScale?: number
  numberOfDefinitionLines: number
  definitionLines: DwgHatchDefinitionLine[]
  pixelSize: number
  numberOfSeedPoints: number
  offsetVector?: DwgPoint3D
  seedPoints?: DwgPoint2D[]
  gradientFlag?: DwgHatchGradientFlag
}

export interface DwgGradientHatchEntity extends DwgHatchEntityBase {
  gradientFlag: DwgHatchGradientFlag.Gradient
  gradientColorFlag: DwgHatchGradientColorFlag
  numberOfColors: 0 | 2
  gradientRotation?: number
  gradientDefinition: number
  colorTint?: number
}

export type DwgHatchEntity = DwgGradientHatchEntity | DwgHatchEntityBase
