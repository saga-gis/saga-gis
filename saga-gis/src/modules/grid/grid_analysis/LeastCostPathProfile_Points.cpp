///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            LeastCostPathProfile_Points.cpp            //
//                                                       //
//              Copyright (C) 2004-2010 by               //
//      Olaf Conrad, Victor Olaya & Volker Wichmann      //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
#include "LeastCostPathProfile_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VALUE_OFFSET	5


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLeastCostPathProfile_Points::CLeastCostPathProfile_Points(void)
{
	Set_Name(_TL("Least Cost Paths"));

	Set_Author(SG_T("O. Conrad, V. Olaya, V. Wichmann (c) 2004-2010"));

	Parameters.Set_Description(_TW(
		"This module allows to compute least cost path profile(s). It takes an "
		"accumulated cost surface grid and a point shapefile as input. Each "
		"point in the shapefile represents a source for which the least cost path "
		"is calculated.\n"
		"In case the point shapefile has more than one source point "
		"defined, a separate least cost path is calculated for each point. "
		"The module outputs a point and a line shapefile for each least cost path.\n "
		"The module allows for optional input grids. The cell values of these grids "
		"along the least cost path are written to the outputs as additional table fields.\n\n")
	);

	Parameters.Add_Shapes(
		NULL, "SOURCE"	,
		_TL("Source Point(s)"),
		_TL("Point shapefile with source point(s)"),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL, "DEM"		, 
		_TL("Accumulated cost"),
		_TL("Accumulated cost surface"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, 
		"VALUES", 
		_TL("Values"),
		_TL("Allows to write cell values from additional grids to the output"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes_List(
		NULL, 
		"POINTS", 
		_TL("Profile (points)"),
		_TL("Least cost path profile points"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes_List(
		NULL, 
		"LINE", 
		_TL("Profile (lines)"),
		_TL("Least cost path profile line"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}

//---------------------------------------------------------
CLeastCostPathProfile_Points::~CLeastCostPathProfile_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile_Points::On_Execute(void)
{
	int							x, y;
	CSG_Shape					*pShape;
	CSG_Shapes					*pSources;
	CSG_Shapes					*pPoints, *pLine;
	CSG_Parameter_Shapes_List	*pShapesPoints, *pShapesLine;

	pSources		= Parameters("SOURCE")	->asShapes();
	m_pDEM			= Parameters("DEM")		->asGrid();
	m_pValues		= Parameters("VALUES")	->asGridList();
	pShapesPoints	= Parameters("POINTS")	->asShapesList();
	pShapesLine		= Parameters("LINE")	->asShapesList();

	//-----------------------------------------------------
	pShapesPoints	->Del_Items();
	pShapesLine		->Del_Items();


	//-----------------------------------------------------
	for( int i=0; i<pSources->Get_Count(); i++ )
	{
		pShape		= pSources->Get_Shape(i);

		if( Get_System()->Get_World_to_Grid(x, y, pShape->Get_Point(0)) && m_pDEM->is_InGrid(x, y) )
		{

			//-----------------------------------------------------
			pPoints			= SG_Create_Shapes(SHAPE_TYPE_Point, CSG_String::Format(_TL("Profile_Points_[%s]_%d"), m_pDEM->Get_Name(), i+1));
			pPoints			->Add_Field("ID"	, SG_DATATYPE_Int);
			pPoints			->Add_Field("D"		, SG_DATATYPE_Double);
			pPoints			->Add_Field("X"		, SG_DATATYPE_Double);
			pPoints			->Add_Field("Y"		, SG_DATATYPE_Double);
			pPoints			->Add_Field("Z"		, SG_DATATYPE_Double);

			for( int j=0; j<m_pValues->Get_Count(); j++)
			{
				pPoints->Add_Field(m_pValues->asGrid(j)->Get_Name(), SG_DATATYPE_Double);
			}

			//-----------------------------------------------------
			pLine			= SG_Create_Shapes(SHAPE_TYPE_Line, CSG_String::Format(_TL("Profile_Line_[%s]_%d"), m_pDEM->Get_Name(), i+1));
			pLine			->Add_Field("ID", SG_DATATYPE_Int);
			pLine			->Add_Shape()->Set_Value(0, 1);


			Set_Profile(x, y, pPoints, pLine);

			//-----------------------------------------------------
			if( pPoints->Get_Count() > 0 )
			{
				pShapesPoints->Add_Item(pPoints);
				pShapesLine->Add_Item(pLine);
			}
			else
			{
				delete(pPoints);
				delete(pLine);
			}
		}
	}

	return( pShapesPoints->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CLeastCostPathProfile_Points::Set_Profile(int iX, int iY, CSG_Shapes *pPoints, CSG_Shapes *pLine)
{
	int		iNextX, iNextY;

	iNextX	= iX;
	iNextY	= iY;
	do
	{
		iX	= iNextX;
		iY	= iNextY;
		getNextCell(m_pDEM, iX, iY, iNextX, iNextY);
	}
	while( Add_Point(iX, iY, pPoints, pLine) && (iX != iNextX || iY != iNextY) );
}


//---------------------------------------------------------
void CLeastCostPathProfile_Points::getNextCell(CSG_Grid *g,	int iX,	int iY,	int &iNextX, int &iNextY)
{
    float	fMaxSlope	= 0;
    float	fSlope		= 0;

    if( iX<1 || iX>=g->Get_NX()-1 || iY<1 || iY>=g->Get_NY()-1 || g->is_NoData(iX,iY) )
	{
        iNextX = iX;
		iNextY = iY;
		return;
    }

    for( int i=-1; i<2; i++ )
	{
        for( int j=-1; j<2; j++)
		{                	
            if( !g->is_NoData(iX+i,iY+j) )
			{
				fSlope = g->asFloat(iX+i,iY+j) - g->asFloat(iX,iY);                                				
				if( fSlope <= fMaxSlope )
				{
					iNextX		= iX + i;
					iNextY		= iY + j;                        
                    fMaxSlope	= fSlope;
                }
            }                    
        }
    }

	return;
}

//---------------------------------------------------------
bool CLeastCostPathProfile_Points::Add_Point(int x, int y, CSG_Shapes *pPoints, CSG_Shapes *pLine)
{
	int			i;
	double		Distance;
	TSG_Point	Point;
	CSG_Shape	*pPoint, *pLast;

	if( m_pDEM->is_InGrid(x, y) )
	{
		Point	= Get_System()->Get_Grid_to_World(x, y);

		if( pPoints->Get_Count() == 0 )
		{
			Distance	= 0.0;
		}
		else
		{
			pLast		= pPoints->Get_Shape(pPoints->Get_Count() - 1);
			Distance	= SG_Get_Distance(Point, pLast->Get_Point(0));
			Distance	+= pLast->asDouble(1);
		}

		pPoint	= pPoints->Add_Shape();
		pPoint->Add_Point(Point);

		pPoint->Set_Value(0, pPoints->Get_Count());
		pPoint->Set_Value(1, Distance);
		pPoint->Set_Value(2, Point.x);
		pPoint->Set_Value(3, Point.y);
		pPoint->Set_Value(4, m_pDEM->asDouble(x, y));

		for( i=0; i<m_pValues->Get_Count(); i++ )
		{
			pPoint->Set_Value(VALUE_OFFSET + i, m_pValues->asGrid(i)->asDouble(x, y, true));
		}

		pLine->Get_Shape(0)->Add_Point(Point);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
