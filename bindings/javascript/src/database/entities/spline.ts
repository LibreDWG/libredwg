import { DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgSplineEntity extends DwgEntity {
  type: 'SPLINE'
  normal?: DwgPoint3D
  flag: number
  degree: number
  numberOfKnots: number
  numberOfControlPoints: number
  numberOfFitPoints: number
  knotTolerance: number
  controlTolerance: number
  fitTolerance: number
  startTangent?: DwgPoint3D
  endTangent?: DwgPoint3D
  knots: number[]
  weights?: number[]
  controlPoints: DwgPoint3D[]
  fitPoints: DwgPoint3D[]
}
