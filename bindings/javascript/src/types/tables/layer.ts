import { DwgCommonTableEntry } from './table'

export interface DwgLayerTableEntry extends DwgCommonTableEntry {
  name: string
  standardFlag: number
  colorIndex: number
  lineType: string
  frozen: boolean
  off: boolean
  frozenInNew: boolean
  locked: boolean
  plotFlag: number
  lineweight: number
  plotStyleNameObjectId?: string
  materialObjectId?: string
}
