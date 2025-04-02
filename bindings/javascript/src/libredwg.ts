import { LibreDwgConverter } from './converter';
import { MainModule } from '../wasm/libredwg-web'
import { createModule, Dwg_File_Type, Dwg_Object_Type } from './utils';
import { DwgPoint2D, DwgPoint3D } from './types';

export type Dwg_Array_Ptr = number;
export type Dwg_Data_Ptr = number;
export type Dwg_Object_Ptr = number;
export type Dwg_Object_Ref_Ptr = number;
export type Dwg_Object_Object_Ptr = number;
export type Dwg_Object_Entity_Ptr = number;

export interface Dwg_Handle {
  code: number
  size: number
  value: number
  is_global: number
}

export interface Dwg_Object_Ref {
  obj: Dwg_Object_Ptr
  handleref: Dwg_Handle
  absolute_ref: number
  r11_idx: number
}

export interface Dwg_Color {
  index: number
  flag: number
  rgb: number
  name: string
  book_name: string
}

export interface Dwg_LTYPE_Dash {
  length: number
  complex_shapecode: number
  style: number
  x_offset: number
  y_offset: number
  scale: number
  rotation: number
  shape_flag: number
  text: string
}

export interface Dwg_Field_Value {
  success: boolean
  message?: string
  data?: string | number | Dwg_Color | Dwg_Array_Ptr | DwgPoint2D | DwgPoint3D
}

export type LibreDwgEx = LibreDwg & MainModule;

export class LibreDwg {
  static instance: LibreDwgEx;
  private wasmInstance!: MainModule;

  private constructor(wasmInstance: MainModule) {
    this.wasmInstance = wasmInstance;
    return new Proxy(this, {
      get: (target, prop, receiver) => {
        if (prop in target) {
          return Reflect.get(target, prop, receiver);
        }
        // Delegate to the wasmInstance for WebAssembly methods
        return Reflect.get(target.wasmInstance, prop, receiver);
      },
    });
  }

  dwg_read_data(fileContent: string | ArrayBuffer, fileType: number) {
    if (fileType == Dwg_File_Type.DWG) {
      const fileName = "tmp.dwg";
      this.wasmInstance.FS.writeFile(fileName, new Uint8Array(fileContent as ArrayBuffer));
      const result = this.wasmInstance.dwg_read_file(fileName);
      if (result.error != 0) {
        console.log('Failed to open dwg file with error code: ', result.error);
      }
      this.wasmInstance.FS.unlink(fileName);
      return result.data as Dwg_Data_Ptr;
    } 
    // else if (fileType == Dwg_File_Type.DXF) {
    //   const fileName = "tmp.dxf";
    //   this.wasmInstance.FS.writeFile(fileName, new Uint8Array(fileContent as ArrayBuffer));
    //   const result = this.wasmInstance.dxf_read_file(fileName);
    //   if (result.error != 0) {
    //     console.log('Failed to open dxf file with error code: ', result.error);
    //   }
    //   this.wasmInstance.FS.unlink(fileName);
    //   return result.data as Dwg_Data_Ptr;
    // }
  }

  /**
   * Converts Dwg_Data instance to DwgDatabase instance.
   * @param data input pointer to Dwg_Data instance.
   * @returns Returns the converted DwgDatabase instance.
   */
  convert(data: Dwg_Data_Ptr) {
    const converter = new LibreDwgConverter(this as unknown as LibreDwgEx)
    return converter.convert(data)
  }

  /**
   * Returns all of entities in the model space. Each item in returned array
   * is one Dwg_Object pointer (Dwg_Object*).
   */
  dwg_getall_entitie_in_model_space(data: Dwg_Data_Ptr) {
    const wasmInstance = this.wasmInstance;
    const model_space = wasmInstance.dwg_model_space_object(data);
    const entities = [];
    let next = wasmInstance.get_first_owned_entity(model_space);
    while (next) {
      entities.push(next);
      next = wasmInstance.get_next_owned_entity(model_space, next);
    }
    return entities;
  }

  dwg_getall_object_by_type(data: Dwg_Data_Ptr, type: number): number[] {
    const wasmInstance = this.wasmInstance;
    const num_objects = wasmInstance.dwg_get_num_objects(data);
    const results = [];
    for (let i = 0; i < num_objects; i++) {                                                  
      const obj = wasmInstance.dwg_get_object(data, i);
      const tio = wasmInstance.dwg_object_to_object_tio(obj);
      if (tio && wasmInstance.dwg_object_get_fixedtype(obj) == type) {
        results.push(tio);
      }
    }
    return results; 
  }

  dwg_getall_entity_by_type(data: Dwg_Data_Ptr, type: number): number[] {
    const wasmInstance = this.wasmInstance;
    const num_objects = wasmInstance.dwg_get_num_objects(data);
    const results = [];
    for (let i = 0; i < num_objects; i++) {                                                 
      const obj = wasmInstance.dwg_get_object(data, i);
      const tio = wasmInstance.dwg_object_to_entity_tio(obj);
      if (tio && wasmInstance.dwg_object_get_fixedtype(obj) == type) {
        results.push(tio);
      }
    }
    return results; 
  }

  dwg_getall_LAYER(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LAYER);
  }

  dwg_getall_LTYPE(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LTYPE);
  }
  
  dwg_getall_STYLE(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_STYLE);
  };
  
  dwg_getall_DIMSTYLE(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_DIMSTYLE);
  };
  
  dwg_getall_VPORT(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_VPORT);
  };
  
  dwg_getall_LAYOUT(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LAYOUT);
  };
  
  dwg_getall_BLOCK(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_BLOCK);
  };
  
  dwg_getall_BLOCK_HEADER(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_BLOCK_HEADER);
  };

  dwg_getall_IMAGEDEF(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_IMAGEDEF);
  };

  dwg_getall_VERTEX_2D(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_VERTEX_2D);
  };

  dwg_getall_VERTEX_3D(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_VERTEX_3D);
  };

  dwg_getall_POLYLINE_2D(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_POLYLINE_2D);
  };

  dwg_getall_POLYLINE_3D(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_POLYLINE_3D);
  };

  dwg_getall_IMAGE(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_IMAGE);
  };

  dwg_getall_LWPOLYLINE(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_LWPOLYLINE);
  };

  dwg_ptr_to_unsigned_char_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_unsigned_char_array(ptr, size)
  }
  
  dwg_ptr_to_signed_char_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_signed_char_array(ptr, size)
  }

  dwg_ptr_to_uint16_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_uint16_t_array(ptr, size)
  }

  dwg_ptr_to_int16_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_int16_t_array(ptr, size)
  }

  dwg_ptr_to_uint32_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_uint32_t_array(ptr, size)
  }

  dwg_ptr_to_int32_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_int32_t_array(ptr, size)
  }

  dwg_ptr_to_uint64_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_uint64_t_array(ptr, size)
  }

  dwg_ptr_to_int64_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_int64_t_array(ptr, size)
  }

  dwg_ptr_to_double_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_double_array(ptr, size)
  }

  dwg_ptr_to_point2d_array(ptr: Dwg_Array_Ptr, size: number): DwgPoint2D[] {
    return this.wasmInstance.dwg_ptr_to_point2d_array(ptr, size)
  }

  dwg_ptr_to_point3d_array(ptr: Dwg_Array_Ptr, size: number): DwgPoint3D[] {
    return this.wasmInstance.dwg_ptr_to_point3d_array(ptr, size)
  }

  dwg_ptr_to_ltype_dash_array(ptr: Dwg_Array_Ptr, size: number): Dwg_LTYPE_Dash[] {
    return this.wasmInstance.dwg_ptr_to_ltype_dash_array(ptr, size)
  }

  dwg_dynapi_entity_value(obj: number, field: string): Dwg_Field_Value {
    return this.wasmInstance.dwg_dynapi_entity_value(obj, field);
  }

  dwg_object_get_handle_object(ptr: Dwg_Object_Ptr): Dwg_Handle {
    return this.wasmInstance.dwg_object_get_handle_object(ptr);  
  }

  dwg_object_object_get_handle_object(ptr: Dwg_Object_Ptr): Dwg_Handle {
    return this.wasmInstance.dwg_object_object_get_handle_object(ptr);  
  }

  dwg_object_object_get_ownerhandle_object(ptr: Dwg_Object_Ptr): Dwg_Object_Ref {
    return this.wasmInstance.dwg_object_object_get_ownerhandle_object(ptr);  
  }

  static createByWasmInstance(wasmInstance: MainModule): LibreDwgEx {
    return this.instance == null ? new LibreDwg(wasmInstance) as LibreDwgEx : this.instance;
  }

  static async create(): Promise<LibreDwgEx> {
    const wasmInstance = await createModule();
    return this.createByWasmInstance(wasmInstance);
  }
}
