
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Projection_GeoTRANS                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   GEOTRANS_Grid.cpp                   //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GEOTRANS_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEOTRANS_Grid::CGEOTRANS_Grid(void)
{
	Set_Name		(_TL("GeoTrans (Grid)"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Coordinate Transformation for Grids. "
		"This library makes use of the Geographic Translator (GeoTrans) library. "
		"The GeoTrans library is maintained by the National Geospatial Agency (NGA)."
	));

	Add_Reference("http://earth-info.nga.mil/GandG/geotrans/");

	//-----------------------------------------------------
	Parameters.Add_Grid("SOURCE_NODE",
		"SOURCE"    , _TL("Source"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice("TARGET_NODE",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(Add_Parameters("TARGET", _TL("Target Grid System"), _TL("")));
	m_Grid_Target.Add_Grid("X", _TL("X Coordinates"), true);
	m_Grid_Target.Add_Grid("Y", _TL("Y Coordinates"), true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGEOTRANS_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGEOTRANS_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Grid::On_Execute_Conversion(void)
{
	CSG_Grid *pSource = Parameters("SOURCE")->asGrid(), *pGrid;

	TSG_Grid_Resampling Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	TSG_Data_Type Type = Resampling == GRID_RESAMPLING_NearestNeighbour ? pSource->Get_Type() : SG_DATATYPE_Float;

	//-----------------------------------------------------
	TSG_Rect Extent;

	if( Get_Target_Extent(pSource, Extent, true) )
	{
		m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), Extent, pSource->Get_NY());
	}

	if( Dlg_Parameters("TARGET") && (pGrid = m_Grid_Target.Get_Grid(Type)) != NULL )
	{
		return( Set_Grid(pSource, pGrid, Resampling) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CGEOTRANS_Grid::Get_MinMax(TSG_Rect &r, double x, double y)
{
	if( Get_Converted(x, y) )
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
bool CGEOTRANS_Grid::Get_Target_Extent(CSG_Grid *pSource, TSG_Rect &Extent, bool bEdge)
{
	if( !pSource )
	{
		return( false );
	}

	int			x, y;

	Extent.xMin	= Extent.yMin	= 1.0;
	Extent.xMax	= Extent.yMax	= 0.0;

	if( bEdge )
	{
		double		d;

		for(y=0, d=pSource->Get_YMin(); y<pSource->Get_NY(); y++, d+=pSource->Get_Cellsize())
		{
			Get_MinMax(Extent, pSource->Get_XMin(), d);
			Get_MinMax(Extent, pSource->Get_XMax(), d);
		}

		for(x=0, d=pSource->Get_XMin(); x<pSource->Get_NX(); x++, d+=pSource->Get_Cellsize())
		{
			Get_MinMax(Extent, d, pSource->Get_YMin());
			Get_MinMax(Extent, d, pSource->Get_YMax());
		}
	}
	else
	{
		TSG_Point	p;

		for(y=0, p.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, p.y+=pSource->Get_Cellsize())
		{
			for(x=0, p.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, p.x+=pSource->Get_Cellsize())
			{
				if( !pSource->is_NoData(x, y) )
				{
					Get_MinMax(Extent, p.x, p.y);
				}
			}
		}
	}

	return( is_Progress() && Extent.xMin < Extent.xMax && Extent.yMin < Extent.yMax );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Grid::Set_Grid(CSG_Grid *pSource, CSG_Grid *pTarget, TSG_Grid_Resampling Resampling)
{
	double		z;

	if( !pSource || !pTarget || !Set_Transformation_Inverse() )
	{
		return( false );
	}

	pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_Value(true));
	pTarget->Set_Scaling(pSource->Get_Scaling(), pSource->Get_Offset());
	pTarget->Set_Name	(pSource->Get_Name());
	pTarget->Set_Unit	(pSource->Get_Unit());

	pTarget->Assign_NoData();

	CSG_Grid *pX = m_Grid_Target.Get_Grid("X");
	CSG_Grid *pY = m_Grid_Target.Get_Grid("Y");

	//-----------------------------------------------------
	for(int y=0; y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++)
	{
		double py = pTarget->Get_YMin() + y * pTarget->Get_Cellsize();

		for(int x=0; x<pTarget->Get_NX(); x++)
		{
			TSG_Point p; p.y = py; p.x = pTarget->Get_XMin() + x * pTarget->Get_Cellsize();

			if( Get_Converted(p) )
			{
				if( pSource->Get_Value(p, z, Resampling) )
				{
					pTarget->Set_Value(x, y, z);
				}

				if( pX ) { pX->Set_Value(x, y, p.x); }
				if( pY ) { pY->Set_Value(x, y, p.y); }
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGEOTRANS_Grid::Set_Shapes(CSG_Grid *pSource, CSG_Shapes *pTarget)
{
	if( !pSource || !pTarget )
	{
		return( false );
	}

	pTarget->Create(SHAPE_TYPE_Point, pSource->Get_Name());
	pTarget->Add_Field("Z", SG_DATATYPE_Double);

	for(int y=0; y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++)
	{
		double py = pSource->Get_YMin() + y * pSource->Get_Cellsize();

		for(int x=0; x<pSource->Get_NX(); x++)
		{
			if( !pSource->is_NoData(x, y) )
			{
				TSG_Point p; p.y = py; p.x = pSource->Get_XMin() + x * pSource->Get_Cellsize();

				if( Get_Converted(p) )
				{
					CSG_Shape *pPoint = pTarget->Add_Shape();
					pPoint->Add_Point(p);
					pPoint->Set_Value(0, pSource->asDouble(x, y));
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
