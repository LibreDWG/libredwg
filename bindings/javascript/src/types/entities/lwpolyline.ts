import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export interface DwgLWPolylineEntity extends DwgEntity {
  type: 'LWPOLYLINE'
  numberOfVertices: number
  flag: number
  constantWidth?: number
  elevation: number
  thickness: number
  extrusionDirection: DwgPoint3D
  vertices: DwgLWPolylineVertex[]
}

export interface DwgLWPolylineVertex extends DwgPoint2D {
  id: number
  startWidth?: number
  endWidth?: number
  bulge: number
}
