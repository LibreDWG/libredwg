import { DwgPoint2D } from '../database'

export class Box2D {
  public min: DwgPoint2D
  public max: DwgPoint2D
  public valid: boolean

  constructor() {
    this.min = { x: Infinity, y: Infinity }
    this.max = { x: -Infinity, y: -Infinity }
    this.valid = false
  }

  expandByPoint(point: DwgPoint2D): Box2D {
    this.min.x = Math.min(this.min.x, point.x)
    this.min.y = Math.min(this.min.y, point.y)
    this.max.x = Math.max(this.max.x, point.x)
    this.max.y = Math.max(this.max.y, point.y)
    this.valid = true
    return this
  }
}
