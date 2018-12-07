
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       pj_proj4                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                crs_transform_point.cpp                //
//                                                       //
//                 Copyright (C) 2018 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "crs_transform_point.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_Point::CCRS_Transform_Point(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Single Coordinate Transformation"));

	Set_Author		("O. Conrad (c) 2018");

	Set_Description	(_TW(
		"Transformation of a single coordinate. "
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	CCRS_Picker	CRS_Picker;

	Parameters.Add_Node  (""      , "SOURCE"    , _TL("Source"          ), _TL(""));
	Parameters.Add_String("SOURCE", "SOURCE_CRS", _TL("Proj4 Parameters"), _TL(""), "+proj=longlat +datum=WGS84");
	Parameters.Add_Double("SOURCE", "SOURCE_X"  , _TL("X"               ), _TL(""));
	Parameters.Add_Double("SOURCE", "SOURCE_Y"  , _TL("Y"               ), _TL(""));

	Parameters.Add_Node  (""      , "TARGET"    , _TL("Target"          ), _TL(""));
	Parameters.Add_String("TARGET", "TARGET_CRS", _TL("Proj4 Parameters"), _TL(""), "+proj=longlat +datum=WGS84");
	Parameters.Add_Double("TARGET", "TARGET_X"  , _TL("X"               ), _TL(""));
	Parameters.Add_Double("TARGET", "TARGET_Y"  , _TL("Y"               ), _TL(""));

//	Parameters.Add_Parameters("", "PICKER", _TL("CRS Picker"), _TL(""))->asParameters()->Assign_Parameters(CRS_Picker.Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Point::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Get_Parent() )
	{
		CSG_String	Source(pParameter->Get_Parent()->Cmp_Identifier("SOURCE") ? "SOURCE_" : "TARGET_");
		CSG_String	Target(pParameter->Get_Parent()->Cmp_Identifier("TARGET") ? "SOURCE_" : "TARGET_");

		double	x	= (*pParameters)(Source + "X")->asDouble();
		double	y	= (*pParameters)(Source + "Y")->asDouble();

		if( Transform(x, y,
			CSG_Projection((*pParameters)(Source + "CRS")->asString(), SG_PROJ_FMT_Proj4),
			CSG_Projection((*pParameters)(Target + "CRS")->asString(), SG_PROJ_FMT_Proj4)) )
		{
			pParameters->Set_Parameter(Target + "X", x);
			pParameters->Set_Parameter(Target + "Y", y);
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Point::On_Execute(void)
{
	double	x	= Parameters("SOURCE_X")->asDouble();
	double	y	= Parameters("SOURCE_Y")->asDouble();

	if( Transform(x, y,
		CSG_Projection(Parameters("SOURCE_CRS")->asString(), SG_PROJ_FMT_Proj4),
		CSG_Projection(Parameters("TARGET_CRS")->asString(), SG_PROJ_FMT_Proj4)) )
	{
		Parameters.Set_Parameter("TARGET_X", x);
		Parameters.Set_Parameter("TARGET_Y", y);

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Point::Transform(double &x, double &y, const CSG_Projection &Source, const CSG_Projection &Target)
{
	CSG_CRSProjector	Projector;

	return( Projector.Set_Source(Source) && Projector.Set_Target(Target) && Projector.Get_Projection(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
