export class Vector2D {
  x: number
  y: number

  constructor(x: number, y: number) {
    this.x = x
    this.y = y
  }

  add(v: Vector2D): Vector2D {
    return new Vector2D(this.x + v.x, this.y + v.y)
  }

  sub(v: Vector2D): Vector2D {
    return new Vector2D(this.x - v.x, this.y - v.y)
  }

  multiply(scalar: number): Vector2D {
    return new Vector2D(this.x * scalar, this.y * scalar)
  }

  length(): number {
    return Math.sqrt(this.x ** 2 + this.y ** 2)
  }

  norm(): Vector2D {
    const len = this.length()
    if (len === 0) return new Vector2D(0, 0)
    return new Vector2D(this.x / len, this.y / len)
  }
}
