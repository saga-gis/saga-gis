/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    ThresholdBuffer.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MLB_Interface.h"

class CThresholdBuffer : public CSG_Module_Grid
{
public:
	CThresholdBuffer(void);
	virtual ~CThresholdBuffer(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Grid|Distances") );	}


protected:

	virtual bool				On_Execute				(void);


private:

	int							m_iThresholdType;

	double						m_dThreshold;

	CSG_Grid					*m_pFeatures, *m_pBuffer, *m_pValueGrid, *m_pThresholdGrid;

	CSG_Points_Int				m_pCentralPoints, m_pAdjPoints;


	void						BufferPoint				(int x, int y);

};
