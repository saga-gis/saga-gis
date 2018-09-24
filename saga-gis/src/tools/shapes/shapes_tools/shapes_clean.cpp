/**********************************************************
 * Version $Id: shapes_clean.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   shapes_clean.cpp                    //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "shapes_clean.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Clean::CShapes_Clean(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Remove Invalid Shapes"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool deletes geometrically invalid elements from a shapes layer."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Clean::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Clean::On_Execute(void)
{
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	int	n	= pShapes->Get_Count();

	//-----------------------------------------------------
	for(int i=n-1; i>=0 && Set_Progress(n-1-i, n); i--)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(i);

		if( !pShape->is_Valid() )
		{
			pShapes->Del_Shape(i);
		}
		else switch( pShapes->Get_Type() )
		{
		default:
			break;

		case SHAPE_TYPE_Polygon:
			if( ((CSG_Shape_Polygon *)pShape)->Get_Area() <= 0.0 )
			{
				pShapes->Del_Shape(i);
			}
			break;
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("Number of removed shapes"), n - pShapes->Get_Count());

	if( n - pShapes->Get_Count() > 0 )
	{
		DataObject_Update(pShapes);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
