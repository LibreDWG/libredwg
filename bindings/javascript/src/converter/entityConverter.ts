import { Dwg_Object_Type } from '../enums'
import { Dwg_Object_Entity_Ptr, Dwg_Object_Ptr, LibreDwgEx } from '../libredwg'
import { DwgArcEntity, DwgCircleEntity, DwgEntity, DwgPoint3D } from '../types'


export class LibreEntityConverter {
  libredwg: LibreDwgEx

  constructor(instance: LibreDwgEx) {
    this.libredwg = instance
  }

  convert(object_ptr: Dwg_Object_Ptr): DwgEntity | undefined {
    const libredwg = this.libredwg
    const tio = libredwg.dwg_object_to_entity_tio(object_ptr)
    if (tio) {
      const fixedtype = libredwg.dwg_object_get_fixedtype(object_ptr)
      if (fixedtype == Dwg_Object_Type.DWG_TYPE_ARC) {
        return this.convertArc(tio)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_CIRCLE) {
        return this.convertCircle(tio)
      }
    }
    return undefined
  }

  private convertArc(entity: Dwg_Object_Entity_Ptr): DwgArcEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center').data as DwgPoint3D
    const radius = libredwg.dwg_dynapi_entity_value(entity, 'radius').data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness').data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(entity, 'extrusion').data as DwgPoint3D
    const startAngle = libredwg.dwg_dynapi_entity_value(entity, 'start_angle').data as number
    const endAngle = libredwg.dwg_dynapi_entity_value(entity, 'end_angle').data as number

    return {
      type: 'ARC',
      ...commonAttrs,
      thickness: thickness,
      center: center,
      radius: radius,
      startAngle: startAngle,
      endAngle: endAngle,
      extrusionDirection: extrusionDirection
    }
  }

  private convertCircle(entity: Dwg_Object_Entity_Ptr): DwgCircleEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center').data as DwgPoint3D
    const radius = libredwg.dwg_dynapi_entity_value(entity, 'radius').data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness').data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(entity, 'extrusion').data as DwgPoint3D

    return {
      type: 'CIRCLE',
      ...commonAttrs,
      thickness: thickness,
      center: center,
      radius: radius,
      extrusionDirection: extrusionDirection
    }
  }

  private getCommonAttrs(entity: Dwg_Object_Entity_Ptr) {
    const libredwg = this.libredwg
    const color = libredwg.dwg_object_entity_get_color_object(entity)
    const layer = libredwg.dwg_object_entity_get_layer_name(entity)
    const handle = libredwg.dwg_object_entity_get_handle_object(entity)
    const ownerhandle = libredwg.dwg_object_entity_get_ownerhandle_object(entity)
    const lineType = libredwg.dwg_object_entity_get_ltype_name(entity)
    const lineweight = libredwg.dwg_object_entity_get_line_weight(entity)
    const lineTypeScale = libredwg.dwg_object_entity_get_ltype_scale(entity)
    const isVisible = !libredwg.dwg_object_entity_get_invisible(entity)

    return {
      handle: handle.value,
      ownerHandle: ownerhandle.absolute_ref,
      layer: layer,
      color: color.rgb,
      colorIndex: color.index,
      colorName: color.name,
      lineType: lineType,
      lineweight: lineweight,
      lineTypeScale: lineTypeScale,
      isVisible: isVisible,
      transparency: 0, // TODO: Set the correct value
    }
  }
}