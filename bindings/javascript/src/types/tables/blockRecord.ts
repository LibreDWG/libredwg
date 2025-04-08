import { DwgEntity } from '../entities'
import { DwgCommonTableEntry } from './table'

export interface DwgBlockRecordTableEntry extends DwgCommonTableEntry {
  name: string
  layout: number
  insertionUnits: number
  explodability: number
  scalability: number
  bmpPreview?: string
  entities: DwgEntity[]
}
