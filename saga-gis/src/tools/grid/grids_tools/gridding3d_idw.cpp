
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                      grids_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   gridding3d_idw.cpp                  //
//                                                       //
//                  Olaf Conrad (C) 2019                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gridding3d_idw.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding3D_IDW::CGridding3D_IDW(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Inverse Distance Weighted (3D)"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Inverse distance weighted interpolation for 3-dimensional data points. "
		"Output will be a grid collection with evenly spaced Z-levels "
		"representing the 3rd dimension. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"Z_FIELD"	, _TL("Z"),
		_TL("")
	);

	Parameters.Add_Double("POINTS",
		"Z_SCALE"	, _TL("Z Factor"),
		_TL(""),
		1.
	);

	Parameters.Add_Table_Field("POINTS",
		"V_FIELD"	, _TL("Value"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_"); 

	m_Grid_Target.Add_Grids("GRIDS", _TL("Grid Collection"), false, true);

	//-----------------------------------------------------
	m_Search_Options.Create(&Parameters, "NODE_SEARCH", 1);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting (SG_DISTWGHT_IDW);
	m_Weighting.Set_IDW_Offset(false);
	m_Weighting.Set_IDW_Power (2.0);

	m_Weighting.Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding3D_IDW::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	if( pParameter->Cmp_Identifier("POINTS") || pParameter->Cmp_Identifier("Z_FIELD") )
	{
		CSG_Shapes	*pPoints = (*pParameters)("POINTS")->asShapes();

		if( pPoints )
		{
			int	zField	= pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XY ? (*pParameters)("Z_FIELD")->asInt() : -1;

			m_Grid_Target.Set_User_Defined_ZLevels(pParameters,
				zField < 0 ? pPoints->Get_ZMin() : pPoints->Get_Minimum(zField),
				zField < 0 ? pPoints->Get_ZMax() : pPoints->Get_Maximum(zField), 10
			);
		}
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGridding3D_IDW::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		pParameters->Set_Enabled("Z_FIELD", pParameter->asShapes() && pParameter->asShapes()->Get_Vertex_Type() == SG_VERTEX_TYPE_XY);
	}

	m_Grid_Target   .On_Parameters_Enable(pParameters, pParameter);

	m_Search_Options.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(pParameters);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding3D_IDW::On_Execute(void)
{
	m_pPoints	= Parameters("POINTS")->asShapes();

	m_Field		= Parameters("V_FIELD")->asInt();

	CSG_Grids	*pGrids	= m_Grid_Target.Get_Grids("GRIDS");

	if( pGrids == NULL )
	{
		return( false );
	}

	pGrids->Fmt_Name("%s.%s [%s]", m_pPoints->Get_Name(), Parameters("V_FIELD")->asString(), Get_Name().c_str());

	//-----------------------------------------------------
	m_zField	= m_pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XY ? Parameters("Z_FIELD")->asInt() : -1;

	m_zScale	= Parameters("Z_SCALE")->asDouble();

	if( m_zScale == 0. )
	{
		Error_Set(_TL("Z factor is zero! Please use 2D instead of 3D interpolation."));

		return( false );
	}

	if( !m_Search_Options.Do_Use_All(true) && !m_Search.Create(m_pPoints, m_zField, m_zScale) )
	{
		Error_Set(_TL("failed to initialize search engine"));

		return( false );
	}

	//-----------------------------------------------------
	for(int x=0; x<pGrids->Get_NX() && Set_Progress(x, pGrids->Get_NX()); x++)
	{
		#pragma omp parallel for
		for(int y=0; y<pGrids->Get_NY(); y++)
		{
			double	c[3], v;

			c[0]	= pGrids->Get_XMin() + x * pGrids->Get_Cellsize();
			c[1]	= pGrids->Get_YMin() + y * pGrids->Get_Cellsize();

			for(int z=0; z<pGrids->Get_NZ(); z++)
			{
				c[2]	= pGrids->Get_Z(z) * m_zScale;

				if( Get_Value(c, v) )
				{
					pGrids->Set_Value(x, y, z, v);
				}
				else
				{
					pGrids->Set_NoData(x, y, z);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Search.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding3D_IDW::Get_Value(double Coordinate[3], double &Value)
{
	CSG_Array_Int	Index;	CSG_Vector	Distance;

	if( m_Search.is_Okay() )
	{
		if( m_Search.Get_Nearest_Points(
			Coordinate, m_Search_Options.Get_Max_Points(), m_Search_Options.Get_Radius(), Index, Distance
		) < m_Search_Options.Get_Min_Points() )
		{
			return( false );
		}
	}

	CSG_Simple_Statistics	s;

	int	nPoints	= m_Search.is_Okay() ? (int)Index.Get_Size() : m_pPoints->Get_Count();

	for(int i=0; i<nPoints; i++)
	{
		CSG_Shape	*pPoint	= m_pPoints->Get_Shape(m_Search.is_Okay() ? Index[i] : i);

		double	d	= m_Search.is_Okay() ? Distance[i] : Get_Distance(Coordinate, pPoint);

		if( d > 0. )
		{
			s.Add_Value(pPoint->asDouble(m_Field), m_Weighting.Get_Weight(d));
		}
		else	// d == 0! there is a point at the requested coordinate!
		{
			s.Create();	s	+= pPoint->asDouble(m_Field);

			for(++i; i<nPoints; i++)	// is there more than one point?!
			{
				if( Distance[i] <= 0. )
				{
					s	+= pPoint->asDouble(m_Field);
				}
			}

			break;
		}
	}

	Value	= s.Get_Mean();

	return( true );
}

//---------------------------------------------------------
inline double CGridding3D_IDW::Get_Distance(double Coordinate[3], CSG_Shape *pPoint)
{
	double	dx	= Coordinate[0] - pPoint->Get_Point(0).x;
	double	dy	= Coordinate[1] - pPoint->Get_Point(0).y;
	double	dz	= Coordinate[2] - m_zScale * (m_zField < 0 ? pPoint->Get_Z(0) : pPoint->asDouble(m_zField));

	return( sqrt(dx*dx + dy*dy + dz*dz) );
}

//---------------------------------------------------------
inline bool CGridding3D_IDW::is_Identical(double Coordinate[3], CSG_Shape *pPoint)
{
	return( Coordinate[0] == pPoint->Get_Point(0).x
		&&  Coordinate[1] == pPoint->Get_Point(0).y
		&&  Coordinate[2] == (m_zField < 0 ? pPoint->Get_Z(0) : pPoint->asDouble(m_zField))
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
