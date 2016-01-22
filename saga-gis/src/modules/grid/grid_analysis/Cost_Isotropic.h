/**********************************************************
 * Version $Id$
 *********************************************************/
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
#ifndef HEADER_INCLUDED__Cost_Isotropic_H
#define HEADER_INCLUDED__Cost_Isotropic_H


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
class CCost_Accumulated : public CSG_Module_Grid
{
public:

	CCost_Accumulated(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Cost Analysis") );	}


protected:	

	int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool					On_Execute				(void);


private:

	bool					m_bDegree;

	double					m_dK;

	CSG_Grid				*m_pDirection;


	double					Get_CostInDirection		(const TSG_Point_Int &p, int i);


//---------------------------------------------------------
private:

	class CPoints
	{
	public:
		CPoints(void)	{}

		void				Clear			(void)	{	m_Points.Clear();	m_Allocation.Destroy();	}

		bool				Add				(int x, int y, int iAllocation)
		{
			return( m_Points.Add(x, y) && m_Allocation.Add(iAllocation) );
		}

		int					Get_Count		(void)	{	return( m_Points.Get_Count() );	}
		TSG_Point_Int		Get_Point		(int i)	{	return( m_Points    [i] );	}
		int					Get_Allocation	(int i)	{	return( m_Allocation[i] );	}


	private:

		CSG_Points_Int		m_Points;

		CSG_Array_Int		m_Allocation;

	};
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Cost_Isotropic_H
