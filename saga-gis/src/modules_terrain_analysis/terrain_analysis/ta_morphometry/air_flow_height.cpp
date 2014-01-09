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
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Air_Flow_Height.cpp                  //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    e-mail:     conrad@geowiss.uni-hamburg.de          //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "air_flow_height.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAir_Flow_Height::CAir_Flow_Height(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Effective Air Flow Heights"));

	Set_Author		(SG_T("J.Boehner, O.Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "AFH"			, _TL("Effective Air Flow Heights"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "DIR"			, _TL("Wind Direction"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "LEN"			, _TL("Wind Speed"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	pNode	= Parameters.Add_Value(
		NULL	, "DIR_CONST"	, _TL("Constant Wind Direction [Degree]"),
		_TL("constant wind direction to be used if no direction grid is given"),
		PARAMETER_TYPE_Double, 135.0
	);

	Parameters.Add_Value(
		pNode	, "OLDVER"		, _TL("Old Version"),
		_TL("use old version (no acceleration option) with constant wind direction"),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "MAXDIST"		, _TL("Search Distance [km]"),
		_TL(""),
		PARAMETER_TYPE_Double, 300.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "ACCEL"		, _TL("Acceleration"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.5, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "PYRAMIDS"	, _TL("Use Pyramids with New Version"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "LEEFACT"		, _TL("Lee Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.5
	);

	Parameters.Add_Value(
		NULL	, "LUVFACT"		, _TL("Luv Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);

	Parameters.Add_Choice(
		NULL	, "DIR_UNITS"	, _TL("Wind Direction Units"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("radians"),
			_TL("degree")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "LEN_SCALE"	, _TL("Wind Speed Scale Factor"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

/*	Parameters.Add_Value(
		NULL	, "DIR_TRACE"	, _TL("Precise Tracing"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);/**/
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAir_Flow_Height::On_Execute(void)
{
	bool		bOldVer;
	int			x, y;
	CSG_Grid	*pDirection, *pSpeed, *pAFH;

	//-----------------------------------------------------
	m_pDEM			= Parameters("DEM")			->asGrid();
	pDirection		= Parameters("DIR")			->asGrid();
	pSpeed			= Parameters("LEN")			->asGrid();
	pAFH			= Parameters("AFH")			->asGrid();
	m_maxDistance	= Parameters("MAXDIST")		->asDouble() * 1000.0;
	m_Acceleration	= Parameters("ACCEL")		->asDouble();
	bOldVer			= Parameters("OLDVER")		->asBool() && !pDirection;
	m_dLee			= Parameters("LEEFACT")		->asDouble();
	m_dLuv			= Parameters("LUVFACT")		->asDouble();
//	m_bTrace		= Parameters("DIR_TRACE")	->asBool();

	//-----------------------------------------------------
	CSG_Colors	Colors(5);

	Colors.Set_Color(0, 255, 127,  63);
	Colors.Set_Color(1, 255, 255, 127);
	Colors.Set_Color(2, 255, 255, 255);
	Colors.Set_Color(3, 127, 127, 175);
	Colors.Set_Color(4,   0,   0, 100);

	Colors.Set_Count(100);

	DataObject_Set_Colors(pAFH, Colors);

	//-----------------------------------------------------
	if( pDirection )
	{
		double	dRadians	= Parameters("DIR_UNITS")->asInt() == 0 ? 1.0 : M_DEG_TO_RAD;
		double	dScale		= Parameters("LEN_SCALE")->asDouble();

		m_DX.Create(*Get_System());
		m_DY.Create(*Get_System());

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( pDirection->is_NoData(x, y) )
				{
					m_DX.Set_NoData(x, y);
				}
				else
				{
					double	d	= pSpeed ? (!pSpeed->is_NoData(x, y) ? dScale * pSpeed->asDouble(x, y) : 0.0) : 1.0;

					m_DX.Set_Value(x, y, d * sin(pDirection->asDouble(x, y) * dRadians));
					m_DY.Set_Value(x, y, d * cos(pDirection->asDouble(x, y) * dRadians));
				}
			}
		}
	}
	else
	{
		m_dx	= sin(Parameters("DIR_CONST")->asDouble() * M_DEG_TO_RAD);
		m_dy	= cos(Parameters("DIR_CONST")->asDouble() * M_DEG_TO_RAD);

		if( fabs(m_dx) > fabs(m_dy) )
		{
			m_dy	/= fabs(m_dx);
			m_dx	= m_dx < 0 ? -1 : 1;
		}
		else
		{
			m_dx	/= fabs(m_dy);
			m_dy	= m_dy < 0 ? -1 : 1;
		}
	}

	if( Parameters("PYRAMIDS")->asBool() && (pDirection || !bOldVer) )
	{
		m_DEM.Create(m_pDEM, 2.0);
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				pAFH->Set_NoData(x, y);
			}
			else
			{
				double	Luv, Luv_Lee, Lee, d, z;

				if( bOldVer )
				{
					Get_Luv(x, y,  m_dx,  m_dy, Luv);
					Get_Lee(x, y, -m_dx, -m_dy, Luv_Lee, Lee);
				}
				else
				{
					Get_Luv(x, y, Luv);
					Get_Lee(x, y, Luv_Lee, Lee);
				}

				z	= m_pDEM->asDouble(x, y);

				d	= 1.0 + (z + Lee != 0.0 ? (z - Lee) / (z + Lee) : 0.0);
				d	= (Luv > Luv_Lee ? Luv - Luv_Lee : 0.0) + z * d*d / 2.0;

				pAFH->Set_Value(x, y, d < 0.0 ? 0.0 : d);
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
inline bool CAir_Flow_Height::Get_Next(TSG_Point &Position, double Distance, bool bReverse)
{
	if( Distance <= 0.0 )
	{
		return( false );
	}

	double	dx, dy;

	if( m_DX.is_Valid() )
	{
/*		if( m_bTrace )
		{
			double	dMove	= bReverse ? -Get_Cellsize() : Get_Cellsize();

			for( ; Distance>Get_Cellsize(); Distance-=Get_Cellsize())
			{
				if( !m_DX.Get_Value(Position, dx) || !m_DY.Get_Value(Position, dy) )
				{
					return( false );
				}

				Position.x	+= dMove * dx;
				Position.y	+= dMove * dy;
			}
		}
/**/
		if( !m_DX.Get_Value(Position, dx) || !m_DY.Get_Value(Position, dy) )
		{
			return( false );
		}
	}
	else
	{
		dx	= m_dx;
		dy	= m_dy;
	}

	if( bReverse )
	{
		Position.x	-= Distance * dx;
		Position.y	-= Distance * dy;
	}
	else
	{
		Position.x	+= Distance * dx;
		Position.y	+= Distance * dy;
	}

	return( Get_System()->Get_Extent(true).Contains(Position) );
}

//---------------------------------------------------------
inline bool CAir_Flow_Height::Get_Z(const TSG_Point &Position, double Distance, double &z)
{
	if( m_DEM.Get_Count() > 0 )
	{
		Distance	/= 4.0;

		for(int i=0; i<m_DEM.Get_Count(); i++)
		{
			if( Distance < m_DEM.Get_Grid(i)->Get_Cellsize() )
			{
				return( m_DEM.Get_Grid(i)->Get_Value(Position, z) );
			}
		}
	}

	return( m_pDEM->Get_Value(Position, z) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CAir_Flow_Height::Get_Luv(int x, int y, double &Sum_A)
{
	double	Weight_A	= Sum_A	= 0.0;

	if( m_pDEM->is_InGrid(x, y) )
	{
		double		z, d, id, w;
		TSG_Point	p;

		d	= id	= Get_Cellsize();
		p	= Get_System()->Get_Grid_to_World(x, y);

		while( id <= m_maxDistance && Get_Next(p, d, false) )
		{
			if( Get_Z(p, d, z) )
			{
				Weight_A	+= w = d * pow(id, -m_dLuv);
				Sum_A		+= w * z;
			}

			d	*= m_Acceleration;
			id	+= d;
		}

		if( Weight_A > 0.0 )	{	Sum_A	/= Weight_A;	}
	}
}

//---------------------------------------------------------
void CAir_Flow_Height::Get_Lee(int x, int y, double &Sum_A, double &Sum_B)
{
	double	Weight_A	= Sum_A	= 0.0;
	double	Weight_B	= Sum_B	= 0.0;

	if( m_pDEM->is_InGrid(x, y) )
	{
		double		z, d, id, w;
		TSG_Point	p;

		d	= id	= Get_Cellsize();
		p	= Get_System()->Get_Grid_to_World(x, y);

		while( id <= m_maxDistance && Get_Next(p, d, true) )
		{
			if( Get_Z(p, d, z) )
			{
				Weight_A	+= w = d * pow(id, -m_dLuv);
				Sum_A		+= w * z;

				Weight_B	+= w = d * pow(id, -m_dLee);
				Sum_B		+= w * z;
			}

			d	*= m_Acceleration;
			id	+= d;
		}

		if( Weight_A > 0.0 )	{	Sum_A	/= Weight_A;	}
		if( Weight_B > 0.0 )	{	Sum_B	/= Weight_B;	}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CAir_Flow_Height::Get_Luv(int x, int y, double dx, double dy, double &Sum_A)
{
	double	Weight_A	= Sum_A	= 0.0;

	double	w, d	= Get_Cellsize() * sqrt(dx*dx + dy*dy);

	for(double ix=x+dx+0.5, iy=y+dy+0.5, id=d; is_InGrid(x = (int)ix, y = (int)iy) && id<=m_maxDistance; ix+=dx, iy+=dy, id+=d)
	{
		if( !m_pDEM->is_NoData(x, y) )
		{
			Weight_A	+= w = pow(id, -m_dLuv);
			Sum_A		+= w * m_pDEM->asDouble(x, y);
		}
	}

	if( Weight_A > 0.0 )	{	Sum_A	/= Weight_A;	}
}

//---------------------------------------------------------
void CAir_Flow_Height::Get_Lee(int x, int y, double dx, double dy, double &Sum_A, double &Sum_B)
{
	double	Weight_A	= Sum_A	= 0.0;
	double	Weight_B	= Sum_B	= 0.0;

	double	w, d	= Get_Cellsize() * sqrt(dx*dx + dy*dy);

	for(double ix=x+dx+0.5, iy=y+dy+0.5, id=d; is_InGrid(x = (int)ix, y = (int)iy) && id<=m_maxDistance; ix+=dx, iy+=dy, id+=d)
	{
		if( !m_pDEM->is_NoData(x, y) )
		{
			double	z	= m_pDEM->asDouble(x, y);

			Weight_A	+= w = pow(id, -m_dLuv);
			Sum_A		+= w * z;

			Weight_B	+= w = pow(id, -m_dLee);
			Sum_B		+= w * z;
		}
	}

	if( Weight_A > 0.0 )	{	Sum_A	/= Weight_A;	}
	if( Weight_B > 0.0 )	{	Sum_B	/= Weight_B;	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
