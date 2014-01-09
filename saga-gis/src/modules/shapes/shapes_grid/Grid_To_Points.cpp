/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_To_Points.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Grid_To_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_To_Points::CGrid_To_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Values to Points"));

	Set_Author		(SG_T("O.Conrad (c) 2001"));

	Set_Description	(_TW(
		"This module saves grid values to point (grid nodes) or polygon (grid cells) shapes. Optionally only points "
		"can be saved, which are contained by polygons of the specified shapes layer. "
		"In addition, it is possible to exclude all cells that are coded NoData in the "
		"first grid of the grid list."
	));


	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "NODATA"		, _TL("Exclude NoData Cells"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("nodes"),
			_TL("cells")
		)
	);
}

//---------------------------------------------------------
CGrid_To_Points::~CGrid_To_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Points::On_Execute(void)
{
	bool					bZFactor, bNoNoData;
	int						x, y, iGrid, iPoint, Type;
	double					xPos, yPos;
	CSG_Grid				*pGrid;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Shape				*pShape;
	CSG_Shapes				*pShapes, *pPolygons;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	pPolygons	= Parameters("POLYGONS")->asShapes();
	pShapes		= Parameters("SHAPES")	->asShapes();
	bNoNoData	= Parameters("NODATA")	->asBool();
	Type		= Parameters("TYPE")	->asInt();

	bZFactor	= true;

	//-----------------------------------------------------
	if( pGrids->Get_Count() > 0 )
	{
		switch( Type )
		{
		case 0:	pShapes->Create(SHAPE_TYPE_Point  , _TL("Grid Values [Nodes]"));	break;
		case 1:	pShapes->Create(SHAPE_TYPE_Polygon, _TL("Grid Values [Cells]"));	break;
		}

		pShapes->Add_Field("ID"	, SG_DATATYPE_Int);
		pShapes->Add_Field("X"	, SG_DATATYPE_Double);
		pShapes->Add_Field("Y"	, SG_DATATYPE_Double);

		for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			pShapes->Add_Field(CSG_String::Format(SG_T("%s"),pGrids->asGrid(iGrid)->Get_Name()).BeforeFirst(SG_Char('.')).c_str(), SG_DATATYPE_Double);
		}

		//-------------------------------------------------
		for(y=0, yPos=Get_YMin() - (Type ? 0.5 * Get_Cellsize() : 0.0), iPoint=0; y<Get_NY() && Set_Progress(y); y++, yPos+=Get_Cellsize())
		{
			for(x=0, xPos=Get_XMin() - (Type ? 0.5 * Get_Cellsize() : 0.0); x<Get_NX(); x++, xPos+=Get_Cellsize())
			{
				if( (!bNoNoData || (bNoNoData && !pGrids->asGrid(0)->is_NoData(x, y)))
				&&	(!pPolygons || is_Contained(xPos, yPos, pPolygons)) )
				{
					pShape	= pShapes->Add_Shape();

					switch( Type )
					{
					case 0:
						pShape->Add_Point(xPos, yPos);
						break;

					case 1:
						pShape->Add_Point(xPos                 , yPos                 );
						pShape->Add_Point(xPos + Get_Cellsize(), yPos                 );
						pShape->Add_Point(xPos + Get_Cellsize(), yPos + Get_Cellsize());
						pShape->Add_Point(xPos                 , yPos + Get_Cellsize());
						break;
					}

					pShape->Set_Value(0, ++iPoint);
					pShape->Set_Value(1, xPos);
					pShape->Set_Value(2, yPos);

					for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
					{
						pGrid	= pGrids->asGrid(iGrid);

						pShape->Set_Value(iGrid + 3, pGrid->is_NoData(x, y) ? -99999 : pGrid->asDouble(x, y, bZFactor));
					}
				}
			}
		}

		return( pShapes->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CGrid_To_Points::is_Contained(double x, double y, CSG_Shapes *pPolygons)
{
	if( pPolygons && pPolygons->Get_Type() == SHAPE_TYPE_Polygon )
	{
		for(int iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape_Polygon *pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

			if( pPolygon->Contains(x, y) )
			{
				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
