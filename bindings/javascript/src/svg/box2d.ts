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

  /**
   * Expands the bounding box to include a point.
   * @param point point to include
   * @returns Returned this bounding box
   */
  expandByPoint(point: DwgPoint2D): Box2D {
    this.min.x = Math.min(this.min.x, point.x)
    this.min.y = Math.min(this.min.y, point.y)
    this.max.x = Math.max(this.max.x, point.x)
    this.max.y = Math.max(this.max.y, point.y)
    this.valid = true
    return this
  }

  /**
   * Applies a transformation to the bounding box and return a new bounding box.
   * @param scale scale factor
   * @param rotationRad rotation in radians
   * @param translation translation vector
   * @returns Returned the transformed bounding box
   */
  applyTransform(
    scale: DwgPoint2D,
    rotationRad: number,
    translation: DwgPoint2D
  ): Box2D {
    if (!this.valid) return this

    const cos = Math.cos(rotationRad)
    const sin = Math.sin(rotationRad)

    // Helper to transform a single point
    const transformPoint = (p: DwgPoint2D): DwgPoint2D => {
      const x = p.x * scale.x
      const y = p.y * scale.y
      return {
        x: x * cos - y * sin + translation.x,
        y: x * sin + y * cos + translation.y
      }
    }

    // Get all 4 corners of the box
    const corners = [
      { x: this.min.x, y: this.min.y },
      { x: this.min.x, y: this.max.y },
      { x: this.max.x, y: this.min.y },
      { x: this.max.x, y: this.max.y }
    ]

    // Create a new bounding box from transformed corners
    const newBBox = new Box2D()
    for (const corner of corners) {
      const transformed = transformPoint(corner)
      newBBox.expandByPoint(transformed)
    }

    return newBBox
  }
}
