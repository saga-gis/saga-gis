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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 
#ifndef HEADER_INCLUDED__Cost_Isotropic_H
#define HEADER_INCLUDED__Cost_Isotropic_H

#include "MLB_Interface.h"
#include "PointsEx.h"

class CCost_Isotropic : public CSG_Module_Grid
{
public:

	CCost_Isotropic(void);
	virtual ~CCost_Isotropic(void);

	virtual const SG_Char *	Get_MenuPath	(void)	{	return( _TL("R:Cost Analysis") );	}


protected:	

	bool					On_Execute		(void);


private:

	double					m_dThreshold;

	CPointsEx				m_CentralPoints;
	CPointsEx				m_AdjPoints;

	CSG_Grid					*m_pCostGrid;
	CSG_Grid					*m_pPointsGrid;	
	CSG_Grid					*m_pAccCostGrid;
	CSG_Grid					*m_pClosestPtGrid;


	void					CalculateCost	(void);
	
};

#endif // #ifndef HEADER_INCLUDED__Cost_Isotropic_H
