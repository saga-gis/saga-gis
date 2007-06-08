
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
CSG_Grid_System::CSG_Grid_System(void)
{
	m_Cellsize	= -1.0;
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(const CSG_Grid_System &System)
{
	m_Cellsize	= -1.0;

	Assign(System);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, const CSG_Rect &Extent)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, Extent);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CSG_Grid_System::CSG_Grid_System(double Cellsize, double xMin, double yMin, int NX, int NY)
{
	m_Cellsize	= -1.0;

	Assign(Cellsize, xMin, yMin, NX, NY);
}

//---------------------------------------------------------
CSG_Grid_System::~CSG_Grid_System(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::is_Valid(void) const
{
	return( m_Cellsize > 0.0 );
}

//---------------------------------------------------------
const SG_Char * CSG_Grid_System::Get_Name(bool bShort)
{
	if( is_Valid() )
	{
		if( bShort )
		{
			m_Name.Printf(SG_T("%.*f; %dx %dy; %.*fW %.*fS"),
				SG_Get_Significant_Decimals(Get_Cellsize()),
				Get_Cellsize(),
				Get_NX(),
				Get_NY(),
				SG_Get_Significant_Decimals(Get_XMin()), Get_XMin(),
				SG_Get_Significant_Decimals(Get_YMin()), Get_YMin()
			);
		}
		else
		{
			m_Name.Printf(SG_T("%s: %f, %s: %dx/%dy, %s: %fW/%fS"),
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
bool CSG_Grid_System::operator == (const CSG_Grid_System &System) const
{
	return( is_Equal(System) );
}

//---------------------------------------------------------
void CSG_Grid_System::operator = (const CSG_Grid_System &System)
{
	Assign(System);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_System::Assign(const CSG_Grid_System &System)
{
	return( Assign(System.m_Cellsize, System.m_Extent) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(double Cellsize, const CSG_Rect &Extent)
{
	return( Assign(Cellsize, Extent.m_rect.xMin, Extent.m_rect.yMin, Extent.m_rect.xMax, Extent.m_rect.yMax) );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, double xMax, double yMax)
{
	if( Cellsize > 0.0 && xMin < xMax && yMin < yMax )
	{
		return( Assign(Cellsize, xMin, yMin, 1 + (int)(0.5 + (xMax - xMin) / Cellsize), 1 + (int)(0.5 + (yMax - yMin) / Cellsize)) );
	}

	m_Cellsize	= -1.0;

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_System::Assign(double Cellsize, double xMin, double yMin, int NX, int NY)
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
bool CSG_Grid_System::is_Equal(const CSG_Grid_System &System) const
{
	return( is_Equal(System.m_Cellsize, System.m_Extent.m_rect) );
}

//---------------------------------------------------------
bool CSG_Grid_System::is_Equal(double Cellsize, const TSG_Rect &Extent) const
{
	return( m_Cellsize == Cellsize && m_Extent == Extent );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
