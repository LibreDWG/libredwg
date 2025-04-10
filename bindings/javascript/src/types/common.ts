import { DwgPoint2D, DwgPoint3D } from '../database'

export type Dwg_Array_Ptr = number
export type Dwg_Data_Ptr = number
export type Dwg_Object_Ptr = number
export type Dwg_Object_Ref_Ptr = number
export type Dwg_Object_Object_Ptr = number
export type Dwg_Object_Entity_Ptr = number

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

export interface Dwg_TableCellContent_Attr {
  attdef: number
  value: string
  index: number
}

export interface Dwg_ContentFormat {
  property_override_flags: number
  property_flags: number
  value_data_type: number
  value_unit_type: number
  value_format_string: string
  rotation: number
  block_scale: number
  cell_alignment: number
  content_color: Dwg_Color
  text_style: number
  text_height: number
}

export interface Dwg_TableCellContent {
  type: number
  attrs: Dwg_TableCellContent_Attr[]
  has_content_format_overrides: boolean
  content_format: Dwg_ContentFormat
}

export interface Dwg_LinkedData {
  name: string // max 16, dxf 1
  description: string // max 24, dxf 300
}

export interface Dwg_TABLE_AttrDef {
  attdef: Dwg_Object_Ref
  index: number
  text: string
}

export interface Dwg_TABLE_Cell {
  type: number
  flags: number
  is_merged_value: number
  is_autofit_flag: number
  merged_width_flag: number
  merged_height_flag: number
  rotation: number
  text_value: string
  text_style: number
  block_handle: Dwg_Object_Ref
  block_scale: number
  additional_data_flag: number
  cell_flag_override: number
  virtual_edge_flag: number
  cell_alignment: number
  bg_fill_none: number
  bg_color: Dwg_Color
  content_color: Dwg_Color
  text_height: number
  top_grid_color: Dwg_Color
  top_grid_linewt: number
  top_visibility: number
  right_grid_color: Dwg_Color
  right_grid_linewt: number
  right_visibility: number
  bottom_grid_color: Dwg_Color
  bottom_grid_linewt: number
  bottom_visibility: number
  left_grid_color: Dwg_Color
  left_grid_linewt: number
  left_visibility: number
  attr_defs: Dwg_TABLE_AttrDef[]
}

export interface Dwg_TableCell {
  flag: number
  tooltip: string
  has_linked_data: boolean
  data_link: number
  num_rows: number
  num_cols: number
  cell_contents: Dwg_TableCellContent[]
}

export interface Dwg_Field_Value {
  success: boolean
  message?: string
  data?: string | number | Dwg_Color | Dwg_Array_Ptr | DwgPoint2D | DwgPoint3D
}
