import { DwgPoint2D } from '../common'
import { DwgCommonObject } from './common'

export interface DwgImageDefObject extends DwgCommonObject {
  fileName: string
  size: DwgPoint2D
  sizeOfOnePixel: DwgPoint2D
  isLoaded: number
  resolutionUnits: number
}
