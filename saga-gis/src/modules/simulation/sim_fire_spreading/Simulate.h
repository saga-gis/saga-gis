/**********************************************************
 * Version $Id: Simulate.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
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

class CSimulate : public CSG_Module_Grid {

private:

	CSG_Grid *m_pDEM;    
    CSG_Grid *m_pIgnGrid;           /* ptr to ignition time map (minutes) */
    CSG_Grid *m_pWindSpdGrid;       /* ptr to wind speed map (m/s) */
    CSG_Grid *m_pWindDirGrid;       /* ptr to wind direction map (deg from north) */
    CSG_Grid *m_pM1Grid;            /* ptr to 1-hr dead fuel moisture map */
    CSG_Grid *m_pM10Grid;           /* ptr to 10-hr dead fuel moisture map */
    CSG_Grid *m_pM100Grid;          /* ptr to 100-hr dead fuel moisture map */
    CSG_Grid *m_pMHerbGrid;         /* ptr to live herbaceous fuel moisture map */
    CSG_Grid *m_pMWoodGrid;         /* ptr to live stem fuel moisture map */
	CSG_Grid *m_pFuelGrid;          /* ptr to fuel model map */
	CSG_Grid *m_pValueGrid;

	CSG_Grid *m_pFlameGrid;         /* ptr to flame length map (m) */
	CSG_Grid *m_pIntensityGrid;     

    CSG_Grid *m_pSlopeGrid;         /* ptr to slope map (rise/reach) */
    CSG_Grid *m_pAspectGrid;        /* ptr to aspect map (degrees from north) */

	CSG_Grid *m_pTimeGrid;
	//CSG_Grid *m_pVolatileTimeGrid;
	
	FuelCatalogPtr m_Catalog;    /* fuel catalog handle */

	int m_iLength;

	CSG_Points_Int	m_CentralPoints;
	CSG_Points_Int	m_AdjPoints;

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

