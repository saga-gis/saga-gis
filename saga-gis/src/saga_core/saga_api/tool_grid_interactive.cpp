
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
//              tool_grid_interactive.cpp                //
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Grid_Interactive::CSG_Tool_Grid_Interactive(void)
{
	m_pTool	= this;
}

//---------------------------------------------------------
CSG_Tool_Grid_Interactive::~CSG_Tool_Grid_Interactive(void)
{
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Grid_Interactive::Get_Grid_Pos(int &x, int &y) const
{
	if( !Get_System().is_Valid() )
	{
		x = 0; y = 0; return( false );
	}

	bool bResult = true;

	x = (int)(0.5 + (Get_xPosition() - Get_XMin()) / Get_Cellsize());
	if     ( x <  0        ) { x = 0           ; bResult = false; }
	else if( x >= Get_NX() ) { x = Get_NX() - 1; bResult = false; }

	y = (int)(0.5 + (Get_yPosition() - Get_YMin()) / Get_Cellsize());
	if     ( y <  0        ) { y = 0           ; bResult = false; }
	else if( y >= Get_NY() ) { y = Get_NY() - 1; bResult = false; }

	return( bResult );
}

//---------------------------------------------------------
int CSG_Tool_Grid_Interactive::Get_xGrid(void) const
{
	if( Get_System().is_Valid() )
	{
		int x = (int)(0.5 + (Get_xPosition() - Get_XMin()) / Get_Cellsize());

		return( x < 0 ? 0 : x < Get_NX() ? x : Get_NX() - 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Tool_Grid_Interactive::Get_yGrid(void) const
{
	if( Get_System().is_Valid() )
	{
		int y = (int)(0.5 + (Get_yPosition() - Get_YMin()) / Get_Cellsize());

		return( y < 0 ? 0 : y < Get_NY() ? y : Get_NY() - 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
