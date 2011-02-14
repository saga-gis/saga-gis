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

class CThresholdBuffer : public CSG_Module_Grid {

private:

	CSG_Grid* m_pFeatures; 
	CSG_Grid* m_pBuffer;
	CSG_Grid* m_pValueGrid;
	CSG_Grid* m_pThresholdGrid;
	double m_dThreshold;
	int m_iThresholdType;
//	CAPI_3D_iPoints	m_pCentralPoints;
//	CAPI_3D_iPoints	m_pAdjPoints;
	CSG_Points_Int	m_pCentralPoints;
	CSG_Points_Int	m_pAdjPoints;
	
	void BufferPoint(int,int);

public:

	CThresholdBuffer(void);
	virtual ~CThresholdBuffer(void);

	virtual const SG_Char *	Get_MenuPath	(void)	{	return( _TL("R:Buffers") );	}

protected:
	virtual bool On_Execute(void);

};

