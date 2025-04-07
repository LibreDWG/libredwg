import { Dwg_Object_Type } from '../enums'
import { Dwg_Object_Entity_Ptr, Dwg_Object_Ptr, LibreDwgEx } from '../libredwg'
import {
  DwgArcEntity,
  DwgCircleEntity,
  DwgEllipseEntity,
  DwgEntity,
  DwgLineEntity,
  DwgLWPolylineEntity,
  DwgLWPolylineVertex,
  DwgPoint2D,
  DwgPoint3D,
  DwgPointEntity,
  DwgTextEntity,
  DwgTextHorizontalAlign,
  DwgTextVerticalAlign
} from '../types'

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
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_ELLIPSE) {
        return this.convertEllise(tio)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_LINE) {
        return this.convertLine(tio)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_LWPOLYLINE) {
        return this.convertLWPolyline(tio)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_POINT) {
        return this.convertPoint(tio)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_TEXT) {
        return this.convertText(tio)
      }
    }
    return undefined
  }

  private convertArc(entity: Dwg_Object_Entity_Ptr): DwgArcEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center')
      .data as DwgPoint3D
    const radius = libredwg.dwg_dynapi_entity_value(entity, 'radius')
      .data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const startAngle = libredwg.dwg_dynapi_entity_value(entity, 'start_angle')
      .data as number
    const endAngle = libredwg.dwg_dynapi_entity_value(entity, 'end_angle')
      .data as number

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
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center')
      .data as DwgPoint3D
    const radius = libredwg.dwg_dynapi_entity_value(entity, 'radius')
      .data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    return {
      type: 'CIRCLE',
      ...commonAttrs,
      thickness: thickness,
      center: center,
      radius: radius,
      extrusionDirection: extrusionDirection
    }
  }

  private convertEllise(entity: Dwg_Object_Entity_Ptr): DwgEllipseEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const center = libredwg.dwg_dynapi_entity_value(entity, 'center')
      .data as DwgPoint3D
    const majorAxisEndPoint = libredwg.dwg_dynapi_entity_value(
      entity,
      'sm_axis'
    ).data as DwgPoint3D
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const axisRatio = libredwg.dwg_dynapi_entity_value(entity, 'axis_ratio')
      .data as number
    const startAngle = libredwg.dwg_dynapi_entity_value(entity, 'start_angle')
      .data as number
    const endAngle = libredwg.dwg_dynapi_entity_value(entity, 'end_angle')
      .data as number

    return {
      type: 'ELLIPSE',
      ...commonAttrs,
      center: center,
      majorAxisEndPoint: majorAxisEndPoint,
      extrusionDirection: extrusionDirection,
      axisRatio: axisRatio,
      startAngle: startAngle,
      endAngle: endAngle
    }
  }

  private convertLine(entity: Dwg_Object_Entity_Ptr): DwgLineEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const startPoint = libredwg.dwg_dynapi_entity_value(entity, 'start')
      .data as DwgPoint3D
    const endPoint = libredwg.dwg_dynapi_entity_value(entity, 'end')
      .data as DwgPoint3D
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    return {
      type: 'LINE',
      ...commonAttrs,
      thickness: thickness,
      startPoint: startPoint,
      endPoint: endPoint,
      extrusionDirection: extrusionDirection
    }
  }

  private convertLWPolyline(
    entity: Dwg_Object_Entity_Ptr
  ): DwgLWPolylineEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const numberOfVertices = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_points'
    ).data as number
    const flag = libredwg.dwg_dynapi_entity_value(entity, 'flag').data as number
    const constantWidth = libredwg.dwg_dynapi_entity_value(
      entity,
      'const_width'
    ).data as number
    const elevation = libredwg.dwg_dynapi_entity_value(entity, 'elevation')
      .data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    const vertices: DwgLWPolylineVertex[] = []
    const num_points = libredwg.dwg_dynapi_entity_value(entity, 'num_points')
      .data as number
    const points_ptr = libredwg.dwg_dynapi_entity_value(entity, 'points')
      .data as number
    const points = libredwg.dwg_ptr_to_point2d_array(points_ptr, num_points)
    const num_bulges = libredwg.dwg_dynapi_entity_value(entity, 'num_bulges')
      .data as number
    const bulges_ptr = libredwg.dwg_dynapi_entity_value(entity, 'bulges')
      .data as number
    const bulges = libredwg.dwg_ptr_to_double_array(bulges_ptr, num_bulges)
    points.forEach((point, index) => {
      vertices.push({
        id: index,
        x: point.x,
        y: point.y,
        bulge: bulges.length > index ? bulges[index] : 0
      })
    })

    return {
      type: 'LWPOLYLINE',
      ...commonAttrs,
      numberOfVertices: numberOfVertices,
      flag: flag,
      constantWidth: constantWidth,
      elevation: elevation,
      thickness: thickness,
      extrusionDirection: extrusionDirection,
      vertices: vertices
    }
  }

  private convertPoint(entity: Dwg_Object_Entity_Ptr): DwgPointEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const x = libredwg.dwg_dynapi_entity_value(entity, 'x').data as number
    const y = libredwg.dwg_dynapi_entity_value(entity, 'y').data as number
    const z = libredwg.dwg_dynapi_entity_value(entity, 'z').data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const angle = libredwg.dwg_dynapi_entity_value(entity, 'x_ang')
      .data as number

    return {
      type: 'POINT',
      ...commonAttrs,
      position: { x, y, z },
      thickness: thickness,
      extrusionDirection: extrusionDirection,
      angle: angle
    }
  }

  private convertText(entity: Dwg_Object_Entity_Ptr): DwgTextEntity {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonAttrs(entity)
    const text = libredwg.dwg_dynapi_entity_value(entity, 'text_value')
      .data as string
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const startPoint = libredwg.dwg_dynapi_entity_value(entity, 'ins_pt')
      .data as DwgPoint2D
    const endPoint = libredwg.dwg_dynapi_entity_value(entity, 'alignment_pt')
      .data as DwgPoint2D
    const rotation = libredwg.dwg_dynapi_entity_value(entity, 'rotation')
      .data as number
    const textHeight = libredwg.dwg_dynapi_entity_value(entity, 'height')
      .data as number
    const xScale = libredwg.dwg_dynapi_entity_value(entity, 'width_factor')
      .data as number
    const obliqueAngle = libredwg.dwg_dynapi_entity_value(
      entity,
      'oblique_angle'
    ).data as number
    // const style_ptr = libredwg.dwg_dynapi_entity_value(entity, 'style').data as number
    const generationFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'generation'
    ).data as number
    const halign = libredwg.dwg_dynapi_entity_value(entity, 'horiz_alignment')
      .data as number
    const valign = libredwg.dwg_dynapi_entity_value(entity, 'vert_alignment')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    return {
      type: 'TEXT',
      ...commonAttrs,
      text: text,
      thickness: thickness,
      startPoint: startPoint,
      endPoint: endPoint,
      textHeight: textHeight,
      rotation: rotation,
      xScale: xScale,
      obliqueAngle: obliqueAngle,
      styleName: '', // TODO: Set the correct value
      generationFlag: generationFlag,
      halign: halign as DwgTextHorizontalAlign,
      valign: valign as DwgTextVerticalAlign,
      extrusionDirection: extrusionDirection
    }
  }

  private getCommonAttrs(entity: Dwg_Object_Entity_Ptr) {
    const libredwg = this.libredwg
    const color = libredwg.dwg_object_entity_get_color_object(entity)
    const layer = libredwg.dwg_object_entity_get_layer_name(entity)
    const handle = libredwg.dwg_object_entity_get_handle_object(entity)
    const ownerhandle =
      libredwg.dwg_object_entity_get_ownerhandle_object(entity)
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
      transparency: 0 // TODO: Set the correct value
    }
  }
}
