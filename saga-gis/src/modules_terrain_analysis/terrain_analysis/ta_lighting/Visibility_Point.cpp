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
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Visibility_Point.cpp                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "Visibility_Point.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVisibility_Point::CVisibility_Point(void)
{
	Set_Name(_TL("Visibility (single point)"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description(
		_TL("")
	);

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "VISIBILITY"	, _TL("Visibility"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEIGHT"		, _TL("Height"),
		_TL("Height of the light source above ground."),
		PARAMETER_TYPE_Double, 100.0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Unit"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Visibility"),
			_TL("Shade"),
			_TL("Distance"),
			_TL("Size")
		), 1
	);
}

//---------------------------------------------------------
CVisibility_Point::~CVisibility_Point(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVisibility_Point::On_Execute(void)
{
	CSG_Colors	Colors;

	m_pDTM			= Parameters("ELEVATION")	->asGrid();
	m_pVisibility	= Parameters("VISIBILITY")	->asGrid();
	m_Height		= Parameters("HEIGHT")		->asDouble();
	m_Method		= Parameters("METHOD")		->asInt();

	switch( m_Method )
	{
	case 0:		// Visibility
		m_pVisibility->Set_ZFactor(1.0);
		Colors.Set_Count(2);
		Colors.Set_Ramp(SG_GET_RGB(0, 0, 0), SG_GET_RGB(255, 255, 255));
		break;

	case 1:		// Shade
		m_pVisibility->Set_ZFactor(M_RAD_TO_DEG);
		Colors.Set_Ramp(SG_GET_RGB(255, 255, 255), SG_GET_RGB(0, 0, 0));
		break;

	case 2:		// Distance
		m_pVisibility->Set_ZFactor(1.0);
		Colors.Set_Ramp(SG_GET_RGB(255, 255, 191), SG_GET_RGB(0, 95, 0));
		break;

	case 3:		// Size
		m_pVisibility->Set_ZFactor(M_RAD_TO_DEG);
		Colors.Set_Ramp(SG_GET_RGB(0, 95, 0), SG_GET_RGB(255, 255, 191));
		break;
	}

	DataObject_Set_Colors(m_pVisibility, Colors);

	return( true );
}

//---------------------------------------------------------
bool CVisibility_Point::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	int		x, y, x_Pos, y_Pos;

	double	z_Pos,
			aziDTM, decDTM,
			aziSrc, decSrc,
			d, dx, dy, dz,
			Exaggeration	= 1.0;

	//-----------------------------------------------------
	if(	Mode != MODULE_INTERACTIVE_LDOWN
	||	!m_pDTM->is_InGrid_byPos(Get_xPosition(), Get_yPosition()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	x_Pos	= Get_xGrid();
	y_Pos	= Get_yGrid();;
	z_Pos	= m_pDTM->asDouble(x_Pos, y_Pos) + m_Height;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_NoData(x, y) )
			{
				m_pVisibility->Set_NoData(x, y);
			}
			else
			{
				dx		= x_Pos - x;
				dy		= y_Pos - y;
				dz		= z_Pos - m_pDTM->asDouble(x, y);

				//-----------------------------------------
				if( Trace_Point(x, y, dx, dy, dz) )
				{
					switch( m_Method )
					{
					case 0:		// Visibility
						m_pVisibility->Set_Value(x, y, 1);
						break;

					case 1:		// Shade
						m_pDTM->Get_Gradient(x, y, decDTM, aziDTM);
						decDTM	= M_PI_090 - atan(Exaggeration * tan(decDTM));

						decSrc	= atan2(dz, sqrt(dx*dx + dy*dy));
						aziSrc	= atan2(dx, dy);

						d		= acos(sin(decDTM) * sin(decSrc) + cos(decDTM) * cos(decSrc) * cos(aziDTM - aziSrc));

						m_pVisibility->Set_Value(x, y, d < M_PI_090 ? d : M_PI_090);
						break;

					case 2:		// Distance
						m_pVisibility->Set_Value(x, y, Get_Cellsize() * sqrt(dx*dx + dy*dy));
						break;

					case 3:		// Size
						if( (d = Get_Cellsize() * sqrt(dx*dx + dy*dy)) > 0.0 )
						{
							m_pVisibility->Set_Value(x, y, atan2(m_Height, d));
						}
						else
						{
							m_pVisibility->Set_NoData(x, y);
						//	m_pVisibility->Set_Value(x, y, 0);
						}
						break;
					}
				}

				//-----------------------------------------
				else
				{
					switch( m_Method )
					{
					case 0:		// Visibility
						m_pVisibility->Set_Value(x, y, 0);
						break;

					case 1:		// Shade
						m_pVisibility->Set_Value(x, y, M_PI_090);
						break;

					case 2:		// Distance
					case 3:		// Size
						m_pVisibility->Set_NoData(x, y);
						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	switch( m_Method )
	{
	case 0:		// Visibility
		DataObject_Update(m_pVisibility, 0.0, 1.0, true);
		break;

	case 1:		// Shade
		DataObject_Update(m_pVisibility, 0.0, M_PI_090, true);
		break;

	case 2:		// Distance
	case 3:		// Size
		DataObject_Update(m_pVisibility, true);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CVisibility_Point::Trace_Point(int x, int y, double dx, double dy, double dz)
{
	double	ix, iy, iz, id, d, dist;

	d		= fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);

	if( d > 0 )
	{
		dist	= sqrt(dx*dx + dy*dy);

		dx		/= d;
		dy		/= d;
		dz		/= d;

		d		= dist / d;

		id		= 0.0;
		ix		= x + 0.5;
		iy		= y + 0.5;
		iz		= m_pDTM->asDouble(x, y);

		while( id < dist )
		{
			id	+= d;

			ix	+= dx;
			iy	+= dy;
			iz	+= dz;

			x	= (int)ix;
			y	= (int)iy;

			if( !is_InGrid(x, y) )
			{
				return( true );
			}
			else if( iz < m_pDTM->asDouble(x, y) )
			{
				return( false );
			}
			else if( iz > m_pDTM->Get_ZMax() )
			{
				return( true );
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
