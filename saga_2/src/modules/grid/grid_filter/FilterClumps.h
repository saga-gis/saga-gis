/*******************************************************************************
    FilterClumps.h
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
#include "MLB_Interface.h"

class CFilterClumps : public CModule_Grid
{
public:
	CFilterClumps(void);
	virtual ~CFilterClumps(void);

protected:

	bool On_Execute(void);

private:

	int CalculateCellBlockArea();
	void EliminateClump();

	CSG_Points_Int	m_CentralPoints;
	CSG_Points_Int	m_AdjPoints;

	CGrid *m_pInputGrid;
	CGrid *m_pOutputGrid;
	CGrid *m_pMaskGrid;
	CGrid *m_pMaskGridB;

};