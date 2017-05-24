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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                pc_thinning_simple.cpp                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_thinning_simple.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Thinning_Simple::CPC_Thinning_Simple(void)
{

	Set_Name(_TL("Point Cloud Thinning (simple)"));

	Set_Author(SG_T("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The tool can be used to remove every i-th point from a point cloud. This thinning "
		"method is most suited for data in chronological order.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "INPUT"		,_TL("Input"),
		_TL("Point Cloud to drop attribute from."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "RESULT"		, _TL("Result"),
		_TL("Resulting Point Cloud."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "STEP"		, _TL("Stepwidth"),
		_TL("Remove every i-th point."),
		PARAMETER_TYPE_Int, 2, 2, true
	);

}

//---------------------------------------------------------
CPC_Thinning_Simple::~CPC_Thinning_Simple(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Thinning_Simple::On_Execute(void)
{
	CSG_PointCloud		*pInput, *pResult;
	int					step;

	pInput				= Parameters("INPUT")->asPointCloud();
	pResult				= Parameters("RESULT")->asPointCloud();
	step				= Parameters("STEP")->asInt() - 1;


	pResult->Create(pInput);
	pResult->Assign(pInput);
	pResult->Set_Name(CSG_String::Format(SG_T("%s_thinned"), pInput->Get_Name()));

	for (int i=step; i<pInput->Get_Point_Count() && Set_Progress(i, pInput->Get_Point_Count()); i+=step)
	{
		pResult->Del_Point(i);
	}


	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
