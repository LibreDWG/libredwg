/*
 * dwg_transform_v4 - Módulo de transformación espacial V4 para GNU LibreDWG
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "v4_transform.h"
#include <stdint.h>
#include <stdlib.h>

inline static void v4_transform_3d(double *x, double *y, double *z) {
    if (!x || !y || !z) return;
    int32_t ix = (int32_t)*x, iy = (int32_t)*y, iz = (int32_t)*z, iw = 1;
    const int32_t s0 = ix + iy, d0 = ix - iy;
    const int32_t s1 = iz + iw, d1 = iz - iw;
    *x = (double)(s0 + s1);
    *y = (double)(d0 + d1);
    *z = (double)(s0 - s1);
}

uint32_t dwg_transform_v4(Dwg_Data *dwg) {
    if (!dwg || !dwg->object) return 0;

    uint32_t count = 0;

    for (uint32_t i = 0; i < dwg->num_objects; i++) {
        Dwg_Object *obj = &dwg->object[i];
        if (!obj || !obj->tio.entity) continue;

        switch (obj->type) {
            case DWG_TYPE_LINE: {
                Dwg_Entity_LINE *e = obj->tio.entity->tio.LINE;
                if (e) {
                    v4_transform_3d(&e->start.x, &e->start.y, &e->start.z);
                    v4_transform_3d(&e->end.x, &e->end.y, &e->end.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_CIRCLE: {
                Dwg_Entity_CIRCLE *e = obj->tio.entity->tio.CIRCLE;
                if (e) {
                    v4_transform_3d(&e->center.x, &e->center.y, &e->center.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_ARC: {
                Dwg_Entity_ARC *e = obj->tio.entity->tio.ARC;
                if (e) {
                    v4_transform_3d(&e->center.x, &e->center.y, &e->center.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_ELLIPSE: {
                Dwg_Entity_ELLIPSE *e = obj->tio.entity->tio.ELLIPSE;
                if (e) {
                    v4_transform_3d(&e->center.x, &e->center.y, &e->center.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE__3DFACE: {
                Dwg_Entity__3DFACE *e = obj->tio.entity->tio._3DFACE;
                if (e) {
                    v4_transform_3d(&e->corner1.x, &e->corner1.y, &e->corner1.z);
                    v4_transform_3d(&e->corner2.x, &e->corner2.y, &e->corner2.z);
                    v4_transform_3d(&e->corner3.x, &e->corner3.y, &e->corner3.z);
                    v4_transform_3d(&e->corner4.x, &e->corner4.y, &e->corner4.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_LWPOLYLINE: {
                Dwg_Entity_LWPOLYLINE *e = obj->tio.entity->tio.LWPOLYLINE;
                if (e && e->points) {
                    for (uint32_t j = 0; j < e->num_points; j++) {
                        double z = e->elevation;
                        v4_transform_3d(&e->points[j].x, &e->points[j].y, &z);
                    }
                    count++;
                }
                break;
            }
            case DWG_TYPE_INSERT: {
                Dwg_Entity_INSERT *e = obj->tio.entity->tio.INSERT;
                if (e) {
                    v4_transform_3d(&e->ins_pt.x, &e->ins_pt.y, &e->ins_pt.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_TEXT: {
                Dwg_Entity_TEXT *e = obj->tio.entity->tio.TEXT;
                if (e) {
                    // TEXT usa un punto 2D (x,y) y almacena Z en 'elevation'
                    double z = e->elevation;
                    v4_transform_3d(&e->ins_pt.x, &e->ins_pt.y, &z);
                    e->elevation = z;
                    count++;
                }
                break;
            }
            case DWG_TYPE_MTEXT: {
                Dwg_Entity_MTEXT *e = obj->tio.entity->tio.MTEXT;
                if (e) {
                    // MTEXT sí usa un punto de inserción 3D real
                    v4_transform_3d(&e->ins_pt.x, &e->ins_pt.y, &e->ins_pt.z);
                    count++;
                }
                break;
            }
            default:
                break;
        }
    }

    return count;
}