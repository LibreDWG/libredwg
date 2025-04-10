import { DwgPoint2D, DwgPoint3D } from '../common'
import { DwgEntity } from './entity'

export declare enum DwgViewportStatusFlag {
  PERSPECTIVE_MODE = 1,
  FRONT_CLIPPING = 2,
  BACK_CLIPPING = 4,
  UCS_FOLLOW = 8,
  FRONT_CLIP_NOT_AT_EYE = 16,
  UCS_ICON_VISIBILITY = 32,
  UCS_ICON_AT_ORIGIN = 64,
  FAST_ZOOM = 128,
  SNAP_MODE = 256,
  GRID_MODE = 512,
  ISOMETRIC_SNAP_STYLE = 1024,
  HIDE_PLOT_MODE = 2048,
  K_ISO_PAIR_TOP = 4096,
  K_ISO_PAIR_RIGHT = 8192,
  VIEWPORT_ZOOM_LOCKING = 16384,
  UNUSED = 32768,
  NON_RECTANGULAR_CLIPPING = 65536,
  VIEWPORT_OFF = 131072,
  GRID_BEYOND_DRAWING_LIMITS = 262144,
  ADAPTIVE_GRID_DISPLAY = 524288,
  SUBDIVISION_BELOW_SPACING = 1048576,
  GRID_FOLLOWS_WORKPLANE = 2097152
}

export declare enum DwgRenderMode {
  OPTIMIZED_2D = 0, // classic 2D
  WIREFRAME = 1,
  HIDDEN_LINE = 2,
  FLAT_SHADED = 3,
  GOURAUD_SHADED = 4,
  FLAT_SHADED_WITH_WIREFRAME = 5,
  GOURAUD_SHADED_WITH_WIREFRAME = 6
}

export declare enum DwgUCSPerViewport {
  UCS_UNCHANGED = 0,
  HAS_OWN_UCS = 1
}

export declare enum DwgOrthographicType {
  NON_ORTHOGRAPHIC = 0,
  TOP = 1,
  BOTTOM = 2,
  FRONT = 3,
  BACK = 4,
  LEFT = 5,
  RIGHT = 6
}

export declare enum DwgShadePlotMode {
  AS_DISPLAYED = 0,
  WIREFRAME = 1,
  HIDDEN = 2,
  RENDERED = 3
}

export declare enum DwgDefaultLightingType {
  ONE_DISTANT_LIGHT = 0,
  TWO_DISTANT_LIGHTS = 1
}

export interface DwgViewportEntity extends DwgEntity {
  type: 'VIEWPORT'
  viewportCenter: DwgPoint3D
  width: number
  height: number
  status: number
  viewportId: number
  displayCenter: DwgPoint2D
  snapBase: DwgPoint2D
  snapSpacing: DwgPoint2D
  gridSpacing: DwgPoint2D
  viewDirection: DwgPoint3D
  targetPoint: DwgPoint3D
  perspectiveLensLength: number
  frontClipZ: number
  backClipZ: number
  viewHeight: number
  snapAngle: number
  viewTwistAngle: number
  circleZoomPercent: number
  frozenLayerIds?: string[]
  statusBitFlags: number
  clippingBoundaryId?: string
  sheetName: string
  renderMode: DwgRenderMode
  ucsPerViewport: DwgUCSPerViewport
  ucsOrigin?: DwgPoint3D
  ucsXAxis?: DwgPoint3D
  ucsYAxis?: DwgPoint3D
  ucsId?: string
  ucsBaseId?: string
  orthographicType?: DwgOrthographicType
  elevation: number
  shadePlotMode: DwgShadePlotMode
  majorGridFrequency: number
  backgroundId?: string
  shadePlotId?: string
  visualStyleId?: string
  isDefaultLighting: boolean
  defaultLightingType?: DwgDefaultLightingType
  brightness: number
  contrast: number
  ambientLightColor?: string
  sunId?: string
  softPointer?: string
}
