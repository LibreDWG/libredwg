import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgLeaderCreationFlag {
  TextAnnotation = 0,
  ToleranceAnnotation = 1,
  BlockReferenceAnnotation = 2,
  NoAnnotation = 3
}

export interface DwgLeaderEntity extends DwgEntity {
  type: 'LEADER'
  /**
   * Dimension style name
   */
  styleName: string
  /**
   * Arrowhead flag:
   * - 0: Disabled
   * - 1: Enabled
   */
  isArrowheadEnabled: boolean
  /**
   * Leader path type:
   * - 0: Straight line segments
   * - 1: Spline
   */
  isSpline: boolean
  /**
   * Leader creation flag (default = 3):
   * - 0: Created with text annotation
   * - 1: Created with tolerance annotation
   * - 2: Created with block reference annotation
   * - 3: Created without any annotation
   */
  leaderCreationFlag: DwgLeaderCreationFlag
  /**
   * Hookline direction flag:
   * - 0: Hookline (or end of tangent for a splined leader) is the opposite direction from the horizontal vector
   * - 1: Hookline (or end of tangent for a splined leader) is the same direction as horizontal vector (see code 75)
   */
  isHooklineSameDirection: boolean
  /**
   * Hookline flag:
   * - 0: No hookline
   * - 1: Has a hookline
   */
  isHooklineExists: boolean
  /**
   * Text annotation height
   */
  textHeight?: number
  /**
   * Text annotation width
   */
  textWidth?: number
  /**
   * Number of vertices in leader (ignored for OPEN)
   */
  numberOfVertices?: number
  /**
   * Vertex coordinates (one entry for each vertex)
   */
  vertices: DwgPoint3D[]
  /**
   * Color to use if leader's DIMCLRD = BYBLOCK
   */
  byBlockColor?: number
  /**
   * Hard reference to associated annotation (mtext, tolerance, or insert entity)
   */
  associatedAnnotation?: string
  /**
   * Normal vector
   */
  normal?: DwgPoint3D
  /**
   * Horizontal direction for leader
   */
  horizontalDirection?: DwgPoint3D
  /**
   * Offset of last leader vertex from block reference insertion point
   */
  offsetFromBlock?: DwgPoint3D
  /**
   * Offset of last leader vertex from annotation placement point
   */
  offsetFromAnnotation?: DwgPoint3D
}
