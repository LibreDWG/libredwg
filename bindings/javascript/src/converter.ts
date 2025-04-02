import { 
  DwgCommonObject,
  DwgCommonTableEntry, 
  DwgDatabase, 
  DwgImageDefObject, 
  DwgLayerTableEntry, 
  DwgLineTypeElement, 
  DwgLTypeTableEntry,
  DwgPoint2D,
  DwgPoint3D,
  DwgVPortTableEntry
} from './types'
import { 
  Dwg_Color,
  Dwg_Data_Ptr,
  Dwg_LTYPE_Dash,
  Dwg_Object_Object_Ptr,
  Dwg_Object_Ptr,
  Dwg_Object_Ref_Ptr,
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
        },
        VPORT: {
          entries: []
        }
      },
      objects: {
        IMAGEDEF: []
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
          case Dwg_Object_Type.DWG_TYPE_VPORT:
            db.tables.VPORT.entries.push(this.convertViewport(tio, obj))
            break;
          case Dwg_Object_Type.DWG_TYPE_IMAGEDEF:
            db.objects.IMAGEDEF.push(this.convertImageDef(tio, obj))
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
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonTableEntryAttrs(item, obj)
    const flag = libredwg.dwg_dynapi_entity_value(item, 'flag').data as number
    const frozen = libredwg.dwg_dynapi_entity_value(item, 'frozen').data as number
    const off = libredwg.dwg_dynapi_entity_value(item, 'off').data as number
    const frozenInNew = libredwg.dwg_dynapi_entity_value(item, 'frozen_in_new').data as number
    const locked = libredwg.dwg_dynapi_entity_value(item, 'plotflockedlag').data as number
    const plotFlag = libredwg.dwg_dynapi_entity_value(item, 'plotflag').data as number
    const linewt = libredwg.dwg_dynapi_entity_value(item, 'linewt').data as number
    const color = libredwg.dwg_dynapi_entity_value(item, 'color').data as Dwg_Color
    
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
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonTableEntryAttrs(item, obj)
    const flag = libredwg.dwg_dynapi_entity_value(item, 'flag').data as number
    const description = libredwg.dwg_dynapi_entity_value(item, 'description').data as string
    const numDashes = libredwg.dwg_dynapi_entity_value(item, 'numdashes').data as number
    const patternLen = libredwg.dwg_dynapi_entity_value(item, 'pattern_len').data as number
    // const dashes = libredwg.dwg_dynapi_entity_value(item, 'dashes').data as Dwg_Array_Ptr
    // const dashArray = dashes ? libredwg.dwg_ptr_to_ltype_dash_array(dashes, numDashes) : []
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

  private convertViewport(item: Dwg_Object_Object_Ptr, obj: Dwg_Object_Ptr): DwgVPortTableEntry {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonTableEntryAttrs(item, obj)
    const standardFlag = libredwg.dwg_dynapi_entity_value(item, 'flag').data as number
    const viewHeight = libredwg.dwg_dynapi_entity_value(item, 'VIEWSIZE').data as number
    // BITCODE_BD view_width;   // in DWG r13+, needed to calc. aspect_ratio
    // BITCODE_BD aspect_ratio; // DXF 41 = view_width / VIEWSIZE
    const center = libredwg.dwg_dynapi_entity_value(item, 'VIEWCTR').data as DwgPoint2D
    const viewTarget = libredwg.dwg_dynapi_entity_value(item, 'view_target').data as DwgPoint3D
    const viewDirectionFromTarget = libredwg.dwg_dynapi_entity_value(item, 'VIEWDIR').data as DwgPoint3D
    const viewTwistAngle = libredwg.dwg_dynapi_entity_value(item, 'view_twist').data as number
    const lensLength = libredwg.dwg_dynapi_entity_value(item, 'lens_length').data as number
    const frontClippingPlane = libredwg.dwg_dynapi_entity_value(item, 'front_clip_z').data as number
    const backClippingPlane = libredwg.dwg_dynapi_entity_value(item, 'back_clip_z').data as number
    const viewMode = libredwg.dwg_dynapi_entity_value(item, 'VIEWMODE').data as number
    const renderMode = libredwg.dwg_dynapi_entity_value(item, 'render_mode').data as number
    const isDefaultLightingOn = (libredwg.dwg_dynapi_entity_value(item, 'use_default_lights').data as number != 0)
    const defaultLightningType = libredwg.dwg_dynapi_entity_value(item, 'default_lightning_type').data as number
    const brightness = libredwg.dwg_dynapi_entity_value(item, 'brightness').data as number
    const contrast = libredwg.dwg_dynapi_entity_value(item, 'contrast').data as number
    const ambient_color = libredwg.dwg_dynapi_entity_value(item, 'ambient_color').data as Dwg_Color

    // ViewportTableRecord
    const lowerLeftCorner = libredwg.dwg_dynapi_entity_value(item, 'lower_left').data as DwgPoint2D
    const upperRightCorner = libredwg.dwg_dynapi_entity_value(item, 'upper_right').data as DwgPoint2D
    // TODO: Not sure whether 'circleSides' is equal to 'circle_zoom'
    const circleSides = libredwg.dwg_dynapi_entity_value(item, 'circle_zoom').data as number
    const ucsIconSetting = libredwg.dwg_dynapi_entity_value(item, 'UCSICON').data as number
    // TODO: Not sure whether 'gridSpacing' is equal to 'GRIDUNIT'
    const gridSpacing = libredwg.dwg_dynapi_entity_value(item, 'GRIDUNIT').data as DwgPoint2D
    const snapRotationAngle = libredwg.dwg_dynapi_entity_value(item, 'SNAPANG').data as number
    const snapBasePoint = libredwg.dwg_dynapi_entity_value(item, 'SNAPBASE').data as DwgPoint2D
    // TODO: Not sure whether 'snapSpacing' is equal to 'SNAPUNIT'
    const snapSpacing = libredwg.dwg_dynapi_entity_value(item, 'SNAPUNIT').data as DwgPoint2D
    const ucsOrigin = libredwg.dwg_dynapi_entity_value(item, 'ucsorg').data as DwgPoint3D
    const ucsXAxis = libredwg.dwg_dynapi_entity_value(item, 'ucsxdir').data as DwgPoint3D
    const ucsYAxis = libredwg.dwg_dynapi_entity_value(item, 'ucsydir').data as DwgPoint3D
    const elevation = libredwg.dwg_dynapi_entity_value(item, 'ucs_elevation').data as number
    const majorGridLines = libredwg.dwg_dynapi_entity_value(item, 'grid_major').data as number
    const background = libredwg.dwg_dynapi_entity_value(item, 'background').data as Dwg_Object_Ref_Ptr
    const backgroundObjectId = background ? libredwg.dwg_ref_get_absref(background).toString() : undefined
    const visualstyle = libredwg.dwg_dynapi_entity_value(item, 'visualstyle').data as Dwg_Object_Ref_Ptr
    const visualStyleObjectId = visualstyle ? libredwg.dwg_ref_get_absref(visualstyle).toString() : undefined

    // BITCODE_B UCSFOLLOW;
    // BITCODE_B FASTZOOM;
    // BITCODE_B GRIDMODE;     /* DXF 76: on or off */
    // BITCODE_B SNAPMODE;     /* DXF 75: on or off */
    // BITCODE_B SNAPSTYLE;
    // BITCODE_BS SNAPISOPAIR;
    // BITCODE_B ucs_at_origin;
    // BITCODE_B UCSVP;
    // BITCODE_BS UCSORTHOVIEW;
    // BITCODE_BS grid_flags; /* bit 1: bound to limits, bit 2: adaptive */
    // BITCODE_H sun;
    // BITCODE_H named_ucs;
    // BITCODE_H base_ucs;

    return {
      ...commonAttrs,
      standardFlag: standardFlag,
      lowerLeftCorner: lowerLeftCorner,
      upperRightCorner: upperRightCorner,
      center: center,
      snapBasePoint: snapBasePoint,
      snapSpacing: snapSpacing,
      gridSpacing: gridSpacing,
      viewDirectionFromTarget: viewDirectionFromTarget,
      viewTarget: viewTarget,
      lensLength: lensLength,
      frontClippingPlane: frontClippingPlane,
      backClippingPlane: backClippingPlane,
      viewHeight: viewHeight,
      snapRotationAngle: snapRotationAngle,
      viewTwistAngle: viewTwistAngle,
      circleSides: circleSides,
      frozenLayers: [], // TODO: Set the correct value
      styleSheet: '',   // TODO: Set the correct value
      renderMode: renderMode,
      viewMode: viewMode,
      ucsIconSetting: ucsIconSetting,
      ucsOrigin: ucsOrigin,
      ucsXAxis: ucsXAxis,
      ucsYAxis: ucsYAxis,
      orthographicType: 0, // TODO: Set the correct value
      elevation: elevation,
      shadePlotSetting: 0, // TODO: Set the correct value
      majorGridLines: majorGridLines,
      backgroundObjectId: backgroundObjectId,
      // shadePlotObjectId: undefined,
      visualStyleObjectId: visualStyleObjectId,
      isDefaultLightingOn: isDefaultLightingOn,
      defaultLightingType: defaultLightningType,
      brightness: brightness,
      contrast: contrast,
      // TODO: Not sure whether 'index' or 'rgb' should be used
      ambientColor: ambient_color.index
    }
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
      ownerHandle: ownerhandle.absolute_ref,
      name: libredwg.dwg_dynapi_entity_value(tio, 'name').data as string
    }
  }

  private convertImageDef(item: Dwg_Object_Object_Ptr, obj: Dwg_Object_Ptr): DwgImageDefObject {
    const libredwg = this.libredwg
    const commonAttrs = this.getCommonObjectAttrs(obj)
    // const classVersion = libredwg.dwg_dynapi_entity_value(item, 'class_version').data as number
    const size = libredwg.dwg_dynapi_entity_value(item, 'image_size').data as DwgPoint2D
    const fileName = libredwg.dwg_dynapi_entity_value(item, 'file_path').data as string
    const isLoaded = libredwg.dwg_dynapi_entity_value(item, 'is_loaded').data as number
    const sizeOfOnePixel = libredwg.dwg_dynapi_entity_value(item, 'pixel_size').data as DwgPoint2D
    const resolutionUnits = libredwg.dwg_dynapi_entity_value(item, 'resunits').data as number
  
    return {
      ...commonAttrs,
      fileName: fileName,
      size: size,
      sizeOfOnePixel: sizeOfOnePixel,
      isLoaded: isLoaded,
      resolutionUnits: resolutionUnits
    }
  }

  private getCommonObjectAttrs(obj: Dwg_Object_Ptr): DwgCommonObject {
    const libredwg = this.libredwg
    const object_tio = libredwg.dwg_object_get_tio(obj)
    const ownerhandle = libredwg.dwg_object_object_get_ownerhandle_object(object_tio)
    const handle = libredwg.dwg_object_get_handle_object(obj)
    return {
      handle: handle.value,
      ownerHandle: ownerhandle.absolute_ref,
    }
  }
}