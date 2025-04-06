import { DwgPoint3D } from '../common';
import { DwgAttachmentPoint } from './dimension';
import { DwgEntity } from './entity';

export interface DwgTableCell {
  text: string;
  attachmentPoint: DwgAttachmentPoint;
  textStyle?: string;
  rotation?: number;
  cellType: number;
  flagValue?: number;
  mergedValue?: number;
  autoFit?: number;
  borderWidth?: number;
  borderHeight?: number;
  topBorderVisibility: boolean;
  bottomBorderVisibility: boolean;
  leftBorderVisibility: boolean;
  rightBorderVisibility: boolean;
  overrideFlag?: number;
  virtualEdgeFlag?: number;
  fieldObjetId?: string;
  blockTableRecordId?: string;
  blockScale?: number;
  blockAttrNum?: number;
  attrDefineId?: string[];
  attrText?: string;
  textHeight: number;
  extendedCellFlags?: number;
}

export interface DwgTableEntity extends DwgEntity {
  handle: string;
  ownerDictionaryId: string;
  name: string;
  version: number;
  startPoint: DwgPoint3D;
  directionVector: DwgPoint3D;
  attachmentPoint: DwgAttachmentPoint;
  tableValue: number;
  rowCount: number;
  columnCount: number;
  overrideFlag?: number;
  borderColorOverrideFlag?: number;
  borderLineWeightOverrideFlag?: number;
  borderVisibilityOverrideFlag?: number;
  rowHeightArr: number[];
  columnWidthArr: number[];
  tableStyleId?: string;
  blockRecordHandle?: string;
  cells: DwgTableCell[];
  bmpPreview: string;
}
