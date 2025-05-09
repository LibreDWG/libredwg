import { DwgPoint2D } from '../database'

/**
 * Represents a 2D axis-aligned bounding box.
 */
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
   * Expands the bounding box to include a given point.
   *
   * @param point - The point to include in the bounding box.
   * @returns This bounding box after expansion.
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
   * Applies a scaling and translation transformation to this bounding box.
   *
   * @param scale - The scaling factors in x and y directions.
   * @param translation - The translation offsets in x and y directions.
   * @returns This bounding box after transformation.
   */
  transform(scale: DwgPoint2D, translation: DwgPoint2D): Box2D {
    const corners = this.getCorners().map(p => ({
      x: p.x * scale.x + translation.x,
      y: p.y * scale.y + translation.y
    }))

    this.reset()
    for (const pt of corners) {
      this.expandByPoint(pt)
    }

    return this
  }

  /**
   * Applies a rotation around a specific point to this bounding box.
   *
   * @param angleInRad - The angle of rotation in radians.
   * @param point - The center of rotation.
   * @returns This bounding box after rotation.
   */
  rotate(angleInRad: number, point: DwgPoint2D): Box2D {
    const cos = Math.cos(angleInRad)
    const sin = Math.sin(angleInRad)

    const corners = this.getCorners().map(p => {
      const dx = p.x - point.x
      const dy = p.y - point.y
      return {
        x: point.x + dx * cos - dy * sin,
        y: point.y + dx * sin + dy * cos
      }
    })

    this.reset()
    for (const pt of corners) {
      this.expandByPoint(pt)
    }

    return this
  }

  /**
   * Creates a deep copy of this bounding box.
   *
   * @returns A new instance of Box2D with the same properties.
   */
  clone(): Box2D {
    const box = new Box2D()
    box.min = { x: this.min.x, y: this.min.y }
    box.max = { x: this.max.x, y: this.max.y }
    box.valid = this.valid
    return box
  }

  /**
   * Resets this bounding box to its initial unbounded state.
   */
  private reset(): void {
    this.min = { x: Infinity, y: Infinity }
    this.max = { x: -Infinity, y: -Infinity }
    this.valid = false
  }

  /**
   * Retrieves the four corner points of the bounding box.
   *
   * @returns An array of corner points in the order:
   * bottom-left, top-left, bottom-right, top-right.
   */
  private getCorners(): DwgPoint2D[] {
    return [
      { x: this.min.x, y: this.min.y },
      { x: this.min.x, y: this.max.y },
      { x: this.max.x, y: this.min.y },
      { x: this.max.x, y: this.max.y }
    ]
  }
}
