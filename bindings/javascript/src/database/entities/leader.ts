import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgLeaderCreationFlag {
  TextAnnotation = 0,
  ToleranceAnnotation = 1,
  BlockReferenceAnnotation = 2,
  NoAnnotation = 3
}

export interface DwgLeaderEntity extends DwgEntity {
  type: 'LEADER'
  styleName: string
  isArrowheadEnabled: boolean
  isSpline: boolean
  leaderCreationFlag: DwgLeaderCreationFlag
  isHooklineSameDirection: boolean
  isHooklineExists: boolean
  textHeight?: number
  textWidth?: number
  numberOfVertices?: number
  vertices: DwgPoint3D[]
  byBlockColor?: number
  associatedAnnotation?: string
  normal?: DwgPoint3D
  horizontalDirection?: DwgPoint3D
  offsetFromBlock?: DwgPoint3D
  offsetFromAnnotation?: DwgPoint3D
}
