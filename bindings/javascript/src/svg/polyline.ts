import { DwgLWPolylineEntity, DwgPoint2D, DwgPolylineEntity } from '../database'
import { Vector2D } from './vector'

export function createPolylineArcPoints(
  from: DwgPoint2D,
  to: DwgPoint2D,
  bulge: number,
  resolution: number = 5
): DwgPoint2D[] {
  let theta: number
  let a: Vector2D
  let b: Vector2D

  if (bulge < 0) {
    theta = Math.atan(-bulge) * 4
    a = new Vector2D(from.x, from.y)
    b = new Vector2D(to.x, to.y)
  } else {
    theta = Math.atan(bulge) * 4
    a = new Vector2D(to.x, to.y)
    b = new Vector2D(from.x, from.y)
  }

  const ab = b.sub(a)
  const lengthAB = ab.length()
  const c = a.add(ab.multiply(0.5))

  const lengthCD = Math.abs(lengthAB / 2 / Math.tan(theta / 2))
  const normAB = ab.norm()

  const rotated = new Vector2D(
    normAB.x * Math.cos(Math.PI / 2) - normAB.y * Math.sin(Math.PI / 2),
    normAB.y * Math.cos(Math.PI / 2) + normAB.x * Math.sin(Math.PI / 2)
  )

  const d =
    theta < Math.PI
      ? c.add(rotated.multiply(-lengthCD))
      : c.add(rotated.multiply(lengthCD))

  const startAngle = (Math.atan2(b.y - d.y, b.x - d.x) / Math.PI) * 180
  let endAngle = (Math.atan2(a.y - d.y, a.x - d.x) / Math.PI) * 180
  if (endAngle < startAngle) {
    endAngle += 360
  }

  const r = b.sub(d).length()
  const startInter =
    Math.floor(startAngle / resolution) * resolution + resolution
  const endInter = Math.ceil(endAngle / resolution) * resolution - resolution

  const points: DwgPoint2D[] = []
  for (let i = startInter; i <= endInter; i += resolution) {
    const angleRad = (i / 180) * Math.PI
    const point = d.add(
      new Vector2D(Math.cos(angleRad) * r, Math.sin(angleRad) * r)
    )
    points.push(point)
  }

  if (bulge < 0) {
    points.reverse()
  }

  return points
}

export function interpolatePolyline(
  entity: DwgPolylineEntity | DwgLWPolylineEntity,
  closed: boolean = false
) {
  let points: DwgPoint2D[] = []
  const vertices = entity.vertices.map(v => {
    return {
      x: v.x,
      y: v.y,
      bulge: v.bulge
    }
  })
  if (closed) {
    vertices.push(vertices[0])
  }
  for (let i = 0, len = vertices.length; i < len - 1; ++i) {
    const from = vertices[i]
    const to = vertices[i + 1]
    points.push(from)
    if (vertices[i].bulge) {
      points = points.concat(
        createPolylineArcPoints(from, to, entity.vertices[i].bulge)
      )
    }
    // The last iteration of the for loop
    if (i === len - 2) {
      points.push(to)
    }
  }
  return points
}
