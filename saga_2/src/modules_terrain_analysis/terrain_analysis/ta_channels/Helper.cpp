/*******************************************************************************
    Helper.cpp
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

#include "Helper.h"

// getNextCell() >> changed by O.Conrad, 28 July 2005...
void getNextCell(CSG_Grid *g, int iX, int iY, int &iNextX, int &iNextY)
{
	int		Dir	= g->Get_Gradient_NeighborDir(iX, iY, true);

	if( Dir >= 0 )
	{
		iNextX	= g->Get_System().Get_xTo(Dir, iX);
		iNextY	= g->Get_System().Get_yTo(Dir, iY);
	}
	else
	{
		iNextX	= iX;
		iNextY	= iY;
	}
}

/*void getNextCell(
		CSG_Grid *g,
		int iX,
        int iY,
		int &iNextX,
		int &iNextY) {

    float fDist;
    float fMaxSlope;
    float fSlope;

    fMaxSlope = 0;
    fSlope = 0;

    if (iX < 1 || iX >= g->Get_NX() - 1 || iY < 1 || iY >= g->Get_NY() - 1
            || g->is_NoData(iX,iY)) {
        iNextX = iX;
		iNextY = iY;
		return;
    }// if

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {                	
            if (!g->is_NoData(iX + i, iY + i)){
                if (i == 0 || j == 0) {
                    fDist = 1.0f;
                }// if
                else {
                    fDist = 1.414f;
                }// else
                fSlope = (g->asFloat(iX+i,iY+j)
                         - g->asFloat(iX,iY)) / fDist;                                				
                if (fSlope <= fMaxSlope) {
                    iNextX = iX+i;
					iNextY = iY+j;                        
                    fMaxSlope = fSlope;
                }// if
            }//if                    
        }// for
    }// for

}// method*/

void getNextCell(
		CSG_Grid *g,
		CSG_Grid *g2,
		int iX,
        int iY,
		int &iNextX,
		int &iNextY) {
	
	double fDist;
    double fMaxSlope;
    double fSlope;

    fMaxSlope = 0.0000001;
    fSlope = 0;

    if (iX < 1 || iX >= g->Get_NX() - 1 || iY < 1 || iY >= g->Get_NY() - 1
            || g->is_NoData(iX,iY)) {
        iNextX = iX;
		iNextY = iY;
		return;
    }// if

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {                	
            if (!g->is_NoData(iX+i,iY+j) &&
				!g2->is_NoData(iX+i,iY+j)){ 
                if (i == 0 || j == 0) {
                    fDist = 1.0f;
                }// if
                else {
                    fDist = 1.414f;
                }// else
                fSlope = (g->asFloat(iX+i,iY+j)
                         - g->asFloat(iX,iY)) / fDist;                                				
                if (fSlope < fMaxSlope) {
                    iNextX = iX+i;
					iNextY = iY+j;                        
                    fMaxSlope = fSlope;
                }// if
            }//if                    
        }// for
    }// for

}// method

double FlowDistance(CSG_Grid *pDEM,
					CSG_Grid *pBasinGrid,
					int iBasin,
				    int iX,
				    int iY,
				    int iX2,
				    int iY2){// result in m, coords in grid coords. Returns 0 if no distance is calculated.
            
    bool bIsInBasin;
	double dDist = 1;
	int iNextX = iX;
	int iNextY = iY;      

    if (iX2 <= 0 || iX2 >= pDEM->Get_NX() || iY2 <= 0 || iY2 >= pDEM->Get_NY() || 
            iX <= 0 || iX >= pDEM->Get_NX() || iY <= 0 || iY >= pDEM->Get_NY() ) {
        return 0;
    }// if
    do {
        iX = iNextX;
		iY = iNextY;
        getNextCell(pDEM,iX,iY,iNextX,iNextY);            
        if (fabs((double)(iX - iNextX + iY - iNextY)) == 1.0) {
            dDist = dDist + pDEM->Get_Cellsize();
        }// if
        else {
            dDist = dDist + 1.414 * pDEM->Get_Cellsize();
        }// else
        if (iX == iX2 && iY == iY2) {
            return dDist;
        }// if
		if (iBasin == GLOBAL_BASIN){
			bIsInBasin = !pBasinGrid->is_NoData(iX,iY);
		}//if
		else{
			bIsInBasin = (pBasinGrid->asInt(iX,iY) == iBasin);
		}//else
    }while (bIsInBasin
		&&(iX!=iNextX || iY!=iNextY));

    return 0;

}// method

/*
#include <vector>

TSG_Point ** RiverProfile(int iX, 
				  int iY, 
				  CSG_Grid* pDEM, 
				  CSG_Grid* pBasinGrid,
				  CSG_Grid* pExtGrid,
				  int &iProfileLength){

    int i;
	float fLength = 0;        	
	int iNextX, iNextY;
	CSG_Points	Profile;
	CSG_Points	Ext;
	TSG_Point		P;
	TSG_Point		**pProfile;
	iProfileLength = 0;
	
	if (!pBasinGrid->is_NoData(iX,iY)) {
        iNextX = iX;
		iNextY = iY;
		do {
			iX = iNextX;
			iY = iNextY;
			getNextCell(pDEM, iX, iY, iNextX, iNextY);
		
			if (fabs(iX - iNextX + iY - iNextY) == 1) {
	            fLength += pDEM->Get_DX();
            }//if
            else {
				fLength += (1.414f * pDEM->Get_DX());
            }//else			
			P.x = fLength;
			P.y = pDEM->asFloat(iNextX, iNextY);
			Profile.push_back(P);
			P.y = pExtGrid->asFloat(iNextX, iNextY);
			Ext.push_back(P);
		}while (!pBasinGrid->is_NoData(iX, iY)
			&& (iX != iNextX || iY != iNextY));

		pProfile = new Pt*[2];
		for (i = 0; i<2; i++){
			pProfile[i] = new Pt [Profile.size()];
		}//for
		for (i = 0; i<Profile.size(); i++){			
			pProfile[0][i]=Profile.at(i);		
			pProfile[1][i]=Ext.at(i);		
		}//for
		iProfileLength = Profile.size();
	
	}// if

	return pProfile;

}//method

Pt* RiverCoords(int iX,  //the resulting coords are grid coords, 
				  int iY, 
				  CSG_Grid* pDEM, 
				  CSG_Grid* pBasinGrid, 				   
				  int &iProfileLength){

    float fLength = 0;        	
	int iNextX, iNextY;
	CSG_Points	Profile;
	TSG_Point		P;
	TSG_Point		*pProfile;
	iProfileLength = 0;
	
	if (!pBasinGrid->is_NoData(iX,iY)) {
        iNextX = iX;
		iNextY = iY;
		do {
			iX = iNextX;
			iY = iNextY;
			getNextCell(pDEM, iX, iY, iNextX, iNextY);				
			P.x = iX;
			P.y = iY;
			Profile.push_back(P);
		}while (!pBasinGrid->is_NoData(iX, iY)
			&& (iX != iNextX || iY != iNextY));

		pProfile = new Pt [Profile.size()];
		for (int i = 0; i<Profile.size(); i++){
			pProfile[i]=Profile.at(i);
		}//for
		iProfileLength = Profile.size();
	
	}// if

	return pProfile;

}//method

float DrainageDensity(CSG_Shapes *pHeaders,
					  CSG_Shapes *pBasins,
					  CSG_Grid *pBasinGrid,
					  CSG_Grid *pDEM){

	CSG_Grid * pChannelsGrid;	
	float fLength = 0;        
	int iX, iY;
	int iNextX, iNextY;

	pChannelsGrid = new CSG_Grid(pDEM, GRID_TYPE_Byte);
	pChannelsGrid->Assign(0);
		
	for (int i = 0; i < pHeaders->Get_Count(); i++) {

        iX = (pHeaders->Get_Shape(i)->Get_Point(0).x - pBasinGrid->Get_XMin())/pBasinGrid->Get_DX();
		iY = (pHeaders->Get_Shape(i)->Get_Point(0).y - pBasinGrid->Get_YMin())/pBasinGrid->Get_DX();  

        if (!pBasinGrid->is_NoData(iX,iY)) {
                iNextX = iX;
				iNextY = iY;
			do {
				iX = iNextX;
				iY = iNextY;
				getNextCell(pDEM, iX, iY, iNextX, iNextY);
			
				if (fabs(iX - iNextX + iY - iNextY) == 1) {
	                fLength += pChannelsGrid->Get_DX();
                }//if
                else {
					fLength += (1.414f * pChannelsGrid->Get_DX());
                }//else
                if (pChannelsGrid->asFloat(iX,iY) == 0) {
				    pChannelsGrid->Set_Value(iX, iY, 1);
                }// if
                else {
					break;
                }// else								
			}while (!pBasinGrid->is_NoData(iX, iY)
				&& (iX != iNextX || iY != iNextY));

		}// if
	}// for	

	float fArea = pBasins->Get_Shape(0)->Get_Record()->asFloat(4);
		
	return fLength / fArea / 10000.0;

}//method

void ClosingPoint(CSG_Grid* pDEM, 
				  CSG_Grid* pBasinGrid, 
				  int &iClosingX,
				  int &iClosingY){

    int x,y;
	int iX,iY;
	int iNextX, iNextY;

	for (x = 0; x<pBasinGrid->Get_NX(); x++){
		for (y = 0; y<pBasinGrid->Get_NY(); y++){
			if (!pBasinGrid->is_NoData(x,y)){
				iX = x;
				iY = y;
				goto out;
			}//if
		}//for
	}//for
	return;
out:

    iNextX = iX;
	iNextY = iY;
	do {
		iX = iNextX;
		iY = iNextY;
		getNextCell(pDEM, iX, iY, iNextX, iNextY);
	}while (!pBasinGrid->is_NoData(iX, iY)
		&& (iX != iNextX || iY != iNextY));

	iClosingX = iX;
	iClosingY = iY;

}//method*/