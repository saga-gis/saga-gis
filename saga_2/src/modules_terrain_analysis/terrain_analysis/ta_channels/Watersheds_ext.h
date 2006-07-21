/*******************************************************************************
    Watersheds.h
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

#define FRAGMENTATION_TO_MAIN 0
#define FRAGMENTATION_ALL 1

#include "MLB_Interface.h"

class CWatersheds_ext : public CModule_Grid {

private:
	CGrid *m_pDEM;
	CGrid* m_pCNGrid;
	CGrid* m_pSoilLossGrid;
	CGrid* m_pChannelsGrid;
	CGrid *m_pBasinGrid;
	CGrid *m_pDistanceGrid;
	CShapes* m_pBasins;
	CShapes* m_pHeaders;
	CSG_Points	m_Headers;
	int m_fCells;
	int m_iNumBasins;
	float *m_fMaxDistance;
	float *m_fHeightDif;
	int m_iClosingX, m_iClosingY;
	int m_iFragmentationType;
	void CalculateBasin();
	void CreateShapesLayer();
	bool isHeader(int,int);
	bool isLastCell(int,int);
	bool isTopHeader(CSG_Points &,int, bool*);
	void WriteBasin(int,int,int);
	void DeleteBasin(int,int,int);
	float DistanceToClosingPoint(int,int);
	float OrographicIndex(float,float);
	float MassivityIndex(float,float);
	CSG_String GraveliusType(float,float);
	void EquivalentRectangle(float,float,float&,float&);

public:
	CWatersheds_ext(void);
	virtual ~CWatersheds_ext(void);

protected:
	virtual bool		On_Execute(void);

};

