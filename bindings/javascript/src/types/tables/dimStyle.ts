import { DwgCommonTableEntry } from './table'

export interface DwgDimStyleTableEntry extends DwgCommonTableEntry {
  DIMPOST?: string
  DIMAPOST?: string
  DIMBLK: string
  DIMBLK1: string
  DIMBLK2: string
  DIMSCALE: number
  DIMASZ: number
  DIMEXO: number
  DIMDLI: number
  DIMEXE: number
  DIMRND: number
  DIMDLE: number
  DIMTP: number
  DIMTM: number
  DIMTXT: number
  DIMCEN: number
  DIMTSZ: number
  DIMALTF: number
  DIMLFAC: number
  DIMTVP: number
  DIMTFAC: number
  DIMGAP: number
  DIMALTRND: number
  DIMTOL: number
  DIMLIM: number
  DIMTIH: number
  DIMTOH: number
  DIMSE1: 0 | 1
  DIMSE2: 0 | 1
  DIMTAD: number
  DIMZIN: number
  DIMAZIN: number
  DIMALT: 0 | 1
  DIMALTD: number
  DIMTOFL: 0 | 1
  DIMSAH: 0 | 1
  DIMTIX: 0 | 1
  DIMSOXD: 0 | 1
  DIMCLRD: number
  DIMCLRE: number
  DIMCLRT: number
  DIMADEC?: number
  DIMUNIT?: number
  DIMDEC: number
  DIMTDEC: number
  DIMALTU: number
  DIMALTTD: number
  DIMAUNIT: number
  DIMFRAC: number
  DIMLUNIT: number
  DIMDSEP: string
  DIMTMOVE: number
  DIMJUST: number
  DIMSD1: 0 | 1
  DIMSD2: 0 | 1
  DIMTOLJ: number
  DIMTZIN: number
  DIMALTZ: number
  DIMALTTZ: number
  DIMFIT?: number
  DIMUPT: number
  DIMATFIT: number
  DIMTXSTY?: number
  DIMLDRBLK?: number
  DIMLWD: number
  DIMLWE: number

  DIMFXL: number
  DIMJOGANG: number
  DIMTFILL: number
  DIMTFILLCLR: number
  DIMARCSYM: number
  DIMCLRD_N: number
  DIMCLRE_N: number
  DIMCLRT_N: number
  DIMFXLON: number
  DIMTXTDIRECTION: number
  DIMALTMZF: number
  DIMALTMZS: string
  DIMMZF: number
  DIMMZS: string
}
