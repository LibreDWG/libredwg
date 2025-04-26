import { DwgEntity } from './entity'

export interface DwgOleFrameEntity extends DwgEntity {
  /**
   * Entity type
   */
  type: 'OLEFRAME'
  flag: number
  mode: number
  /**
   * Length of binary data
   */
  dataSize: number
  /**
   * Binary data
   */
  binaryData: string
}
