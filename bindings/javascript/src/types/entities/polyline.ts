import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'
import { DwgVertexEntity } from './vertex'

export declare enum DwgPolylineFlag {
  CLOSED_POLYLINE = 1,
  CURVE_FIT = 2,
  SPLINE_FIT = 4,
  POLYLINE_3D = 8,
  POLYGON_3D = 16,
  CLOSED_POLYGON = 32,
  POLYFACE = 64,
  CONTINUOUS = 128
}

export declare enum DwgSmoothType {
  NONE = 0,
  QUADRATIC = 5,
  CUBIC = 6,
  BEZIER = 8
}

export interface DwgPolylineEntity extends DwgEntity {
  type: 'POLYLINE'
  thickness: number
  flag: number
  startWidth: number
  endWidth: number
  meshMVertexCount: number
  meshNVertexCount: number
  surfaceMDensity: number
  surfaceNDensity: number
  smoothType: DwgSmoothType
  extrusionDirection: DwgPoint3D
  vertices: DwgVertexEntity[]
}
