
/*******************************************************************************
    Cost_Isotropic.h
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
#ifndef HEADER_INCLUDED__Cost_Isotropic_H
#define HEADER_INCLUDED__Cost_Isotropic_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCost_Accumulated : public CSG_Tool_Grid
{
public:

	CCost_Accumulated(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Cost Analysis") );	}


protected:	

	int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool					On_Execute				(void);


private:

	double                  m_Cost_Min;

	CSG_Grid				*m_pCost, *m_pAccumulated, *m_pAllocation;


	bool					Get_Destinations		(CSG_Points_Int &Destinations);

	double					Get_Cost				(int x, int y);
	bool					Get_Cost				(CSG_Points_Int &Destinations);

	int						Get_Allocation			(int x, int y);
	bool					Get_Allocation			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Cost_Isotropic_H
