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
	Set_Name		(_TL("Point Cloud Thinning (Simple)"));

	Set_Author		("Volker Wichmann (c) 2010, LASERDATA GmbH");

	Set_Description	(_TW(
		"This simple thinning tool reduces the number of points in a point cloud "
		"by sequential point removal. It is therefore most suited for points "
		"stored in chronological order. "
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		"", "INPUT"		,_TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		"", "RESULT"	, _TL("Thinned Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(
		"", "PERCENT"	, _TL("Percent"),
		_TL("Reduce the number of points to this percentage."),
		50, 0, true, 100, true
	);

	Parameters.Add_Int(
		"", "NUMBER"	, _TL("Number of Points"),
		_TL(""),
		0, 0, true
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPC_Thinning_Simple::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_PointCloud	*pPoints	= pParameters->Get("INPUT")->asPointCloud();

	pParameters->Set_Enabled("NUMBER", pPoints != NULL);

	if( pPoints )
	{
		if( SG_STR_CMP(pParameter->Get_Identifier(), "NUMBER" ) )
		{
			pParameters->Set_Parameter("NUMBER", (int)(pPoints->Get_Point_Count() * pParameters->Get("PERCENT")->asDouble() / 100.0));
		}
		else if( pParameter->asInt() < pPoints->Get_Point_Count() )
		{
			pParameters->Set_Parameter("PERCENT", 100.0 * pParameter->asInt() / pPoints->Get_Point_Count());
		}
		else
		{
			pParameters->Set_Parameter("PERCENT", 100.0);
			pParameters->Set_Parameter("NUMBER" , pPoints->Get_Point_Count());
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Thinning_Simple::On_Execute(void)
{
	CSG_PointCloud	*pPoints	= Parameters("INPUT")->asPointCloud();

	double	d	= pPoints->Get_Point_Count() * Parameters("PERCENT")->asDouble() / 100.0;

	int n	= (int)d;

	if( n < 1 )
	{
		Error_Set(_TL("Execution stopped, because this would delete all points."));

		return( false );
	}
	else if( n >= pPoints->Get_Point_Count() - 1 )
	{
		Error_Set(_TL("Execution stopped, because this would delete no point at all."));

		return( false );
	}

	d	= pPoints->Get_Point_Count() / (double)n;

	//-----------------------------------------------------
	if( Parameters("RESULT")->asPointCloud() && Parameters("RESULT")->asPointCloud() != pPoints )
	{
		CSG_PointCloud	*pResult	= Parameters("RESULT")->asPointCloud();

		pResult->Create(pPoints);

		pResult->Set_Name(CSG_String::Format("%s [%.1f%%]", pPoints->Get_Name(), Parameters("PERCENT")->asDouble()));

		for(int i=0; i<n && Set_Progress(i, n); i++)
		{
			pResult->Add_Record(pPoints->Get_Record((int)(i * d)));
		}
	}

	//-----------------------------------------------------
	else
	{
		pPoints->Select();

		for(int i=0; i<n && Set_Progress(i, n); i++)
		{
			pPoints->Select((int)(i * d), true);
		}

		pPoints->Inv_Selection();
		pPoints->Del_Selection();

		DataObject_Update(pPoints);
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
