const MODEL_SPACE = '*MODEL_SPACE'
const MODEL_SPACE_PREFIX = '*PAPER_SPACE'

export const isModelSpace = (name: string) => {
  return name && name.toUpperCase() == MODEL_SPACE
}

export const isPaperSpace = (name: string) => {
  return name && name.toUpperCase().startsWith(MODEL_SPACE_PREFIX)
}

export interface DwgPoint2D {
  x: number
  y: number
}

export interface DwgPoint2DWithWeight {
  x: number
  y: number
  w: number
}

export interface DwgPoint3D {
  x: number
  y: number
  z: number
}

export interface DwgPoint4D {
  x: number
  y: number
  z: number
  w: number
}
