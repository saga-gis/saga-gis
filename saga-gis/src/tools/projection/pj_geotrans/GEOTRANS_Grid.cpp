/**********************************************************
 * Version $Id: GEOTRANS_Grid.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
#include "GEOTRANS_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEOTRANS_Grid::CGEOTRANS_Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("GeoTrans (Grid)"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Coordinate Transformation for Grids. "
		"This library makes use of the Geographic Translator (GeoTrans) library. "
		"The GeoTrans library is maintained by the National Geospatial Agency (NGA)."
	));

	Add_Reference("http://earth-info.nga.mil/GandG/geotrans/");

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"OUT_GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Grid_Output("",
		"OUT_X"		, _TL("X Coordinates"),
		_TL("")
	);

	Parameters.Add_Grid_Output("",
		"OUT_Y"		, _TL("Y Coordinates"),
		_TL("")
	);

	Parameters.Add_Shapes_Output("",
		"OUT_SHAPES", _TL("Shapes"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("SOURCE_NODE",
		"SOURCE"	, _TL("Source"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Bool("TARGET_NODE",
		"CREATE_XY"	, _TL("Create X/Y Grids"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("TARGET_NODE",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(Add_Parameters("TARGET", _TL("Target Grid System"), _TL("")));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGEOTRANS_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGEOTRANS_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Grid::On_Execute_Conversion(void)
{
	TSG_Data_Type	Type;
	TSG_Rect		Extent;
	CSG_Grid		*pSource, *pGrid;

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline         ;	break;
	}

	//-----------------------------------------------------
	pSource	= Parameters("SOURCE")->asGrid();
	Type	= Resampling == GRID_RESAMPLING_NearestNeighbour ? pSource->Get_Type() : SG_DATATYPE_Float;

	//-----------------------------------------------------
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
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGEOTRANS_Grid::Set_Grid(CSG_Grid *pSource, CSG_Grid *pTarget, TSG_Grid_Resampling Resampling)
{
	int			x, y;
	double		z;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Grid	*pX, *pY;

	if( pSource && pTarget && Set_Transformation_Inverse() )
	{
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_Scaling(pSource->Get_Scaling(), pSource->Get_Offset());
		pTarget->Set_Name	(pSource->Get_Name());
		pTarget->Set_Unit	(pSource->Get_Unit());

		pTarget->Assign_NoData();

		if( Parameters("CREATE_XY")->asBool() )
		{
			pX	= SG_Create_Grid(pTarget->Get_System(), SG_DATATYPE_Float);
			pX->Assign_NoData();
			pX->Set_Name(_TL("X-Coordinate"));
			Parameters("OUT_X")->Set_Value(pX);

			pY	= SG_Create_Grid(pTarget->Get_System(), SG_DATATYPE_Float);
			pY->Assign_NoData();
			pY->Set_Name(_TL("Y-Coordinate"));
			Parameters("OUT_Y")->Set_Value(pY);
		}
		else
		{
			pX	= pY	= NULL;
		}

		//-------------------------------------------------
		for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
		{
			for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( Get_Converted(Pt_Source) )
				{
					if( pSource->Get_Value(Pt_Source, z, Resampling) )
					{
						pTarget->Set_Value(x, y, z);
					}

					if( pX && pY )
					{
						pX->Set_Value(x, y, Pt_Source.x);
						pY->Set_Value(x, y, Pt_Source.y);
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGEOTRANS_Grid::Set_Shapes(CSG_Grid *pSource, CSG_Shapes *pTarget)
{
	int			x, y;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Shape		*pShape;

	if( pSource && pTarget )
	{
		pTarget->Create(SHAPE_TYPE_Point, pSource->Get_Name());
		pTarget->Add_Field("Z", SG_DATATYPE_Double);

		for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=pSource->Get_Cellsize())
		{
			for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
			{
				if( !pSource->is_NoData(x, y) )
				{
					Pt_Target	= Pt_Source;

					if( Get_Converted(Pt_Target) )
					{
						pShape		= pTarget->Add_Shape();
						pShape->Add_Point(Pt_Target);
						pShape->Set_Value(0, pSource->asDouble(x, y));
					}
				}
			}
		}

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
