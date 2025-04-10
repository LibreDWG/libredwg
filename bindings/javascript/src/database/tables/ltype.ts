import { DwgCommonTableEntry } from './table'

export interface DwgLTypeTableEntry extends DwgCommonTableEntry {
  name: string
  standardFlag: number
  description: string
  numberOfLineTypes: number
  totalPatternLength: number
  pattern?: DwgLineTypeElement[]
}

export interface DwgLineTypeElement {
  elementLength: number
  elementTypeFlag: number
  shapeNumber?: number
  styleObjectId?: string
  scale?: number
  rotation?: number
  offsetX?: number
  offsetY?: number
  text?: string
}
