
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 crs_transform_grid.cpp                //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "crs_transform_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Transform_Grid::CCRS_Transform_Grid(bool bList)
{
	CSG_Parameter	*pNode;

	m_bList	= bList;

	//-----------------------------------------------------
	Set_Name		(m_bList
		? _TL("Coordinate Transformation (Grid List)")
		: _TL("Coordinate Transformation (Grid)")
	);

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"Coordinate transformation for grids.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4 Homepage</a>\n"
	));

	//-----------------------------------------------------
	if( m_bList )
	{
		pNode	= Parameters.Add_Grid_List(
			NULL	, "SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Grid_List(
			NULL	, "TARGET"		, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);

		m_Grid_Target.Add_Parameters_User  (Add_Parameters("GET_USER"  , _TL("User Defined Grid System"), _TL("")), false);
		m_Grid_Target.Add_Parameters_System(Add_Parameters("GET_SYSTEM", _TL("Select Grid System")      , _TL("")));
	}

	//-----------------------------------------------------
	else
	{
		pNode	= Parameters.Add_Grid(
			NULL	, "SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		m_Grid_Target.Add_Parameters_User  (Add_Parameters("GET_USER"  , _TL("User Defined Grid"), _TL("")));
		m_Grid_Target.Add_Parameters_Grid  (Add_Parameters("GET_GRID"  , _TL("Select Grid")      , _TL("")));
	}

	//-----------------------------------------------------
	Parameters.Add_Value(
		pNode	, "CREATE_XY"	, _TL("Create X/Y Grids"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid_Output(NULL, "OUT_X", _TL("X Coordinates"), _TL(""));
	Parameters.Add_Grid_Output(NULL, "OUT_Y", _TL("Y Coordinates"), _TL(""));

	//-----------------------------------------------------
	Parameters.Add_Choice(
		pNode	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neigbhor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		pNode	, "TARGET_TYPE"	, _TL("Target"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("user defined grid system"),
			_TL("existing grid system"),
			_TL("points")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "TARGET_AREA"	, _TL("Use Target Area Polygon"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("POINTS", _TL("Points"), _TL(""));

	pParameters->Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Transform::On_Parameter_Changed(pParameters, pParameter);

	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::On_Execute_Transformation(void)
{
	m_Interpolation	= Parameters("INTERPOLATION")->asInt();

	//-----------------------------------------------------
	if( m_bList )
	{
		int						i;
		CSG_Parameters			Grids;
		CSG_Parameter_Grid_List	*pSources, *pTargets, *pGrids;

		pSources	= Parameters("SOURCE")->asGridList();
		pTargets	= Parameters("TARGET")->asGridList();

		pTargets->Del_Items();

		pGrids		= Grids.Add_Grid_List(NULL, "GRD", SG_T(""), SG_T(""), PARAMETER_INPUT, false)->asGridList();

		for(i=0; i<pSources->Get_Count(); i++)
		{
			if( pSources->asGrid(i)->Get_Projection().is_Okay() )
			{
				pGrids->Add_Item(pSources->asGrid(i));
			}
			else
			{
				Error_Set(CSG_String::Format(SG_T("%s: %s\n"), _TL("unknown projection"), pSources->asGrid(i)->Get_Name()));
			}
		}

		pSources	= Grids.Add_Grid_List(NULL, "SRC", SG_T(""), SG_T(""), PARAMETER_INPUT, false)->asGridList();

		while( pGrids->Get_Count() > 0 )
		{
			pSources->Add_Item(pGrids->asGrid(pGrids->Get_Count() - 1));
			pGrids  ->Del_Item(pGrids->Get_Count() - 1);

			for(i=pGrids->Get_Count()-1; i>=0; i--)
			{
				if( pGrids->asGrid(i)->Get_Projection() == pSources->asGrid(0)->Get_Projection() )
				{
					pSources->Add_Item(pGrids->asGrid(i));
					pGrids  ->Del_Item(i);
				}
			}

			Set_Inverse(false);

			Transform(pSources);

			pSources->Del_Items();
		}

		return( pTargets->Get_Count() > 0 );
	}

	//-----------------------------------------------------
	else
	{
		return( Transform(Parameters("SOURCE")->asGrid()) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Grid *pGrid)
{
	if( pGrid->Get_Projection().is_Okay() && Set_Source(pGrid->Get_Projection()) )
	{
		TSG_Data_Type	Type	= m_Interpolation == 0 ? pGrid->Get_Type() : SG_DATATYPE_Float;

		switch( Parameters("TARGET_TYPE")->asInt() )
		{
		case 0:	// create user defined grid...
			if( Get_Target_System(pGrid->Get_System(), true) )
			{
				return( Transform(pGrid, m_Grid_Target.Get_User(Type)) );
			}
			break;

		case 1:	// select existing grid system...
			if( Dlg_Parameters("GET_GRID") )
			{
				return( Transform(pGrid, m_Grid_Target.Get_Grid(Type)) );
			}
			break;

		case 2:	// points as target...
			if( Dlg_Parameters("POINTS") )
			{
				CSG_Shapes	*pPoints	= Get_Parameters("POINTS")->Get_Parameter("POINTS")->asShapes();

				if( pPoints == DATAOBJECT_NOTSET || pPoints == DATAOBJECT_CREATE )
				{
					Get_Parameters("POINTS")->Get_Parameter("POINTS")->Set_Value(pPoints = SG_Create_Shapes(SHAPE_TYPE_Point));
				}

				return( Transform(pGrid, pPoints) );
			}
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Parameter_Grid_List *pGrids)
{
	if( pGrids->Get_Count() > 0 && Set_Source(pGrids->asGrid(0)->Get_Projection()) )
	{
		CSG_Grid_System	System;

		switch( Parameters("TARGET_TYPE")->asInt() )
		{
		case 0:	// create user defined grid system...
			if( Get_Target_System(pGrids->asGrid(0)->Get_System(), true) && m_Grid_Target.Get_System_User(System) )
			{
				return( Transform(pGrids, Parameters("TARGET")->asGridList(), System) );
			}
			break;

		case 1:	// select existing grid system...
			if( Dlg_Parameters("GET_SYSTEM") && m_Grid_Target.Get_System(System) )
			{
				return( Transform(pGrids, Parameters("TARGET")->asGridList(), System) );
			}
			break;

		case 2:	// points as target...
			if( Dlg_Parameters("POINTS") )
			{
				CSG_Shapes	*pPoints	= Get_Parameters("POINTS")->Get_Parameter("POINTS")->asShapes();

				if( pPoints == DATAOBJECT_NOTSET || pPoints == DATAOBJECT_CREATE )
				{
					Get_Parameters("POINTS")->Get_Parameter("POINTS")->Set_Value(pPoints = SG_Create_Shapes());
				}

				return( Transform(pGrids, pPoints) );
			}
			break;
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Grid *pGrid, CSG_Grid *pTarget)
{
	if( !pTarget || !pGrid )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool			bGeogCS_Adjust	= pGrid->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Geographic && pGrid->Get_XMax() > 180.0;
	int				x, y;
	double			z;
	TSG_Point		Pt_Source, Pt_Target;
	CSG_Grid		*pX, *pY;

	//-----------------------------------------------------
	Set_Target_Area(pGrid->Get_System(), pGrid->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Geographic);

	if( !Set_Inverse(true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("CREATE_XY")->asBool() )
	{
		Parameters("OUT_X")->Set_Value(pX	= SG_Create_Grid(pTarget->Get_System(), SG_DATATYPE_Float));
		pX->Assign_NoData();
		pX->Set_Name(_TL("X-Coordinate"));
		pX->Get_Projection().Create(Get_Target());

		Parameters("OUT_Y")->Set_Value(pY	= SG_Create_Grid(pTarget->Get_System(), SG_DATATYPE_Float));
		pY->Assign_NoData();
		pY->Set_Name(_TL("Y-Coordinate"));
		pY->Get_Projection().Create(Get_Target());
	}
	else
	{
		pX	= pY	= NULL;
	}

	//-------------------------------------------------
	pTarget->Set_NoData_Value_Range	(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
	pTarget->Set_ZFactor			(pGrid->Get_ZFactor());
	pTarget->Set_Name				(CSG_String::Format(SG_T("%s"), pGrid->Get_Name()));
	pTarget->Set_Unit				(pGrid->Get_Unit());
	pTarget->Assign_NoData();
	pTarget->Get_Projection().Create(Get_Target());

	//-----------------------------------------------------
	for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
	{
		for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
		{
			if( is_In_Target_Area(Pt_Target) && Get_Transformation(Pt_Source = Pt_Target) )
			{
				if( pX )	pX->Set_Value(x, y, Pt_Source.x);
				if( pY )	pY->Set_Value(x, y, Pt_Source.y);

				if( bGeogCS_Adjust && Pt_Source.x < 0.0 )
				{
					Pt_Source.x	+= 360.0;
				}

				if( pGrid->Get_Value(Pt_Source, z, m_Interpolation) )
				{
					pTarget->Set_Value(x, y, z);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Target_Area.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Parameter_Grid_List *pSources, CSG_Parameter_Grid_List *pTargets, const CSG_Grid_System &Target_System)
{
	if( !pTargets || !pSources || pSources->Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool			bGeogCS_Adjust	= pSources->asGrid(0)->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Geographic && pSources->asGrid(0)->Get_XMax() > 180.0;
	int				x, y, i, n;
	double			z;
	TSG_Point		Pt_Source, Pt_Target;
	CSG_Grid		*pX, *pY;

	//-----------------------------------------------------
	Set_Target_Area(pSources->asGrid(0)->Get_System(), pSources->asGrid(0)->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Geographic);

	if( !Set_Inverse(true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("CREATE_XY")->asBool() )
	{
		Parameters("OUT_X")->Set_Value(pX	= SG_Create_Grid(Target_System, SG_DATATYPE_Float));
		pX->Assign_NoData();
		pX->Set_Name(_TL("X-Coordinate"));
		pX->Get_Projection().Create(Get_Target());

		Parameters("OUT_Y")->Set_Value(pY	= SG_Create_Grid(Target_System, SG_DATATYPE_Float));
		pY->Assign_NoData();
		pY->Set_Name(_TL("Y-Coordinate"));
		pY->Get_Projection().Create(Get_Target());
	}
	else
	{
		pX	= pY	= NULL;
	}

	//-----------------------------------------------------
	n	= pTargets->Get_Count();

	for(i=0; i<pSources->Get_Count(); i++)
	{
		CSG_Grid	*pSource	= pSources->asGrid(i);
		CSG_Grid	*pTarget	= SG_Create_Grid(Target_System, m_Interpolation == 0 ? pSource->Get_Type() : SG_DATATYPE_Float);

		pTarget->Set_NoData_Value_Range	(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_ZFactor			(pSource->Get_ZFactor());
		pTarget->Set_Name				(CSG_String::Format(SG_T("%s"), pSource->Get_Name()));
		pTarget->Set_Unit				(pSource->Get_Unit());
		pTarget->Assign_NoData();
		pTarget->Get_Projection().Create(Get_Target());

		pTargets->Add_Item(pTarget);
	}

	//-------------------------------------------------
	for(y=0, Pt_Target.y=Target_System.Get_YMin(); y<Target_System.Get_NY() && Set_Progress(y, Target_System.Get_NY()); y++, Pt_Target.y+=Target_System.Get_Cellsize())
	{
		for(x=0, Pt_Target.x=Target_System.Get_XMin(); x<Target_System.Get_NX(); x++, Pt_Target.x+=Target_System.Get_Cellsize())
		{
			if( is_In_Target_Area(Pt_Target) && Get_Transformation(Pt_Source = Pt_Target) )
			{
				if( pX )	pX->Set_Value(x, y, Pt_Source.x);
				if( pY )	pY->Set_Value(x, y, Pt_Source.y);

				if( bGeogCS_Adjust && Pt_Source.x < 0.0 )
				{
					Pt_Source.x	+= 360.0;
				}

				for(i=0; i<pSources->Get_Count(); i++)
				{
					if( pSources->asGrid(i)->Get_Value(Pt_Source, z, m_Interpolation) )
					{
						pTargets->asGrid(n + i)->Set_Value(x, y, z);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Target_Area.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Grid *pGrid, CSG_Shapes *pPoints)
{
	if( !pPoints || !pGrid )
	{
		return( false );
	}

	if( !Set_Source(pGrid->Get_Projection()) )
	{
		return( false );
	}

	int			x, y;
	TSG_Point	Point;

	pPoints->Create(SHAPE_TYPE_Point, _TL("Points"));
	pPoints->Get_Projection()	= Get_Target();
	pPoints->Add_Field(pGrid->Get_Name(), pGrid->Get_Type());

	for(y=0, Point.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, Point.y+=pGrid->Get_Cellsize())
	{
		for(x=0, Point.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, Point.x+=pGrid->Get_Cellsize())
		{
			TSG_Point	Point_Transformed	= Point;

			if( !pGrid->is_NoData(x, y) && Get_Transformation(Point_Transformed) )
			{
				CSG_Shape	*pPoint	= pPoints->Add_Shape();

				pPoint->Add_Point(Point_Transformed);

				pPoint->Set_Value(0, pGrid->asDouble(x, y));
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pPoints)
{
	if( !pPoints || !pGrids || pGrids->Get_Count() < 1 )
	{
		return( false );
	}

	CSG_Grid	*pGrid	= pGrids->asGrid(0);

	if( !Set_Source(pGrid->Get_Projection()) )
	{
		return( false );
	}

	int			x, y, i;
	TSG_Point	Point;

	pPoints->Create(SHAPE_TYPE_Point, _TL("Points"));
	pPoints->Get_Projection()	= Get_Target();

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		pPoints->Add_Field(pGrids->asGrid(i)->Get_Name(), pGrids->asGrid(i)->Get_Type());
	}

	for(y=0, Point.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, Point.y+=pGrid->Get_Cellsize())
	{
		for(x=0, Point.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, Point.x+=pGrid->Get_Cellsize())
		{
			TSG_Point	Point_Transformed	= Point;

			if( Get_Transformation(Point_Transformed) )
			{
				CSG_Shape	*pPoint	= pPoints->Add_Shape();

				pPoint->Add_Point(Point_Transformed);

				for(i=0; i<pGrids->Get_Count(); i++)
				{
					if( !pGrids->asGrid(i)->is_NoData(x, y) )
					{
						pPoint->Set_Value(i, pGrids->asGrid(i)->asDouble(x, y));
					}
					else
					{
						pPoint->Set_NoData(i);
					}
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CCRS_Transform_Grid::Get_MinMax(TSG_Rect &r, double x, double y)
{
	if( Get_Transformation(x, y) )
	{
		if( r.xMin > r.xMax )
		{
			r.xMin	= r.xMax	= x;
		}
		else if( r.xMin > x )
		{
			r.xMin	= x;
		}
		else if( r.xMax < x )
		{
			r.xMax	= x;
		}

		if( r.yMin > r.yMax )
		{
			r.yMin	= r.yMax	= y;
		}
		else if( r.yMin > y )
		{
			r.yMin	= y;
		}
		else if( r.yMax < y )
		{
			r.yMax	= y;
		}
	}
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::Get_Target_System(const CSG_Grid_System &System, bool bEdge)
{
	int			x, y;
	TSG_Rect	Extent;

	Extent.xMin	= Extent.yMin	= 1.0;
	Extent.xMax	= Extent.yMax	= 0.0;

	if( !bEdge )
	{
		double		d;

		for(y=0, d=System.Get_YMin(); y<System.Get_NY(); y++, d+=System.Get_Cellsize())
		{
			Get_MinMax(Extent, System.Get_XMin(), d);
			Get_MinMax(Extent, System.Get_XMax(), d);
		}

		for(x=0, d=System.Get_XMin(); x<System.Get_NX(); x++, d+=System.Get_Cellsize())
		{
			Get_MinMax(Extent, d, System.Get_YMin());
			Get_MinMax(Extent, d, System.Get_YMax());
		}
	}
	else
	{
		TSG_Point	p;

		for(y=0, p.y=System.Get_YMin(); y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++, p.y+=System.Get_Cellsize())
		{
			for(x=0, p.x=System.Get_XMin(); x<System.Get_NX(); x++, p.x+=System.Get_Cellsize())
			{
				Get_MinMax(Extent, p.x, p.y);
			}
		}

		if( !is_Progress() )
		{
			return( false );
		}
	}

	return(	Extent.xMin < Extent.xMax && Extent.yMin < Extent.yMax
		&&	m_Grid_Target.Init_User(Extent, System.Get_NY())
		&&	Dlg_Parameters("GET_USER")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::Set_Target_Area(const CSG_Grid_System &System, bool bGeogCS)
{
	if( Parameters("TARGET_AREA")->asBool() == false )
	{
		m_Target_Area.Destroy();

		return( true );
	}

	m_Target_Area.Create(SHAPE_TYPE_Polygon);

	CSG_Shape	*pArea	= m_Target_Area.Add_Shape();
	CSG_Rect	r		= System.Get_Extent();
	TSG_Point	p, q;

	if( bGeogCS )
	{
		if( r.Get_XMax() > 180.0 )	r.Move(-180.0, 0.0);
		if( r.Get_YMin() < -90.0 )	r.m_rect.yMin	= -90.0;
		if( r.Get_YMax() <  90.0 )	r.m_rect.yMax	=  90.0;
	}

	for(p.x=r.Get_XMin(), p.y=r.Get_YMin(); p.y<r.Get_YMax(); p.y+=System.Get_Cellsize())
	{
		Get_Transformation(q = p);	pArea->Add_Point(q);
	}

	for(p.x=r.Get_XMin(), p.y=r.Get_YMax(); p.x<r.Get_XMax(); p.x+=System.Get_Cellsize())
	{
		Get_Transformation(q = p);	pArea->Add_Point(q);
	}

	for(p.x=r.Get_XMax(), p.y=r.Get_YMax(); p.y>r.Get_YMin(); p.y-=System.Get_Cellsize())
	{
		Get_Transformation(q = p);	pArea->Add_Point(q);
	}

	for(p.x=r.Get_XMax(), p.y=r.Get_YMin(); p.x>r.Get_XMin(); p.x-=System.Get_Cellsize())
	{
		Get_Transformation(q = p);	pArea->Add_Point(q);
	}

	return( true );
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::is_In_Target_Area(const TSG_Point &Point)
{
	if( m_Target_Area.Get_Count() == 1 )
	{
		return( ((CSG_Shape_Polygon *)m_Target_Area.Get_Shape(0))->is_Containing(Point) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
