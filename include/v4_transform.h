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
#ifndef V4_TRANSFORM_H
#define V4_TRANSFORM_H

#include <dwg.h>

/**
 * Aplica la transformación $V_4$ sobre las entidades geométricas de la estructura DWG.
 *
 * @param dwg Puntero al objeto Dwg_Data de LibreDWG.
 * @return Número total de entidades transformadas con éxito.
 */
uint32_t dwg_transform_v4(Dwg_Data *dwg);

#endif // V4_TRANSFORM_H