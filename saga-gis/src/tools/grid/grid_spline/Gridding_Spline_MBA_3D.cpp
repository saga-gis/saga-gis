
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Gridding_Spline_MBA_3D.cpp              //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Gridding_Spline_MBA_3D.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_MBA_3D::CGridding_Spline_MBA_3D(void)
{
	Set_Name		(_TL("Multilevel B-Spline (3D)"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Multilevel B-spline algorithm for spatial interpolation of scattered data "
		"as proposed by Lee, Wolberg and Shin (1997) modified for 3D data.\n"
		"The algorithm makes use of a coarse-to-fine hierarchy of control lattices to "
		"generate a sequence of bicubic B-spline functions, whose sum approaches the "
		"desired interpolation function. Performance gains are realized by using "
		"B-spline refinement to reduce the sum of these functions into one equivalent "
		"B-spline function. "
		"\n\n"
		"The 'Maximum Level' determines the maximum size of the final B-spline matrix "
		"and increases exponential with each level. Where level=10 requires about 1mb "
		"level=12 needs about 16mb and level=14 about 256mb(!) of additional memory. "
	));

	Add_Reference(
		"Lee, S., Wolberg, G., Shin, S.Y.", "1997",
		"Scattered Data Interpolation with Multilevel B-Splines",
		"IEEE Transactions On Visualisation And Computer Graphics, Vol.3, No.3., p.228-244.",
		SG_T("https://www.researchgate.net/profile/George_Wolberg/publication/3410822_Scattered_Data_Interpolation_with_Multilevel_B-Splines/links/00b49518719ac9f08a000000/Scattered-Data-Interpolation-with-Multilevel-B-Splines.pdf"),
		SG_T("ResearchGate")
	);

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

	Parameters.Add_Double("Z_FIELD",
		"Z_SCALE"	, _TL("Z Factor"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Table_Field("POINTS",
		"V_FIELD"	, _TL("Value"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_"); 

	m_Grid_Target.Add_Grids("GRIDS", _TL("Grid Collection"), false);

	Parameters.Add_Node("", "Z_NODE", _TL("Z Levels"), _TL(""));

	Parameters.Add_Double("Z_NODE", "ZSIZE"  , _TL("Cellsize"), _TL(""), 1., 0., true);
	Parameters.Add_Double("Z_NODE", "ZMIN"   , _TL("Minimum" ), _TL(""),   0.0);
	Parameters.Add_Double("Z_NODE", "ZMAX"   , _TL("Maximum" ), _TL(""), 100.0);
	Parameters.Add_Int   ("Z_NODE", "ZLEVELS", _TL("Levels"  ), _TL(""), 100, 1, true);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "EPSILON"	, _TL("Threshold Error"),
		_TL(""),
		0.0001, 0.0, true
	);

	Parameters.Add_Int(
		"", "LEVEL_MAX"	, _TL("Maximum Level"),
		_TL(""),
		11, 1, true, 14, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_MBA_3D::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Shapes	*pPoints = (*pParameters)("POINTS")->asShapes(); int zField = (*pParameters)("Z_FIELD")->asInt();

	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pPoints);
	}

	if( pPoints && zField >= 0 && zField < pPoints->Get_Field_Count() )
	{
		int	zNum	= (*pParameters)("ZLEVELS")->asInt();

		if( pParameter->Cmp_Identifier("POINTS") || pParameter->Cmp_Identifier("Z_FIELD") )
		{
			double	zMin	= pPoints->Get_Minimum(zField);
			double	zMax	= pPoints->Get_Maximum(zField);

			pParameters->Set_Parameter("ZMIN" , zMin);
			pParameters->Set_Parameter("ZMAX" , zMax);
			pParameters->Set_Parameter("ZSIZE", (zMax - zMin) / zNum);
		}
		else
		{
			double	zMin	= (*pParameters)("ZMIN" )->asDouble();
			double	zMax	= (*pParameters)("ZMAX" )->asDouble();
			double	zSize	= (*pParameters)("ZSIZE")->asDouble();

			if( pParameter->Cmp_Identifier("ZSIZE") )
			{
				pParameters->Set_Parameter("ZMAX", zMin + zSize * zNum);
			}

			if( pParameter->Cmp_Identifier("ZMIN") )
			{
				pParameters->Set_Parameter("ZMAX", zMin + zSize * zNum);
			}

			if( pParameter->Cmp_Identifier("ZMAX") )
			{
				pParameters->Set_Parameter("ZMIN", zMax - zSize * zNum);
			}

			if( pParameter->Cmp_Identifier("ZLEVELS") )
			{
				pParameters->Set_Parameter("ZSIZE", (zMax - zMin) / zNum);
			}
		}
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGridding_Spline_MBA_3D::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA_3D::On_Execute(void)
{
	bool	bResult	= false;

	if( !Initialize() )
	{
		return( false );
	}

	m_Epsilon	= Parameters("EPSILON")->asDouble();

	double	Cellsize	= M_GET_MAX(M_GET_MAX(m_pGrids->Get_XRange(), m_pGrids->Get_YRange()), m_pGrids->Get_ZRange());

	bResult	= _Set_MBA(Cellsize);

	m_Points.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA_3D::Initialize(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	int	zField	= Parameters("Z_FIELD")->asInt();
	int	vField	= Parameters("V_FIELD")->asInt();

	if( (m_pGrids = m_Grid_Target.Get_Grids("GRIDS")) == NULL )
	{
		return( false );
	}

	m_pGrids->Fmt_Name("%s.%s [%s]", pPoints->Get_Name(), Parameters("V_FIELD")->asString(), Get_Name().c_str());

	m_pGrids->Del_Grids();

	double	zScale	= Parameters("Z_SCALE")->asDouble();
	int		zLevels	= Parameters("ZLEVELS")->asInt   ();
//	double	zMin	= Parameters("ZMIN"   )->asDouble() * zScale;
//	double	zMax	= Parameters("ZMAX"   )->asDouble() * zScale;
	double	z		= Parameters("ZMIN"   )->asDouble() * zScale;
	m_zCellsize		= Parameters("ZSIZE"  )->asDouble() * zScale;

	for(int iz=0; iz<zLevels; iz++, z+=m_zCellsize)
	{
		m_pGrids->Add_Grid(z);
	}

	//-----------------------------------------------------
	m_Points.Destroy();

	for(int i=0; i<pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( m_pGrids->Get_Extent().Contains(pPoint->Get_Point(0)) )
		{
			CSG_Vector	p(4);

			p[0]	= pPoint->Get_Point(0).x;
			p[1]	= pPoint->Get_Point(0).y;
			p[2]	= pPoint->asDouble(zField) * zScale;
			p[3]	= pPoint->asDouble(vField);

			m_Points.Add_Row(p);
		}
	}

	return( m_Points.Get_NRows() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA_3D::_Set_MBA(double Cellsize)
{
	CSG_Grids	Phi;

	bool	bContinue	= true;

	int	Levels	= Parameters("LEVEL_MAX")->asInt();

	for(int Level=0; bContinue && Level<Levels && Process_Get_Okay(false); Level++, Cellsize/=2.)
	{
		bContinue	= BA_Set_Phi(Phi, Cellsize) && _Get_Difference(Phi, Level);

		BA_Set_Grids(Phi, Level > 0);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA_3D::_Get_Difference(const CSG_Grids &Phi, int Level)
{
	CSG_Simple_Statistics	Differences;

	for(int i=0; i<m_Points.Get_NRows(); i++)
	{
		CSG_Vector	p(4, m_Points[i]);

		p[0]	= (p[0] - Phi.Get_XMin()) / Phi.Get_Cellsize();
		p[1]	= (p[1] - Phi.Get_YMin()) / Phi.Get_Cellsize();
		p[2]	= (p[2] - Phi.Get_ZMin()) / Phi.Get_Cellsize();
		p[3]	=  p[3] - BA_Get_Phi(Phi, p[0], p[1], p[2]);

		m_Points[i][3]	= p[3];

		if( fabs(p[3]) > m_Epsilon )
		{
			Differences	+= fabs(p[3]);
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s:%d %s:%d %s:%f %s:%f",
		_TL("level"  ),      Level + 1,
		_TL("errors" ), (int)Differences.Get_Count  (),
		_TL("maximum"),      Differences.Get_Maximum(),
		_TL("mean"   ),      Differences.Get_Mean   ()
	);

	Process_Set_Text(CSG_String::Format("%s %d [%d]", _TL("Level"), Level + 1, (int)Differences.Get_Count()));

	return( Differences.Get_Maximum() > m_Epsilon );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGridding_Spline_MBA_3D::BA_Get_B(int i, double d) const
{
	switch( i )
	{
	case  0: d = 1. - d; return( d*d*d / 6. );

	case  1: return( ( 3. * d*d*d - 6. * d*d + 4.) / 6. );

	case  2: return( (-3. * d*d*d + 3. * d*d + 3. * d + 1.) / 6. );

	case  3: return( d*d*d / 6. );

	default: return( 0. );
	}
}

//---------------------------------------------------------
bool CGridding_Spline_MBA_3D::BA_Set_Phi(CSG_Grids &Phi, double Cellsize)
{
	int	n	= 4 + (int)(M_GET_MAX(M_GET_MAX(m_pGrids->Get_XRange(), m_pGrids->Get_YRange()), m_pGrids->Get_ZRange()) / Cellsize);

	Phi.Create(n, n, n, Cellsize, m_pGrids->Get_XMin(), m_pGrids->Get_YMin(), m_pGrids->Get_ZMin(), SG_DATATYPE_Float);

	CSG_Grids	Delta(n, n, n, Cellsize, m_pGrids->Get_XMin(), m_pGrids->Get_YMin(), m_pGrids->Get_ZMin(), SG_DATATYPE_Float);

	if( Phi.Get_NZ() < n || Delta.Get_NZ() < n )
	{
		Message_Fmt("\n%s", _TL("failed to allocate memory for phi calculation"));

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<m_Points.Get_NRows(); i++)
	{
		CSG_Vector	p(4, m_Points[i]);

		int	x	= (int)(p[0] = (p[0] - Phi.Get_XMin()) / Phi.Get_Cellsize());
		int	y	= (int)(p[1] = (p[1] - Phi.Get_YMin()) / Phi.Get_Cellsize());
		int	z	= (int)(p[2] = (p[2] - Phi.Get_ZMin()) / Phi.Get_Cellsize());

		if(	x >= 0 && x < Phi.Get_NX() - 3 && y >= 0 && y < Phi.Get_NY() - 3 && z >= 0 && z < Phi.Get_NZ() - 3 )
		{
			int	iz;	double	W[4][4][4], SW2	= 0.0;

			for(iz=0; iz<4; iz++)	// compute W[k,l] and Sum[a=0-3, b=0-3](W²[a,b])
			{
				double	wz	= BA_Get_B(iz, p[2] - z);

				for(int iy=0; iy<4; iy++)
				{
					double	wyz	= wz * BA_Get_B(iy, p[1] - y);

					for(int ix=0; ix<4; ix++)
					{
						SW2	+= SG_Get_Square(W[iz][iy][ix] = wyz * BA_Get_B(ix, p[0] - x));
					}
				}
			}

			if( SW2 > 0.0 )
			{
				double	dz	= p[3] / SW2;

				for(iz=0; iz<4; iz++)
				{
					for(int iy=0; iy<4; iy++)
					{
						for(int ix=0; ix<4; ix++)
						{
							double	wxyz	= W[iz][iy][ix];

							Delta.Add_Value(x + ix, y + iy, z + iz, wxyz*wxyz*wxyz * dz); // numerator
							Phi  .Add_Value(x + ix, y + iy, z + iz, wxyz*wxyz          ); // denominator
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int z=0; z<Phi.Get_NZ(); z++)
	{
		for(int y=0; y<Phi.Get_NY(); y++)
		{
			for(int x=0; x<Phi.Get_NX(); x++)
			{
				double	v	=  Phi.asDouble(x, y, z);

				if( v != 0. )
				{
					Phi.Set_Value(x, y, z, Delta.asDouble(x, y, z) / v);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
double CGridding_Spline_MBA_3D::BA_Get_Phi(const CSG_Grids &Phi, double px, double py, double pz) const
{
	double	v	= 0.0;

	int	x	= (int)px;	px	-= x;
	int	y	= (int)py;	py	-= y;
	int	z	= (int)pz;	pz	-= z;

	if(	x >= 0 && x < Phi.Get_NX() - 3 && y >= 0 && y < Phi.Get_NY() - 3 && z >= 0 && z < Phi.Get_NZ() - 3 )
	{
		for(int iz=0; iz<4; iz++)
		{
			double	bz	= BA_Get_B(iz, pz);

			for(int iy=0; iy<4; iy++)
			{
				double	byz	= bz * BA_Get_B(iy, py);

				for(int ix=0; ix<4; ix++)
				{
					v	+= byz * BA_Get_B(ix, px) * Phi.asDouble(x + ix, y + iy, z + iz);
				}
			}
		}
	}

	return( v );
}

//---------------------------------------------------------
void CGridding_Spline_MBA_3D::BA_Set_Grids(const CSG_Grids &Phi, bool bAdd)
{
	double	d	= m_pGrids->Get_Cellsize() / Phi.Get_Cellsize();

	#pragma omp parallel for
	for(int z=0; z<m_pGrids->Get_NZ(); z++)
	{
		double	pz	= z * m_zCellsize / Phi.Get_Cellsize();

		for(int y=0; y<m_pGrids->Get_NY(); y++)
		{
			double	py	= y * d;

			for(int x=0; x<m_pGrids->Get_NX(); x++)
			{
				double	px	= x * d;

				if( bAdd )
				{	m_pGrids->Add_Value(x, y, z, BA_Get_Phi(Phi, px, py, pz));	}
				else
				{	m_pGrids->Set_Value(x, y, z, BA_Get_Phi(Phi, px, py, pz));	}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
