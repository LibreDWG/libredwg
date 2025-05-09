import {
  DwgArcEntity,
  DwgAttachmentPoint,
  DwgBlockRecordTableEntry,
  DwgCircleEntity,
  DwgDatabase,
  DwgDimensionEntity,
  DwgEllipseEntity,
  DwgEntity,
  DwgInsertEntity,
  DwgLayerTableEntry,
  DwgLineEntity,
  DwgLWPolylineEntity,
  DwgMTextEntity,
  DwgPoint2D,
  DwgPoint3D,
  DwgPolylineEntity,
  DwgRayEntity,
  DwgSplineEntity,
  DwgTableCell,
  DwgTableEntity,
  DwgTextEntity,
  DwgTextHorizontalAlign,
  DwgXlineEntity,
  isModelSpace
} from '../database'
import { Box2D } from './box2d'
import { evaluateBSpline } from './bspline'
import { Color } from './color'
import { interpolatePolyline } from './polyline'
import { BBoxAndElement } from './transformBoundingBoxAndElement'

type SvgAnchorType = 'start' | 'middle' | 'end'

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
    if (
      'extrusionDirection' in entity &&
      (entity.extrusionDirection as DwgPoint3D).z === -1
    ) {
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

  private line(entity: DwgLineEntity): BBoxAndElement {
    const bbox = new Box2D()
      .expandByPoint({ x: entity.startPoint.x, y: entity.startPoint.y })
      .expandByPoint({ x: entity.endPoint.x, y: entity.endPoint.y })
    const element = `<line x1="${entity.startPoint.x}" y1="${entity.startPoint.y}" x2="${entity.endPoint.x}" y2="${entity.endPoint.y}" />`
    return { bbox, element }
  }

  private ray(entity: DwgRayEntity): BBoxAndElement {
    const scale = 10000
    const firstPoint = entity.firstPoint
    const secondPoint = {
      x: firstPoint.x + entity.unitDirection.x * scale,
      y: firstPoint.y + entity.unitDirection.y * scale
    }
    const bbox = new Box2D()
      .expandByPoint(firstPoint)
      .expandByPoint(secondPoint)
    const element = `<line x1="${firstPoint.x}" y1="${firstPoint.y}" x2="${secondPoint.x}" y2="${secondPoint.y}" />`
    return { bbox, element }
  }

  private xline(entity: DwgXlineEntity): BBoxAndElement {
    const scale = 10000
    const firstPoint = {
      x: entity.firstPoint.x - entity.unitDirection.x * scale,
      y: entity.firstPoint.y - entity.unitDirection.y * scale
    }
    const secondPoint = {
      x: entity.firstPoint.x + entity.unitDirection.x * scale,
      y: entity.firstPoint.y + entity.unitDirection.y * scale
    }
    const bbox = new Box2D()
      .expandByPoint(firstPoint)
      .expandByPoint(secondPoint)
    const element = `<line x1="${firstPoint.x}" y1="${firstPoint.y}" x2="${secondPoint.x}" y2="${secondPoint.y}" />`
    return { bbox, element }
  }

  private extractMTextLines(mtext: string) {
    return (
      mtext
        // Convert Unicode codes to characters
        .replace(/\\U\+([0-9A-Fa-f]{4})/g, (_, hex) =>
          String.fromCharCode(parseInt(hex, 16))
        )
        // Preserve line breaks: replace \P with newline placeholder
        .replace(/\\P/g, '\n')
        // Remove underline, overline
        .replace(/\\[LOlo]/g, '')
        // Remove font specs like \FArial|b0|i0|c134|p49;
        .replace(/\\[Ff][^;\\]*?(?:\|[^;\\]*)*;/g, '')
        // Remove formatting codes like \H1.0x; \W0.5; \C7 etc.
        .replace(/\\[KkCcHhWwTtAa][^;\\]*;?/g, '')
        // Remove general \word; style control codes like \x; \pxqc;
        .replace(/\\[a-zA-Z]+;?/g, '')
        // Remove AutoCAD %% control sequences like %%d, %%p, etc.
        .replace(/%%(d|p|c|%)/gi, '')
        // Replace escaped backslash
        .replace(/\\\\/g, '\\')
        // Replace non-breaking space
        .replace(/\\~/g, '\u00A0')
        // Remove grouping braces
        .replace(/[{}]/g, '')
        // Split by preserved newlines
        .split('\n')
        .map(line => line.trim())
        .filter(line => line.length > 0)
    )
  }

  private lines(
    lines: string[],
    fontsize: number,
    insertionPoint: DwgPoint2D,
    extentsWidth: number,
    anchor: SvgAnchorType = 'start'
  ): BBoxAndElement {
    const bbox = new Box2D()
      .expandByPoint({
        x: insertionPoint.x,
        y: insertionPoint.y
      })
      .expandByPoint({
        x: insertionPoint.x + extentsWidth,
        y: insertionPoint.y - lines.length * fontsize * 1.5
      })
    const texts = lines.map((line, index) => {
      const x = insertionPoint.x
      const y = insertionPoint.y - index * fontsize * 1.5
      const transform = `translate(${x},${y}) scale(1,-1) translate(${-x},${-y})`

      return `<text x="${x}" y="${y}" font-size="${fontsize}" text-anchor="${anchor}" transform="${transform}">${line}</text>`
    })
    return { bbox, element: texts.join('\n') }
  }

  private mtext(entity: DwgMTextEntity): BBoxAndElement {
    const fontsize = entity.textHeight
    const insertionPoint = entity.insertionPoint
    const lines = this.extractMTextLines(entity.text)
    const attachmentPoint = entity.attachmentPoint
    let anchor: SvgAnchorType = 'start'
    if (
      attachmentPoint == DwgAttachmentPoint.BottomCenter ||
      attachmentPoint == DwgAttachmentPoint.MiddleCenter ||
      attachmentPoint == DwgAttachmentPoint.TopCenter
    ) {
      anchor = 'middle'
    } else if (
      attachmentPoint == DwgAttachmentPoint.BottomRight ||
      attachmentPoint == DwgAttachmentPoint.MiddleRight ||
      attachmentPoint == DwgAttachmentPoint.TopRight
    ) {
      anchor = 'end'
    }
    return this.lines(
      lines,
      fontsize,
      insertionPoint,
      entity.extentsWidth,
      anchor
    )
  }

  private table(entity: DwgTableEntity) {
    const {
      rowCount,
      columnCount,
      rowHeightArr,
      columnWidthArr,
      startPoint,
      cells
    } = entity

    const originX = startPoint.x
    const originY = startPoint.y

    // Compute cell rectangles
    const cellRects: {
      x: number
      y: number
      width: number
      height: number
      cell: DwgTableCell
      row: number
      col: number
    }[] = []

    for (let row = 0, y = originY; row < rowCount; row++) {
      const height = rowHeightArr[row]
      let x = originX
      for (let col = 0; col < columnCount; col++) {
        const cellIndex = row * columnCount + col
        const cell = cells[cellIndex]
        const width = columnWidthArr[col]

        cellRects.push({ x, y, width, height, cell, row, col })
        x += width
      }
      y += height
    }

    // Create SVG content
    const svgElements = cellRects
      .map(({ x, y, width, height, cell }) => {
        const lines: string[] = []

        if (cell.topBorderVisibility)
          lines.push(
            `<line x1="${x}" y1="${y}" x2="${x + width}" y2="${y}" stroke="black" />`
          )
        if (cell.bottomBorderVisibility)
          lines.push(
            `<line x1="${x}" y1="${y + height}" x2="${x + width}" y2="${y + height}" stroke="black" />`
          )
        if (cell.leftBorderVisibility)
          lines.push(
            `<line x1="${x}" y1="${y}" x2="${x}" y2="${y + height}" stroke="black" />`
          )
        if (cell.rightBorderVisibility)
          lines.push(
            `<line x1="${x + width}" y1="${y}" x2="${x + width}" y2="${y + height}" stroke="black" />`
          )

        const textX = x + width / 2
        const textY = y + height / 2 + cell.textHeight / 3
        const text = `<text x="${textX}" y="${textY}" font-size="${cell.textHeight}" text-anchor="middle" dominant-baseline="middle">${cell.text}</text>`

        return [...lines, text].join('\n')
      })
      .join('\n')

    const totalWidth = columnWidthArr.reduce((sum, w) => sum + w, 0)
    const totalHeight = rowHeightArr.reduce((sum, h) => sum + h, 0)

    const bbox: Box2D = new Box2D()
      .expandByPoint({ x: originX, y: originY })
      .expandByPoint({ x: originX + totalWidth, y: originY + totalHeight })

    const svg = `
  <svg xmlns="http://www.w3.org/2000/svg" width="${totalWidth}" height="${totalHeight}" viewBox="${originX} ${originY} ${totalWidth} ${totalHeight}">
  ${svgElements}
  </svg>
    `.trim()

    return {
      bbox,
      element: svg
    }
  }

  private text(entity: DwgTextEntity): BBoxAndElement {
    const fontsize = entity.textHeight
    const insertionPoint = entity.startPoint
    const lines = [entity.text]
    let extentsWidth = entity.endPoint.x - entity.endPoint.x
    if (entity.halign == 0) {
      extentsWidth = entity.text.length * fontsize + entity.startPoint.x
    }
    let anchor: SvgAnchorType = 'start'
    if (entity.halign == DwgTextHorizontalAlign.CENTER) {
      anchor = 'middle'
    } else if (entity.halign == DwgTextHorizontalAlign.RIGHT) {
      anchor = 'end'
    }
    return this.lines(lines, fontsize, insertionPoint, extentsWidth, anchor)
  }

  private vertices(
    vertices: DwgPoint2D[],
    closed: boolean = false
  ): BBoxAndElement {
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
    return { bbox, element: `<path d="${d}" />` }
  }

  private circle(entity: DwgCircleEntity): BBoxAndElement {
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
    return {
      bbox: bbox0,
      element: element0
    }
  }

  private ellipseOrArc(
    cx: number,
    cy: number,
    majorX: number,
    majorY: number,
    axisRatio: number,
    startAngle: number,
    endAngle: number
  ): BBoxAndElement {
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

  private ellipse(entity: DwgEllipseEntity): BBoxAndElement {
    const { bbox: bbox0, element: element0 } = this.ellipseOrArc(
      entity.center.x,
      entity.center.y,
      entity.majorAxisEndPoint.x,
      entity.majorAxisEndPoint.y,
      entity.axisRatio,
      entity.startAngle,
      entity.endAngle
    )
    return {
      bbox: bbox0,
      element: element0
    }
  }

  private arc(entity: DwgArcEntity): BBoxAndElement {
    const { bbox: bbox0, element: element0 } = this.ellipseOrArc(
      entity.center.x,
      entity.center.y,
      entity.radius,
      0,
      1,
      entity.startAngle,
      entity.endAngle
    )
    return {
      bbox: bbox0,
      element: element0
    }
  }

  private dimension(entity: DwgDimensionEntity): BBoxAndElement | null {
    const block = this.blockMap.get(entity.name)
    if (block) {
      return {
        bbox: block.bbox,
        element: `<use href="#${entity.name}" />`
      }
    }
    return null
  }

  private insert(entity: DwgInsertEntity): BBoxAndElement | null {
    const block = this.blockMap.get(entity.name)
    if (block) {
      // In SVG, the unit of rotate is degrees — not radians.
      const insertionPoint = entity.insertionPoint
      // const basePoint = block.bbox.min
      const rotation = entity.rotation * (180 / Math.PI)
      const transform = `translate(${insertionPoint.x},${insertionPoint.y}) rotate(${rotation}) scale(${entity.xScale},${entity.yScale})`
      const newBBox = block.bbox
        .clone()
        .transform(
          { x: entity.xScale, y: entity.yScale },
          { x: insertionPoint.x, y: insertionPoint.y }
        )
        .rotate(entity.rotation, insertionPoint)
      return {
        bbox: newBBox,
        element: `<use href="#${entity.name}" transform="${transform}" />`
      }
    }
    return null
  }

  private block(
    block: DwgBlockRecordTableEntry,
    dwg: DwgDatabase
  ): BBoxAndElement | null {
    const entities = block.entities
    const { bbox, elements } = entities.reduce(
      (acc: { bbox: Box2D; elements: string[] }, entity: DwgEntity) => {
        const boundsAndElement = this.entityToBoundsAndElement(entity)
        if (boundsAndElement) {
          const { bbox, element } = boundsAndElement
          if (bbox.valid) {
            acc.bbox.expandByPoint(bbox.min)
            acc.bbox.expandByPoint(bbox.max)
          }
          const color = this.getEntityColor(dwg.tables.LAYER.entries, entity)
          const fill =
            entity.type == 'TEXT' || entity.type == 'MTEXT'
              ? color.cssColor
              : 'none'
          if (color.isByBlock) {
            acc.elements.push(`<g id="${entity.handle}">${element}</g>`)
          } else {
            acc.elements.push(
              `<g id="${entity.handle}" stroke="${color.cssColor}" fill="${fill}">${element}</g>`
            )
          }
        }
        return acc
      },
      {
        bbox: new Box2D(),
        elements: []
      }
    )
    if (bbox.valid) {
      return {
        bbox,
        element: `<g id="${block.name}">${elements.join('\n')}</g>`
      }
    }
    return null
  }

  private entityToBoundsAndElement(entity: DwgEntity) {
    let result = null
    switch (entity.type) {
      case 'ARC':
        result = this.arc(entity as DwgArcEntity)
        break
      case 'CIRCLE':
        result = this.circle(entity as DwgCircleEntity)
        break
      case 'DIMENSION':
        result = this.dimension(entity as DwgDimensionEntity)
        break
      case 'ELLIPSE':
        result = this.ellipse(entity as DwgEllipseEntity)
        break
      case 'INSERT':
        result = this.insert(entity as DwgInsertEntity)
        break
      case 'LINE':
        result = this.line(entity as DwgLineEntity)
        break
      case 'LWPOLYLINE': {
        const lwpolyline = entity as DwgLWPolylineEntity
        const closed = !!(lwpolyline.flag & 0x200)
        const vertices = interpolatePolyline(lwpolyline, closed)
        result = this.vertices(vertices, closed)
        break
      }
      case 'MTEXT':
        result = this.mtext(entity as DwgMTextEntity)
        break
      case 'SPLINE': {
        const spline = entity as DwgSplineEntity
        result = this.vertices(
          this.interpolateBSpline(
            spline.controlPoints,
            spline.degree,
            spline.knots,
            25,
            spline.weights
          )
        )
        break
      }
      case 'POLYLINE': {
        const polyline = entity as DwgPolylineEntity
        const closed = !!(polyline.flag & 0x1)
        const vertices = interpolatePolyline(polyline, closed)
        result = this.vertices(vertices, closed)
        break
      }
      case 'RAY':
        result = this.ray(entity as DwgRayEntity)
        break
      case 'TABLE':
        result = this.table(entity as DwgTableEntity)
        break
      case 'TEXT':
        result = this.text(entity as DwgTextEntity)
        break
      case 'XLINE':
        result = this.xline(entity as DwgXlineEntity)
        break
      default:
        result = null
        break
    }
    if (result) {
      return this.addFlipXIfApplicable(entity, result)
    }
    return null
  }

  private getEntityColor(
    layers: DwgLayerTableEntry[],
    entity: DwgEntity
  ): Color {
    // Get entity color
    const color = new Color()
    if (entity.colorIndex != null) {
      color.colorIndex = entity.colorIndex
    } else if (entity.colorName) {
      color.colorName = entity.colorName
    } else if (entity.color != null) {
      color.color = entity.color
    }

    // If it is white color, convert it to black because the background of svg is white
    if (color.colorIndex == 7) {
      color.colorIndex = 256
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
      if (isModelSpace(block.name)) {
        modelSpace = block
      } else {
        const item = this.block(block, dwg)
        if (item) {
          blockElements += item.element
          this.blockMap.set(block.name, item)
        }
      }
    })

    const ms = this.block(modelSpace!, dwg)
    const viewBox =
      ms && ms.bbox.valid
        ? {
            x: ms.bbox.min.x,
            y: -ms.bbox.max.y,
            width: ms.bbox.max.x - ms.bbox.min.x,
            height: ms.bbox.max.y - ms.bbox.min.y
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
  <defs>${blockElements}</defs>
  <g stroke="#000000" stroke-width="0.1%" fill="none" transform="matrix(1,0,0,-1,0,0)">
    ${ms ? ms.element : ''}
  </g>
</svg>`
  }
}
