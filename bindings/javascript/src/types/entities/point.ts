import { DwgPoint3D } from "../common";
import { DwgEntity } from "./entity";

export interface DwgPointEntity extends DwgEntity {
  position: DwgPoint3D;
  thickness: number;
  extrusionDirection: DwgPoint3D;
  angle: number;
}
