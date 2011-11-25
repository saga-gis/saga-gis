/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Cost_Anisotropic.h
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 
#ifndef HEADER_INCLUDED__Cost_Anisotropic_H
#define HEADER_INCLUDED__Cost_Anisotropic_H

#include "MLB_Interface.h"

class CCost_Anisotropic : public CSG_Module_Grid
{
public:

	CCost_Anisotropic(void);
	virtual ~CCost_Anisotropic(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Cost Analysis") );	}


protected:	

	bool					On_Execute		(void);


private:

	double					m_dK;
	double					m_dThreshold;

	CSG_Points_Int			m_CentralPoints;
	CSG_Points_Int			m_AdjPoints;

	CSG_Grid					*m_pCostGrid;
	CSG_Grid					*m_pDirectionGrid;
	CSG_Grid					*m_pPointsGrid;	
	CSG_Grid					*m_pAccCostGrid;

	void					CalculateCost				(void);
	double					CalculateCostInDirection	(int,int,int,int);

};

#endif // #ifndef HEADER_INCLUDED__Cost_Anisotropic_H
