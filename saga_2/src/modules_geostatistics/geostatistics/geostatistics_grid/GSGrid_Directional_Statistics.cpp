
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            GSGrid_Directional_Statistics.cpp          //
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
#include "GSGrid_Directional_Statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Directional_Statistics::CGSGrid_Directional_Statistics(void)
{
	Set_Name		(_TL("Directional Statistics for Single Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Calculates for each cell statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) of all cells lying in given direction based on the input grid. "
	));


	Parameters.Add_Grid(
		NULL, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "MEAN"		, _TL("Arithmetic Mean"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "MIN"			, _TL("Minimum"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "MAX"			, _TL("Maximum"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "VAR"			, _TL("Variance"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STDDEV"		, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STDDEVLO"	, _TL("Mean less Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STDDEVHI"	, _TL("Mean plus Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "DIRECTION"	, _TL("Direction [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	Parameters.Add_Value(
		NULL, "TOLERANCE"	, _TL("Tolerance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 45.0, true
	);

	Parameters.Add_Value(
		NULL, "MAXDISTANCE"	, _TL("Maximum Distance [Cellsize]"),
		_TL("Maximum distance parameter is ignored if set to zero (default)."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL, "WEIGHTING"	, _TL("Distance Weighting"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("no distance weighting"),
			_TL("inverse distance to a power"),
			_TL("exponential")
		), 1
	);

	Parameters.Add_Value(
		NULL, "IDW_POWER"	, _TL("Inverse Distance Weighting Power"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Directional_Statistics::On_Execute(void)
{
	CSG_Grid	*pMean, *pMin, *pMax, *pVar, *pStdDev, *pStdDevLo, *pStdDevHi;

	//-----------------------------------------------------
	m_pGrid		= Parameters("GRID")		->asGrid();

	pMean		= Parameters("MEAN")		->asGrid();
	pMin		= Parameters("MIN")			->asGrid();
	pMax		= Parameters("MAX")			->asGrid();
	pVar		= Parameters("VAR")			->asGrid();
	pStdDev		= Parameters("STDDEV")		->asGrid();
	pStdDevLo	= Parameters("STDDEVLO")	->asGrid();
	pStdDevHi	= Parameters("STDDEVHI")	->asGrid();

	//-----------------------------------------------------
	if( !m_pGrid || (!pMean && !pMin && !pMax && !pVar && !pStdDev && !pStdDevLo && !pStdDevHi) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Set_Cells() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( Get_Statistics(x, y) )
			{
				if( pMean     )	pMean		->Set_Value(x, y, m_Statistics.Get_Mean());
				if( pMin      )	pMin		->Set_Value(x, y, m_Statistics.Get_Minimum());
				if( pMax      )	pMax		->Set_Value(x, y, m_Statistics.Get_Maximum());
				if( pVar      )	pVar		->Set_Value(x, y, m_Statistics.Get_Variance());
				if( pStdDev   )	pStdDev		->Set_Value(x, y, m_Statistics.Get_StdDev());
				if( pStdDevLo )	pStdDevLo	->Set_Value(x, y, m_Statistics.Get_Mean() - m_Statistics.Get_StdDev());
				if( pStdDevHi )	pStdDevHi	->Set_Value(x, y, m_Statistics.Get_Mean() + m_Statistics.Get_StdDev());
			}
			else
			{
				if( pMean     )	pMean		->Set_NoData(x, y);
				if( pMin      )	pMin		->Set_NoData(x, y);
				if( pMax      )	pMax		->Set_NoData(x, y);
				if( pVar      )	pVar		->Set_NoData(x, y);
				if( pStdDev   )	pStdDev		->Set_NoData(x, y);
				if( pStdDevLo )	pStdDevLo	->Set_NoData(x, y);
				if( pStdDevHi )	pStdDevHi	->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Directional_Statistics::Get_Statistics(int x, int y)
{
	if( m_pGrid->is_NoData(x, y) )
	{
		return( false );
	}

	int		i, ix, iy;
	double	d, w;

	m_Statistics.Invalidate();

	for(i=0; i<m_Cells.Get_Count(); i++)
	{
		if( Get_Cell(i, x, y, ix, iy, d, w) )
		{
			m_Statistics.Add_Value(m_pGrid->asDouble(ix, iy), w);
		}
	}

	return( m_Statistics.Get_Weights() > 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CGSGrid_Directional_Statistics::Get_Cell(int Index, int xOffset, int yOffset, int &x, int &y, double &d, double &w)
{
	if( Index >= 0 && Index < m_Cells.Get_Count() )
	{
		CSG_Table_Record	*pRecord	= m_Cells.Get_Record_byIndex(Index);

		x	= xOffset + pRecord->asInt(0);
		y	= yOffset + pRecord->asInt(1);

		d	= pRecord->asDouble(2);
		w	= pRecord->asDouble(3);

		return( m_pGrid->is_InGrid(x, y) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Directional_Statistics::Set_Cells(void)
{
	int			Weighting;
	double		Direction, Tolerance, maxDistance, IDW_Power;

	Weighting	= Parameters("WEIGHTING")	->asInt();
	IDW_Power	= Parameters("IDW_POWER")	->asDouble();
	maxDistance	= Parameters("MAXDISTANCE")	->asDouble();
	Direction	= Parameters("DIRECTION")	->asDouble() * M_DEG_TO_RAD;
	Tolerance	= Parameters("TOLERANCE")	->asDouble() * M_DEG_TO_RAD;

	Direction	= fmod(Direction, M_PI_360);	if( Direction < 0.0 )	Direction	+= M_PI_360;

	//-----------------------------------------------------
	m_Cells.Destroy();
	m_Cells.Add_Field(SG_T("X")	, SG_DATATYPE_Int);
	m_Cells.Add_Field(SG_T("Y")	, SG_DATATYPE_Int);
	m_Cells.Add_Field(SG_T("D")	, SG_DATATYPE_Double);
	m_Cells.Add_Field(SG_T("W")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	TSG_Point			a, b;
	CSG_Shapes			Polygons(SHAPE_TYPE_Polygon);	Polygons.Add_Field(SG_T("ID"), SG_DATATYPE_Int);
	CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)Polygons.Add_Shape();

	if( Direction < M_PI_090 )
	{
		a.x	= -0.5;	b.x	=  0.5;
		a.y	=  0.5;	b.y	= -0.5;
	}
	else if( Direction < M_PI_180 )
	{
		a.x	=  0.5;	b.x	= -0.5;
		a.y	=  0.5;	b.y	= -0.5;
	}
	else if( Direction < M_PI_270 )
	{
		a.x	=  0.5;	b.x	= -0.5;
		a.y	= -0.5;	b.y	=  0.5;
	}
	else // if( Direction < M_PI_360 )
	{
		a.x	= -0.5;	b.x	=  0.5;
		a.y	= -0.5;	b.y	=  0.5;
	}

	double	d	= 2.0 * SG_Get_Length(Get_NX(), Get_NY());

	pPolygon->Add_Point(b.x, b.y);
	pPolygon->Add_Point(a.x, a.y);
	pPolygon->Add_Point(a.x + d * sin(Direction - Tolerance), a.y + d * cos(Direction - Tolerance));
	pPolygon->Add_Point(      d * sin(Direction)            ,       d * cos(Direction));
	pPolygon->Add_Point(b.x + d * sin(Direction + Tolerance), a.y + d * cos(Direction + Tolerance));

	//-----------------------------------------------------
	for(int y=-Get_NY(); y<Get_NY(); y++)
	{
		for(int x=-Get_NX(); x<Get_NX(); x++)
		{
			if( (x || y) && ((d = SG_Get_Length(x, y)) < maxDistance || maxDistance <= 0.0)  && pPolygon->is_Containing(x, y) )
			{
				CSG_Table_Record	*pRecord	= m_Cells.Add_Record();

				pRecord->Set_Value(0, x);
				pRecord->Set_Value(1, y);
				pRecord->Set_Value(2, d);

				switch( Weighting )
				{
				case 0:	default:	// no weighting
					pRecord->Set_Value(3, 1.0);
					break;

				case 1:				// inverse distance to a power
					pRecord->Set_Value(3, pow(d, -IDW_Power));
					break;

				case 2:				// exponential
					pRecord->Set_Value(3, exp(-d));
					break;
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Cells.Get_Count() > 0 )
	{
		m_Cells.Set_Index(2, TABLE_INDEX_Ascending);

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
