/**********************************************************
 * Version $Id: grid_metric_conversion.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               grid_metric_conversion.cpp              //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include "grid_metric_conversion.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Metric_Conversion::CGrid_Metric_Conversion(void)
{
	Set_Name		(_TL("Metric Conversions"));

	Set_Author		(SG_T("O. Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONV"		, _TL("Converted Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "CONVERSION"	, _TL("Conversion"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("radians to degree"),
			_TL("degree to radians"),
			_TL("Celsius to Fahrenheit"),
			_TL("Fahrenheit to Celsius")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Metric_Conversion::On_Execute(void)
{
	int			Conversion;
	CSG_Grid	*pGrid, *pConv;

	//-----------------------------------------------------
	pGrid		= Parameters("GRID")		->asGrid();
	pConv		= Parameters("CONV")		->asGrid();
	Conversion	= Parameters("CONVERSION")	->asInt();

	switch( Conversion )
	{
	case  0:	pConv->Set_Unit(SG_T("\xbo"));		break;	// radians to degree
	case  1:	pConv->Set_Unit(SG_T("\xbo"));		break;	// degree to radians
	case  2:	pConv->Set_Unit(SG_T("\xboF"));		break;	// Celsius to Fahrenheit
	case  3:	pConv->Set_Unit(SG_T("\xboC"));		break;	// Fahrenheit to Celsius
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
			{
				pConv->Set_NoData(x, y);
			}
			else
			{
				double	z	= pGrid->asDouble(x, y);

				switch( Conversion )
				{
				case  0:	z	= z * M_RAD_TO_DEG;					break;	// radians to degree
				case  1:	z	= z * M_DEG_TO_RAD;					break;	// degree to radians
				case  2:	z	= z * 1.8 + 32.0;					break;	// Celsius to Fahrenheit
				case  3:	z	= (z - 32.0) / 1.8;					break;	// Fahrenheit to Celsius
				}

				pConv->Set_Value(x, y, z);
			}
		}
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
