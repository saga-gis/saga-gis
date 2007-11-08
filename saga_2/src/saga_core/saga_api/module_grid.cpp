
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
//                    module_grid.cpp                    //
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
CSG_Module_Grid::CSG_Module_Grid(void)
	: CSG_Module()
{
	m_pLock		= NULL;

	Parameters.Create(this, SG_T(""), SG_T(""), SG_T(""), true);
}

//---------------------------------------------------------
CSG_Module_Grid::~CSG_Module_Grid(void)
{
	Lock_Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Grid::Set_Progress(int iRow)
{
	return( CSG_Module::Set_Progress(iRow, Get_System()->Get_NY() - 1) );
}

//---------------------------------------------------------
bool CSG_Module_Grid::Set_Progress(double Position, double Range)
{
	return( CSG_Module::Set_Progress(Position, Range) );
}

//---------------------------------------------------------
bool CSG_Module_Grid::Set_Progress_NCells(int iCell)
{
	if( Get_System()->is_Valid() && (Get_System()->Get_NCells() <= 100 || !(iCell % (Get_System()->Get_NCells() / 100))) )
	{
		return( CSG_Module::Set_Progress(iCell, Get_System()->Get_NCells()) );
	}

	return( is_Progress() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module_Grid::Lock_Create(void)
{
	if( Get_System()->is_Valid() )
	{
		if( m_pLock && Get_System()->is_Equal(m_pLock->Get_System()) )
		{
			m_pLock->Assign(0.0);
		}
		else
		{
			Lock_Destroy();

			m_pLock	= new CSG_Grid(
				GRID_TYPE_Char,
				Get_System()->Get_NX(),
				Get_System()->Get_NY(),
				Get_System()->Get_Cellsize(),
				Get_System()->Get_XMin(),
				Get_System()->Get_YMin()
			);
		}
	}
}

//---------------------------------------------------------
void CSG_Module_Grid::Lock_Destroy(void)
{
	if( m_pLock )
	{
		delete(m_pLock);

		m_pLock	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
