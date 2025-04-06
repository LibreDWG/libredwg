import { DwgPoint3D } from 'types'

import { Dwg_Object_Entity_Ptr, LibreDwgEx } from '../libredwg'
import { DwgArcEntity } from '../types/entities'

export class AcDbEntityConverter {
  libredwg: LibreDwgEx

  constructor(instance: LibreDwgEx) {
    this.libredwg = instance
  }

  convertArc(entity: Dwg_Object_Entity_Ptr): DwgArcEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center').data as DwgPoint3D
    const radius = libredwg.dwg_dynapi_entity_value(entity, 'radius').data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness').data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(entity, 'extrusion').data as DwgPoint3D
    const startAngle = libredwg.dwg_dynapi_entity_value(entity, 'start_angle').data as number
    const endAngle = libredwg.dwg_dynapi_entity_value(entity, 'end_angle').data as number

    return {
      ...commonAttrs,
      thickness: thickness,
      center: center,
      radius: radius,
      startAngle: startAngle,
      endAngle: endAngle,
      extrusionDirection: extrusionDirection
    }
  }

  private getCommonAttrs(entity: Dwg_Object_Entity_Ptr) {
    const libredwg = this.libredwg
    const layer = libredwg.dwg_object_entity_get_layer_name(entity)
    const handle = libredwg.dwg_object_entity_get_handle_object(entity)
    const ownerhandle = libredwg.dwg_object_entity_get_ownerhandle_object(entity)
    const lineType = libredwg.dwg_object_entity_get_ltype_name(entity)
    const lineweight = libredwg.dwg_object_entity_get_line_weight(entity)
    const lineTypeScale = libredwg.dwg_object_entity_get_ltype_scale(entity)
    const isVisible = !libredwg.dwg_object_entity_get_invisible(entity)

    return {
      layer: layer,
      handle: handle.value,
      ownerHandle: ownerhandle.absolute_ref,
      lineType: lineType,
      lineweight: lineweight,
      lineTypeScale: lineTypeScale,
      isVisible: isVisible,
      transparency: 0, // TODO: Set the correct value
    }
  }
}