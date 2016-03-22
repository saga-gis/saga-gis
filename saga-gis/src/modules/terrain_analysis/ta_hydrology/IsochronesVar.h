/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    IsochronesVar.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MLB_Interface.h"

class CIsochronesVar : public CSG_Module_Grid_Interactive
{
public:
	CIsochronesVar(void);
	virtual ~CIsochronesVar(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Dynamics" ));	}

protected:
	virtual bool On_Execute(void);
	virtual bool On_Execute_Finish();
	virtual bool On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);

private:
	CSG_Grid *m_pDEM;
	CSG_Grid *m_pTime;
	CSG_Grid *m_pSpeed;
	CSG_Grid *m_pManning;
	CSG_Grid *m_pCN;
	CSG_Grid *m_pCatchArea;
	CSG_Grid *m_pSlope;
	CSG_Grid m_Direction;
	double m_dManning;
	double m_dCN;
	double m_dRainfall;
	double m_dMixedThresh;
	double m_dChannelThresh;
	double m_dChannelSlope;
	double m_dMinSpeed;
	void ZeroToNoData(void);
	double Runoff(double, double);

	void _CalculateTime(int x, int y);

};

