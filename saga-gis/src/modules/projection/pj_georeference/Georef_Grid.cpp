/**********************************************************
 * Version $Id: Georef_Grid.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    pj_georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Georef_Grid.cpp                    //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "Georef_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Grid::CGeoref_Grid(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Rectify Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2006"));

	Set_Description	(_TW(
		"Georeferencing and rectification for grids. Either choose the attribute fields (x/y) "
		"with the projected coordinates for the reference points (origin) or supply a "
		"additional points layer with correspondend points in the target projection. "
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "REF_SOURCE"	, _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "REF_TARGET"	, _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "XFIELD"		, _TL("x Position"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "YFIELD"		, _TL("y Position"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		GEOREF_METHODS_CHOICE, 0
	);

	Parameters.Add_Value(
		NULL	, "ORDER"		,_TL("Polynomial Order"),
		_TL(""),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("nearest neigbhour"),
			_TL("bilinear"),
			_TL("inverse distance"),
			_TL("bicubic spline"),
			_TL("B-spline")
		), 4
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "TARGET_TYPE"	, _TL("Target"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("user defined"),
			_TL("grid or grid system"),
			_TL("points from grid nodes")
		), 0
	);

	//-----------------------------------------------------
	m_Grid_Target.Add_Parameters_User(Add_Parameters("GET_USER", _TL("User Defined Grid"), _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GET_GRID", _TL("Choose Grid"      ), _TL("")));

	//-----------------------------------------------------
	Add_Parameters("GET_POINTS", _TL("Grid Nodes as Points"), _TL(""))->Add_Shapes(
		NULL, "POINTS", _TL("Grid Nodes"), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoref_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGeoref_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "REF_TARGET") )
	{
		pParameters->Get_Parameter("XFIELD")->Set_Enabled(pParameter->asShapes() == NULL);
		pParameters->Get_Parameter("YFIELD")->Set_Enabled(pParameter->asShapes() == NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Get_Parameter("ORDER")->Set_Enabled(pParameter->asInt() == GEOREF_Polynomial);	// only show for polynomial, user defined order
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TARGET_TYPE") )
	{
		pParameters->Get_Parameter("INTERPOLATION")->Set_Enabled(pParameter->asInt() != 2);	// don't show for points
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pShapes_A	= Parameters("REF_SOURCE")->asShapes();
	CSG_Shapes	*pShapes_B	= Parameters("REF_TARGET")->asShapes();

	int	xField	= Parameters("XFIELD")->asInt();
	int	yField	= Parameters("YFIELD")->asInt();

	//-----------------------------------------------------
	if( ( pShapes_B && m_Engine.Set_Reference(pShapes_A, pShapes_B))
	||	(!pShapes_B && m_Engine.Set_Reference(pShapes_A, xField, yField))	)
	{
		int	Method	= Parameters("METHOD")->asInt();
		int	Order	= Parameters("ORDER" )->asInt();

		if( m_Engine.Evaluate(Method, Order) && Get_Conversion() )
		{
			m_Engine.Destroy();

			return( true );
		}
	}

	//-----------------------------------------------------
	if( !m_Engine.Get_Error().is_Empty() )
	{
		Error_Set(m_Engine.Get_Error());
	}

	m_Engine.Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Get_Conversion(void)
{
	int				Interpolation;
	TSG_Data_Type	Type;
	CSG_Rect		Extent;
	CSG_Grid		*pSource, *pReferenced;
	CSG_Shapes		*pPoints;

	//-----------------------------------------------------
	pSource			= Parameters("GRID")->asGrid();

	pReferenced		= NULL;
	pPoints			= NULL;

	Interpolation	= Parameters("INTERPOLATION")->asInt();
	Type			= Interpolation == 0 ? pSource->Get_Type() : SG_DATATYPE_Float;

	//-----------------------------------------------------
	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// create new user defined grid...
		if( Get_Target_Extent(Extent, true) && m_Grid_Target.Init_User(Extent, pSource->Get_NY()) && Dlg_Parameters("GET_USER") )
		{
			pReferenced	= m_Grid_Target.Get_User(Type);
		}
		break;

	case 1:	// select grid...
		if( Dlg_Parameters("GET_GRID") )
		{
			pReferenced	= m_Grid_Target.Get_Grid(Type);
		}
		break;

	case 2:	// shapes...
		if( Dlg_Parameters("GET_POINTS") )
		{
			pPoints	= Get_Parameters("GET_POINTS")->Get_Parameter("POINTS")->asShapes();

			if( pPoints == DATAOBJECT_NOTSET || pPoints == DATAOBJECT_CREATE )
			{
				Get_Parameters("GET_POINTS")->Get_Parameter("POINTS")->Set_Value(pPoints = SG_Create_Shapes(SHAPE_TYPE_Point));
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( pPoints )
	{
		return( Set_Points(pSource, pPoints) );
	}

	if( pReferenced )
	{
		return( Set_Grid(pSource, pReferenced, Interpolation) );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Get_Target_Extent(CSG_Rect &Extent, bool bEdge)
{
	if( Parameters("METHOD")->asInt() == GEOREF_Triangulation )	// triangulation
	{
		return( m_Engine.Get_Reference_Extent(Extent) );
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	Extent.m_rect.xMin	= Extent.m_rect.yMin	= 1.0;
	Extent.m_rect.xMax	= Extent.m_rect.yMax	= 0.0;

	//-----------------------------------------------------
	if( bEdge )
	{
		for(int y=0; y<pGrid->Get_NY(); y++)
		{
			Add_Target_Extent(Extent, pGrid->Get_XMin(), pGrid->Get_System().Get_yGrid_to_World(y));
			Add_Target_Extent(Extent, pGrid->Get_XMax(), pGrid->Get_System().Get_yGrid_to_World(y));
		}

		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			Add_Target_Extent(Extent, pGrid->Get_System().Get_xGrid_to_World(x), pGrid->Get_YMin());
			Add_Target_Extent(Extent, pGrid->Get_System().Get_xGrid_to_World(x), pGrid->Get_YMax());
		}
	}

	//-----------------------------------------------------
	else
	{
		for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
		{
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				if( !pGrid->is_NoData(x, y) )
				{
					TSG_Point	p	= pGrid->Get_System().Get_Grid_to_World(x, y);

					Add_Target_Extent(Extent, p.x, p.y);
				}
			}
		}
	}

	return( is_Progress() && Extent.Get_XRange() > 0.0 && Extent.Get_YRange() > 0.0 );
}

//---------------------------------------------------------
inline void CGeoref_Grid::Add_Target_Extent(CSG_Rect &Extent, double x, double y)
{
	if( m_Engine.Get_Converted(x, y) )
	{
		if( Extent.Get_XRange() >= 0.0 && Extent.Get_YRange() >= 0.0 )
		{
			Extent.Union(CSG_Point(x, y));
		}
		else
		{
			Extent.Assign(x, y, x, y);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Set_Grid(CSG_Grid *pGrid, CSG_Grid *pReferenced, int Interpolation)
{
	if( !pGrid || !pReferenced || !m_Engine.is_Okay() )
	{
		return( false );
	}

	//-----------------------------------------------------
	pReferenced->Set_Name	(pGrid->Get_Name());
	pReferenced->Set_Unit	(pGrid->Get_Unit());
	pReferenced->Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());
	pReferenced->Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());

	//-----------------------------------------------------
	for(int y=0; y<pReferenced->Get_NY() && Set_Progress(y, pReferenced->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pReferenced->Get_NX(); x++)
		{
			double		z;
			TSG_Point	p	= pReferenced->Get_System().Get_Grid_to_World(x, y);

			if( m_Engine.Get_Converted(p, true) && pGrid->Get_Value(p, z, Interpolation) )
			{
				pReferenced->Set_Value(x, y, z);
			}
			else
			{
				pReferenced->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGeoref_Grid::Set_Points(CSG_Grid *pGrid, CSG_Shapes *pReferenced)
{
	if( !pGrid || !pReferenced || pReferenced->Get_Type() != SHAPE_TYPE_Point || !m_Engine.is_Okay() )
	{
		return( false );
	}

	//-----------------------------------------------------
	pReferenced->Create(SHAPE_TYPE_Point, pGrid->Get_Name());
	pReferenced->Add_Field("Z", SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				TSG_Point	Point	= pGrid->Get_System().Get_Grid_to_World(x, y);

				if( m_Engine.Get_Converted(Point) )
				{
					CSG_Shape	*pPoint	= pReferenced->Add_Shape();

					pPoint->Add_Point(Point);
					pPoint->Set_Value(0, pGrid->asDouble(x, y));
				}
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
