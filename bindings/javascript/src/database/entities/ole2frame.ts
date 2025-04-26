import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgOle2FrameEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'OLE2FRAME'
  /**
   * OLE version number
   */
  oleVersion: number
  /**
   * OLE or Paintbrush Picture
   */
  oleClient?: string
  /**
   * Length of binary data
   */
  dataSize: number
  /**
   * Upper-left corner (WCS)
   */
  leftUpPoint: DwgPoint3D
  /**
   * Lower-right corner (WCS)
   */
  rightDownPoint: DwgPoint3D
  /**
   * Flag whether to lock aspect.
   * - 0: not locked
   * - 1: locked
   */
  lockAspect: number
  /**
   * OLE object type
   * - 1: Link
   * - 2: Embedded
   * - 3: Static
   */
  oleObjectType: number
  /**
   * Tile mode descriptor:
   * 0: Object resides in model space
   * 1: Object resides in paper space
   */
  tileModeDescriptor: number
  /**
   * Binary data
   */
  binaryData: string
}
