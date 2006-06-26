
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   grid_system.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                Universit� G�tingen                  //
//                Goldschmidtstr. 5                      //
//                37077 G�tingen                        //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_System::CGrid_System(void)
{
	m_Cellsize	= -1.0;
}

//---------------------------------------------------------
CGrid_System::CGrid_System(const CGrid_System &System)
{
	m_Cellsize	= -1.0;

	Assign(System);
}

//---------------------------------------------------------
CGrid_System::CGrid_System(double Cellsize, const CGEO_Rect &Extent)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, Extent);
}

//---------------------------------------------------------
CGrid_System::CGrid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CGrid_System::CGrid_System(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, xMin, yMin, NX, NY);
}

//---------------------------------------------------------
CGrid_System::~CGrid_System(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_System::is_Valid(void)
{
	return( m_Cellsize > 0.0 );
}

//---------------------------------------------------------
const char * CGrid_System::Get_Name(bool bShort)
{
	if( is_Valid() )
	{
		if( bShort )
		{
			m_Name.Printf("%.*f; %dx %dy; %.*fW %.*fS",
				API_Get_Significant_Decimals(Get_Cellsize()),
				Get_Cellsize(),
				Get_NX(),
				Get_NY(),
				API_Get_Significant_Decimals(Get_XMin()), Get_XMin(),
				API_Get_Significant_Decimals(Get_YMin()), Get_YMin()
			);
		}
		else
		{
			m_Name.Printf("%s: %f, %s: %dx/%dy, %s: %fW/%fS",
				LNG("[DAT] Cell size"),
				Get_Cellsize(),
				LNG("[DAT] Number of cells"),
				Get_NX(),
				Get_NY(),
				LNG("[DAT] Lower left corner"),
				Get_XMin(),
				Get_YMin()
			);
		}

		return( m_Name );
	}

	return( LNG("[DAT] [not set]") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_System::operator == (const CGrid_System &System) const
{
	return( is_Equal(System) );
}

//---------------------------------------------------------
void CGrid_System::operator = (const CGrid_System &System)
{
	Assign(System);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_System::Assign(const CGrid_System &System)
{
	return( Assign(System.m_Cellsize, System.m_Extent) );
}

//---------------------------------------------------------
bool CGrid_System::Assign(double Cellsize, const CGEO_Rect &Extent)
{
	return( Assign(Cellsize, Extent.m_rect.xMin, Extent.m_rect.yMin, Extent.m_rect.xMax, Extent.m_rect.yMax) );
}

//---------------------------------------------------------
bool CGrid_System::Assign(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	if( Cellsize > 0.0 && xMin < xMax && yMin < yMax )
	{
		return( Assign(Cellsize, xMin, yMin, 1 + (int)(0.5 + (xMax - xMin) / Cellsize), 1 + (int)(0.5 + (yMax - yMin) / Cellsize)) );
	}

	m_Cellsize	= -1.0;

	return( false );
}

//---------------------------------------------------------
bool CGrid_System::Assign(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	if( Cellsize > 0.0 && NX > 0 && NY > 0 )
	{
		m_NX		= NX;
		m_NY		= NY;
		m_NCells	= NY * NX;

		m_Cellsize	= Cellsize;
		m_Cellarea	= Cellsize * Cellsize;

		m_Extent.Assign(xMin, yMin, xMin + (NX - 1) * Cellsize, yMin + (NY - 1) * Cellsize);

		m_Diagonal	= Cellsize * sqrt(2.0);

		return( true );
	}

	m_Cellsize	= -1.0;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_System::is_Equal(const CGrid_System &System) const
{
	return( is_Equal(System.m_Cellsize, System.m_Extent.m_rect) );
}

//---------------------------------------------------------
bool CGrid_System::is_Equal(double Cellsize, const TGEO_Rect &Extent) const
{
	return( m_Cellsize == Cellsize && m_Extent == Extent );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
