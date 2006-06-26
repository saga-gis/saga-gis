/*******************************************************************************
    Simulate.h
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
#include "fireLib.h"

class CSimulate : public CModule_Grid {

private:

	CGrid *m_pDEM;    
    CGrid *m_pIgnGrid;           /* ptr to ignition time map (minutes) */
    CGrid *m_pWindSpdGrid;       /* ptr to wind speed map (m/s) */
    CGrid *m_pWindDirGrid;       /* ptr to wind direction map (deg from north) */
    CGrid *m_pM1Grid;            /* ptr to 1-hr dead fuel moisture map */
    CGrid *m_pM10Grid;           /* ptr to 10-hr dead fuel moisture map */
    CGrid *m_pM100Grid;          /* ptr to 100-hr dead fuel moisture map */
    CGrid *m_pMHerbGrid;         /* ptr to live herbaceous fuel moisture map */
    CGrid *m_pMWoodGrid;         /* ptr to live stem fuel moisture map */
	CGrid *m_pFuelGrid;          /* ptr to fuel model map */
	CGrid *m_pValueGrid;

	CGrid *m_pFlameGrid;         /* ptr to flame length map (m) */
	CGrid *m_pIntensityGrid;     

    CGrid *m_pSlopeGrid;         /* ptr to slope map (rise/reach) */
    CGrid *m_pAspectGrid;        /* ptr to aspect map (degrees from north) */

	CGrid *m_pTimeGrid;
	//CGrid *m_pVolatileTimeGrid;
	
	FuelCatalogPtr m_Catalog;    /* fuel catalog handle */

	int m_iLength;

	CAPI_iPoints	m_CentralPoints;
	CAPI_iPoints	m_AdjPoints;

	int *m_pLength; 
	int m_iMaxTime; //in s
	int m_iTimeInterval; //in s

	bool AssignParameters();
	void CalculateFire();
	int CalculateFireSpreading(float);
	void DeleteObjects();

public:

	CSimulate(void);
	virtual ~CSimulate(void);

protected:

	virtual bool On_Execute(void);

};

