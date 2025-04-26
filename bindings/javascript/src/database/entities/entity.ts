import { DwgXData } from '../shared/xdata'

export interface DwgEntity {
  /**
   * Entity type
   */
  type: string
  /**
   * Handle
   */
  handle: number
  /**
   * Soft-pointer ID/handle to owner BLOCK_RECORD object
   */
  ownerBlockRecordSoftId: number
  /**
   * Absent or zero indicates entity is in model space. 1 indicates entity is in paper space (optional).
   */
  isInPaperSpace?: boolean
  /**
   * Layer name
   */
  layer: string
  /**
   * Linetype name (present if not BYLAYER). The special name BYBLOCK indicates a floating linetype (optional)
   */
  lineType?: string
  /**
   * Hard-pointer ID/handle to material object (present if not BYLAYER)
   */
  materialObjectHardId?: string
  /**
   * Color number (present if not BYLAYER); zero indicates the BYBLOCK (floating) color; 256 indicates
   * BYLAYER; a negative value indicates that the layer is turned off (optional)
   */
  colorIndex?: number
  /**
   * Lineweight enum value. Stored and moved around as a 16-bit integer.
   */
  lineweight?: number
  /**
   * Linetype scale (optional)
   */
  lineTypeScale?: number
  /**
   * Object visibility (optional):
   * - 0: Visible
   * - 1: Invisible
   */
  isVisible?: boolean
  /**
   * Number of bytes in the proxy entity graphics represented in the subsequent 310 groups,
   * which are binary chunk records (optional)
   */
  proxyByte?: number
  /**
   * Proxy entity graphics data (multiple lines; 256 characters max. per line) (optional)
   */
  proxyEntity?: string
  /**
   * A 24-bit color value that should be dealt with in terms of bytes with values of 0 to 255.
   * The lowest byte is the blue value, the middle byte is the green value, and the third byte
   * is the red value. The top byte is always 0. The group code cannot be used by custom entities
   * for their own data because the group code is reserved for AcDbEntity, class-level color data
   * and AcDbEntity, class-level transparency data
   */
  color?: number
  /**
   * Color name. The group code cannot be used by custom entities for their own data because
   * the group code is reserved for AcDbEntity, class-level color data and AcDbEntity, class-level
   * transparency data
   */
  colorName?: string
  /**
   * Transparency value. The group code cannot be used by custom entities for their own data because
   * the group code is reserved for AcDbEntity, class-level color data and AcDbEntity, class-level
   * transparency data
   */
  transparency?: number
  /**
   * Hard-pointer ID/handle to the plot style object
   */
  plotStyleHardId?: string
  /**
   * Shadow mode
   * - 0: Casts and receives shadows
   * - 1: Casts shadows
   * - 2: Receives shadows
   * - 3: Ignores shadows
   *
   * Note: Starting with AutoCAD 2016-based products, this property is obsolete but still supported
   * for backwards compatibility.
   */
  shadowMode?: number
  /**
   * The extension dictionary attached to the entity (optional).
   */
  xdata?: DwgXData
  /**
   * Hard-owner ID/handle to owner dictionary (optional)
   */
  ownerDictionaryHardId?: string | number | boolean
  /**
   * Soft-pointer ID/handle to owner dictionary (optional)
   */
  ownerDictionarySoftId?: string | number | boolean
}
