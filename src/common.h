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
 Tipoj de (eble kompaktitaj) dateneroj, kiuj komponas dvg-dosierojn.
 */
typedef enum DVG_DATENERO_TIPO
{
	DVG_DT_B,   /** bito (1 or 0) */
	DVG_DT_BB,  /** speciala 2-bita kodo (\angla{entmode} en \angla{entities}, ekzemple) */
	DVG_DT_RC,  /** kruda bitoko (ne kompaktita) */
	DVG_DT_RS,  /** kruda dubitoko (ne kompaktita) */
	DVG_DT_RD,  /** kruda duglitajxo (ne kompaktita) */
	DVG_DT_RL,  /** kruda kvarbitoko (ne kompaktita) */
	DVG_DT_BS,  /** bit-dubitoko (\angla{bitshort}) */
	DVG_DT_BL,  /** bit-kvarbitoko (\angla{bitlong}) */
	DVG_DT_BD,  /** bit-duglitajxo (\angla{bitdouble}) */
	DVG_DT_MC,  /** moduleca bitoko  */
	DVG_DT_MS,  /** moduleca dubitoko  */
	DVG_DT_BE,  /** bit-forpusxigo (\angla{BitExtrusion}) */
	DVG_DT_DD,  /** bit-duglitajxo kun antauxdifinajxo */
	DVG_DT_BT,  /** bit-dikeco (\angla{BitThickness}) */
	DVG_DT_H,   /** traktila referenco (vidu la sekcion \angla{HANDLE REFERENCES}) */
	DVG_DT_CMC, /** valoro de koloro? \angla{CmColor} */
	DVG_DT_T,   /** teksto (bit-dubitoka grandeco, sekvata de la signocxeno) */
	DVG_DT_TU,  /** Unikoda teksto (bit-dubitoka grandeco (laux kiom signoj), sekvata de unikoda signocxeno, po 2 bitokoj por signo) */
	DVG_DT_2RD,
	DVG_DT_2BD,
	DVG_DT_3BD,
} Dvg_Datenero_Tipo;

/**
 Identigaj referencoj de gardostarantoj.
 */
typedef enum DVG_GARDOSTARANTO
{
	DVG_GS_KAPO_FINO,
	DVG_GS_BILDO_EKO,
	DVG_GS_BILDO_FINO,
	DVG_GS_VARIABLO_EKO,
	DVG_GS_VARIABLO_FINO,
	DVG_GS_KLASO_EKO,
	DVG_GS_KLASO_FINO,
	DVG_GS_DUAKAPO_EKO,
	DVG_GS_DUAKAPO_FINO
} Dvg_Gardostaranto;

Dvg_Datenero_Tipo dvg_varmapo (int indico);

unsigned char *dvg_gardostaranto (Dvg_Gardostaranto kiu_gardostaranto);

#endif
