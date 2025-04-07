import { DwgCommonObject } from './common'

export interface DwgPlotSettingObject extends DwgCommonObject {
  pageSetupName: string
  configName: string
  paperSize: string
  plotViewName: string
  marginLeft: number
  marginBottom: number
  marginRight: number
  marginTop: number
  paperWidth: number
  paperHeight: number
  plotOriginX: number
  plotOriginY: number
  windowAreaXMin: number
  windowAreaYMin: number
  windowAreaXMax: number
  windowAreaYMax: number
  printScaleNominator: number
  printScaleDenominator: number
  layoutFlag: number
  plotPaperUnit: number
  plotRotation: 0 | 1 | 2 | 3
  plotType: number
  currentStyleSheet: string
  standardScaleType: number
  shadePlotMode: number
  shadePlotResolution: number
  shadePlotCustomDPI?: number
  scaleFactor: number
  imageOriginX: number
  imageOriginY: number
  shadePlotId: string
}
