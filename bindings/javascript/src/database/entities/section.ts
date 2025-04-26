import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgSectionEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'SECTION'
  /**
   * Section state
   */
  state: number
  /**
   * Section flags
   */
  flag: number
  /**
   * Name
   */
  name: string
  /**
   * Vertical direction
   */
  verticalDirection: DwgPoint3D
  /**
   * Top height
   */
  topHeight: number
  /**
   * Bottom height
   */
  bottomHeight: number
  /**
   * Indicator transparency
   */
  indicatorTransparency: number
  /**
   * Indicator color
   */
  indicatorColor: number
  /**
   * Number of vertices
   */
  numberOfVertices: number
  /**
   * Vertex (repeats for number of vertices)
   */
  vertices: DwgPoint3D[]
  /**
   * Number of back line vertices
   */
  numberOfBackLineVertices: number
  /**
   * Back line vertex (repeats for number of back line vertices)
   */
  backLineVertices: DwgPoint3D[]
  /**
   * Hard-pointer ID/handle to geometry settings object
   */
  geometrySettingHardId: bigint
}
