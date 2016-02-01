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
private:

	class CPoints
	{
	public:
		CPoints(void)
		{
			m_Points.Create(sizeof(TSG_Point_Int), 0, SG_ARRAY_GROWTH_1);
		}

		void					Clear			(void)	{	m_Points.Set_Array(0, false);	}

		bool					Add				(int x, int y)
		{
			if( m_Points.Inc_Array() )
			{
				TSG_Point_Int	*p	= (TSG_Point_Int *)m_Points.Get_Entry(m_Points.Get_Size() - 1);

				p->x	= x;
				p->y	= y;

				return( true );
			}

			return( false );
		}

		int						Get_Count		(void)	{	return( (int)m_Points.Get_Size() );	}
		const TSG_Point_Int &	operator []		(int i)	{	return( *((TSG_Point_Int *)m_Points.Get_Entry(i)) );	}


	private:

		CSG_Array				m_Points;

	};


//---------------------------------------------------------
public:

	CCost_Accumulated(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Cost Analysis") );	}


protected:	

	int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool					On_Execute				(void);


private:

	bool					m_bDegree;

	double					m_dK;

	CSG_Grid				*m_pCost, *m_pDirection, *m_pAccumulated, *m_pAllocation;


	bool					Get_Destinations		(CPoints &Points);

	bool					Get_Cost				(CPoints &Points);

	bool					Get_Allocation			(void);
	int						Get_Allocation			(int x, int y);


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Cost_Isotropic_H
