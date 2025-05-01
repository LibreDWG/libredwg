import { Box2D } from './box2d'

type Transform =
  | { type: 'translate'; x: number; y: number }
  | { type: 'rotate'; angle: number }
  | { type: 'scale'; x: number; y: number }

export const transformBoundingBoxAndElement = (
  bbox: Box2D,
  element: string,
  transforms: Transform[] | undefined = undefined
): { bbox: Box2D; element: string } => {
  if (!transforms || transforms.length === 0) {
    return { bbox, element }
  }

  let transformedElement = element

  transforms.forEach(transform => {
    // Apply each transformation to the element
    if (transform.type === 'translate') {
      transformedElement = `<g transform="translate(${transform.x}, ${transform.y})">${transformedElement}</g>`
    } else if (transform.type === 'rotate') {
      transformedElement = `<g transform="rotate(${transform.angle})">${transformedElement}</g>`
    } else if (transform.type === 'scale') {
      transformedElement = `<g transform="scale(${transform.x}, ${transform.y})">${transformedElement}</g>`
    }
  })

  return { bbox, element: transformedElement }
}
