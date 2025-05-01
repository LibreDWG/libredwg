import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'
import { DwgVertexEntity } from './vertex'

export enum DwgPolylineFlag {
  CLOSED_POLYLINE = 1,
  CURVE_FIT = 2,
  SPLINE_FIT = 4,
  POLYLINE_3D = 8,
  POLYGON_3D = 16,
  CLOSED_POLYGON = 32,
  POLYFACE = 64,
  CONTINUOUS = 128
}

export enum DwgSmoothType {
  NONE = 0,
  QUADRATIC = 5,
  CUBIC = 6,
  BEZIER = 8
}

export interface DwgPolylineEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'POLYLINE'
  /**
   * polyline's elevation (in OCS when 2D; WCS when 3D)
   */
  elevation: number
  /**
   * Thickness (optional; default = 0)
   */
  thickness?: number
  /**
   * Polyline flag (bit-coded; default = 0):
   * - 1: This is a closed polyline (or a polygon mesh closed in the M direction)
   * - 2: Curve-fit vertices have been added
   * - 4: Spline-fit vertices have been added
   * - 8: This is a 3D polyline
   * - 16: This is a 3D polygon mesh
   * - 32: The polygon mesh is closed in the N direction
   * - 64: The polyline is a polyface mesh
   * - 128: The linetype pattern is generated continuously around the vertices of this polyline
   */
  flag: number
  /**
   * Default start width (optional; default = 0)
   */
  startWidth?: number
  /**
   * Default end width (optional; default = 0)
   */
  endWidth?: number
  /**
   * Polygon mesh M vertex count (optional; default = 0)
   */
  meshMVertexCount?: number
  /**
   * Polygon mesh N vertex count (optional; default = 0)
   */
  meshNVertexCount?: number
  /**
   * Smooth surface M density (optional; default = 0)
   */
  surfaceMDensity?: number
  /**
   * Smooth surface N density (optional; default = 0)
   */
  surfaceNDensity?: number
  /**
   * Curves and smooth surface type (optional; default = 0); integer codes, not bit-coded:
   * - 0: No smooth surface fitted
   * - 5: Quadratic B-spline surface
   * - 6: Cubic B-spline surface
   * - 8: Bezier surface
   */
  smoothType?: DwgSmoothType
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection?: DwgPoint3D
  vertices: DwgVertexEntity[]
}
