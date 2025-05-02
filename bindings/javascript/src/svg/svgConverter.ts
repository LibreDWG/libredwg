import {
  DwgArcEntity,
  DwgBlockRecordTableEntry,
  DwgCircleEntity,
  DwgDatabase,
  DwgEllipseEntity,
  DwgEntity,
  DwgInsertEntity,
  DwgLayerTableEntry,
  DwgLineEntity,
  DwgLWPolylineEntity,
  DwgPoint2D,
  DwgPolylineEntity,
  DwgSplineEntity,
  DwgTextEntity,
  MODEL_SPACE
} from '../database'
import { Box2D } from './box2d'
import { evaluateBSpline } from './bspline'
import { Color } from './color'
import { interpolatePolyline } from './polyline'
import {
  BBoxAndElement,
  transformBoundingBoxAndElement
} from './transformBoundingBoxAndElement'

export class SvgConverter {
  private blockMap = new Map<string, BBoxAndElement>()

  private rotate(point: DwgPoint2D, angle: number) {
    const cos = Math.cos(angle)
    const sin = Math.sin(angle)
    return {
      x: point.x * cos - point.y * sin,
      y: point.x * sin + point.y * cos
    }
  }

  /**
   * Interpolates a B-spline curve and returns the resulting polyline.
   *
   * @param controlPoints The control points of the B-spline.
   * @param degree The degree of the B-spline.
   * @param knots The knot vector.
   * @param interpolationsPerSplineSegment Number of interpolated points per spline segment.
   * @param weights Optional weight vector for rational B-splines.
   * @returns An array of interpolated 2D points representing the polyline.
   */
  private interpolateBSpline(
    controlPoints: DwgPoint2D[],
    degree: number,
    knots: number[],
    interpolationsPerSplineSegment: number = 25,
    weights?: number[]
  ): DwgPoint2D[] {
    const polyline: DwgPoint2D[] = []

    const controlPointsForLib: number[][] = controlPoints.map(
      (p: DwgPoint2D) => [p.x, p.y]
    )

    const segmentTs: number[] = [knots[degree]]
    const domain: [number, number] = [
      knots[degree],
      knots[knots.length - 1 - degree]
    ]

    for (let k = degree + 1; k < knots.length - degree; ++k) {
      if (segmentTs[segmentTs.length - 1] !== knots[k]) {
        segmentTs.push(knots[k])
      }
    }

    for (let i = 1; i < segmentTs.length; ++i) {
      const uMin: number = segmentTs[i - 1]
      const uMax: number = segmentTs[i]

      for (let k = 0; k <= interpolationsPerSplineSegment; ++k) {
        const u: number =
          (k / interpolationsPerSplineSegment) * (uMax - uMin) + uMin
        let t: number = (u - domain[0]) / (domain[1] - domain[0])
        t = Math.max(0, Math.min(1, t)) // Clamp t to [0, 1]

        const p: number[] = evaluateBSpline(
          t,
          degree,
          controlPointsForLib,
          knots,
          weights
        )
        polyline.push({ x: p[0], y: p[1] })
      }
    }

    return polyline
  }

  private addFlipXIfApplicable(
    entity: DwgEntity,
    { bbox, element }: { bbox: Box2D; element: string }
  ) {
    if ('extrusionDirection' in entity && entity.extrusionDirection === -1) {
      return {
        bbox: new Box2D()
          .expandByPoint({ x: -bbox.min.x, y: bbox.min.y })
          .expandByPoint({ x: -bbox.max.x, y: bbox.max.y }),
        element: `<g transform="matrix(-1 0 0 1 0 0)">${element}</g>`
      }
    } else {
      return { bbox, element }
    }
  }

  private line(entity: DwgLineEntity) {
    const bbox = new Box2D()
      .expandByPoint({ x: entity.startPoint.x, y: entity.startPoint.y })
      .expandByPoint({ x: entity.endPoint.x, y: entity.endPoint.y })
    const element = `<line x1="${entity.startPoint.x}" y1="${entity.startPoint.y}" x2="${entity.endPoint.x}" y2="${entity.endPoint.y}" />`
    return transformBoundingBoxAndElement(bbox, element)
  }

  private text(entity: DwgTextEntity) {
    const fontsize = entity.textHeight
    const bbox = new Box2D().expandByPoint({
      x: entity.startPoint.x,
      y: entity.startPoint.y
    })
    if (entity.halign != 0) {
      bbox.expandByPoint({ x: entity.endPoint.x, y: entity.endPoint.y })
    } else {
      bbox.expandByPoint({
        x: entity.text.length * fontsize + entity.startPoint.x,
        y: entity.endPoint.y
      })
    }
    const insertionPoint = entity.startPoint
    const element = `\t<text x="${insertionPoint.x}" y="${insertionPoint.y}" font-size="${fontsize}" fill="blue">${entity.text}</text>\n`
    return transformBoundingBoxAndElement(bbox, element)
  }

  private vertices(vertices: DwgPoint2D[], closed: boolean = false) {
    const bbox = vertices.reduce(
      (acc: Box2D, point: DwgPoint2D) => acc.expandByPoint(point),
      new Box2D()
    )
    let d = vertices.reduce((acc: string, point: DwgPoint2D, i: number) => {
      acc += i === 0 ? 'M' : 'L'
      acc += point.x + ',' + point.y
      return acc
    }, '')
    if (closed) {
      d += 'Z'
    }
    return transformBoundingBoxAndElement(bbox, `<path d="${d}" />`)
  }

  private circle(entity: DwgCircleEntity) {
    const bbox0 = new Box2D()
      .expandByPoint({
        x: entity.center.x + entity.radius,
        y: entity.center.y + entity.radius
      })
      .expandByPoint({
        x: entity.center.x - entity.radius,
        y: entity.center.y - entity.radius
      })
    const element0 = `<circle cx="${entity.center.x}" cy="${entity.center.y}" r="${entity.radius}" />`
    const { bbox, element } = this.addFlipXIfApplicable(entity, {
      bbox: bbox0,
      element: element0
    })
    return transformBoundingBoxAndElement(bbox, element)
  }

  private ellipseOrArc(
    cx: number,
    cy: number,
    majorX: number,
    majorY: number,
    axisRatio: number,
    startAngle: number,
    endAngle: number
  ) {
    const rx = Math.sqrt(majorX * majorX + majorY * majorY)
    const ry = axisRatio * rx
    const rotationAngle = -Math.atan2(-majorY, majorX)

    const bbox = this.bboxEllipseOrArc(
      cx,
      cy,
      majorX,
      majorY,
      axisRatio,
      startAngle,
      endAngle
    )

    if (
      Math.abs(startAngle - endAngle) < 1e-9 ||
      Math.abs(startAngle - endAngle + Math.PI * 2) < 1e-9
    ) {
      const element = `<g transform="rotate(${
        (rotationAngle / Math.PI) * 180
      } ${cx}, ${cy})"><ellipse cx="${cx}" cy="${cy}" rx="${rx}" ry="${ry}" /></g>`
      return { bbox, element }
    } else {
      const startOffset = this.rotate(
        { x: Math.cos(startAngle) * rx, y: Math.sin(startAngle) * ry },
        rotationAngle
      )
      const startPoint = { x: cx + startOffset.x, y: cy + startOffset.y }
      const endOffset = this.rotate(
        { x: Math.cos(endAngle) * rx, y: Math.sin(endAngle) * ry },
        rotationAngle
      )
      const endPoint = { x: cx + endOffset.x, y: cy + endOffset.y }
      const adjustedEndAngle =
        endAngle < startAngle ? endAngle + Math.PI * 2 : endAngle
      const largeArcFlag = adjustedEndAngle - startAngle < Math.PI ? 0 : 1
      const d = `M ${startPoint.x} ${startPoint.y} A ${rx} ${ry} ${
        (rotationAngle / Math.PI) * 180
      } ${largeArcFlag} 1 ${endPoint.x} ${endPoint.y}`
      const element = `<path d="${d}" />`
      return { bbox, element }
    }
  }

  private bboxEllipseOrArc(
    cx: number,
    cy: number,
    majorX: number,
    majorY: number,
    axisRatio: number,
    startAngle: number,
    endAngle: number
  ) {
    while (startAngle < 0) startAngle += Math.PI * 2
    while (endAngle <= startAngle) endAngle += Math.PI * 2

    const angles: number[] = []
    if (Math.abs(majorX) < 1e-12 || Math.abs(majorY) < 1e-12) {
      for (let i = 0; i < 4; i++) {
        angles.push((i / 2) * Math.PI)
      }
    } else {
      angles[0] = Math.atan((-majorY * axisRatio) / majorX) - Math.PI
      angles[1] = Math.atan((majorX * axisRatio) / majorY) - Math.PI
      angles[2] = angles[0] - Math.PI
      angles[3] = angles[1] - Math.PI
    }

    for (let i = 4; i >= 0; i--) {
      while (angles[i] < startAngle) angles[i] += Math.PI * 2
      if (angles[i] > endAngle) {
        angles.splice(i, 1)
      }
    }

    angles.push(startAngle)
    angles.push(endAngle)

    const pts = angles.map(a => ({ x: Math.cos(a), y: Math.sin(a) }))

    const M = [
      [majorX, -majorY * axisRatio],
      [majorY, majorX * axisRatio]
    ]

    const rotatedPts = pts.map(p => ({
      x: p.x * M[0][0] + p.y * M[0][1] + cx,
      y: p.x * M[1][0] + p.y * M[1][1] + cy
    }))

    const bbox = rotatedPts.reduce(
      (acc: Box2D, p: { x: number; y: number }) => {
        acc.expandByPoint(p)
        return acc
      },
      new Box2D()
    )

    return bbox
  }

  private ellipse(entity: DwgEllipseEntity) {
    const { bbox: bbox0, element: element0 } = this.ellipseOrArc(
      entity.center.x,
      entity.center.y,
      entity.majorAxisEndPoint.x,
      entity.majorAxisEndPoint.y,
      entity.axisRatio,
      entity.startAngle,
      entity.endAngle
    )
    const { bbox, element } = this.addFlipXIfApplicable(entity, {
      bbox: bbox0,
      element: element0
    })
    return transformBoundingBoxAndElement(bbox, element)
  }

  private arc(entity: DwgArcEntity) {
    const { bbox: bbox0, element: element0 } = this.ellipseOrArc(
      entity.center.x,
      entity.center.y,
      entity.radius,
      0,
      1,
      entity.startAngle,
      entity.endAngle
    )
    const { bbox, element } = this.addFlipXIfApplicable(entity, {
      bbox: bbox0,
      element: element0
    })
    return transformBoundingBoxAndElement(bbox, element)
  }

  private insert(entity: DwgInsertEntity) {
    const block = this.blockMap.get(entity.name)
    if (block) {
      const transform = `matrix(${entity.xScale},0,0,${entity.yScale},${entity.insertionPoint.x},${entity.insertionPoint.y})`
      const newBBox = block.bbox.applyTransform(
        { x: entity.xScale, y: entity.yScale },
        entity.rotation,
        entity.insertionPoint
      )
      return {
        bbox: newBBox,
        element: `<use href="#${entity.name}" transform="${transform}" />`
      }
    }
    return null
  }

  private block(block: DwgBlockRecordTableEntry, dwg: DwgDatabase) {
    const entities = block.entities
    const { bbox, elements } = entities.reduce(
      (acc: { bbox: Box2D; elements: string[] }, entity: DwgEntity) => {
        const color = this.getEntityColor(dwg.tables.LAYER.entries, entity)
        const boundsAndElement = this.entityToBoundsAndElement(entity)
        if (boundsAndElement) {
          const { bbox, element } = boundsAndElement
          if (bbox.valid) {
            acc.bbox.expandByPoint(bbox.min)
            acc.bbox.expandByPoint(bbox.max)
          }
          acc.elements.push(`<g stroke="${color.cssColor}">${element}</g>`)
        }
        return acc
      },
      {
        bbox: new Box2D(),
        elements: []
      }
    )
    return {
      bbox,
      element: `<g id="${block.name}" stroke="#000000" stroke-width="0.1%" fill="none" transform="matrix(1,0,0,-1,0,0)">${elements.join('\n')}</g>`
    }
  }

  private entityToBoundsAndElement(entity: DwgEntity) {
    switch (entity.type) {
      case 'CIRCLE':
        return this.circle(entity as DwgCircleEntity)
      case 'ELLIPSE':
        return this.ellipse(entity as DwgEllipseEntity)
      case 'ARC':
        return this.arc(entity as DwgArcEntity)
      case 'SPLINE': {
        const spline = entity as DwgSplineEntity
        return this.vertices(
          this.interpolateBSpline(
            spline.controlPoints,
            spline.degree,
            spline.knots,
            25,
            spline.weights
          )
        )
      }
      case 'INSERT':
        return this.insert(entity as DwgInsertEntity)
      case 'LINE':
        return this.line(entity as DwgLineEntity)
      case 'LWPOLYLINE': {
        const lwpolyline = entity as DwgLWPolylineEntity
        const closed = !!(lwpolyline.flag & 0x200)
        const vertices = interpolatePolyline(lwpolyline, closed)
        return this.vertices(vertices, closed)
      }
      case 'POLYLINE': {
        const polyline = entity as DwgPolylineEntity
        const closed = !!(polyline.flag & 0x1)
        const vertices = interpolatePolyline(polyline, closed)
        return this.vertices(vertices, closed)
      }
      case 'TEXT':
        return this.text(entity as DwgTextEntity)
      default:
        return null
    }
  }

  private getEntityColor(
    layers: DwgLayerTableEntry[],
    entity: DwgEntity
  ): Color {
    // Get entity color
    const color = new Color()
    if (entity.color != null) {
      color.color = entity.color
    }
    if (entity.colorIndex != null) {
      color.colorIndex = entity.colorIndex
    }
    if (entity.colorName != null) {
      color.colorName = entity.colorName
    }

    // If color is 'byLayer', use the layer color
    if (color.isByLayer) {
      const layer = layers.find(
        (layer: DwgLayerTableEntry) => layer.name === entity.layer
      )
      if (layer != null) {
        color.colorIndex = layer.colorIndex
      }
    }
    if (color.color == null) {
      color.color = 0xffffff
    }
    return color
  }

  public convert(dwg: DwgDatabase) {
    let modelSpace = null
    this.blockMap.clear()
    let blockElements = ''
    dwg.tables.BLOCK_RECORD.entries.forEach(block => {
      if (block.name === MODEL_SPACE) {
        modelSpace = block
      } else {
        const item = this.block(block, dwg)
        blockElements += item.element
        this.blockMap.set(block.name, item)
      }
    })

    const { bbox, element } = this.block(modelSpace!, dwg)
    const viewBox = bbox.valid
      ? {
          x: bbox.min.x,
          y: -bbox.max.y,
          width: bbox.max.x - bbox.min.x,
          height: bbox.max.y - bbox.min.y
        }
      : {
          x: 0,
          y: 0,
          width: 0,
          height: 0
        }
    return `<?xml version="1.0"?>
<svg
  xmlns="http://www.w3.org/2000/svg"
  xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1"
  preserveAspectRatio="xMinYMin meet"
  viewBox="${viewBox.x} ${viewBox.y} ${viewBox.width} ${viewBox.height}"
  width="100%" height="100%"
>
  <defs>${blockElements}</defs>${element}
</svg>`
  }
}
