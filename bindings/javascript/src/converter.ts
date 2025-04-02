import { 
  DwgCommonTableEntry, 
  DwgDatabase, 
  DwgLayerTableEntry, 
  DwgLineTypeElement, 
  DwgLTypeTableEntry 
} from './types'
import { 
  Dwg_Color, 
  Dwg_Data_Ptr, 
  Dwg_LTYPE_Dash, 
  Dwg_Object_Object_Ptr, 
  Dwg_Object_Ptr, 
  LibreDwgEx 
} from './libredwg'
import { Dwg_Object_Type } from './utils'

/**
 * Class used to convert Dwg_Data instance to DwgDatabase instance.
 */
export class LibreDwgConverter {
  libredwg: LibreDwgEx

  constructor(instance: LibreDwgEx) {
    this.libredwg = instance
  }

  convert(data: Dwg_Data_Ptr) {
    const db: DwgDatabase = {
      tables: {
        LAYER: {
          entries: []
        },
        LTYPE: {
          entries: []
        }
      }
    }
    const libredwg = this.libredwg
    const num_objects = libredwg.dwg_get_num_objects(data);
    const results = [];
    for (let i = 0; i < num_objects; i++) {                                                  
      const obj = libredwg.dwg_get_object(data, i);
      const tio = libredwg.dwg_object_to_object_tio(obj);
      if (tio) {
        const fixedtype = libredwg.dwg_object_get_fixedtype(obj);
        switch(fixedtype) {
          case Dwg_Object_Type.DWG_TYPE_LAYER:
            db.tables.LAYER.entries.push(this.convertLayer(tio, obj))
            break;
          case Dwg_Object_Type.DWG_TYPE_LTYPE:
            db.tables.LTYPE.entries.push(this.convertLineType(tio, obj))
            break;
          default:
            break; 
        }
        results.push(tio);
      }
    }
    return db
  }

  private convertLayer(item: Dwg_Object_Object_Ptr, obj: Dwg_Object_Ptr): DwgLayerTableEntry {
    const commonAttrs = this.getCommonTableEntryAttrs(item, obj)
    const flag = this.libredwg.dwg_dynapi_entity_value(item, 'flag').data as number
    const frozen = this.libredwg.dwg_dynapi_entity_value(item, 'frozen').data as number
    const off = this.libredwg.dwg_dynapi_entity_value(item, 'off').data as number
    const frozenInNew = this.libredwg.dwg_dynapi_entity_value(item, 'frozen_in_new').data as number
    const locked = this.libredwg.dwg_dynapi_entity_value(item, 'plotflockedlag').data as number
    const plotFlag = this.libredwg.dwg_dynapi_entity_value(item, 'plotflag').data as number
    const linewt = this.libredwg.dwg_dynapi_entity_value(item, 'linewt').data as number
    const color = this.libredwg.dwg_dynapi_entity_value(item, 'color').data as Dwg_Color
    
    return {
      ...commonAttrs,
      standardFlag: flag,
      colorIndex: color.index,
      lineType: '',
      frozen: (frozen != 0),
      off: (off != 0),
      frozenInNew: (frozenInNew != 0),
      locked: (locked != 0),
      plotFlag: plotFlag,
      lineweight: linewt,
      plotStyleNameObjectId: '', 
      materialObjectId: '' 
    }
  }

  private convertLineType(item: Dwg_Object_Object_Ptr, obj: Dwg_Object_Ptr): DwgLTypeTableEntry {
    const commonAttrs = this.getCommonTableEntryAttrs(item, obj)
    const flag = this.libredwg.dwg_dynapi_entity_value(item, 'flag').data as number
    const description = this.libredwg.dwg_dynapi_entity_value(item, 'description').data as string
    const numDashes = this.libredwg.dwg_dynapi_entity_value(item, 'numdashes').data as number
    const patternLen = this.libredwg.dwg_dynapi_entity_value(item, 'pattern_len').data as number
    // const dashes = this.libredwg.dwg_dynapi_entity_value(item, 'dashes').data as Dwg_Array_Ptr
    // const dashArray = dashes ? this.libredwg.dwg_ptr_to_ltype_dash_array(dashes, numDashes) : []
    const dashArray: Dwg_LTYPE_Dash[] = []
  
    return {
      ...commonAttrs,
      description: description,
      standardFlag: flag,
      numberOfLineTypes: numDashes,
      totalPatternLength: patternLen,
      pattern: this.convertLineTypePattern(dashArray)
    }
  }

  private convertLineTypePattern(dashes: Dwg_LTYPE_Dash[]) {
    const patterns: DwgLineTypeElement[] = []
    dashes.forEach((dash) => {
      // For now always convert complex line type to simple line type
      patterns.push({
        elementLength: dash.length || 0,
        elementTypeFlag: 0
      })
    })
    return patterns
  }

  private getCommonTableEntryAttrs(
    tio: number,
    obj: Dwg_Object_Ptr,
  ): DwgCommonTableEntry {
    const libredwg = this.libredwg
    const object_tio = libredwg.dwg_object_get_tio(obj)
    const ownerhandle = libredwg.dwg_object_object_get_ownerhandle_object(object_tio)
    const handle = libredwg.dwg_object_get_handle_object(obj)
    return {
      handle: handle.value,
      ownerHandle: ownerhandle.value,
      name: libredwg.dwg_dynapi_entity_value(tio, 'name').data as string
    }
  }
}