
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
//            gridding3d_nearest_neighbour.cpp           //
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
#include "gridding3d_nearest_neighbour.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding3D_Nearest_Neighbour::CGridding3D_Nearest_Neighbour(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Nearest Neighbour (3D)"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Nearest neighbour interpolation for 3-dimensional data points. "
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
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding3D_Nearest_Neighbour::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
int CGridding3D_Nearest_Neighbour::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		pParameters->Set_Enabled("Z_FIELD", pParameter->asShapes() && pParameter->asShapes()->Get_Vertex_Type() == SG_VERTEX_TYPE_XY);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding3D_Nearest_Neighbour::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	int	zField	= pPoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XY ? Parameters("Z_FIELD")->asInt() : -1;

	int	Field	= Parameters("V_FIELD")->asInt();

	CSG_Grids	*pGrids	= m_Grid_Target.Get_Grids("GRIDS");

	if( pGrids == NULL )
	{
		return( false );
	}

	pGrids->Fmt_Name("%s.%s [%s]", pPoints->Get_Name(), Parameters("V_FIELD")->asString(), Get_Name().c_str());

	//-----------------------------------------------------
	double	zScale	= Parameters("Z_SCALE")->asDouble();

	if( zScale == 0. )
	{
		Error_Set(_TL("Z factor is zero! Please use 2D instead of 3D interpolation."));

		return( false );
	}

	CSG_KDTree_3D	Search(pPoints, Field, zField, zScale);

	//-----------------------------------------------------
	for(int x=0; x<pGrids->Get_NX() && Set_Progress(x, pGrids->Get_NX()); x++)
	{
		#pragma omp parallel for
		for(int y=0; y<pGrids->Get_NY(); y++)
		{
			double	c[3], d;	size_t	i;

			c[0]	= pGrids->Get_XMin() + x * pGrids->Get_Cellsize();
			c[1]	= pGrids->Get_YMin() + y * pGrids->Get_Cellsize();

			for(int z=0; z<pGrids->Get_NZ(); z++)
			{
				c[2]	= pGrids->Get_Z(z) * zScale;

				if( Search.Get_Nearest_Point(c, i, d) )
				{
					pGrids->Set_Value(x, y, z, Search.Get_Point_Value(i));
				}
				else
				{
					pGrids->Set_NoData(x, y, z);
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
