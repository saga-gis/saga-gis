
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  select_3d_points.cpp                 //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "select_3d_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_3D_Points::CSelect_3D_Points(void)
{
	Set_Name		(_TL("3D Points Selection"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Select points with three dimensional coordinates that fall "
		"between a given upper and lower surface, both provided as grids. "
	));

	Parameters.Add_Grid("",
		"LOWER"		, _TL("Lower Surface"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"UPPER"		, _TL("Upper Surface"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"Z_FIELD"	, _TL("Z"),
		_TL("")
	);

	Parameters.Add_Shapes("",
		"COPY"		, _TL("Copy Selection"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSelect_3D_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		pParameters->Set_Enabled("Z_FIELD", pParameter->asShapes() && pParameter->asShapes()->Get_Vertex_Type() == SG_VERTEX_TYPE_XY);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_3D_Points::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( !pPoints->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	int	zField	= pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XY ? Parameters("Z_FIELD")->asInt() : -1;

	//-----------------------------------------------------
	CSG_Grid	*pLower	= Parameters("LOWER")->asGrid();
	CSG_Grid	*pUpper	= Parameters("UPPER")->asGrid();

	CSG_Shapes	*pCopy	= Parameters("COPY")->asShapes();

	if( pCopy )
	{
		pCopy->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("Selection")),
			pPoints, pPoints->Get_Vertex_Type()
		);
	}

	//-----------------------------------------------------
	pPoints->Select();	// unselect everything...

	for(int i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		int	x, y;	CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( Get_System().Get_World_to_Grid(x, y, pPoint->Get_Point(0))
		&&  !pLower->is_NoData(x, y) && !pUpper->is_NoData(x, y) )
		{
			double	z	= pPoint->Get_Z(0);

			if( pLower->asDouble(x, y) <= z && z < pUpper->asDouble(x, y) )
			{
				pPoints->Select(i, true);

				if( pCopy )
				{
					pCopy->Add_Shape(pPoint);
				}
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pPoints);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
