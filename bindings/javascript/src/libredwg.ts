import { MainModule } from '../wasm/libredwg-web'
import { createModule, Dwg_File_Type, Dwg_Object_Type } from './utils';

export type Dwg_Data_Ptr = number;
export type Dwg_Object_Ptr = number;

export class LibreDwg {
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
      return result.data;
    } else if (fileType == Dwg_File_Type.DXF) {
      const fileName = "tmp.dxf";
      this.wasmInstance.FS.writeFile(fileName, new Uint8Array(fileContent as ArrayBuffer));
      const result = this.wasmInstance.dxf_read_file(fileName);
      if (result.error != 0) {
        console.log('Failed to open dxf file with error code: ', result.error);
      }
      this.wasmInstance.FS.unlink(fileName);
      return result.data;
    }
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

  dwg_getall_object_by_type(data: Dwg_Data_Ptr, type: number) {
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


  dwg_getall_entity_by_type(data: Dwg_Data_Ptr, type: number) {
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

  dwg_getall_LAYER(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LAYER);
  }

  dwg_getall_LTYPE(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LTYPE);
  }
  
  dwg_getall_STYLE(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_STYLE);
  };
  
  dwg_getall_DIMSTYLE(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_DIMSTYLE);
  };
  
  dwg_getall_VPORT(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_VPORT);
  };
  
  dwg_getall_LAYOUT(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LAYOUT);
  };
  
  dwg_getall_BLOCK(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_BLOCK);
  };
  
  dwg_getall_BLOCK_HEADER(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_BLOCK_HEADER);
  };

  dwg_getall_IMAGEDEF(data: Dwg_Data_Ptr) {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_IMAGEDEF);
  };

  dwg_getall_VERTEX_2D(data: Dwg_Data_Ptr) {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_VERTEX_2D);
  };

  dwg_getall_VERTEX_3D(data: Dwg_Data_Ptr) {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_VERTEX_3D);
  };

  dwg_getall_POLYLINE_2D(data: Dwg_Data_Ptr) {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_POLYLINE_2D);
  };

  dwg_getall_POLYLINE_3D(data: Dwg_Data_Ptr) {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_POLYLINE_3D);
  };

  dwg_getall_IMAGE(data: Dwg_Data_Ptr) {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_IMAGE);
  };

  dwg_getall_LWPOLYLINE(data: Dwg_Data_Ptr) {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_LWPOLYLINE);
  };

  static async create(): Promise<LibreDwg & MainModule> {
    const wasmInstance = await createModule();
    return new LibreDwg(wasmInstance) as LibreDwg & MainModule;
  }
}
