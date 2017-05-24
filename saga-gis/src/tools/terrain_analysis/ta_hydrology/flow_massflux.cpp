/**********************************************************
 * Version $Id: flow_massflux.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   flow_massflux.cpp                   //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "flow_massflux.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const int	xDir[4]	= {	1, 1, 0, 0	};
const int	yDir[4]	= {	1, 0, 0, 1	};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_MassFlux::CFlow_MassFlux(void)
{
	Set_Name		(_TL("Flow Accumulation (Mass-Flux Method)"));

	Set_Author		("O. Conrad (c) 2009");

	Set_Description	(_TW(
		"The Mass-Flux Method (MFM) for the DEM based calculation of flow accumulation "
		"as proposed by Gruber and Peckham (2008).\n"
		"\n"
		"!!!UNDER DEVELOPMENT!!! To be done: solving the streamline resolution problem\n"
		"\n"
		"References:\n"
		"Gruber, S., Peckham, S. (2008): Land-Surface Parameters and Objects in Hydrology. "
		"In: Hengl, T. and Reuter, H.I. [Eds.]: Geomorphometry: Concepts, Software, Applications. "
		"Developments in Soil Science, Elsevier, Bd.33, S.293-308.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "AREA"	, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Flow Split Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("flow width (original)"),
			_TL("cell area")
		), 0
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Node(NULL, "NODE_QUARTERS", _TL("Create Output of Quarter Cell Grids"), _TL(""));

	Parameters.Add_Value(
		pNode	, "B_SLOPE"	, _TL("Slope"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid_Output(
		NULL	, "G_SLOPE"	, _TL("Slope")	, _TL("")
	);

	Parameters.Add_Value(
		pNode	, "B_ASPECT", _TL("Aspect"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid_Output(
		NULL	, "G_ASPECT", _TL("Aspect")	, _TL("")
	);

	Parameters.Add_Value(
		pNode	, "B_AREA"	, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Grid_Output(
		NULL	, "G_AREA"	, _TL("Flow Accumulation")	, _TL("")
	);

	Parameters.Add_Value(
		pNode	, "B_FLOW"	, _TL("Flow Lines"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Shapes_Output(
		NULL	, "G_FLOW"	, _TL("Flow Lines")	, _TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_MassFlux::On_Execute(void)
{
	int			x, y, i, ix, iy;
	CSG_Grid	*pArea;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM")		->asGrid();
	pArea		= Parameters("AREA")	->asGrid();
	m_Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	if( 1 )
	{
		CSG_Grid_System	System(0.5 * Get_Cellsize(), Get_XMin() - 0.25 * Get_Cellsize(), Get_YMin() - 0.25 * Get_Cellsize(), 2 * Get_NX(), 2 * Get_NY());

		m_Area	.Create(System, SG_DATATYPE_Float);
		m_dir	.Create(System, SG_DATATYPE_Byte);
		m_dif	.Create(System, SG_DATATYPE_Float);

		m_dir	.Assign(-1.0);
		m_Area	.Assign( 0.0);
		m_Area	.Set_NoData_Value(0.0);

		Parameters("G_SLOPE" )->Set_Value(m_pSlope  = !Parameters("B_SLOPE" )->asBool() ? NULL : SG_Create_Grid(System, SG_DATATYPE_Float));
		Parameters("G_ASPECT")->Set_Value(m_pAspect = !Parameters("B_ASPECT")->asBool() ? NULL : SG_Create_Grid(System, SG_DATATYPE_Float));
		Parameters("G_FLOW"  )->Set_Value(m_pFlow   = !Parameters("B_FLOW"  )->asBool() ? NULL : SG_Create_Shapes(SHAPE_TYPE_Line, _TL("Flow Lines")));

		//-------------------------------------------------
		// Calculate flow portions...
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				for(i=0; i<4; i++)
				{
					Set_Flow(x, y, i);
				}
			}
		}

		//-------------------------------------------------
		// Check for consistent flow directions...

		// still missing...


		//-------------------------------------------------
		// Calculate flow accumulation...
		for(y=0, iy=0; y<Get_NY() && Set_Progress(y); y++, iy+=2)
		{
			for(x=0, ix=0; x<Get_NX(); x++, ix+=2)
			{
				for(i=0; i<4; i++)
				{
					Get_Area(ix + xDir[i], iy + yDir[i]);
				}
			}
		}

		//-------------------------------------------------
		// Scale flow accumulation to original cell size...
		for(y=0, iy=0; y<Get_NY() && Set_Progress(y); y++, iy+=2)
		{
			for(x=0, ix=0; x<Get_NX(); x++, ix+=2)
			{
				double	Area	= 0.0, d;

				for(i=0; i<4; i++)
				{
					if( (d = m_Area.asDouble(ix + xDir[i], iy + yDir[i])) > 0.0 )
					{
						Area	+= d;
					}
				}

				if( Area > 0.0 )
				{
					pArea->Set_Value(x, y, Area * m_Area.Get_Cellarea());
				}
				else
				{
					pArea->Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
		if( Parameters("B_AREA")->asBool() )
		{
			Parameters("G_AREA")->Set_Value(SG_Create_Grid(m_Area));
		}

		m_Area	.Destroy();
		m_dif	.Destroy();
		m_dir	.Destroy();

		DataObject_Set_Colors(pArea, 100, SG_COLORS_WHITE_BLUE);

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_MassFlux::Set_Flow(int x, int y, int Direction)
{
	int		dir, ix, iy, jx, jy;
	double	dif, Z, A, B;

	if( m_pDEM->is_InGrid(x, y)
	&&	m_pDEM->is_InGrid(ix = Get_xTo(2 * Direction    , x), iy = Get_yTo(2 * Direction    , y))
	&&	m_pDEM->is_InGrid(jx = Get_xTo(2 * Direction + 2, x), jy = Get_yTo(2 * Direction + 2, y)) )
	{
		Z	=  m_pDEM->asDouble( x,  y);
		A	= (m_pDEM->asDouble(ix, iy) - Z) / Get_Cellsize();
		B	= (m_pDEM->asDouble(jx, jy) - Z) / Get_Cellsize();

		dif	= A != 0.0 ? M_PI_180 + atan2(B, A) : (B > 0.0 ? M_PI_270 : (B < 0.0 ? M_PI_090 : -1.0));

		if( dif >= 0.0 )
		{
			x	= 2 * x + xDir[Direction];
			y	= 2 * y + yDir[Direction];

			dif	= fmod(dif + M_PI_090 * Direction, M_PI_360);

			if( m_pFlow )
			{
				double		dScale	= 0.50 * m_dir.Get_System().Get_Cellsize();
				TSG_Point	Point	= m_dir.Get_System().Get_Grid_to_World(x, y);
				CSG_Shape	*pLine	= m_pFlow->Add_Shape();

				pLine->Add_Point(
					Point.x - dScale * sin(dif),
					Point.y - dScale * cos(dif), 0
				);
				pLine->Add_Point(Point, 0);

				dScale	= 0.20 * m_dir.Get_System().Get_Cellsize();
				pLine->Add_Point(
					Point.x - dScale * sin(dif + 25.0 * M_DEG_TO_RAD),
					Point.y - dScale * cos(dif + 25.0 * M_DEG_TO_RAD), 1
				);
				pLine->Add_Point(Point, 1);
				pLine->Add_Point(
					Point.x - dScale * sin(dif - 25.0 * M_DEG_TO_RAD),
					Point.y - dScale * cos(dif - 25.0 * M_DEG_TO_RAD), 1
				);
			}

			if( m_pSlope )	m_pSlope ->Set_Value(x, y, atan(sqrt(A*A + B*B)));
			if( m_pAspect )	m_pAspect->Set_Value(x, y, dif);

			dir	= (int)(dif / M_PI_090);

			dif	= dif - dir * M_PI_090;

			switch( m_Method )
			{
			case 0:
				dif	= cos(dif) / (cos(dif) + sin(dif));
				break;

			case 1:
				dif	= dif < M_PI_045 ? 1.0 - tan(dif) / 2.0 : tan(M_PI_090 - dif) / 2.0;
				break;
			}

			m_dir.Set_Value(x, y, dir * 2);
			m_dif.Set_Value(x, y, dif);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CFlow_MassFlux::Get_Flow(int x, int y, int Direction)
{
	if( m_dir.is_InGrid(x, y) )
	{
		int		i	= m_dir.asInt(x, y);

		if( Direction == i )
		{
			return( m_dif.asDouble(x, y) );
		}

		if( Direction == (i + 2) % 8 )
		{
			return( 1.0 - m_dif.asDouble(x, y) );
		}
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CFlow_MassFlux::Get_Area(int x, int y)
{
	if( m_Area.is_NoData(x, y) )		// cell has not been processed yet...
	{
		int		i, ix, iy;
		double	d;

		m_Area.Set_Value(x, y, 1.0);	// add this cell's contribution...

		for(i=0; i<8; i+=2)
		{
			ix	= Get_xFrom(i, x);
			iy	= Get_yFrom(i, y);

			d	= Get_Flow(ix, iy, i);

			if( d > 0.0 )				// which portion drains ith neighbour into this cell???
			{
				m_Area.Add_Value(x, y, d * Get_Area(ix, iy));	// then recursive call of this function...
			}
		}
	}

	return( m_Area.asDouble(x, y) );	// return this cell's area...
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
