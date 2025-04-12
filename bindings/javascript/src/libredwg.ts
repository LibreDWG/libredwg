import { MainModule } from '../wasm/libredwg-web'
import createModule from '../wasm/libredwg-web.js'
import { LibreDwgConverter } from './converter'
import { DwgPoint2D, DwgPoint3D, DwgPoint4D } from './database'
import {
  Dwg_Array_Ptr,
  Dwg_Color,
  Dwg_Data_Ptr,
  Dwg_Entity_IMAGE_Ptr,
  Dwg_Entity_LWPOLYLINE_Ptr,
  Dwg_Entity_POLYLINE_2D_Ptr,
  Dwg_Entity_POLYLINE_3D_Ptr,
  Dwg_Field_Value,
  Dwg_File_Type,
  Dwg_Handle,
  Dwg_HATCH_DefLine,
  Dwg_HATCH_Path,
  Dwg_LTYPE_Dash,
  Dwg_Object_BLOCK_HEADER_Ptr,
  Dwg_Object_BLOCK_Ptr,
  Dwg_Object_DIMSTYLE_Ptr,
  Dwg_Object_Entity_Ptr,
  Dwg_Object_Entity_TIO_Ptr,
  Dwg_Object_IMAGEDEF_Ptr,
  Dwg_Object_LAYER_Ptr,
  Dwg_Object_LTYPE_Ptr,
  Dwg_Object_Object_Ptr,
  Dwg_Object_Object_TIO_Ptr,
  Dwg_Object_Ptr,
  Dwg_Object_Ref,
  Dwg_Object_STYLE_Ptr,
  Dwg_Object_Type,
  Dwg_Object_VERTEX_2D_Ptr,
  Dwg_Object_VERTEX_3D_Ptr,
  Dwg_Object_VPORT_Ptr,
  Dwg_TABLE_Cell
} from './types'

export { createModule }

export type LibreDwgEx = LibreDwg & MainModule

export class LibreDwg {
  static instance: LibreDwgEx
  private wasmInstance!: MainModule

  private constructor(wasmInstance: MainModule) {
    this.wasmInstance = wasmInstance
    return new Proxy(this, {
      get: (target, prop, receiver) => {
        if (prop in target) {
          return Reflect.get(target, prop, receiver)
        }
        // Delegate to the wasmInstance for WebAssembly methods
        return Reflect.get(target.wasmInstance, prop, receiver)
      }
    })
  }

  dwg_read_data(fileContent: string | ArrayBuffer, fileType: number) {
    if (fileType == Dwg_File_Type.DWG) {
      const fileName = 'tmp.dwg'
      this.wasmInstance.FS.writeFile(
        fileName,
        new Uint8Array(fileContent as ArrayBuffer)
      )
      const result = this.wasmInstance.dwg_read_file(fileName)
      if (result.error != 0) {
        console.log('Failed to open dwg file with error code: ', result.error)
      }
      this.wasmInstance.FS.unlink(fileName)
      return result.data as Dwg_Data_Ptr
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
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns the converted DwgDatabase instance.
   */
  convert(data: Dwg_Data_Ptr) {
    const converter = new LibreDwgConverter(this as unknown as LibreDwgEx)
    return converter.convert(data)
  }

  /**
   * Returns all of entities in the model space. Each item in returned array
   * is one Dwg_Object pointer (Dwg_Object*).
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of entities in the model space.
   */
  dwg_getall_entitie_in_model_space(data: Dwg_Data_Ptr) {
    const wasmInstance = this.wasmInstance
    const model_space = wasmInstance.dwg_model_space_object(data)
    const entities = []
    let next = wasmInstance.get_first_owned_entity(model_space)
    while (next) {
      entities.push(next)
      next = wasmInstance.get_next_owned_entity(model_space, next)
    }
    return entities
  }

  /**
   * Returns all of objects in Dwg_Data instance with the specified type.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @param type Object type.
   * @returns Returns all of objects with the specified type.
   */
  dwg_getall_object_by_type(
    data: Dwg_Data_Ptr,
    type: Dwg_Object_Type
  ): Dwg_Object_Object_TIO_Ptr[] {
    const wasmInstance = this.wasmInstance
    const num_objects = wasmInstance.dwg_get_num_objects(data)
    const results = []
    for (let i = 0; i < num_objects; i++) {
      const obj = wasmInstance.dwg_get_object(data, i)
      const tio = wasmInstance.dwg_object_to_object_tio(obj)
      if (tio && wasmInstance.dwg_object_get_fixedtype(obj) == type) {
        results.push(tio)
      }
    }
    return results
  }

  /**
   * Returns all of objects in Dwg_Data instance with the specified type.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @param type Object type.
   * @returns Returns all of objects with the specified type.
   */
  dwg_getall_entity_by_type(
    data: Dwg_Data_Ptr,
    type: Dwg_Object_Type
  ): Dwg_Object_Entity_TIO_Ptr[] {
    const wasmInstance = this.wasmInstance
    const num_objects = wasmInstance.dwg_get_num_objects(data)
    const results = []
    for (let i = 0; i < num_objects; i++) {
      const obj = wasmInstance.dwg_get_object(data, i)
      const tio = wasmInstance.dwg_object_to_entity_tio(obj)
      if (tio && wasmInstance.dwg_object_get_fixedtype(obj) == type) {
        results.push(tio)
      }
    }
    return results
  }

  /**
   * Returns all of layer objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of layer objects in Dwg_Data instance.
   */
  dwg_getall_LAYER(data: Dwg_Data_Ptr): Dwg_Object_LAYER_Ptr[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LAYER)
  }

  /**
   * Returns all of line type objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of line type objects in Dwg_Data instance.
   */
  dwg_getall_LTYPE(data: Dwg_Data_Ptr): Dwg_Object_LTYPE_Ptr[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LTYPE)
  }

  /**
   * Returns all of text style objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of text style objects in Dwg_Data instance.
   */
  dwg_getall_STYLE(data: Dwg_Data_Ptr): Dwg_Object_STYLE_Ptr[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_STYLE)
  }

  /**
   * Returns all of dimension style objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of dimension style objects in Dwg_Data instance.
   */
  dwg_getall_DIMSTYLE(data: Dwg_Data_Ptr): Dwg_Object_DIMSTYLE_Ptr[] {
    return this.dwg_getall_object_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_DIMSTYLE
    )
  }

  /**
   * Returns all of viewport objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of viewport objects in Dwg_Data instance.
   */
  dwg_getall_VPORT(data: Dwg_Data_Ptr): Dwg_Object_VPORT_Ptr[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_VPORT)
  }

  /**
   * Returns all of layout objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of layout objects in Dwg_Data instance.
   */
  dwg_getall_LAYOUT(data: Dwg_Data_Ptr): number[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_LAYOUT)
  }

  /**
   * Returns all of block objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of block objects in Dwg_Data instance.
   */
  dwg_getall_BLOCK(data: Dwg_Data_Ptr): Dwg_Object_BLOCK_Ptr[] {
    return this.dwg_getall_object_by_type(data, Dwg_Object_Type.DWG_TYPE_BLOCK)
  }

  /**
   * Returns all of block header objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of block header objects in Dwg_Data instance.
   */
  dwg_getall_BLOCK_HEADER(data: Dwg_Data_Ptr): Dwg_Object_BLOCK_HEADER_Ptr[] {
    return this.dwg_getall_object_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_BLOCK_HEADER
    )
  }

  /**
   * Returns all of image definition objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of image definition objects in Dwg_Data instance.
   */
  dwg_getall_IMAGEDEF(data: Dwg_Data_Ptr): Dwg_Object_IMAGEDEF_Ptr[] {
    return this.dwg_getall_object_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_IMAGEDEF
    )
  }

  /**
   * Returns all of 2d vertex objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of 2d vertex objects in Dwg_Data instance.
   */
  dwg_getall_VERTEX_2D(data: Dwg_Data_Ptr): Dwg_Object_VERTEX_2D_Ptr[] {
    return this.dwg_getall_entity_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_VERTEX_2D
    )
  }

  /**
   * Returns all of 3d vertex objects in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of 3d vertex objects in Dwg_Data instance.
   */
  dwg_getall_VERTEX_3D(data: Dwg_Data_Ptr): Dwg_Object_VERTEX_3D_Ptr[] {
    return this.dwg_getall_entity_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_VERTEX_3D
    )
  }

  /**
   * Returns all of 2d polyline entities in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of 2d polyline entities in Dwg_Data instance.
   */
  dwg_getall_POLYLINE_2D(data: Dwg_Data_Ptr): Dwg_Entity_POLYLINE_2D_Ptr[] {
    return this.dwg_getall_entity_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_POLYLINE_2D
    )
  }

  /**
   * Returns all of 3d polyline entities in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of 3d polyline entities in Dwg_Data instance.
   */
  dwg_getall_POLYLINE_3D(data: Dwg_Data_Ptr): Dwg_Entity_POLYLINE_3D_Ptr[] {
    return this.dwg_getall_entity_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_POLYLINE_3D
    )
  }

  /**
   * Returns all of image entities in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of image entities in Dwg_Data instance.
   */
  dwg_getall_IMAGE(data: Dwg_Data_Ptr): Dwg_Entity_IMAGE_Ptr[] {
    return this.dwg_getall_entity_by_type(data, Dwg_Object_Type.DWG_TYPE_IMAGE)
  }

  /**
   * Returns all of lwpolyline entities in Dwg_Data instance.
   * @group GetAll Methods
   * @param data Pointer to Dwg_Data instance.
   * @returns Returns all of lwpolyline entities in Dwg_Data instance.
   */
  dwg_getall_LWPOLYLINE(data: Dwg_Data_Ptr): Dwg_Entity_LWPOLYLINE_Ptr[] {
    return this.dwg_getall_entity_by_type(
      data,
      Dwg_Object_Type.DWG_TYPE_LWPOLYLINE
    )
  }

  /**
   * Returns the first entity owned by the block header or null
   * @param ptr Pointer to the block header.
   * @returns Returns the first entity owned by the block header or null
   */
  get_first_owned_entity(ptr: Dwg_Object_Ptr): Dwg_Object_Ptr {
    return this.wasmInstance.get_first_owned_entity(ptr)
  }

  /**
   * Returns the next entity owned by the block header or null.
   * @param ptr Pointer to the block header.
   * @param current Pointer to the current entity in the block header.
   * @returns Returns the next entity owned by the block header or null.
   */
  get_next_owned_entity(
    ptr: Dwg_Object_Ptr,
    current: Dwg_Object_Ptr
  ): Dwg_Object_Ptr {
    return this.wasmInstance.get_next_owned_entity(ptr, current)
  }

  /**
   * Converts one C++ unsigned char array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ unsigned char array.
   * @param size The size of C++ unsigned char array.
   * @returns Returns one JavaScript number array from the specified C++ unsigned char array.
   */
  dwg_ptr_to_unsigned_char_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_unsigned_char_array(ptr, size)
  }

  /**
   * Converts one C++ signed char array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ signed char array.
   * @param size The size of C++ signed char array.
   * @returns Returns one JavaScript number array from the specified C++ signed char array.
   */
  dwg_ptr_to_signed_char_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_signed_char_array(ptr, size)
  }

  /**
   * Converts one C++ unsigned int16 array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ unsigned int16 array.
   * @param size The size of C++ unsigned int16 array.
   * @returns Returns one JavaScript number array from the specified C++ unsigned int16 array.
   */
  dwg_ptr_to_uint16_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_uint16_t_array(ptr, size)
  }

  /**
   * Converts one C++ int16 array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ int16 array.
   * @param size The size of C++ int16 array.
   * @returns Returns one JavaScript number array from the specified C++ int16 array.
   */
  dwg_ptr_to_int16_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_int16_t_array(ptr, size)
  }

  /**
   * Converts one C++ unsigned int32 array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ unsigned int32 array.
   * @param size The size of C++ unsigned int32 array.
   * @returns Returns one JavaScript number array from the specified C++ unsigned int32 array.
   */
  dwg_ptr_to_uint32_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_uint32_t_array(ptr, size)
  }

  /**
   * Converts one C++ int32 array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ int32 array.
   * @param size The size of C++ int32 array.
   * @returns Returns one JavaScript number array from the specified C++ int32 array.
   */
  dwg_ptr_to_int32_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_int32_t_array(ptr, size)
  }

  /**
   * Converts one C++ unsigned int64 array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ unsigned int64 array.
   * @param size The size of C++ unsigned int64 array.
   * @returns Returns one JavaScript number array from the specified C++ unsigned int64 array.
   */
  dwg_ptr_to_uint64_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_uint64_t_array(ptr, size)
  }

  /**
   * Converts one C++ int64 array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ int64 array.
   * @param size The size of C++ int64 array.
   * @returns Returns one JavaScript number array from the specified C++ int64 array.
   */
  dwg_ptr_to_int64_t_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_int64_t_array(ptr, size)
  }

  /**
   * Converts one C++ double array to one JavaScript number array.
   * @group Array Methods
   * @param ptr Pointer to C++ double array.
   * @param size The size of C++ double array.
   * @returns Returns one JavaScript number array from the specified C++ double array.
   */
  dwg_ptr_to_double_array(ptr: Dwg_Array_Ptr, size: number): number[] {
    return this.wasmInstance.dwg_ptr_to_double_array(ptr, size)
  }

  /**
   * Converts one C++ 2d point array to one JavaScript 2d point array.
   * @group Array Methods
   * @param ptr Pointer to C++ 2d point array.
   * @param size The size of C++ 2 point array.
   * @returns Returns one JavaScript 2d point array from the specified C++ 2d point array.
   */
  dwg_ptr_to_point2d_array(ptr: Dwg_Array_Ptr, size: number): DwgPoint2D[] {
    return this.wasmInstance.dwg_ptr_to_point2d_array(ptr, size)
  }

  /**
   * Converts one C++ 3d point array to one JavaScript 3d point array.
   * @group Array Methods
   * @param ptr Pointer to C++ 3d point array.
   * @param size The size of C++ 3d point array.
   * @returns Returns one JavaScript 3d point array from the specified C++ 3d point array.
   */
  dwg_ptr_to_point3d_array(ptr: Dwg_Array_Ptr, size: number): DwgPoint3D[] {
    return this.wasmInstance.dwg_ptr_to_point3d_array(ptr, size)
  }

  /**
   * Converts one C++ 4d point array to one JavaScript 4d point array.
   * @group Array Methods
   * @param ptr Pointer to C++ 4d point array.
   * @param size The size of C++ 4d point array.
   * @returns Returns one JavaScript 4d point array from the specified C++ 4d point array.
   */
  dwg_ptr_to_point4d_array(ptr: Dwg_Array_Ptr, size: number): DwgPoint4D[] {
    return this.wasmInstance.dwg_ptr_to_point4d_array(ptr, size)
  }

  /**
   * Converts one C++ line type array to one JavaScript line type array.
   * @group Array Methods
   * @param ptr Pointer to C++ line type array.
   * @param size The size of C++ line type array.
   * @returns Returns one JavaScript line type array from the specified C++ line type array.
   */
  dwg_ptr_to_ltype_dash_array(
    ptr: Dwg_Array_Ptr,
    size: number
  ): Dwg_LTYPE_Dash[] {
    return this.wasmInstance.dwg_ptr_to_ltype_dash_array(ptr, size)
  }

  /**
   * Converts one C++ table cell array to one JavaScript table cell array.
   * @group Array Methods
   * @param ptr Pointer to C++ table cell array.
   * @param size The size of C++ table cell array.
   * @returns Returns one JavaScript table cell array from the specified C++ table cell array.
   */
  dwg_ptr_to_table_cell_array(
    ptr: Dwg_Array_Ptr,
    size: number
  ): Dwg_TABLE_Cell[] {
    return this.wasmInstance.dwg_ptr_to_table_cell_array(ptr, size)
  }

  /**
   * Converts one C++ hatch definition line array to one JavaScript hatch definition line array.
   * @group Array Methods
   * @param ptr Pointer to C++ hatch definition line array.
   * @param size The size of C++ hatch definition line array.
   * @returns Returns one JavaScript hatch definition line array from the specified C++ hatch definition line array.
   */
  dwg_ptr_to_hatch_defline_array(
    ptr: Dwg_Array_Ptr,
    size: number
  ): Dwg_HATCH_DefLine[] {
    return this.wasmInstance.dwg_ptr_to_hatch_defline_array(ptr, size)
  }

  /**
   * Converts one C++ hatch path array to one JavaScript hatch path array.
   * @group Array Methods
   * @param ptr Pointer to C++ hatch path array.
   * @param size The size of C++ hatch path array.
   * @returns Returns one JavaScript hatch path array from the specified C++ hatch path array.
   */
  dwg_ptr_to_hatch_path_array(
    ptr: Dwg_Array_Ptr,
    size: number
  ): Dwg_HATCH_Path[] {
    return this.wasmInstance.dwg_ptr_to_hatch_path_array(ptr, size)
  }

  /**
   * Generic field value getter. Used to get the field value of one object or entity.
   * @group Dynamic API Methods
   * @param obj Pointer to one object or entity
   * @param field Field name of one object or entity
   * @returns Returns the field value of one object or entity.
   */
  dwg_dynapi_entity_value(
    obj: Dwg_Object_Object_TIO_Ptr | Dwg_Object_Entity_TIO_Ptr,
    field: string
  ): Dwg_Field_Value {
    return this.wasmInstance.dwg_dynapi_entity_value(obj, field)
  }

  /**
   * Header field value getter. Used to get the field value of dwg/dxf header.
   * @group Dynamic API Methods
   * @param data Pointer to Dwg_Data instance.
   * @param field Field name of header.
   * @returns Returns the field value of dwg/dxf header.
   */
  dwg_dynapi_header_value(data: Dwg_Data_Ptr, field: string): Dwg_Field_Value {
    return this.wasmInstance.dwg_dynapi_header_value(data, field)
  }

  /**
   * The common field value getter. Used to get the value of object or entity common fields.
   * @group Dynamic API Methods
   * @param obj Pointer to one object or entity
   * @param field The name of object or entity common fields.
   * @returns Returns the value of object or entity common fields.
   */
  dwg_dynapi_common_value(
    obj: Dwg_Object_Object_TIO_Ptr | Dwg_Object_Entity_TIO_Ptr,
    field: string
  ): Dwg_Field_Value {
    return this.wasmInstance.dwg_dynapi_common_value(obj, field)
  }

  /**
   * The field of one object or entity may not be primitive type. It means one field may consist of
   * multiple sub-fields. This method is used to get the sub-field value of those complex field.
   * @group Dynamic API Methods
   * @param obj Pointer to one object or entity.
   * @param subclass The class name of the field with complex type.
   * @param field The field name of one object or entit.
   * @returns Returns the sub-field value of one complex field.
   */
  dwg_dynapi_subclass_value(
    obj: Dwg_Object_Object_TIO_Ptr | Dwg_Object_Entity_TIO_Ptr,
    subclass: string,
    field: string
  ): Dwg_Field_Value {
    return this.wasmInstance.dwg_dynapi_subclass_value(obj, subclass, field)
  }

  /**
   * Returns the handle of one Dwg_Object instance.
   * @group Dwg_Object Methods
   * @param ptr Pointer to one Dwg_Object instance.
   * @returns Returns the handle of one Dwg_Object instance.
   */
  dwg_object_get_handle_object(ptr: Dwg_Object_Ptr): Dwg_Handle {
    return this.wasmInstance.dwg_object_get_handle_object(ptr)
  }

  /**
   * Returns the handle of one Dwg_Object_Object instance.
   * @group Dwg_Object_Object Methods
   * @param ptr Pointer to one Dwg_Object_Object instance.
   * @returns Returns the handle of one Dwg_Object_Object instance.
   */
  dwg_object_object_get_handle_object(ptr: Dwg_Object_Object_Ptr): Dwg_Handle {
    return this.wasmInstance.dwg_object_object_get_handle_object(ptr)
  }

  /**
   * Returns the owner handle of one Dwg_Object_Object instance.
   * @group Dwg_Object_Object Methods
   * @param ptr Pointer to one Dwg_Object_Object instance.
   * @returns Returns the owner handle of one Dwg_Object_Object instance.
   */
  dwg_object_object_get_ownerhandle_object(
    ptr: Dwg_Object_Object_Ptr
  ): Dwg_Object_Ref {
    return this.wasmInstance.dwg_object_object_get_ownerhandle_object(ptr)
  }

  /**
   * Returns the handle of one Dwg_Object_Entity instance.
   * @group Dwg_Object_Entity Methods
   * @param ptr Pointer to one Dwg_Object_Entity instance.
   * @returns Returns the handle of one Dwg_Object_Entity instance.
   */
  dwg_object_entity_get_handle_object(ptr: Dwg_Object_Entity_Ptr): Dwg_Handle {
    return this.wasmInstance.dwg_object_entity_get_handle_object(ptr)
  }

  /**
   * Returns the owner handle of one Dwg_Object_Entity instance.
   * @group Dwg_Object_Entity Methods
   * @param ptr Pointer to one Dwg_Object_Entity instance.
   * @returns Returns the owner handle of one Dwg_Object_Entity instance.
   */
  dwg_object_entity_get_ownerhandle_object(
    ptr: Dwg_Object_Entity_Ptr
  ): Dwg_Object_Ref {
    return this.wasmInstance.dwg_object_entity_get_ownerhandle_object(ptr)
  }

  /**
   * Returns color value of one Dwg_Object_Entity instance.
   * @group Dwg_Object_Entity Methods
   * @param ptr Pointer to one Dwg_Object_Entity instance.
   * @returns Returns color value of one Dwg_Object_Entity instance.
   */
  dwg_object_entity_get_color_object(ptr: Dwg_Object_Entity_Ptr): Dwg_Color {
    return this.wasmInstance.dwg_object_entity_get_color_object(ptr)
  }

  static createByWasmInstance(wasmInstance: MainModule): LibreDwgEx {
    return this.instance == null
      ? (new LibreDwg(wasmInstance) as LibreDwgEx)
      : this.instance
  }

  static async create(): Promise<LibreDwgEx> {
    const wasmInstance = await createModule()
    return this.createByWasmInstance(wasmInstance)
  }
}
