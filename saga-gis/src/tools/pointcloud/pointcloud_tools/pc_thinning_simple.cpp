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
		"The tool can be used to remove every i-th point from a point cloud. "
		"This thinning method is most suited for data in chronological order."
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		"", "INPUT"		,_TL("Input"),
		_TL("Point Cloud to drop attribute from."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		"", "RESULT"	, _TL("Result"),
		_TL("Resulting Point Cloud."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(
		"", "PERCENT"	, _TL("Percent"),
		_TL("Reduce the size to this percentage of the original."),
		50, 0, true, 100, true
	);
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
		CSG_PointCloud	*pInput	= pPoints;	pPoints	= Parameters("RESULT")->asPointCloud();

		pPoints->Create(*pInput);

		pPoints->Set_Name(CSG_String::Format("%s_thinned", pInput->Get_Name()));
	}

	//-----------------------------------------------------
	for(int i=0; i<n && Set_Progress(i, n); i++)
	{
		int	j	= (int)(i * d);

		for(int Field=0; Field<pPoints->Get_Field_Count(); Field++)
		{
			if( SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(Field)) )
			{
				pPoints->Set_Value(i, Field, pPoints->Get_Value(j, Field));
			}
			else
			{
				CSG_String	Value;

				pPoints->Get_Value(j, Field, Value);
				pPoints->Set_Value(i, Field, Value);
			}
		}
	}

	pPoints->Set_Record_Count(n);

	//-----------------------------------------------------
	if( pPoints != Parameters("RESULT")->asPointCloud() )
	{
		DataObject_Update(pPoints);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
