/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Grid_Value_Replace_Interactive.h
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Value_Replace_Interactive_H
#define HEADER_INCLUDED__Grid_Value_Replace_Interactive_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Value_Replace_Interactive : public CSG_Tool_Grid_Interactive
{
public:
	CGrid_Value_Replace_Interactive(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("A:Grid|Values") );	}


protected:

	virtual bool			On_Execute			(void);
	virtual bool			On_Execute_Finish	(void);
	virtual bool			On_Execute_Position	(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);


private:

	int						m_Method;

	double					m_Value;

	TSG_Point_Int			m_Last;

	CSG_Grid				*m_pGrid;

	CSG_Grid_Cell_Addressor	m_Kernel;


	void					Set_Value			(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Value_Replace_Interactive_H
