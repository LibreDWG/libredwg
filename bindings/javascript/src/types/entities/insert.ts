import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgInsertEntity extends DwgEntity {
  type: 'INSERT'
  isVariableAttributes?: boolean
  name: string
  insertionPoint: DwgPoint3D
  xScale: number
  yScale: number
  zScale: number
  rotation: number
  columnCount: number
  rowCount: number
  columnSpacing: number
  rowSpacing: number
  extrusionDirection: DwgPoint3D
}
