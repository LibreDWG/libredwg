/**
 * Copied and ported to code standard as the b-spline library is not maintained any longer.
 * Source:
 * https://github.com/thibauts/b-spline
 * Copyright (c) 2015 Thibaut Séguy <thibaut.seguy@gmail.com>
 *
 * B-spline evaluator with optional weights and knot vector.
 * Returns a point on the curve given parameter t ∈ [0,1].
 */
export function evaluateBSpline(
  t: number,
  degree: number,
  points: number[][],
  knots?: number[],
  weights?: number[]
): number[] {
  const n = points.length // Number of control points
  if (n === 0) throw new Error('points must not be empty')

  const d = points[0].length // Dimensionality

  if (t < 0 || t > 1) {
    throw new Error(`t out of bounds [0,1]: ${t}`)
  }
  if (degree < 1) {
    throw new Error('degree must be at least 1 (linear)')
  }
  if (degree > n - 1) {
    throw new Error('degree must be less than or equal to point count - 1')
  }

  const weightsSafe: number[] = weights ?? new Array(n).fill(1)

  const knotsSafe: number[] =
    knots ??
    (() => {
      const result: number[] = []
      for (let i = 0; i < n + degree + 1; i++) {
        result.push(i)
      }
      return result
    })()

  if (knotsSafe.length !== n + degree + 1) {
    throw new Error('bad knot vector length')
  }

  const domain: [number, number] = [degree, knotsSafe.length - 1 - degree]
  const low = knotsSafe[domain[0]]
  const high = knotsSafe[domain[1]]
  t = t * (high - low) + low

  t = Math.max(t, low)
  t = Math.min(t, high)

  let s: number = domain[0]
  for (; s < domain[1]; s++) {
    if (t >= knotsSafe[s] && t <= knotsSafe[s + 1]) {
      break
    }
  }

  // Homogeneous coordinates
  const v: number[][] = new Array(n)
  for (let i = 0; i < n; i++) {
    v[i] = new Array(d + 1)
    for (let j = 0; j < d; j++) {
      v[i][j] = points[i][j] * weightsSafe[i]
    }
    v[i][d] = weightsSafe[i]
  }

  // De Boor algorithm
  for (let l = 1; l <= degree + 1; l++) {
    for (let i = s; i > s - degree - 1 + l; i--) {
      const denom = knotsSafe[i + degree + 1 - l] - knotsSafe[i]
      const alpha = denom === 0 ? 0 : (t - knotsSafe[i]) / denom
      for (let j = 0; j < d + 1; j++) {
        v[i][j] = (1 - alpha) * v[i - 1][j] + alpha * v[i][j]
      }
    }
  }

  const result: number[] = new Array(d)
  for (let i = 0; i < d; i++) {
    result[i] = round10(v[s][i] / v[s][d], -9)
  }

  return result
}

/**
 * Rounds a number to a specified exponent of 10.
 * Equivalent to shifting the decimal point, rounding, then shifting back.
 *
 * @param value - The number to round.
 * @param exp - The exponent (e.g., -1 for 1 decimal place, -9 for nanometers).
 * @returns The rounded number.
 */
function round10(value: number, exp: number): number {
  if (exp === 0 || exp === undefined) {
    return Math.round(value)
  }

  if (isNaN(value) || !Number.isInteger(exp)) {
    return NaN
  }

  // Shift
  const [base, exponent = '0'] = value.toString().split('e')
  const shifted = Math.round(Number(`${base}e${+exponent - exp}`))

  // Shift back
  const [shiftedBase, shiftedExp = '0'] = shifted.toString().split('e')
  return Number(`${shiftedBase}e${+shiftedExp + exp}`)
}
