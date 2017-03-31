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
	m_bList	= bList;

	//-----------------------------------------------------
	Set_Name		(CSG_String::Format("%s (%s)", _TL("Coordinate Transformation"),
		bList ? _TL("Grid List") : _TL("Grid")
	));

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		"Coordinate transformation for grids."
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"SOURCE_NODE"	, _TL("Source"),
		_TL("")
	);

	if( m_bList )
	{
		Parameters.Add_Grid_List("SOURCE_NODE",
			"SOURCE"	, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);
	}
	else
	{
		Parameters.Add_Grid("SOURCE_NODE",
			"SOURCE"	, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);
	}

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"TARGET_NODE"	, _TL("Target"),
		_TL("")
	);

	Parameters.Add_Choice("TARGET_NODE",
		"RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("TARGET_NODE",
		"KEEP_TYPE"		, _TL("Preserve Data Type"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("TARGET_NODE",
		"TARGET_AREA"	, _TL("Use Target Area Polygon"),
		_TL("Restricts targeted grid cells to area of the projected bounding rectangle. Useful with certain projections for global data."),
		false
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, Parameters("TARGET_NODE"), "TARGET_");

	if( m_bList )
	{
		Parameters.Add_Grid_List("TARGET_NODE",
			"GRIDS"		, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}
	else
	{
		m_Grid_Target.Add_Grid("GRID", _TL("Target"), false);
	}

	m_Grid_Target.Add_Grid("OUT_X", _TL("X Coordinates"), true);
	m_Grid_Target.Add_Grid("OUT_Y", _TL("Y Coordinates"), true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Transform_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	int	Result	= CCRS_Transform::On_Parameter_Changed(pParameters, pParameter);

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_METHOD")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_PROJ4")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_DIALOG")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_GRID")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_SHAPES")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_FILE")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG_AUTH")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG_GEOGCS")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG_PROJCS")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "SOURCE") )
	{
		Set_Target_System(pParameters);
	}
	else
	{
		m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);
	}

	return( Result );
}

//---------------------------------------------------------
int CCRS_Transform_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "RESAMPLING") )
	{
		pParameters->Set_Enabled("KEEP_TYPE", pParameter->asInt() != 0);	// nearest neighbour
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CCRS_Transform::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform_Grid::On_Execute_Transformation(void)
{
	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	m_Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	m_Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
	case  2:	m_Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
	case  3:	m_Resampling	= GRID_RESAMPLING_BSpline         ;	break;
	}

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

		for(i=pSources->Get_Count()-1; i>=0; i--)
		{
			if( pSources->asGrid(i)->Get_Projection().is_Okay() )
			{
				pGrids->Add_Item(pSources->asGrid(i));
			}
			else
			{
				Error_Fmt("%s: %s\n", _TL("unknown projection"), pSources->asGrid(i)->Get_Name());
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
	if( m_Grid_Target.Get_System().is_Valid() && pGrid
	&&  m_Projector.Set_Source(pGrid->Get_Projection()) )
	{
		TSG_Data_Type	Type	= m_Resampling == GRID_RESAMPLING_NearestNeighbour || Parameters("KEEP_TYPE")->asBool() ? pGrid->Get_Type() : SG_DATATYPE_Float;

		return( Transform(pGrid, m_Grid_Target.Get_Grid("GRID", Type)) );
	}

	return( false );
}

//---------------------------------------------------------
bool CCRS_Transform_Grid::Transform(CSG_Parameter_Grid_List *pGrids)
{
	if( m_Grid_Target.Get_System().is_Valid() && pGrids->Get_Count() > 0
	&&  m_Projector.Set_Source(pGrids->asGrid(0)->Get_Projection()) )
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
	pTarget->Set_NoData_Value_Range (pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
	pTarget->Set_Scaling            (pGrid->Get_Scaling(), pGrid->Get_Offset());
	pTarget->Set_Name               (pGrid->Get_Name());
	pTarget->Set_Unit               (pGrid->Get_Unit());
	pTarget->Get_Projection().Create(m_Projector.Get_Target());
	pTarget->Assign_NoData();

//	CSG_Parameters Parms; if( DataObject_Get_Parameters(pGrid, Parms) ) { DataObject_Add(pTarget); DataObject_Set_Parameters(pTarget, Parms); }

	//-----------------------------------------------------
	for(int y=0; y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++)
	{
		double	yTarget	= pTarget->Get_YMin() + y * pTarget->Get_Cellsize();

	//	#pragma omp parallel for
		for(int x=0; x<pTarget->Get_NX(); x++)
		{
			double	z, ySource, xSource	= pTarget->Get_XMin() + x * pTarget->Get_Cellsize();

			if( is_In_Target_Area(x, y) && m_Projector.Get_Projection(xSource, ySource = yTarget) )
			{
				if( bGeogCS_Adjust )
				{
					if( xSource < 0.0 )
					{
						xSource	+= 360.0;
					}
					else if( xSource >= 360.0 )
					{
						xSource	-= 360.0;
					}
				}

				if( pX ) pX->Set_Value(x, y, xSource);
				if( pY ) pY->Set_Value(x, y, ySource);

				if( pGrid->Get_Value(xSource, ySource, z, m_Resampling, false, true) )
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

	bool	bKeepType	= m_Resampling == GRID_RESAMPLING_NearestNeighbour || Parameters("KEEP_TYPE")->asBool();

	//-----------------------------------------------------
	int	i, n	= pTargets->Get_Count();

	for(i=0; i<pSources->Get_Count(); i++)
	{
		CSG_Grid	*pSource	= pSources->asGrid(i);
		CSG_Grid	*pTarget	= SG_Create_Grid(Target_System, bKeepType ? pSource->Get_Type() : SG_DATATYPE_Float);

		if( pTarget )
		{
			pTargets->Add_Item(pTarget);

			pTarget->Set_NoData_Value_Range (pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
			pTarget->Set_Scaling            (pSource->Get_Scaling(), pSource->Get_Offset());
			pTarget->Set_Name               (pSource->Get_Name());
			pTarget->Set_Unit               (pSource->Get_Unit());
			pTarget->Get_Projection().Create(m_Projector.Get_Target());
			pTarget->Assign_NoData();

			CSG_Parameters Parms; if( DataObject_Get_Parameters(pSource, Parms) ) { DataObject_Add(pTarget); DataObject_Set_Parameters(pTarget, Parms); }
		}
	}

	//-------------------------------------------------
	for(int y=0; y<Target_System.Get_NY() && Set_Progress(y, Target_System.Get_NY()); y++)
	{
		double	yTarget	= Target_System.Get_YMin() + y * Target_System.Get_Cellsize();

	//	#pragma omp parallel for private(i)
		for(int x=0; x<Target_System.Get_NX(); x++)
		{
			double	z, ySource, xSource	= Target_System.Get_XMin() + x * Target_System.Get_Cellsize();

			if( is_In_Target_Area(x, y) && m_Projector.Get_Projection(xSource, ySource = yTarget) )
			{
				if( bGeogCS_Adjust )
				{
					if( xSource < 0.0 )
					{
						xSource	+= 360.0;
					}
					else if( xSource >= 360.0 )
					{
						xSource	-= 360.0;
					}
				}

				if( pX ) pX->Set_Value(x, y, xSource);
				if( pY ) pY->Set_Value(x, y, ySource);

				for(i=0; i<pSources->Get_Count(); i++)
				{
					if( pSources->asGrid(i)->Get_Value(xSource, ySource, z, m_Resampling, false, true) )
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
bool CCRS_Transform_Grid::Set_Target_System(CSG_Parameters *pParameters, int Resolution, bool bEdges)
{
	if( !pParameters || !pParameters->Get_Parameter("SOURCE") || !pParameters->Get_Parameter("CRS_PROJ4") )
	{
		return( false );
	}

	CSG_Grid	*pGrid	= m_bList
		? pParameters->Get_Parameter("SOURCE")->asGridList()->asGrid(0)
		: pParameters->Get_Parameter("SOURCE")->asGrid();

	if( !pGrid || !pGrid->is_Valid() || !pGrid->Get_Projection().is_Okay()
	||  !m_Projector.Set_Target(CSG_Projection(pParameters->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4))
	||  !m_Projector.Get_Target().is_Okay()
	||  !m_Projector.Set_Source(pGrid->Get_Projection()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			x, y;
	TSG_Rect	Extent;

	Extent.xMin	= Extent.yMin	= 1.0;
	Extent.xMax	= Extent.yMax	= 0.0;

	Get_MinMax(Extent, pGrid->Get_XMin(), pGrid->Get_YMin());
	Get_MinMax(Extent, pGrid->Get_XMax(), pGrid->Get_YMin());
	Get_MinMax(Extent, pGrid->Get_XMin(), pGrid->Get_YMax());
	Get_MinMax(Extent, pGrid->Get_XMax(), pGrid->Get_YMax());

	//-----------------------------------------------------
	if( bEdges )	// edges
	{
		double	d;

		int	yStep	= 1 + pGrid->Get_NY() / Resolution;

		for(y=0, d=pGrid->Get_YMin(); y<pGrid->Get_NY(); y+=yStep, d+=yStep*pGrid->Get_Cellsize())
		{
			Get_MinMax(Extent, pGrid->Get_XMin(), d);
			Get_MinMax(Extent, pGrid->Get_XMax(), d);
		}

		int	xStep	= 1 + pGrid->Get_NX() / Resolution;

		for(x=0, d=pGrid->Get_XMin(); x<pGrid->Get_NX(); x+=xStep, d+=xStep*pGrid->Get_Cellsize())
		{
			Get_MinMax(Extent, d, pGrid->Get_YMin());
			Get_MinMax(Extent, d, pGrid->Get_YMax());
		}
	}

	//-----------------------------------------------------
	else			// all cells
	{
		TSG_Point	p;

		int	xStep	= 1 + pGrid->Get_NX() / Resolution;
		int	yStep	= 1 + pGrid->Get_NY() / Resolution;

		for(y=0, p.y=pGrid->Get_YMin(); y<pGrid->Get_NY(); y+=yStep, p.y+=yStep*pGrid->Get_Cellsize())
		{
			for(x=0, p.x=pGrid->Get_XMin(); x<pGrid->Get_NX(); x+=xStep, p.x+=xStep*pGrid->Get_Cellsize())
			{
				Get_MinMax(Extent, p.x, p.y);
			}
		}
	}

	return(	Extent.xMin < Extent.xMax && Extent.yMin < Extent.yMax
		&&	m_Grid_Target.Set_User_Defined(pParameters, Extent, pGrid->Get_NY())
		&&  m_Grid_Target.Get_System().is_Valid()
	);
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
