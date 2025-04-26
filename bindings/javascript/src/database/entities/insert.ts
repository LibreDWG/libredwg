import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgInsertEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'INSERT'
  /**
   * Variable attributes-follow flag (optional; default = 0); if the value of attributes-follow
   * flag is 1, a series of attribute entities is expected to follow the insert, terminated by
   * a seqend entity
   */
  isVariableAttributes?: boolean
  /**
   * Block name
   */
  name: string
  /**
   * Insertion point (in OCS)
   */
  insertionPoint: DwgPoint3D
  /**
   * X scale factor (optional; default = 1)
   */
  xScale: number
  /**
   * Y scale factor (optional; default = 1)
   */
  yScale: number
  /**
   * Z scale factor (optional; default = 1)
   */
  zScale: number
  /**
   * Rotation angle (optional; default = 0)
   */
  rotation: number
  /**
   * Column count (optional; default = 1)
   */
  columnCount: number
  /**
   * Row count (optional; default = 1)
   */
  rowCount: number
  /**
   * Column spacing (optional; default = 0)
   */
  columnSpacing: number
  /**
   * Row spacing (optional; default = 0)
   */
  rowSpacing: number
  /**
   * Extrusion direction (optional; default = 0, 0, 1)
   */
  extrusionDirection: DwgPoint3D
}
