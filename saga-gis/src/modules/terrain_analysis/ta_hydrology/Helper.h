/**********************************************************
 * Version $Id: Helper.h 1016 2011-04-27 18:40:36Z oconrad $
 *********************************************************/
/*******************************************************************************
    Helper.h
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

#include <saga_api/saga_api.h>

#define GLOBAL_BASIN -1

void Init_FlowDirectionsD8(CSG_Grid *pDEM, CSG_Grid *pDirection);
void getNextCell(CSG_Grid*,int,int,int &,int &);
void getNextCell(CSG_Grid*,CSG_Grid*,int,int,int&,int&);
double FlowDistance(CSG_Grid*,CSG_Grid*,int,int,int,int,int);

double CalculateBasinGrid(CSG_Grid*, CSG_Grid*, int, int);
void CalculateFlowAccGrid(CSG_Grid*, CSG_Grid*);

//TSG_Point ** RiverProfile(int, int, CSG_Grid*, CSG_Grid*, CSG_Grid*, int &);
//TSG_Point  * RiverCoords (int, int, CSG_Grid*, CSG_Grid*, int &);
//float DrainageDensity(CSG_Shapes*, CSG_Shapes*, CSG_Grid*, CSG_Grid*);
//void ClosingPoint(CSG_Grid*, CSG_Grid*, int &, int &);
