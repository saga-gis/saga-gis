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

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		"Coordinate transformation for grids.\n"
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	m_Grid_Target.Create(Add_Parameters("TARGET", _TL("Target Grid System"), _TL("")), false);

	if( m_bList )
	{
		pNode	= Parameters.Add_Grid_List(
			NULL	, "SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Grid_List(
			NULL	, "GRIDS"		, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}

	//-----------------------------------------------------
	else
	{
		pNode	= Parameters.Add_Grid(
			NULL	, "SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		m_Grid_Target.Add_Grid("GRID", _TL("Target"), false);
	}

	m_Grid_Target.Add_Grid("OUT_X", _TL("X Coordinates"), true);
	m_Grid_Target.Add_Grid("OUT_Y", _TL("Y Coordinates"), true);

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

	Parameters.Add_Value(
		pNode	, "KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		pNode	, "TARGET_AREA"	, _TL("Use Target Area Polygon"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Transform::On_Parameter_Changed(pParameters, pParameter);

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CCRS_Transform_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CCRS_Transform::On_Parameters_Enable(pParameters, pParameter);

	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
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
		pTargets	= Parameters("GRIDS" )->asGridList();

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

			m_Projector.Set_Inverse(false);

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
	if( pGrid->Get_Projection().is_Okay() && m_Projector.Set_Source(pGrid->Get_Projection())
	&&  Get_Target_System(pGrid->Get_System(), true) )
	{
		TSG_Data_Type	Type	= m_Interpolation == 0 || Parameters("KEEP_TYPE")->asBool() ? pGrid->Get_Type() : SG_DATATYPE_Float;

		return( Transform(pGrid, m_Grid_Target.Get_Grid("GRID", Type)) );
	}

	return( false );
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Parameter_Grid_List *pGrids)
{
	if( pGrids->Get_Count() > 0 && m_Projector.Set_Source(pGrids->asGrid(0)->Get_Projection())
	&&  Get_Target_System(pGrids->asGrid(0)->Get_System(), true) )
	{
		CSG_Grid_System	System(m_Grid_Target.Get_System());

		return( Transform(pGrids, Parameters("GRIDS")->asGridList(), System) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Grid *pGrid, CSG_Grid *pTarget)
{
	if( !m_Projector.Set_Inverse(true) || !pTarget || !pGrid )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pX, *pY;

	if( (pX = m_Grid_Target.Get_Grid("OUT_X")) != NULL )
	{
		pX->Assign_NoData();
		pX->Set_Name(_TL("X Coordinates"));
		pX->Get_Projection().Create(m_Projector.Get_Target());
	}

	if( (pY = m_Grid_Target.Get_Grid("OUT_Y")) != NULL )
	{
		pY->Assign_NoData();
		pY->Set_Name(_TL("Y Coordinates"));
		pY->Get_Projection().Create(m_Projector.Get_Target());
	}

	//-----------------------------------------------------
	Set_Target_Area(pGrid->Get_System(), pTarget->Get_System());

	bool	bGeogCS_Adjust	= m_Projector.Get_Source().Get_Type() == SG_PROJ_TYPE_CS_Geographic && pGrid->Get_XMax() > 180.0;

	//-------------------------------------------------
	pTarget->Set_NoData_Value_Range	(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
	pTarget->Set_Scaling			(pGrid->Get_Scaling(), pGrid->Get_Offset());
	pTarget->Set_Name				(CSG_String::Format(SG_T("%s"), pGrid->Get_Name()));
	pTarget->Set_Unit				(pGrid->Get_Unit());
	pTarget->Assign_NoData();
	pTarget->Get_Projection().Create(m_Projector.Get_Target());

	//-----------------------------------------------------
	for(int y=0; y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++)
	{
		double	yTarget	= pTarget->Get_YMin() + y * pTarget->Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<pTarget->Get_NX(); x++)
		{
			double	z, ySource, xSource	= pTarget->Get_XMin() + x * pTarget->Get_Cellsize();

			if( is_In_Target_Area(x, y) && m_Projector.Get_Projection(xSource, ySource = yTarget) )
			{
				if( pX )	pX->Set_Value(x, y, xSource);
				if( pY )	pY->Set_Value(x, y, ySource);

				if( bGeogCS_Adjust && xSource < 0.0 )
				{
					xSource	+= 360.0;
				}

				if( pGrid->Get_Value(xSource, ySource, z, m_Interpolation) )
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
	if( !m_Projector.Set_Inverse(true) || !pTargets || !pSources || pSources->Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pX, *pY;

	if( (pX = m_Grid_Target.Get_Grid("OUT_X")) != NULL )
	{
		pX->Assign_NoData();
		pX->Set_Name(_TL("X Coordinates"));
		pX->Get_Projection().Create(m_Projector.Get_Target());
	}

	if( (pY = m_Grid_Target.Get_Grid("OUT_Y")) != NULL )
	{
		pY->Assign_NoData();
		pY->Set_Name(_TL("Y Coordinates"));
		pY->Get_Projection().Create(m_Projector.Get_Target());
	}

	//-----------------------------------------------------
	bool	bGeogCS_Adjust	= m_Projector.Get_Source().Get_Type() == SG_PROJ_TYPE_CS_Geographic && pSources->asGrid(0)->Get_System().Get_XMax() > 180.0;

	Set_Target_Area(pSources->asGrid(0)->Get_System(), Target_System);

	//-----------------------------------------------------
	int	i, n	= pTargets->Get_Count();

	for(i=0; i<pSources->Get_Count(); i++)
	{
		CSG_Grid	*pSource	= pSources->asGrid(i);
		CSG_Grid	*pTarget	= SG_Create_Grid(Target_System, m_Interpolation == 0 ? pSource->Get_Type() : SG_DATATYPE_Float);

		if( pTarget )
		{
			pTarget->Set_NoData_Value_Range	(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
			pTarget->Set_Scaling			(pSource->Get_Scaling(), pSource->Get_Offset());
			pTarget->Set_Name				(CSG_String::Format(SG_T("%s"), pSource->Get_Name()));
			pTarget->Set_Unit				(pSource->Get_Unit());
			pTarget->Assign_NoData();
			pTarget->Get_Projection().Create(m_Projector.Get_Target());

			pTargets->Add_Item(pTarget);
		}
	}

	//-------------------------------------------------
	for(int y=0; y<Target_System.Get_NY() && Set_Progress(y, Target_System.Get_NY()); y++)
	{
		double	yTarget	= Target_System.Get_YMin() + y * Target_System.Get_Cellsize();

		#pragma omp parallel for private(i)
		for(int x=0; x<Target_System.Get_NX(); x++)
		{
			double	z, ySource, xSource	= Target_System.Get_XMin() + x * Target_System.Get_Cellsize();

			if( is_In_Target_Area(x, y) && m_Projector.Get_Projection(xSource, ySource = yTarget) )
			{
				if( pX )	pX->Set_Value(x, y, xSource);
				if( pY )	pY->Set_Value(x, y, ySource);

				if( bGeogCS_Adjust && xSource < 0.0 )
				{
					xSource	+= 360.0;
				}

				for(i=0; i<pTargets->Get_Count(); i++)
				{
					if( pSources->asGrid(i)->Get_Value(xSource, ySource, z, m_Interpolation) )
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

	if( !m_Projector.Set_Source(pGrid->Get_Projection()) )
	{
		return( false );
	}

	int			x, y;
	TSG_Point	Point;

	pPoints->Create(SHAPE_TYPE_Point, _TL("Points"));
	pPoints->Get_Projection()	= m_Projector.Get_Target();
	pPoints->Add_Field(pGrid->Get_Name(), pGrid->Get_Type());

	for(y=0, Point.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, Point.y+=pGrid->Get_Cellsize())
	{
		for(x=0, Point.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, Point.x+=pGrid->Get_Cellsize())
		{
			TSG_Point	Point_Transformed	= Point;

			if( !pGrid->is_NoData(x, y) && m_Projector.Get_Projection(Point_Transformed) )
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

	if( !m_Projector.Set_Source(pGrid->Get_Projection()) )
	{
		return( false );
	}

	int			x, y, i;
	TSG_Point	Point;

	pPoints->Create(SHAPE_TYPE_Point, _TL("Points"));
	pPoints->Get_Projection()	= m_Projector.Get_Target();

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		pPoints->Add_Field(pGrids->asGrid(i)->Get_Name(), pGrids->asGrid(i)->Get_Type());
	}

	for(y=0, Point.y=pGrid->Get_YMin(); y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, Point.y+=pGrid->Get_Cellsize())
	{
		for(x=0, Point.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x++, Point.x+=pGrid->Get_Cellsize())
		{
			TSG_Point	Point_Transformed	= Point;

			if( m_Projector.Get_Projection(Point_Transformed) )
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
	if( m_Projector.Get_Projection(x, y) )
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
	int			x, y, Resolution;
	TSG_Rect	Extent;

	Extent.xMin	= Extent.yMin	= 1.0;
	Extent.xMax	= Extent.yMax	= 0.0;

	Get_MinMax(Extent, System.Get_XMin(), System.Get_YMin());
	Get_MinMax(Extent, System.Get_XMax(), System.Get_YMin());
	Get_MinMax(Extent, System.Get_XMin(), System.Get_YMax());
	Get_MinMax(Extent, System.Get_XMax(), System.Get_YMax());

	Resolution	= 256;

	switch( 1 )
	{
	case 1:	// edges
		{
			double	d;

			int	yStep	= 1 + System.Get_NY() / Resolution;

			for(y=0, d=System.Get_YMin(); y<System.Get_NY(); y+=yStep, d+=yStep*System.Get_Cellsize())
			{
				Get_MinMax(Extent, System.Get_XMin(), d);
				Get_MinMax(Extent, System.Get_XMax(), d);
			}

			int	xStep	= 1 + System.Get_NX() / Resolution;

			for(x=0, d=System.Get_XMin(); x<System.Get_NX(); x+=xStep, d+=xStep*System.Get_Cellsize())
			{
				Get_MinMax(Extent, d, System.Get_YMin());
				Get_MinMax(Extent, d, System.Get_YMax());
			}
		}
		break;

	case 2:	// all cells
		{
			TSG_Point	p;

			int	xStep	= 1 + System.Get_NX() / Resolution;
			int	yStep	= 1 + System.Get_NY() / Resolution;

			for(y=0, p.y=System.Get_YMin(); y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y+=yStep, p.y+=yStep*System.Get_Cellsize())
			{
				for(x=0, p.x=System.Get_XMin(); x<System.Get_NX(); x+=xStep, p.x+=xStep*System.Get_Cellsize())
				{
					Get_MinMax(Extent, p.x, p.y);
				}
			}
		}
		break;
	}

	return(	is_Progress() && Extent.xMin < Extent.xMax && Extent.yMin < Extent.yMax
		&&	m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), Extent, System.Get_NY())
		&&  Dlg_Parameters("TARGET") && m_Grid_Target.Get_System().is_Valid()
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::Set_Target_Area(const CSG_Grid_System &Source, const CSG_Grid_System &Target)
{
	if( Parameters("TARGET_AREA")->asBool() == false )
	{
		m_Target_Area.Destroy();

		return( true );
	}

	//-----------------------------------------------------
	CSG_Rect	r(Source.Get_Extent());

	if( m_Projector.Get_Source().Get_Type() == SG_PROJ_TYPE_CS_Geographic )
	{
		if( r.Get_XMax() > 180.0 )	r.Move(-180.0, 0.0);
		if( r.Get_YMin() < -90.0 )	r.m_rect.yMin	= -90.0;
		if( r.Get_YMax() <  90.0 )	r.m_rect.yMax	=  90.0;
	}

	//-----------------------------------------------------
	CSG_Shapes			Area(SHAPE_TYPE_Polygon);
	CSG_Shape_Polygon	*pArea	= (CSG_Shape_Polygon *)Area.Add_Shape();
	TSG_Point			p, q;
	double				dx	= Source.Get_XRange() / 100.0;
	double				dy	= Source.Get_YRange() / 100.0;

	m_Projector.Set_Inverse(false);

	for(p.x=r.Get_XMin(), p.y=r.Get_YMin(); p.y<r.Get_YMax(); p.y+=dy)
	{
		m_Projector.Get_Projection(q = p);	pArea->Add_Point(q);
	}

	for(p.x=r.Get_XMin(), p.y=r.Get_YMax(); p.x<r.Get_XMax(); p.x+=dx)
	{
		m_Projector.Get_Projection(q = p);	pArea->Add_Point(q);
	}

	for(p.x=r.Get_XMax(), p.y=r.Get_YMax(); p.y>r.Get_YMin(); p.y-=dy)
	{
		m_Projector.Get_Projection(q = p);	pArea->Add_Point(q);
	}

	for(p.x=r.Get_XMax(), p.y=r.Get_YMin(); p.x>r.Get_XMin(); p.x-=dx)
	{
		m_Projector.Get_Projection(q = p);	pArea->Add_Point(q);
	}

	m_Projector.Set_Inverse(true);

	//-----------------------------------------------------
	m_Target_Area.Create(Target, SG_DATATYPE_Char);
	m_Target_Area.Set_NoData_Value(0);

	for(int y=0; y<m_Target_Area.Get_NY() && Set_Progress(y, m_Target_Area.Get_NY()); y++)
	{
		double	yWorld	= Target.Get_yGrid_to_World(y);

		#pragma omp parallel for
		for(int x=0; x<m_Target_Area.Get_NX(); x++)
		{
			m_Target_Area.Set_Value(x, y, pArea->Contains(Target.Get_xGrid_to_World(x), yWorld) ? 1 : 0);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
inline bool CCRS_Transform_Grid::is_In_Target_Area(int x, int y)
{
	return( !m_Target_Area.is_Valid() || m_Target_Area.is_InGrid(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
