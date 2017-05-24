/**********************************************************
 * Version $Id$
 *********************************************************/
 
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      sim_erosion                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      MMF_SAGA.h                       //
//                                                       //
//                Copyright (C) 2009-12 by               //
//                    Volker Wichmann                    //
//                   Muhammad Setiawan                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__MMF_SAGA_H
#define HEADER_INCLUDED__MMF_SAGA_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// For debugging purposes the source code is currently configured
// to output additional grid datasets; comment the line below and
// recompile to disable this functionality.
#define _TMP_OUT


//---------------------------------------------------------
// coding of transport conditions
#define TCOND_SED_LIMITED		0	// sediment limited
#define TCOND_TRANS_LIMITED_TC	1	// transport limited (SL = TC)
#define TCONF_TRANS_LIMITED_G	2	// transport limited (SL = G)


//---------------------------------------------------------
typedef enum EMMF_KE_I_Key
{
	MMF_KE_I_WISCHMEIER_SMITH_1978	= 0,
	MMF_KE_I_MARSHALL_PALMER,
	MMF_KE_I_ZANCHI_TORRI_1980,
	MMF_KE_I_COUTINHO_TOMAS_1995,
	MMF_KE_I_HUDSON_1965,
	MMF_KE_I_ONEGA_ET_AL_1998,
	MMF_KE_I_ROSEWELL_1986,
	MMF_KE_I_MCISAAC_1990,
	MMF_KE_I_Count
}
TMMF_KE_I_Key;


//---------------------------------------------------------
const SG_Char	gMMF_KE_I_Key_Name[MMF_KE_I_Count][64]	=
{
	SG_T("North America east of Rocky Mountains (Wischmeier & Smith 1978)"),
	SG_T("North-western Europe (Marshall & Palmer)"),
	SG_T("Mediterranean-type climates (Zanchi & Torri 1980)"),
	SG_T("Western Mediterranean (Coutinho & Tomas 1995)"),
	SG_T("Tropical climates (Hudson 1965)"),
	SG_T("Eastern Asia (Onaga et al. 1998)"),
	SG_T("Southern hemisphere climates (Rosewell 1986)"),
	SG_T("Bogor, West-Java, Indonesia (McISaac 1990)")
};



//---------------------------------------------------------
class CMMF_SAGA : public CSG_Tool_Grid
{
public:
	CMMF_SAGA(void);
	virtual ~CMMF_SAGA(void);


protected:

	virtual bool		On_Execute(void);


private:


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__MMF_SAGA_H

