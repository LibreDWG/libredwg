import { DwgXData } from '../shared/xdata';

export interface DwgEntity {
  handle: number;
  ownerBlockRecordSoftId?: string;
  isInPaperSpace?: boolean;
  layer: string;
  lineType?: string;
  materialObjectHardId?: string;
  colorIndex?: number;
  lineweight?: number;
  lineTypeScale?: number;
  isVisible?: boolean;
  proxyByte?: number;
  proxyEntity?: string;
  color?: number;
  colorName?: string;
  transparency?: number;
  plotStyleHardId?: string;
  shadowMode?: number;
  xdata?: DwgXData;
  ownerdictionaryHardId?: string | number | boolean;
  ownerDictionarySoftId?: string | number | boolean;
}