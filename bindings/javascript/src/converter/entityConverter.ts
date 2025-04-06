import { DwgPoint3D } from "types"
import { Dwg_Object_Entity_Ptr, LibreDwgEx } from "../libredwg"
import { DwgArcEntity, DwgEntity } from "../types/entities"

export class AcDbEntityConverter {
  libredwg: LibreDwgEx

  constructor(instance: LibreDwgEx) {
    this.libredwg = instance
  }

  convert(entity_ptr: Dwg_Object_Entity_Ptr): DwgEntity | null {
    const dbEntity = this.createEntity(entity)
    if (dbEntity) {
      this.processCommonAttrs(entity, dbEntity)
    }
    return dbEntity
  }

  private convertArc(entity: Dwg_Object_Entity_Ptr): DwgArcEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity, obj)
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center').data as DwgPoint3D
    const radius = libredwg.dwg_dynapi_entity_value(entity, 'radius').data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness').data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(entity, 'extrusion').data as DwgPoint3D
    const startAngle = libredwg.dwg_dynapi_entity_value(entity, 'start_angle').data as number
    const endAngle = libredwg.dwg_dynapi_entity_value(entity, 'end_angle').data as string

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

  private convertCirle(circle: CircleEntity) {
    const dbEntity = new AcDbCircle(circle.center, circle.radius)
    return dbEntity
  }

  private getCommonAttrs(entity: Dwg_Object_Entity_Ptr) {
    const libredwg = this.libredwg
    const layer_ptr = libredwg.dwg_ent_get_layer_name(entity)
    const layer = libredwg.dwg_dynapi_entity_value(layer_ptr, 'name').data as string
    const  = libredwg.dwg_dynapi_common_value(entity, )

    return {
      layer: layer,
      handle: handle.value,
      ownerHandle: ownerhandle.absolute_ref,
    }

    dwg_dynapi_common_value
    dbEntity.layer = entity.layer
    dbEntity.objectId = entity.handle
    dbEntity.ownerId = entity.ownerBlockRecordSoftId || ''
    if (entity.lineType != null) {
      dbEntity.lineType = entity.lineType
    }
    if (entity.lineweight != null) {
      dbEntity.lineWeight = entity.lineweight
    }
    if (entity.lineTypeScale != null) {
      dbEntity.linetypeScale = entity.lineTypeScale
    }
    if (entity.color != null) {
      dbEntity.color.color = entity.color
    }
    if (entity.colorIndex != null) {
      dbEntity.color.colorIndex = entity.colorIndex
    }
    if (entity.colorName != null) {
      dbEntity.color.colorName = entity.colorName
    }
    if (entity.isVisible != null) {
      dbEntity.visibility = entity.isVisible
    }
    if (entity.transparency != null) {
      dbEntity.transparency = entity.transparency
    }
  }
}