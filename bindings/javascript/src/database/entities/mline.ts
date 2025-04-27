import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgMLineLine {
  /**
   * Number of parameters for this element
   */
  numberOfSegmentParams: number
  /**
   * Element parameters
   */
  segmentParams: number[]
  /**
   * Number of area fill parameters for this element
   */
  numberOfAreaFillParams: number
  /**
   * Area fill parameters
   */
  areaFillParams: number[]
}

export interface DwgMLineVertex {
  /**
   * Vertex coordinates
   */
  vertex: DwgPoint3D
  /**
   * Direction vector of segment starting at this vertex
   */
  vertexDirection: DwgPoint3D
  /**
   * Direction vector of miter at this vertex
   */
  miterDirection: DwgPoint3D
  /**
   * The number of lines
   */
  numberOfLines: number
  /**
   * The line element
   */
  lines: DwgMLineLine[]
}

export interface DwgMLineEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'MLINE'
  /**
   * Scale factor
   */
  scale: number
  /**
   * Justification:
   * - 0: Top
   * - 1: Zero
   * - 2: Bottom
   */
  justification: number
  /**
   * Flags (bit-coded values):
   * - 1: Has at least one vertex (code 72 is greater than 0)
   * - 2: Closed
   * - 4: Suppress start caps
   * - 8: Suppress end caps
   */
  flags: number
  /**
   * Start point (in WCS)
   */
  startPoint: DwgPoint3D
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection?: DwgPoint3D
  /**
   * Number of vertices
   */
  numberOfVertices: number
  /**
   * Number of elements in MLINESTYLE definition
   */
  numberOfLines: number
  /**
   * Vertices
   */
  vertices: DwgMLineVertex[]
  /**
   * String of up to 32 characters. The name of the style used for this mline. An entry for
   * this style must exist in the MLINESTYLE dictionary. Do not modify this field without
   * also updating the associated entry in the MLINESTYLE dictionary.
   */
  mlineStyle: string
}
