
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
//              module_grid_interactive.cpp              //
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
#include "module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CModule_Grid_Interactive::CModule_Grid_Interactive(void)
{
	m_pModule	= this;
}

//---------------------------------------------------------
CModule_Grid_Interactive::~CModule_Grid_Interactive(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule_Grid_Interactive::Get_Grid_Pos(int &x, int &y)
{
	bool	bResult;

	if( Get_System()->is_Valid() )
	{
		bResult	= true;

		//-------------------------------------------------
		x		= (int)(0.5 + (Get_xPosition() - Get_System()->Get_XMin()) / Get_System()->Get_Cellsize());

		if( x < 0 )
		{
			bResult	= false;
			x		= 0;
		}
		else if( x >= Get_System()->Get_NX() )
		{
			bResult	= false;
			x		= Get_System()->Get_NX() - 1;
		}

		//-------------------------------------------------
		y		= (int)(0.5 + (Get_yPosition() - Get_System()->Get_YMin()) / Get_System()->Get_Cellsize());

		if( y < 0 )
		{
			bResult	= false;
			y		= 0;
		}
		else if( y >= Get_System()->Get_NY() )
		{
			bResult	= false;
			y		= Get_System()->Get_NY() - 1;
		}

		return( bResult );
	}

	//-----------------------------------------------------
	x		= 0;
	y		= 0;

	return( false );
}

//---------------------------------------------------------
int CModule_Grid_Interactive::Get_xGrid(void)
{
	int		x;

	if( Get_System()->is_Valid() )
	{
		x		= (int)(0.5 + (Get_xPosition() - Get_System()->Get_XMin()) / Get_System()->Get_Cellsize());

		if( x < 0 )
		{
			x		= 0;
		}
		else if( x >= Get_System()->Get_NX() )
		{
			x		= Get_System()->Get_NX() - 1;
		}

		return( x );
	}

	return( 0 );
}

//---------------------------------------------------------
int CModule_Grid_Interactive::Get_yGrid(void)
{
	int		y;

	if( Get_System()->is_Valid() )
	{
		y		= (int)(0.5 + (Get_yPosition() - Get_System()->Get_YMin()) / Get_System()->Get_Cellsize());

		if( y < 0 )
		{
			y		= 0;
		}
		else if( y >= Get_System()->Get_NY() )
		{
			y		= Get_System()->Get_NY() - 1;
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
