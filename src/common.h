/*****************************************************************************/
/*  LibDWG - Free DWG read-only library                                      */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/// Komunaj kaj gxeneralaj funkcioj

#ifndef COMMON_H
#define COMMON_H

/**
 Tipoj de (eble kompaktitaj) dateneroj, kiuj komponas dwg-dosierojn.
 */
typedef enum DWG_DATENERO_TIPO
{
	DWG_DT_B,   /** bito (1 or 0) */
	DWG_DT_BB,  /** speciala 2-bita kodo (\angla{entmode} en \angla{entities}, ekzemple) */
	DWG_DT_RC,  /** kruda bitoko (ne kompaktita) */
	DWG_DT_RS,  /** kruda dubitoko (ne kompaktita) */
	DWG_DT_RD,  /** kruda duglitajxo (ne kompaktita) */
	DWG_DT_RL,  /** kruda kvarbitoko (ne kompaktita) */
	DWG_DT_BS,  /** bit-dubitoko (\angla{bitshort}) */
	DWG_DT_BL,  /** bit-kvarbitoko (\angla{bitlong}) */
	DWG_DT_BD,  /** bit-duglitajxo (\angla{bitdouble}) */
	DWG_DT_MC,  /** moduleca bitoko  */
	DWG_DT_MS,  /** moduleca dubitoko  */
	DWG_DT_BE,  /** bit-forpusxigo (\angla{BitExtrusion}) */
	DWG_DT_DD,  /** bit-duglitajxo kun antauxdifinajxo */
	DWG_DT_BT,  /** bit-dikeco (\angla{BitThickness}) */
	DWG_DT_H,   /** traktila referenco (vidu la sekcion \angla{HANDLE REFERENCES}) */
	DWG_DT_CMC, /** valoro de koloro? \angla{CmColor} */
	DWG_DT_T,   /** teksto (bit-dubitoka grandeco, sekvata de la signocxeno) */
	DWG_DT_TU,  /** Unikoda teksto (bit-dubitoka grandeco (laux kiom signoj), sekvata de unikoda signocxeno, po 2 bitokoj por signo) */
	DWG_DT_2RD,
	DWG_DT_2BD,
	DWG_DT_3BD,
} Dwg_Datenero_Tipo;

/**
 Identigaj referencoj de gardostarantoj.
 */
typedef enum DWG_GARDOSTARANTO
{
	DWG_GS_KAPO_FINO,
	DWG_GS_BILDO_EKO,
	DWG_GS_BILDO_FINO,
	DWG_GS_VARIABLO_EKO,
	DWG_GS_VARIABLO_FINO,
	DWG_GS_KLASO_EKO,
	DWG_GS_KLASO_FINO,
	DWG_GS_DUAKAPO_EKO,
	DWG_GS_DUAKAPO_FINO
} Dwg_Gardostaranto;

Dwg_Datenero_Tipo dwg_varmapo (int indico);

unsigned char *dwg_gardostaranto (Dwg_Gardostaranto kiu_gardostaranto);

#endif
