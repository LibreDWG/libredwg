import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgVertexFlag {
  CREATED_BY_CURVE_FIT = 1,
  TANGENT_DEFINED = 2,
  NOT_USED = 4,
  CREATED_BY_SPLINE_FIT = 8,
  SPLINE_CONTROL_POINT = 16,
  FOR_POLYLINE = 32,
  FOR_POLYGON = 64,
  POLYFACE = 128
}

export interface DwgVertexEntity extends DwgEntity, DwgPoint3D {
  startWidth: number
  endWidth: number
  bulge: number
  flag: DwgVertexFlag
  tangentDirection: number
  polyfaceIndex0?: number
  polyfaceIndex1?: number
  polyfaceIndex2?: number
  polyfaceIndex3?: number
  id: number
}
