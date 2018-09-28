/**********************************************************
 * Version $Id$
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Grid_Interactive::Get_Grid_Pos(int &x, int &y)
{
	if( Get_System().is_Valid() )
	{
		bool	bResult	= true;

		//-------------------------------------------------
		x	= (int)(0.5 + (Get_xPosition() - Get_XMin()) / Get_Cellsize());

		if( x < 0 )
		{
			bResult	= false;	x	= 0;
		}
		else if( x >= Get_NX() )
		{
			bResult	= false;	x	= Get_NX() - 1;
		}

		//-------------------------------------------------
		y	= (int)(0.5 + (Get_yPosition() - Get_YMin()) / Get_Cellsize());

		if( y < 0 )
		{
			bResult	= false;	y	= 0;
		}
		else if( y >= Get_NY() )
		{
			bResult	= false;	y	= Get_NY() - 1;
		}

		return( bResult );
	}

	//-----------------------------------------------------
	x	= 0;
	y	= 0;

	return( false );
}

//---------------------------------------------------------
int CSG_Tool_Grid_Interactive::Get_xGrid(void)
{
	if( Get_System().is_Valid() )
	{
		int	x	= (int)(0.5 + (Get_xPosition() - Get_XMin()) / Get_Cellsize());

		if( x < 0 )
		{
			x	= 0;
		}
		else if( x >= Get_NX() )
		{
			x	= Get_NX() - 1;
		}

		return( x );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Tool_Grid_Interactive::Get_yGrid(void)
{
	if( Get_System().is_Valid() )
	{
		int	y	= (int)(0.5 + (Get_yPosition() - Get_YMin()) / Get_Cellsize());

		if( y < 0 )
		{
			y	= 0;
		}
		else if( y >= Get_NY() )
		{
			y	= Get_NY() - 1;
		}

		return( y );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
