import { DwgCommonTableEntry } from './table'

export interface DwgStyleTableEntry extends DwgCommonTableEntry {
  standardFlag: number
  fixedTextHeight: number
  widthFactor: number
  obliqueAngle: number
  textGenerationFlag: number
  lastHeight: number
  font: string
  bigFont: string
  extendedFont?: string
}
