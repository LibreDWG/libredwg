import { DwgPoint3D } from '../common';
import { DwgEntity } from './entity';

export interface DwgArcEntity extends DwgEntity {
  thickness: number;
  center: DwgPoint3D;
  radius: number;
  startAngle: number;
  endAngle: number;
  extrusionDirection: DwgPoint3D;
}
