
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Rectify Grid"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Georeferencing and rectification for grids. Either choose the attribute fields (x/y) "
		"with the projected coordinates for the reference points (origin) or supply an "
		"additional points layer with correspondend points in the target projection. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"REF_SOURCE", _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"REF_TARGET", _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("REF_SOURCE",
		"XFIELD"	, _TL("x Position"),
		_TL("")
	);

	Parameters.Add_Table_Field("REF_SOURCE",
		"YFIELD"	, _TL("y Position"),
		_TL("")
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		GEOREF_METHODS_CHOICE, 0
	);

	Parameters.Add_Int("",
		"ORDER"		, _TL("Polynomial Order"),
		_TL(""),
		3, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("",
		"BYTEWISE"	, _TL("Bytewise Interpolation"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(Add_Parameters("TARGET", _TL("Target Grid System"), _TL("")), true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoref_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("REF_SOURCE") && pParameter->asShapes() )
	{
		if( pParameter->asShapes()->Get_Field("X_MAP") >= 0 )
		{
			pParameters->Get_Parameter("XFIELD")->Set_Value(pParameter->asShapes()->Get_Field("X_MAP"));
		}

		if( pParameter->asShapes()->Get_Field("Y_MAP") >= 0 )
		{
			pParameters->Get_Parameter("YFIELD")->Set_Value(pParameter->asShapes()->Get_Field("Y_MAP"));
		}
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGeoref_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("REF_TARGET") )
	{
		pParameters->Set_Enabled("XFIELD", pParameter->asShapes() == NULL);
		pParameters->Set_Enabled("YFIELD", pParameter->asShapes() == NULL);
	}

	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("ORDER", pParameter->asInt() == GEOREF_Polynomial);	// only show for polynomial, user defined order
	}

	if( pParameter->Cmp_Identifier("RESAMPLING") )
	{
		pParameters->Set_Enabled("BYTEWISE", pParameter->asInt() > 0);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::On_Execute(void)
{
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Get_Conversion(void)
{
	//-----------------------------------------------------
	CSG_Rect	Extent;

	CSG_Grid	*pSource	= Parameters("GRID")->asGrid();

	//-----------------------------------------------------
	if( !Get_Target_Extent(Extent, true) ||  !m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), Extent, pSource->Get_NY()) )
	{
		Error_Set(_TL("failed to estimate target extent"));

		return( false );
	}

	if( !Dlg_Parameters("TARGET") )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	CSG_Grid	*pReferenced	= m_Grid_Target.Get_Grid(Resampling == GRID_RESAMPLING_NearestNeighbour ? pSource->Get_Type() : SG_DATATYPE_Float);

	if( !pReferenced )
	{
		Error_Set(_TL("failed to initialize target grid"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Set_Grid(pSource, pReferenced, Resampling) )
	{
		Error_Set(_TL("failed to project target grid"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameters  P;

	if( DataObject_Get_Parameters(pSource, P) )
	{
		DataObject_Add(pReferenced);

		DataObject_Set_Parameters(pReferenced, P);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Set_Grid(CSG_Grid *pGrid, CSG_Grid *pReferenced, TSG_Grid_Resampling Resampling)
{
	if( !pGrid || !pReferenced || !m_Engine.is_Okay() )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool	bBytewise   = Parameters("BYTEWISE")->asBool();

	//-----------------------------------------------------
	pReferenced->Set_Name	(pGrid->Get_Name());
	pReferenced->Set_Unit	(pGrid->Get_Unit());
	pReferenced->Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());
	pReferenced->Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());

	pReferenced->Assign_NoData();

	//-----------------------------------------------------
	for(int y=0; y<pReferenced->Get_NY() && Set_Progress(y, pReferenced->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pReferenced->Get_NX(); x++)
		{
			double		z;
			TSG_Point	p	= pReferenced->Get_System().Get_Grid_to_World(x, y);

			if( m_Engine.Get_Converted(p, true) && pGrid->Get_Value(p, z, Resampling, false, bBytewise) )
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
