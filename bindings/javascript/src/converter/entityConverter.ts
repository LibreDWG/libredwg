import {
  Dwg3dFaceEntity,
  DwgAlignedDimensionEntity,
  DwgAngularDimensionEntity,
  DwgArcEdge,
  DwgArcEntity,
  DwgAttachmentPoint,
  DwgAttdefEntity,
  DwgBoundaryPath,
  DwgBoundaryPathEdge,
  DwgBoundaryPathEdgeType,
  DwgCircleEntity,
  DwgDimensionEntityCommon,
  DwgDimensionTextLineSpacing,
  DwgDimensionType,
  DwgEdgeBoundaryPath,
  DwgEllipseEdge,
  DwgEllipseEntity,
  DwgEmbeddedMText,
  DwgEntity,
  DwgHatchAssociativity,
  DwgHatchEntityBase,
  DwgHatchPatternType,
  DwgHatchSolidFill,
  DwgHatchStyle,
  DwgImageClippingBoundaryType,
  DwgImageEntity,
  DwgImageFlags,
  DwgInsertEntity,
  DwgLeaderEntity,
  DwgLineEdge,
  DwgLineEntity,
  DwgLWPolylineEntity,
  DwgLWPolylineVertex,
  DwgMLineEntity,
  DwgMLineVertex,
  DwgMTextDrawingDirection,
  DwgMTextEntity,
  DwgOle2FrameEntity,
  DwgOleFrameEntity,
  DwgOrdinateDimensionEntity,
  DwgPoint2D,
  DwgPoint3D,
  DwgPointEntity,
  DwgPolylineBoundaryPath,
  DwgPolylineEntity,
  DwgRadialDiameterDimensionEntity,
  DwgRayEntity,
  DwgSectionEntity,
  DwgSmoothType,
  DwgSolidEntity,
  DwgSplineEdge,
  DwgSplineEntity,
  DwgTableCell,
  DwgTableEntity,
  DwgTextBase,
  DwgTextEntity,
  DwgTextHorizontalAlign,
  DwgTextVerticalAlign,
  DwgToleranceEntity,
  DwgVertexEntity,
  DwgWipeoutEntity,
  DwgXlineEntity
} from '../database'
import { LibreDwgEx } from '../libredwg'
import {
  Dwg_Color,
  Dwg_Hatch_Edge_Type,
  Dwg_HATCH_Path,
  Dwg_Object_Entity_Ptr,
  Dwg_Object_Ptr,
  Dwg_Object_Type,
  Dwg_TABLE_Cell
} from '../types'

type DwgCommonAttributes = Omit<DwgEntity, 'type'>
type DwgDimensionCommonAttributes = Omit<
  DwgDimensionEntityCommon,
  'handle' | 'ownerBlockRecordSoftId' | 'layer' | 'subclassMarker'
>

export class LibreEntityConverter {
  libredwg: LibreDwgEx

  constructor(instance: LibreDwgEx) {
    this.libredwg = instance
  }

  convert(object_ptr: Dwg_Object_Ptr): DwgEntity | undefined {
    const libredwg = this.libredwg

    // Get values of the common attributes of one entity
    const entity = libredwg.dwg_object_to_entity(object_ptr)
    const entity_tio = libredwg.dwg_object_to_entity_tio(object_ptr)
    if (entity && entity_tio) {
      // Get values of the common attributes of one entity
      const commonAttrs = this.getCommonAttrs(entity)
      const fixedtype = libredwg.dwg_object_get_fixedtype(object_ptr)
      if (fixedtype == Dwg_Object_Type.DWG_TYPE_3DFACE) {
        return this.convert3dFace(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_ARC) {
        return this.convertArc(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_ATTDEF) {
        return this.convertAttdef(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_CIRCLE) {
        return this.convertCircle(entity_tio, commonAttrs)
      } else if (
        fixedtype == Dwg_Object_Type.DWG_TYPE_DIMENSION_ALIGNED ||
        fixedtype == Dwg_Object_Type.DWG_TYPE_DIMENSION_LINEAR
      ) {
        return this.convertAlignedDimension(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_DIMENSION_ANG3PT) {
        return this.convert3PointAngularDimension(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_DIMENSION_DIAMETER) {
        return this.convertDiameterDimension(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_DIMENSION_ORDINATE) {
        return this.convertOrdinateDimension(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_DIMENSION_RADIUS) {
        return this.convertRadiusDimension(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_ELLIPSE) {
        return this.convertEllise(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_HATCH) {
        return this.convertHatch(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_IMAGE) {
        return this.convertImage(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_INSERT) {
        return this.convertInsert(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_LEADER) {
        return this.convertLeader(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_LINE) {
        return this.convertLine(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_LWPOLYLINE) {
        return this.convertLWPolyline(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_MLINE) {
        return this.convertMLine(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_MTEXT) {
        return this.convertMText(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_OLE2FRAME) {
        return this.convertOle2Frame(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_OLEFRAME) {
        return this.convertOleFrame(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_POINT) {
        return this.convertPoint(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_POLYLINE_2D) {
        return this.convertPolyline2d(entity_tio, commonAttrs, object_ptr)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_RAY) {
        return this.convertRay(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_SECTIONOBJECT) {
        return this.convertSection(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_SOLID) {
        return this.convertSolid(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_SPLINE) {
        return this.convertSpline(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_TABLE) {
        return this.convertTable(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_TEXT) {
        return this.convertText(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_TOLERANCE) {
        return this.convertTolerance(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_WIPEOUT) {
        return this.convertWipeout(entity_tio, commonAttrs)
      } else if (fixedtype == Dwg_Object_Type.DWG_TYPE_XLINE) {
        return this.convertXline(entity_tio, commonAttrs)
      }
    }
    return undefined
  }

  private convert3dFace(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): Dwg3dFaceEntity {
    const libredwg = this.libredwg
    const corner1 = libredwg.dwg_dynapi_entity_value(entity, 'corner1')
      .data as DwgPoint3D
    const corner2 = libredwg.dwg_dynapi_entity_value(entity, 'corner2')
      .data as DwgPoint3D
    const corner3 = libredwg.dwg_dynapi_entity_value(entity, 'corner3')
      .data as DwgPoint3D
    const corner4 = libredwg.dwg_dynapi_entity_value(entity, 'corner4')
      .data as DwgPoint3D
    const flag = libredwg.dwg_dynapi_entity_value(entity, 'invis_flags')
      .data as number

    return {
      type: '3DFACE',
      ...commonAttrs,
      corner1: corner1,
      corner2: corner2,
      corner3: corner3,
      corner4: corner4,
      flag: flag
    }
  }

  private convertArc(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgArcEntity {
    const libredwg = this.libredwg
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

  private convertEmbeddedMText(
    entity: Dwg_Object_Entity_Ptr,
    subclassName: string
  ): DwgEmbeddedMText {
    const libredwg = this.libredwg
    const attachmentPoint = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'attachment'
    ).data as number
    const insertionPoint = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'ins_pt'
    ).data as DwgPoint3D
    const direction = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'x_axis_dir'
    ).data as DwgPoint3D
    const rectHeight = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'rect_height'
    ).data as number
    const rectWidth = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'rect_width'
    ).data as number
    const extentsHeight = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'extents_height'
    ).data as number
    const extentsWidth = libredwg.dwg_dynapi_subclass_value(
      entity,
      subclassName,
      'extents_width'
    ).data as number
    // const columnType = libredwg.dwg_dynapi_subclass_value(entity, subclassName, 'column_type')
    //   .data as number
    // const columnWidth = libredwg.dwg_dynapi_subclass_value(entity, subclassName, 'column_width')
    //   .data as number
    // const columnGutter = libredwg.dwg_dynapi_subclass_value(entity, subclassName, 'gutter')
    //   .data as number
    // const columnAutoHeight = libredwg.dwg_dynapi_subclass_value(
    //     entity,
    //     subclassName,
    //     'auto_height'
    //   ).data as number
    // const columnFlowReversed = libredwg.dwg_dynapi_subclass_value(
    //     entity,
    //     subclassName,
    //     'flow_reversed'
    //   ).data as number
    // const columnHeightCount = libredwg.dwg_dynapi_subclass_value(
    //   entity,
    //   subclassName,
    //   'num_column_heights'
    // ).data as number
    // const columnHeights_ptr = libredwg.dwg_dynapi_subclass_value(
    //   entity,
    //   subclassName,
    //   'column_heights'
    // ).data as number
    // const columnHeights = libredwg.dwg_ptr_to_double_array(
    //   columnHeights_ptr,
    //   columnHeightCount
    // )

    return {
      insertionPoint: insertionPoint,
      rectHeight: rectHeight,
      rectWidth: rectWidth,
      extentsHeight: extentsHeight,
      extentsWidth: extentsWidth,
      attachmentPoint: attachmentPoint as DwgAttachmentPoint,
      direction: direction
      // columnType: columnType,
      // columnFlowReversed: columnFlowReversed,
      // columnAutoHeight: columnAutoHeight,
      // columnWidth: columnWidth,
      // columnGutter: columnGutter,
      // columnHeightCount: columnHeightCount,
      // columnHeights: columnHeights
    }
  }

  private convertAttdef(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgAttdefEntity {
    const libredwg = this.libredwg

    // Because the field name of text string in Dwg_Entity_ATTDEF is 'default_value'
    // instead of 'text_value'. So we need to get its value again using the correct
    // field name.
    const textValue = libredwg.dwg_dynapi_entity_value(entity, 'default_value')
      .data as string
    const text = this.convertTextBase(entity)
    text.text = textValue

    const prompt = libredwg.dwg_dynapi_entity_value(entity, 'prompt')
      .data as string
    const tag = libredwg.dwg_dynapi_entity_value(entity, 'tag').data as string
    const flags = libredwg.dwg_dynapi_entity_value(entity, 'flags')
      .data as number
    const fieldLength = libredwg.dwg_dynapi_entity_value(entity, 'field_length')
      .data as number
    const lockPositionFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'lock_position_flag'
    ).data as number
    const duplicateRecordCloningFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'keep_duplicate_records'
    ).data as number
    const isReallyLocked = libredwg.dwg_dynapi_entity_value(
      entity,
      'is_locked_in_block'
    ).data as number
    // TODO: double check whether 'mtext_type' is 'mtextFlag'
    const mtextFlag = libredwg.dwg_dynapi_entity_value(entity, 'mtext_type')
      .data as number
    const alignmentPoint = libredwg.dwg_dynapi_entity_value(
      entity,
      'alignment_pt'
    ).data as DwgPoint2D

    return {
      type: 'ATTDEF',
      ...commonAttrs,
      text: this.convertTextBase(entity),
      prompt: prompt,
      tag: tag,
      flags: flags,
      fieldLength: fieldLength,
      lockPositionFlag: lockPositionFlag > 0,
      duplicateRecordCloningFlag: duplicateRecordCloningFlag > 0,
      mtextFlag: mtextFlag,
      isReallyLocked: isReallyLocked > 0,
      alignmentPoint: alignmentPoint,
      annotationScale: 1, // TODO: Set the correct value
      attrTag: '', // TODO: Set the correct value
      mtext: this.convertEmbeddedMText(entity, 'ATTDEF_mtext')
    }
  }

  private convertCircle(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgCircleEntity {
    const libredwg = this.libredwg
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

  private convertAlignedDimension(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgAlignedDimensionEntity {
    const libredwg = this.libredwg
    const dimensionCommonAttrs = this.getDimensionCommonAttrs(entity)
    // TODO: Not sure whether 'clone_ins_pt' is same as 'insertionPoint'
    const insertionPoint = libredwg.dwg_dynapi_entity_value(
      entity,
      'clone_ins_pt'
    ).data as DwgPoint2D
    const subDefinitionPoint1 = libredwg.dwg_dynapi_entity_value(
      entity,
      'xline1_pt'
    ).data as DwgPoint3D
    const subDefinitionPoint2 = libredwg.dwg_dynapi_entity_value(
      entity,
      'xline2_pt'
    ).data as DwgPoint3D
    const rotationAngle = libredwg.dwg_dynapi_entity_value(
      entity,
      'ins_rotation'
    ).data as number | undefined
    const obliqueAngle = libredwg.dwg_dynapi_entity_value(
      entity,
      'oblique_angle'
    ).data as number

    return {
      subclassMarker: 'AcDbAlignedDimension',
      ...commonAttrs,
      ...dimensionCommonAttrs,
      insertionPoint: insertionPoint,
      subDefinitionPoint1: subDefinitionPoint1,
      subDefinitionPoint2: subDefinitionPoint2,
      rotationAngle: rotationAngle == null ? 0 : rotationAngle,
      obliqueAngle: obliqueAngle
    }
  }

  private convert3PointAngularDimension(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgAngularDimensionEntity {
    const libredwg = this.libredwg
    const dimensionCommonAttrs = this.getDimensionCommonAttrs(entity)
    const subDefinitionPoint1 = libredwg.dwg_dynapi_entity_value(
      entity,
      'xline1_pt'
    ).data as DwgPoint3D
    const subDefinitionPoint2 = libredwg.dwg_dynapi_entity_value(
      entity,
      'xline2_pt'
    ).data as DwgPoint3D
    const centerPoint = libredwg.dwg_dynapi_entity_value(entity, 'center_pt')
      .data as DwgPoint3D
    const arcPoint = libredwg.dwg_dynapi_entity_value(entity, 'xline2end_pt')
      .data as DwgPoint3D

    return {
      subclassMarker: 'AcDb3PointAngularDimension',
      ...commonAttrs,
      ...dimensionCommonAttrs,
      subDefinitionPoint1: subDefinitionPoint1,
      subDefinitionPoint2: subDefinitionPoint2,
      centerPoint: centerPoint,
      arcPoint: arcPoint
    }
  }

  private convertDiameterDimension(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgRadialDiameterDimensionEntity {
    const libredwg = this.libredwg
    const dimensionCommonAttrs = this.getDimensionCommonAttrs(entity)
    // TODO: Not sure whether 'first_arc_pt' is same as 'centerPoint'
    const centerPoint = libredwg.dwg_dynapi_entity_value(entity, 'first_arc_pt')
      .data as DwgPoint3D
    const leaderLength = libredwg.dwg_dynapi_entity_value(entity, 'leader_len')
      .data as number

    return {
      subclassMarker: 'AcDbDiametricDimension',
      ...commonAttrs,
      ...dimensionCommonAttrs,
      centerPoint: centerPoint,
      leaderLength: leaderLength
    }
  }

  private convertOrdinateDimension(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgOrdinateDimensionEntity {
    const libredwg = this.libredwg
    const dimensionCommonAttrs = this.getDimensionCommonAttrs(entity)
    // TODO: Not sure whether 'feature_location_pt' is same as 'subDefinitionPoint1'
    const subDefinitionPoint1 = libredwg.dwg_dynapi_entity_value(
      entity,
      'feature_location_pt'
    ).data as DwgPoint3D
    // TODO: Not sure whether 'leader_endpt' is same as 'subDefinitionPoint2'
    const subDefinitionPoint2 = libredwg.dwg_dynapi_entity_value(
      entity,
      'leader_endpt'
    ).data as DwgPoint3D

    return {
      subclassMarker: 'AcDbOrdinateDimension',
      ...commonAttrs,
      ...dimensionCommonAttrs,
      subDefinitionPoint1: subDefinitionPoint1,
      subDefinitionPoint2: subDefinitionPoint2
    }
  }

  private convertRadiusDimension(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgRadialDiameterDimensionEntity {
    const libredwg = this.libredwg
    const dimensionCommonAttrs = this.getDimensionCommonAttrs(entity)
    // TODO: Not sure whether 'first_arc_pt' is same as 'centerPoint'
    const centerPoint = libredwg.dwg_dynapi_entity_value(entity, 'first_arc_pt')
      .data as DwgPoint3D
    const leaderLength = libredwg.dwg_dynapi_entity_value(entity, 'leader_len')
      .data as number

    return {
      subclassMarker: 'AcDbRadialDimension',
      ...commonAttrs,
      ...dimensionCommonAttrs,
      centerPoint: centerPoint,
      leaderLength: leaderLength
    }
  }

  private convertEllise(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgEllipseEntity {
    const libredwg = this.libredwg
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

  private convertHatch(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgHatchEntityBase {
    const libredwg = this.libredwg
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const patternName = libredwg.dwg_dynapi_entity_value(entity, 'name')
      .data as string
    const isSolidFill = libredwg.dwg_dynapi_entity_value(
      entity,
      'is_solid_fill'
    ).data as number
    const isAssociative = libredwg.dwg_dynapi_entity_value(
      entity,
      'is_associative'
    ).data as number
    const numberOfBoundaryPaths = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_paths'
    ).data as number
    const paths_ptr = libredwg.dwg_dynapi_entity_value(entity, 'paths')
      .data as number
    const boundaryPaths = libredwg.dwg_ptr_to_hatch_path_array(
      paths_ptr,
      numberOfBoundaryPaths
    )
    const patternStyle = libredwg.dwg_dynapi_entity_value(entity, 'style')
      .data as number
    const patternType = libredwg.dwg_dynapi_entity_value(entity, 'pattern_type')
      .data as number
    const patternAngle = libredwg.dwg_dynapi_entity_value(entity, 'angle')
      .data as number
    const patternScale = libredwg.dwg_dynapi_entity_value(
      entity,
      'scale_spacing'
    ).data as number
    const numberOfDefinitionLines = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_deflines'
    ).data as number
    const definitionLines_ptr = libredwg.dwg_dynapi_entity_value(
      entity,
      'deflines'
    ).data as number
    const definitionLines = libredwg.dwg_ptr_to_hatch_defline_array(
      definitionLines_ptr,
      numberOfDefinitionLines
    )
    const pixelSize = libredwg.dwg_dynapi_entity_value(entity, 'pixel_size')
      .data as number
    const numberOfSeedPoints = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_seeds'
    ).data as number
    const seedPoints_ptr = libredwg.dwg_dynapi_entity_value(entity, 'seeds')
      .data as number
    const seedPoints = libredwg.dwg_ptr_to_point2d_array(
      seedPoints_ptr,
      numberOfSeedPoints
    )

    return {
      type: 'HATCH',
      ...commonAttrs,
      // elevationPoint: DwgPoint3D
      extrusionDirection: extrusionDirection,
      patternName: patternName,
      solidFill: isSolidFill
        ? DwgHatchSolidFill.SolidFill
        : DwgHatchSolidFill.PatternFill,
      // patternFillColor: number
      associativity: isAssociative
        ? DwgHatchAssociativity.Associative
        : DwgHatchAssociativity.NonAssociative,
      numberOfBoundaryPaths: numberOfBoundaryPaths,
      boundaryPaths: this.convertHatchBoundaryPaths(boundaryPaths),
      hatchStyle: patternStyle as DwgHatchStyle,
      patternType: patternType as DwgHatchPatternType,
      patternAngle: patternAngle,
      patternScale: patternScale,
      numberOfDefinitionLines: numberOfDefinitionLines,
      definitionLines: definitionLines.map(value => {
        return {
          angle: value.angle,
          base: value.pt0,
          offset: value.offset,
          numberOfDashLengths: value.dashes.length,
          dashLengths: value.dashes
        }
      }),
      pixelSize: pixelSize,
      numberOfSeedPoints: numberOfSeedPoints,
      // offsetVector?: DwgPoint3D
      seedPoints: seedPoints
      // gradientFlag?: DwgHatchGradientFlag
    }
  }

  private convertHatchBoundaryPaths(paths: Dwg_HATCH_Path[]) {
    const converted: DwgBoundaryPath[] = paths
      .filter(path => path.num_segs_or_paths > 0)
      .map(path => {
        const commonAttrs = {
          boundaryPathTypeFlag: path.flag
        }

        // Check whether it is a polyline
        if (path.flag & 0x02) {
          return {
            ...commonAttrs,
            hasBulge: path.bulges_present,
            isClosed: path.closed,
            numberOfVertices: path.num_segs_or_paths,
            vertices: path.polyline_paths.map(vertex => {
              return {
                x: vertex.point.x,
                y: vertex.point.y,
                bulge: vertex.bulge
              }
            })
          } as DwgPolylineBoundaryPath
        } else {
          const edges = path.segs.map(seg => {
            if (seg.curve_type == Dwg_Hatch_Edge_Type.Line) {
              return {
                type: DwgBoundaryPathEdgeType.Line,
                start: seg.first_endpoint,
                end: seg.second_endpoint
              } as DwgLineEdge
            } else if (seg.curve_type == Dwg_Hatch_Edge_Type.CircularArc) {
              return {
                type: DwgBoundaryPathEdgeType.Circular,
                center: seg.center,
                radius: seg.radius,
                startAngle: seg.start_angle,
                endAngle: seg.end_angle,
                isCCW: seg.is_ccw
              } as DwgArcEdge
            } else if (seg.curve_type == Dwg_Hatch_Edge_Type.EllipticArc) {
              return {
                type: DwgBoundaryPathEdgeType.Elliptic,
                center: seg.center,
                end: seg.endpoint,
                lengthOfMinorAxis: seg.minor_major_ratio,
                startAngle: seg.start_angle,
                endAngle: seg.end_angle,
                isCCW: seg.is_ccw
              } as DwgEllipseEdge
            } else if (seg.curve_type == Dwg_Hatch_Edge_Type.Spline) {
              return {
                type: DwgBoundaryPathEdgeType.Spline,
                degree: seg.degree,
                isRational: seg.is_rational,
                isPeriodic: seg.is_periodic,
                numberOfKnots: seg.num_knots,
                numberOfControlPoints: seg.num_control_points,
                knots: seg.knots,
                controlPoints: seg.control_points,
                numberOfFitData: seg.num_fitpts,
                fitDatum: seg.fitpts,
                startTangent: seg.start_tangent,
                endTangent: seg.end_tangent
              } as DwgSplineEdge
            }
          })
          return {
            ...commonAttrs,
            numberOfEdges: path.num_segs_or_paths,
            edges: edges
          } as DwgEdgeBoundaryPath<DwgBoundaryPathEdge>
        }
      })
    return converted
  }

  private convertImage(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgImageEntity {
    const libredwg = this.libredwg
    const version = libredwg.dwg_dynapi_entity_value(entity, 'class_version')
      .data as number
    const position = libredwg.dwg_dynapi_entity_value(entity, 'pt0')
      .data as DwgPoint3D
    const uPixel = libredwg.dwg_dynapi_entity_value(entity, 'uvec')
      .data as DwgPoint3D
    const vPixel = libredwg.dwg_dynapi_entity_value(entity, 'vvec')
      .data as DwgPoint3D
    const imageSize = libredwg.dwg_dynapi_entity_value(entity, 'image_size')
      .data as DwgPoint2D
    const flags = libredwg.dwg_dynapi_entity_value(entity, 'display_props')
      .data as number
    const clipping = libredwg.dwg_dynapi_entity_value(entity, 'clipping')
      .data as number
    const brightness = libredwg.dwg_dynapi_entity_value(entity, 'brightness')
      .data as number
    const contrast = libredwg.dwg_dynapi_entity_value(entity, 'contrast')
      .data as number
    const fade = libredwg.dwg_dynapi_entity_value(entity, 'fade').data as number
    const clipMode = libredwg.dwg_dynapi_entity_value(entity, 'clip_mode')
      .data as number
    const clippingBoundaryType = libredwg.dwg_dynapi_entity_value(
      entity,
      'clip_boundary_type'
    ).data as number
    const countBoundaryPoints = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_clip_verts'
    ).data as number
    const clip_verts = libredwg.dwg_dynapi_entity_value(entity, 'clip_verts')
      .data as number
    const clippingBoundaryPath = libredwg.dwg_ptr_to_point3d_array(
      clip_verts,
      countBoundaryPoints
    )

    const imagedef_ref = libredwg.dwg_dynapi_entity_value(entity, 'imagedef')
      .data as number
    const imageDefHandle = libredwg.dwg_ref_get_absref(imagedef_ref)
    const imagedefreactor_ref = libredwg.dwg_dynapi_entity_value(
      entity,
      'imagedefreactor'
    ).data as number
    const imageDefReactorHandle =
      libredwg.dwg_ref_get_absref(imagedefreactor_ref)

    return {
      type: 'IMAGE',
      ...commonAttrs,
      version: version,
      position: position,
      uPixel: uPixel,
      vPixel: vPixel,
      imageSize: imageSize,
      imageDefHandle: imageDefHandle,
      flags: flags as DwgImageFlags,
      clipping: clipping,
      brightness: brightness,
      contrast: contrast,
      fade: fade,
      imageDefReactorHandle: imageDefReactorHandle,
      clippingBoundaryType:
        clippingBoundaryType as DwgImageClippingBoundaryType,
      countBoundaryPoints: countBoundaryPoints,
      clippingBoundaryPath: clippingBoundaryPath,
      clipMode: clipMode
    }
  }

  private convertInsert(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgInsertEntity {
    const libredwg = this.libredwg

    // Get block name
    let name = ''
    const block_header_ref = libredwg.dwg_dynapi_entity_value(
      entity,
      'block_header'
    ).data as number
    if (block_header_ref) {
      const block_header_obj = libredwg.dwg_ref_get_object(block_header_ref)
      if (block_header_obj) {
        const block_header_tio =
          libredwg.dwg_object_to_object_tio(block_header_obj)
        if (block_header_tio) {
          name =
            libredwg.dwg_entity_block_header_get_block(block_header_tio).name
        }
      }
    }
    if (name === '') {
      /* pre-R2.0 */
      name = libredwg.dwg_dynapi_entity_value(entity, 'block_name')
        .data as string
    }

    const insertionPoint = libredwg.dwg_dynapi_entity_value(entity, 'ins_pt')
      .data as DwgPoint3D
    const scale = libredwg.dwg_dynapi_entity_value(entity, 'scale')
      .data as DwgPoint3D | null
    const rotation = libredwg.dwg_dynapi_entity_value(entity, 'rotation')
      .data as number
    const columnCount = libredwg.dwg_dynapi_entity_value(entity, 'num_cols')
      .data as number
    const rowCount = libredwg.dwg_dynapi_entity_value(entity, 'num_rows')
      .data as number
    const columnSpacing = libredwg.dwg_dynapi_entity_value(
      entity,
      'col_spacing'
    ).data as number
    const rowSpacing = libredwg.dwg_dynapi_entity_value(entity, 'row_spacing')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    // TODO: convert block attributes
    return {
      type: 'INSERT',
      ...commonAttrs,
      name: name,
      insertionPoint: insertionPoint,
      xScale: scale ? scale.x : 1,
      yScale: scale ? scale.y : 1,
      zScale: scale ? scale.z : 1,
      rotation: rotation,
      columnCount: columnCount,
      rowCount: rowCount,
      columnSpacing: columnSpacing,
      rowSpacing: rowSpacing,
      extrusionDirection: extrusionDirection
    }
  }

  private convertLeader(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgLeaderEntity {
    const libredwg = this.libredwg
    const styleName = libredwg.dwg_entity_mtext_get_style_name(entity)
    const isArrowheadEnabled = libredwg.dwg_dynapi_entity_value(
      entity,
      'arrowhead_type'
    ).data as number
    const isSpline = libredwg.dwg_dynapi_entity_value(entity, 'path_type')
      .data as number
    const leaderCreationFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'annot_type'
    ).data as number
    const isHooklineSameDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'hookline_dir'
    ).data as number
    const isHooklineExists = libredwg.dwg_dynapi_entity_value(
      entity,
      'hookline_on'
    ).data as number
    const textHeight = libredwg.dwg_dynapi_entity_value(entity, 'box_height')
      .data as number
    const textWidth = libredwg.dwg_dynapi_entity_value(entity, 'box_width')
      .data as number
    const numberOfVertices = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_points'
    ).data as number
    const vertices_ptr = libredwg.dwg_dynapi_entity_value(entity, 'points')
      .data as number
    const vertices =
      numberOfVertices > 0
        ? libredwg.dwg_ptr_to_point3d_array(vertices_ptr, numberOfVertices)
        : []
    const byBlockColor = libredwg.dwg_dynapi_entity_value(
      entity,
      'byblock_color'
    ).data as number
    const normal = libredwg.dwg_dynapi_entity_value(entity, 'extrusion')
      .data as DwgPoint3D
    const horizontalDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'x_direction'
    ).data as DwgPoint3D
    const offsetFromBlock = libredwg.dwg_dynapi_entity_value(
      entity,
      'inspt_offset'
    ).data as DwgPoint3D
    const offsetFromAnnotation = libredwg.dwg_dynapi_entity_value(
      entity,
      'endptproj'
    ).data as DwgPoint3D

    return {
      type: 'LEADER',
      ...commonAttrs,
      styleName: styleName,
      isArrowheadEnabled: isArrowheadEnabled > 0,
      isSpline: isSpline > 0,
      leaderCreationFlag: leaderCreationFlag,
      isHooklineSameDirection: isHooklineSameDirection > 0,
      isHooklineExists: isHooklineExists > 0,
      textHeight: textHeight,
      textWidth: textWidth,
      numberOfVertices: numberOfVertices,
      vertices: vertices,
      byBlockColor: byBlockColor,
      normal: normal,
      horizontalDirection: horizontalDirection,
      offsetFromBlock: offsetFromBlock,
      offsetFromAnnotation: offsetFromAnnotation
    }
  }

  private convertLine(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgLineEntity {
    const libredwg = this.libredwg
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
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgLWPolylineEntity {
    const libredwg = this.libredwg
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

  private convertMLine(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgMLineEntity {
    const libredwg = this.libredwg
    const scale = libredwg.dwg_dynapi_entity_value(entity, 'scale')
      .data as number
    const flags = libredwg.dwg_dynapi_entity_value(entity, 'flags')
      .data as number
    const justification = libredwg.dwg_dynapi_entity_value(
      entity,
      'justification'
    ).data as number
    const startPoint = libredwg.dwg_dynapi_entity_value(entity, 'base_point')
      .data as DwgPoint3D
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const numberOfLines = libredwg.dwg_dynapi_entity_value(entity, 'num_lines')
      .data as number
    const numberOfVertices = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_verts'
    ).data as number
    const verts_ptr = libredwg.dwg_dynapi_entity_value(entity, 'verts')
      .data as number
    const verts = libredwg.dwg_ptr_to_mline_vertex_array(
      verts_ptr,
      numberOfVertices
    )

    const vertices: DwgMLineVertex[] = []
    verts.forEach(vert => {
      vertices.push({
        vertex: vert.vertex,
        vertexDirection: vert.vertex_direction,
        miterDirection: vert.miter_direction,
        numberOfLines: vert.num_lines,
        lines: vert.lines.map(line => {
          return {
            numberOfSegmentParams: line.num_segparms,
            segmentParams: line.segparms,
            numberOfAreaFillParams: line.num_areafillparms,
            areaFillParams: line.areafillparms
          }
        })
      })
    })

    return {
      type: 'MLINE',
      ...commonAttrs,
      scale: scale,
      flags: flags,
      justification: justification,
      startPoint: startPoint,
      extrusionDirection: extrusionDirection,
      numberOfLines: numberOfLines,
      numberOfVertices: numberOfVertices,
      vertices: vertices,
      mlineStyle: '' // TODO: Set the correct value
    }
  }

  private convertOle2Frame(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgOle2FrameEntity {
    const libredwg = this.libredwg
    const oleVersion = libredwg.dwg_dynapi_entity_value(entity, 'oleversion')
      .data as number
    const oleClient = libredwg.dwg_dynapi_entity_value(entity, 'oleclient')
      .data as string
    const dataSize = libredwg.dwg_dynapi_entity_value(entity, 'data_size')
      .data as number
    const leftUpPoint = libredwg.dwg_dynapi_entity_value(entity, 'pt1')
      .data as DwgPoint3D
    const rightDownPoint = libredwg.dwg_dynapi_entity_value(entity, 'pt2')
      .data as DwgPoint3D
    const lockAspect = libredwg.dwg_dynapi_entity_value(entity, 'lock_aspect')
      .data as number
    const oleObjectType = libredwg.dwg_dynapi_entity_value(entity, 'type')
      .data as number
    const tileModeDescriptor = libredwg.dwg_dynapi_entity_value(entity, 'mode')
      .data as number
    const binaryData = libredwg.dwg_dynapi_entity_value(entity, 'data')
      .data as string
    return {
      type: 'OLE2FRAME',
      ...commonAttrs,
      oleVersion: oleVersion,
      oleClient: oleClient,
      dataSize: dataSize,
      leftUpPoint: leftUpPoint,
      rightDownPoint: rightDownPoint,
      lockAspect: lockAspect,
      oleObjectType: oleObjectType,
      tileModeDescriptor: tileModeDescriptor,
      binaryData: binaryData
    }
  }

  private convertOleFrame(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgOleFrameEntity {
    const libredwg = this.libredwg
    const flag = libredwg.dwg_dynapi_entity_value(entity, 'flag').data as number
    const mode = libredwg.dwg_dynapi_entity_value(entity, 'mode').data as number
    const dataSize = libredwg.dwg_dynapi_entity_value(entity, 'data_size')
      .data as number
    const binaryData = libredwg.dwg_dynapi_entity_value(entity, 'data')
      .data as string
    return {
      type: 'OLEFRAME',
      ...commonAttrs,
      flag: flag,
      mode: mode,
      dataSize: dataSize,
      binaryData: binaryData
    }
  }

  private convertMText(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgMTextEntity {
    const libredwg = this.libredwg
    const insertionPoint = libredwg.dwg_dynapi_entity_value(entity, 'ins_pt')
      .data as DwgPoint3D
    const textHeight = libredwg.dwg_dynapi_entity_value(entity, 'text_height')
      .data as number
    const rectHeight = libredwg.dwg_dynapi_entity_value(entity, 'rect_height')
      .data as number
    const rectWidth = libredwg.dwg_dynapi_entity_value(entity, 'rect_width')
      .data as number
    const extentsWidth = libredwg.dwg_dynapi_entity_value(
      entity,
      'extents_width'
    ).data as number
    const extentsHeight = libredwg.dwg_dynapi_entity_value(
      entity,
      'extents_height'
    ).data as number
    const attachmentPoint = libredwg.dwg_dynapi_entity_value(
      entity,
      'attachment'
    ).data as number
    const drawingDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'flow_dir'
    ).data as number
    const text = libredwg.dwg_dynapi_entity_value(entity, 'text').data as string
    const styleName = libredwg.dwg_entity_mtext_get_style_name(entity)
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const direction = libredwg.dwg_dynapi_entity_value(entity, 'x_axis_dir')
      .data as DwgPoint3D
    const lineSpacingStyle = libredwg.dwg_dynapi_entity_value(
      entity,
      'linespace_style'
    ).data as number
    const lineSpacing = libredwg.dwg_dynapi_entity_value(
      entity,
      'linespace_factor'
    ).data as number
    const backgroundFill = libredwg.dwg_dynapi_entity_value(
      entity,
      'bg_fill_flag'
    ).data as number
    const fillBoxScale = libredwg.dwg_dynapi_entity_value(
      entity,
      'bg_fill_scale'
    ).data as number
    const backgroundFillColor = libredwg.dwg_dynapi_entity_value(
      entity,
      'bg_fill_color'
    ).data as Dwg_Color
    const backgroundFillTransparency = libredwg.dwg_dynapi_entity_value(
      entity,
      'bg_fill_trans'
    ).data as number

    const columnType = libredwg.dwg_dynapi_entity_value(entity, 'column_type')
      .data as number
    const columnFlowReversed = libredwg.dwg_dynapi_entity_value(
      entity,
      'flow_reversed'
    ).data as number
    const columnAutoHeight = libredwg.dwg_dynapi_entity_value(
      entity,
      'auto_height'
    ).data as number
    const columnWidth = libredwg.dwg_dynapi_entity_value(entity, 'column_width')
      .data as number
    const columnGutter = libredwg.dwg_dynapi_entity_value(entity, 'gutter')
      .data as number
    const columnHeightCount = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_column_heights'
    ).data as number
    const columnHeights_ptr = libredwg.dwg_dynapi_entity_value(
      entity,
      'column_heights'
    ).data as number
    const columnHeights = libredwg.dwg_ptr_to_double_array(
      columnHeights_ptr,
      columnHeightCount
    )

    return {
      type: 'MTEXT',
      ...commonAttrs,
      insertionPoint: insertionPoint,
      textHeight: textHeight,
      rectHeight: rectHeight,
      rectWidth: rectWidth,
      extentsHeight: extentsHeight,
      extentsWidth: extentsWidth,
      attachmentPoint: attachmentPoint as DwgAttachmentPoint,
      drawingDirection: drawingDirection as DwgMTextDrawingDirection,
      text: text,
      styleName: styleName,
      extrusionDirection: extrusionDirection,
      direction: direction,
      rotation: 0, // TODO: Didn't find the corresponding field in libredwg
      lineSpacingStyle: lineSpacingStyle,
      lineSpacing: lineSpacing,
      backgroundFill: backgroundFill,
      // backgroundColor: backgroundColor.rgb, // TODO: Double check whether it should be color index
      fillBoxScale: fillBoxScale,
      backgroundFillColor: backgroundFillColor.rgb, // TODO: Double check whether it should be color index
      backgroundFillTransparency: backgroundFillTransparency,
      columnType: columnType,
      // columnCount: columnCount,
      columnFlowReversed: columnFlowReversed,
      columnAutoHeight: columnAutoHeight,
      columnWidth: columnWidth,
      columnGutter: columnGutter,
      columnHeightCount: columnHeightCount,
      columnHeights: columnHeights
    }
  }

  private convertPoint(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgPointEntity {
    const libredwg = this.libredwg
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

  private convertPolyline2d(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes,
    object: Dwg_Object_Ptr
  ): DwgPolylineEntity {
    const libredwg = this.libredwg
    const flag = libredwg.dwg_dynapi_entity_value(entity, 'flag').data as number
    const startWidth = libredwg.dwg_dynapi_entity_value(entity, 'start_width')
      .data as number
    const endWidth = libredwg.dwg_dynapi_entity_value(entity, 'end_width')
      .data as number
    const elevation = libredwg.dwg_dynapi_entity_value(entity, 'elevation')
      .data as number
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    const vertices = libredwg.dwg_entity_polyline_2d_get_vertices(object)
    return {
      type: 'POLYLINE',
      ...commonAttrs,
      flag: flag,
      startWidth: startWidth,
      endWidth: endWidth,
      elevation: elevation,
      thickness: thickness,
      extrusionDirection: extrusionDirection,
      vertices: vertices.map(vertex => {
        return {
          x: vertex.point.x,
          y: vertex.point.y,
          z: vertex.point.z,
          startWidth: vertex.start_width,
          endWidth: vertex.end_width,
          bulge: vertex.bulge,
          flag: vertex.flag,
          tangentDirection: vertex.tangent_dir
        } as unknown as DwgVertexEntity
      }),
      meshMVertexCount: 0,
      meshNVertexCount: 0,
      surfaceMDensity: 0,
      surfaceNDensity: 0,
      smoothType: DwgSmoothType.NONE
    }
  }

  private convertRay(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgRayEntity {
    const libredwg = this.libredwg
    const firstPoint = libredwg.dwg_dynapi_entity_value(entity, 'point')
      .data as DwgPoint3D
    const unitDirection = libredwg.dwg_dynapi_entity_value(entity, 'vector')
      .data as DwgPoint3D
    return {
      type: 'RAY',
      ...commonAttrs,
      firstPoint: firstPoint,
      unitDirection: unitDirection
    }
  }

  private convertSection(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgSectionEntity {
    const libredwg = this.libredwg
    const state = libredwg.dwg_dynapi_entity_value(entity, 'state')
      .data as number
    const flags = libredwg.dwg_dynapi_entity_value(entity, 'flag')
      .data as number
    const name = libredwg.dwg_dynapi_entity_value(entity, 'name').data as string
    const verticalDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'vert_dir'
    ).data as DwgPoint3D
    const topHeight = libredwg.dwg_dynapi_entity_value(entity, 'top_height')
      .data as number
    const bottomHeight = libredwg.dwg_dynapi_entity_value(
      entity,
      'bottom_height'
    ).data as number
    const indicatorTransparency = libredwg.dwg_dynapi_entity_value(
      entity,
      'indicator_alpha'
    ).data as number
    const indicatorColor = libredwg.dwg_dynapi_entity_value(
      entity,
      'indicator_color'
    ).data as Dwg_Color
    const numberOfVertices = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_verts'
    ).data as number
    const vertices_ptr = libredwg.dwg_dynapi_entity_value(entity, 'verts')
      .data as number
    const vertices =
      numberOfVertices > 0
        ? libredwg.dwg_ptr_to_point3d_array(vertices_ptr, numberOfVertices)
        : []
    const numberOfBackLineVertices = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_blverts'
    ).data as number
    const backLineVertices_ptr = libredwg.dwg_dynapi_entity_value(
      entity,
      'blverts'
    ).data as number
    const backLineVertices =
      numberOfBackLineVertices > 0
        ? libredwg.dwg_ptr_to_point3d_array(
            backLineVertices_ptr,
            numberOfBackLineVertices
          )
        : []
    const geometrySettingHandle = libredwg.dwg_dynapi_entity_value(
      entity,
      'geometrySettingHardId'
    ).data as number
    const geometrySettingHardId = libredwg.dwg_ref_get_handle_absolute_ref(
      geometrySettingHandle
    )
    return {
      type: 'SECTION',
      ...commonAttrs,
      state: state,
      flags: flags,
      name: name,
      verticalDirection: verticalDirection,
      topHeight: topHeight,
      bottomHeight: bottomHeight,
      indicatorTransparency: indicatorTransparency,
      indicatorColor: indicatorColor.rgb,
      numberOfVertices: numberOfVertices,
      vertices: vertices,
      numberOfBackLineVertices: numberOfBackLineVertices,
      backLineVertices: backLineVertices,
      geometrySettingHardId: geometrySettingHardId
    }
  }

  private convertSolid(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgSolidEntity {
    const libredwg = this.libredwg
    const corner1 = libredwg.dwg_dynapi_entity_value(entity, 'corner1')
      .data as DwgPoint2D
    const corner2 = libredwg.dwg_dynapi_entity_value(entity, 'corner2')
      .data as DwgPoint2D
    const corner3 = libredwg.dwg_dynapi_entity_value(entity, 'corner3')
      .data as DwgPoint2D
    const corner4 = libredwg.dwg_dynapi_entity_value(entity, 'corner4')
      .data as DwgPoint2D
    const thickness = libredwg.dwg_dynapi_entity_value(entity, 'thickness')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D

    return {
      type: 'SOLID',
      ...commonAttrs,
      corner1: corner1,
      corner2: corner2,
      corner3: corner3,
      corner4: corner4,
      thickness: thickness,
      extrusionDirection: extrusionDirection
    }
  }

  private convertSpline(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgSplineEntity {
    const libredwg = this.libredwg
    const flag = libredwg.dwg_dynapi_entity_value(entity, 'splineflags')
      .data as number
    const degree = libredwg.dwg_dynapi_entity_value(entity, 'degree')
      .data as number

    // Convert knots
    const knotTolerance = libredwg.dwg_dynapi_entity_value(entity, 'knot_tol')
      .data as number
    const numberOfKnots = libredwg.dwg_dynapi_entity_value(entity, 'num_knots')
      .data as number
    const knots_ptr = libredwg.dwg_dynapi_entity_value(entity, 'knots')
      .data as number
    const knots = libredwg.dwg_ptr_to_double_array(knots_ptr, numberOfKnots)

    // Convert fit points
    const fitTolerance = libredwg.dwg_dynapi_entity_value(entity, 'fit_tol')
      .data as number
    const numberOfFitPoints = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_fit_pts'
    ).data as number
    const fit_pts_ptr = libredwg.dwg_dynapi_entity_value(entity, 'fit_pts')
      .data as number
    const fitPoints = libredwg.dwg_ptr_to_point3d_array(
      fit_pts_ptr,
      numberOfFitPoints
    )

    // Convert control points
    const weighted = libredwg.dwg_dynapi_entity_value(entity, 'weighted')
      .data as number
    const controlTolerance = libredwg.dwg_dynapi_entity_value(
      entity,
      'ctrl_tol'
    ).data as number
    const numberOfControlPoints = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_ctrl_pts'
    ).data as number
    const ctrl_pts_ptr = libredwg.dwg_dynapi_entity_value(entity, 'ctrl_pts')
      .data as number
    const controlPoints = libredwg.dwg_ptr_to_point4d_array(
      ctrl_pts_ptr,
      numberOfControlPoints
    )

    const startTangent = libredwg.dwg_dynapi_entity_value(entity, 'beg_tan_vec')
      .data as DwgPoint3D
    const endTangent = libredwg.dwg_dynapi_entity_value(entity, 'end_tan_vec')
      .data as DwgPoint3D

    return {
      type: 'SPLINE',
      ...commonAttrs,
      // normal?: DwgPoint3D
      flag: flag,
      degree: degree,
      numberOfKnots: numberOfKnots,
      numberOfControlPoints: numberOfControlPoints,
      numberOfFitPoints: numberOfFitPoints,
      knotTolerance: knotTolerance,
      controlTolerance: controlTolerance,
      fitTolerance: fitTolerance,
      startTangent: startTangent,
      endTangent: endTangent,
      knots: knots,
      weights: weighted ? controlPoints.map(value => value.w) : undefined,
      controlPoints: controlPoints.map(value => {
        return {
          x: value.x,
          y: value.y,
          z: value.z
        }
      }),
      fitPoints: fitPoints
    }
  }

  private convertTable(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgTableEntity {
    const libredwg = this.libredwg
    const name = libredwg.dwg_dynapi_subclass_value(entity, 'ldata', 'name')
      .data as string
    const startPoint = libredwg.dwg_dynapi_entity_value(entity, 'ins_pt')
      .data as DwgPoint3D
    const directionVector = libredwg.dwg_dynapi_entity_value(
      entity,
      'horiz_direction'
    ).data as DwgPoint3D
    const tableValue = libredwg.dwg_dynapi_entity_value(
      entity,
      'flag_for_table_value'
    ).data as number
    const rowCount = libredwg.dwg_dynapi_entity_value(entity, 'num_rows')
      .data as number
    const columnCount = libredwg.dwg_dynapi_entity_value(entity, 'num_cols')
      .data as number
    const row_heights_ptr = libredwg.dwg_dynapi_entity_value(
      entity,
      'row_heights'
    ).data as number
    const rowHeightArr = libredwg.dwg_ptr_to_double_array(
      row_heights_ptr,
      rowCount
    )
    const col_widths_ptr = libredwg.dwg_dynapi_entity_value(
      entity,
      'col_widths'
    ).data as number
    const columnWidthArr = libredwg.dwg_ptr_to_double_array(
      col_widths_ptr,
      columnCount
    )
    const overrideFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'table_flag_override'
    ).data as number
    const borderColorOverrideFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'border_color_overrides_flag'
    ).data as number
    const borderLineWeightOverrideFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'border_lineweight_overrides_flag'
    ).data as number
    const borderVisibilityOverrideFlag = libredwg.dwg_dynapi_entity_value(
      entity,
      'border_visibility_overrides_flag'
    ).data as number
    const num_cells = libredwg.dwg_dynapi_entity_value(entity, 'num_cells')
      .data as number
    const cells_ptr = libredwg.dwg_dynapi_entity_value(entity, 'cells')
      .data as number
    const cells = libredwg.dwg_ptr_to_table_cell_array(cells_ptr, num_cells)

    return {
      type: 'ACAD_TABLE',
      ...commonAttrs,
      name: name,
      startPoint: startPoint,
      directionVector: directionVector,
      // attachmentPoint: DwgAttachmentPoint
      tableValue: tableValue,
      rowCount: rowCount,
      columnCount: columnCount,
      overrideFlag: overrideFlag,
      borderColorOverrideFlag: borderColorOverrideFlag,
      borderLineWeightOverrideFlag: borderLineWeightOverrideFlag,
      borderVisibilityOverrideFlag: borderVisibilityOverrideFlag,
      rowHeightArr: rowHeightArr,
      columnWidthArr: columnWidthArr,
      tableStyleId: '', // TODO: Set the correct value
      blockRecordHandle: 0, // TODO: Set the correct value
      cells: this.convertTableCells(cells),
      bmpPreview: ''
    }
  }

  private convertTableCells(cells: Dwg_TABLE_Cell[]) {
    const converted: DwgTableCell[] = []
    cells.forEach(cell => {
      return {
        text: cell.text_value,
        attachmentPoint: cell.cell_alignment as DwgAttachmentPoint,
        textStyle: cell.text_style, // TODO: Set the text style name instead of handle
        rotation: cell.rotation,
        cellType: cell.type,
        flagValue: cell.flags,
        mergedValue: cell.is_merged_value,
        autoFit: cell.is_autofit_flag,
        // borderWidth?: number
        // borderHeight?: number
        topBorderVisibility: cell.top_visibility,
        bottomBorderVisibility: cell.bottom_visibility,
        leftBorderVisibility: cell.left_visibility,
        rightBorderVisibility: cell.right_visibility,
        overrideFlag: cell.cell_flag_override,
        virtualEdgeFlag: cell.virtual_edge_flag,
        // fieldObjetId?: string
        blockTableRecordI: cell.block_handle.absolute_ref,
        blockScale: cell.block_scale,
        blockAttrNum: cell.attr_defs.length,
        attrDefineId: cell.attr_defs.map(value => value.attdef.absolute_ref),
        // attrText?: string
        // textHeight: number
        extendedCellFlags: cell.additional_data_flag
      }
    })
    return converted
  }

  private convertTextBase(entity: Dwg_Object_Entity_Ptr): DwgTextBase {
    const libredwg = this.libredwg
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
    const styleName = libredwg.dwg_entity_text_get_style_name(entity)
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
      text: text,
      thickness: thickness,
      startPoint: startPoint,
      endPoint: endPoint,
      textHeight: textHeight,
      rotation: rotation,
      xScale: xScale,
      obliqueAngle: obliqueAngle,
      styleName: styleName,
      generationFlag: generationFlag,
      halign: halign as DwgTextHorizontalAlign,
      valign: valign as DwgTextVerticalAlign,
      extrusionDirection: extrusionDirection
    }
  }

  private convertText(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgTextEntity {
    return {
      type: 'TEXT',
      ...commonAttrs,
      ...this.convertTextBase(entity)
    }
  }

  private convertTolerance(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgToleranceEntity {
    const libredwg = this.libredwg
    const insertionPoint = libredwg.dwg_dynapi_entity_value(entity, 'ins_pt')
      .data as DwgPoint3D
    const text = libredwg.dwg_dynapi_entity_value(entity, 'text_value')
      .data as string
    const xAxisDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'x_direction'
    ).data as DwgPoint3D
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const dimStyleName = libredwg.dwg_entity_get_dimstyle_name(entity)

    return {
      type: 'TOLERANCE',
      ...commonAttrs,
      styleName: dimStyleName,
      insertionPoint: insertionPoint,
      text: text,
      extrusionDirection: extrusionDirection,
      xAxisDirection: xAxisDirection
    }
  }

  private convertWipeout(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgWipeoutEntity {
    const libredwg = this.libredwg
    const version = libredwg.dwg_dynapi_entity_value(entity, 'class_version')
      .data as number
    const position = libredwg.dwg_dynapi_entity_value(entity, 'pt0')
      .data as DwgPoint3D
    const uPixel = libredwg.dwg_dynapi_entity_value(entity, 'uvec')
      .data as DwgPoint3D
    const vPixel = libredwg.dwg_dynapi_entity_value(entity, 'vvec')
      .data as DwgPoint3D
    const imageSize = libredwg.dwg_dynapi_entity_value(entity, 'image_size')
      .data as DwgPoint2D
    const flags = libredwg.dwg_dynapi_entity_value(entity, 'display_props')
      .data as number
    const clipping = libredwg.dwg_dynapi_entity_value(entity, 'clipping')
      .data as number
    const brightness = libredwg.dwg_dynapi_entity_value(entity, 'brightness')
      .data as number
    const contrast = libredwg.dwg_dynapi_entity_value(entity, 'contrast')
      .data as number
    const fade = libredwg.dwg_dynapi_entity_value(entity, 'fade').data as number
    const clipMode = libredwg.dwg_dynapi_entity_value(entity, 'clip_mode')
      .data as number
    const clippingBoundaryType = libredwg.dwg_dynapi_entity_value(
      entity,
      'clip_boundary_type'
    ).data as number
    const countBoundaryPoints = libredwg.dwg_dynapi_entity_value(
      entity,
      'num_clip_verts'
    ).data as number
    const clip_verts = libredwg.dwg_dynapi_entity_value(entity, 'clip_verts')
      .data as number
    const clippingBoundaryPath = libredwg.dwg_ptr_to_point3d_array(
      clip_verts,
      countBoundaryPoints
    )

    const imagedef_ref = libredwg.dwg_dynapi_entity_value(entity, 'imagedef')
      .data as number
    const imageDefHandle = libredwg.dwg_ref_get_absref(imagedef_ref)
    const imagedefreactor_ref = libredwg.dwg_dynapi_entity_value(
      entity,
      'imagedefreactor'
    ).data as number
    const imageDefReactorHandle =
      libredwg.dwg_ref_get_absref(imagedefreactor_ref)

    return {
      type: 'WIPEOUT',
      ...commonAttrs,
      version: version,
      position: position,
      uPixel: uPixel,
      vPixel: vPixel,
      imageSize: imageSize,
      imageDefHandle: imageDefHandle,
      flags: flags as DwgImageFlags,
      clipping: clipping,
      brightness: brightness,
      contrast: contrast,
      fade: fade,
      imageDefReactorHandle: imageDefReactorHandle,
      clippingBoundaryType:
        clippingBoundaryType as DwgImageClippingBoundaryType,
      countBoundaryPoints: countBoundaryPoints,
      clippingBoundaryPath: clippingBoundaryPath,
      clipMode: clipMode
    }
  }

  private convertXline(
    entity: Dwg_Object_Entity_Ptr,
    commonAttrs: DwgCommonAttributes
  ): DwgXlineEntity {
    const libredwg = this.libredwg
    const firstPoint = libredwg.dwg_dynapi_entity_value(entity, 'point')
      .data as DwgPoint3D
    const unitDirection = libredwg.dwg_dynapi_entity_value(entity, 'vector')
      .data as DwgPoint3D
    return {
      type: 'XLINE',
      ...commonAttrs,
      firstPoint: firstPoint,
      unitDirection: unitDirection
    }
  }

  private getDimensionCommonAttrs(
    entity: Dwg_Object_Entity_Ptr
  ): DwgDimensionCommonAttributes {
    const libredwg = this.libredwg
    const version = libredwg.dwg_dynapi_entity_value(entity, 'class_version')
      .data as number
    const name = libredwg.dwg_entity_get_block_name(entity, 'block')
    const definitionPoint = libredwg.dwg_dynapi_entity_value(entity, 'def_pt')
      .data as DwgPoint3D
    const textPoint = libredwg.dwg_dynapi_entity_value(entity, 'text_midpt')
      .data as DwgPoint2D
    const attachmentPoint = libredwg.dwg_dynapi_entity_value(
      entity,
      'attachmentPoint'
    ).data as number
    const dimensionType = libredwg.dwg_dynapi_entity_value(entity, 'flag')
      .data as number
    const textLineSpacingStyle = libredwg.dwg_dynapi_entity_value(
      entity,
      'lspace_factor'
    ).data as number
    const textLineSpacingFactor = libredwg.dwg_dynapi_entity_value(
      entity,
      'lspace_factor'
    ).data as number
    const measurement = libredwg.dwg_dynapi_entity_value(
      entity,
      'act_measurement'
    ).data as number
    const text = libredwg.dwg_dynapi_entity_value(entity, 'user_text')
      .data as string
    const textRotation = libredwg.dwg_dynapi_entity_value(
      entity,
      'text_rotation'
    ).data as number
    // TODO: Not sure whether 'ins_rotation' is 'ocsRotation'.
    const ocsRotation = libredwg.dwg_dynapi_entity_value(entity, 'ins_rotation')
      .data as number
    const extrusionDirection = libredwg.dwg_dynapi_entity_value(
      entity,
      'extrusion'
    ).data as DwgPoint3D
    const styleName = libredwg.dwg_entity_get_dimstyle_name(entity)

    return {
      type: 'DIMENSION',
      version: version,
      name: name,
      definitionPoint: definitionPoint,
      textPoint: textPoint,
      dimensionType: dimensionType as DwgDimensionType,
      attachmentPoint: attachmentPoint as DwgAttachmentPoint,
      textLineSpacingStyle: textLineSpacingStyle as DwgDimensionTextLineSpacing,
      textLineSpacingFactor: textLineSpacingFactor || 1,
      measurement: measurement,
      text: text,
      textRotation: textRotation,
      ocsRotation: ocsRotation,
      extrusionDirection: extrusionDirection,
      styleName: styleName
    }
  }

  private getCommonAttrs(entity: Dwg_Object_Entity_Ptr): DwgCommonAttributes {
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
      ownerBlockRecordSoftId: ownerhandle.absolute_ref,
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
