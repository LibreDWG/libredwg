import { DwgImageDefObject, DwgLayoutObject } from "./objects";
import { 
  DwgLayerTableEntry,
  DwgLTypeTableEntry,
  DwgTable,
  DwgVPortTableEntry
} from "./tables";

export interface DwgDatabase {
  tables: {
    LAYER: DwgTable<DwgLayerTableEntry>;
    LTYPE: DwgTable<DwgLTypeTableEntry>;
    VPORT: DwgTable<DwgVPortTableEntry>;
  };
  objects: {
    IMAGEDEF: DwgImageDefObject[];
    LAYOUT: DwgLayoutObject[];
  };
}

