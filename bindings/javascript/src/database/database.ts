import { DwgEntity } from './entities'
import { DwgHeader } from './header'
import { DwgImageDefObject, DwgLayoutObject } from './objects'
import {
  DwgBlockRecordTableEntry,
  DwgDimStyleTableEntry,
  DwgLayerTableEntry,
  DwgLTypeTableEntry,
  DwgStyleTableEntry,
  DwgTable,
  DwgVPortTableEntry
} from './tables'

export interface DwgDatabase {
  tables: {
    BLOCK_RECORD: DwgTable<DwgBlockRecordTableEntry>
    DIMSTYLE: DwgTable<DwgDimStyleTableEntry>
    LAYER: DwgTable<DwgLayerTableEntry>
    LTYPE: DwgTable<DwgLTypeTableEntry>
    STYLE: DwgTable<DwgStyleTableEntry>
    VPORT: DwgTable<DwgVPortTableEntry>
  }
  objects: {
    IMAGEDEF: DwgImageDefObject[]
    LAYOUT: DwgLayoutObject[]
  }
  header: DwgHeader
  /**
   * All of entities in the model space.
   */
  entities: DwgEntity[]
}
