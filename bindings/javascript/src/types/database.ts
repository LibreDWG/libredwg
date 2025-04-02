import { DwgLayerTableEntry, DwgLTypeTableEntry, DwgTable } from "./tables";

export interface DwgDatabase {
  tables: {
    LAYER: DwgTable<DwgLayerTableEntry>;
    LTYPE: DwgTable<DwgLTypeTableEntry>;
  };
}

