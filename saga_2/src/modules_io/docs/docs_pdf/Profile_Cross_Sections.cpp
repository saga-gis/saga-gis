/*******************************************************************************
    CrossSections.cpp
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

///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Profile_Cross_Sections.h"

#include "Polygon_Clipper.h"


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define OFFSET_X 100
#define OFFSET_Y 50
#define TABLE_WIDTH 450
#define TABLE_LANDSCAPE_WIDTH 680
#define TABLE_OFFSET_X 70
#define TABLE_CELLPAD 10
#define SECTIONS_PER_PAGE 3
#define CROSS_SECTION_HEIGHT 200
#define CROSS_SECTION_WIDTH 400
#define PROFILE_HEIGHT 320
#define PROFILE_WIDTH 1000
#define GUITAR_LEYEND_WIDTH 50
#define GUITAR_LEYEND_HEIGHT 230
#define GUITAR_WIDTH PROFILE_WIDTH
#define GUITAR_HEIGHT GUITAR_LEYEND_HEIGHT
#define SEPARATION 35
#define VOLUMES_TABLE_COLUMNS 7
#define VOLUMES_TABLE_COLUMN_WIDTH ((596 - 2 * OFFSET_X) / VOLUMES_TABLE_COLUMNS)
#define VOLUMES_TABLE_ROW_HEIGHT 20
#define VOLUMES_TABLE_HEADER_SEPARATION 35
#define VOLUMES_TABLE_MAX_ROWS 30
#define SLOPE_RATIO (1. / 3.)
#define LONG_SEGMENT 5000.


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//Compute the dot product AB ? BC
double dot(double *A, double *B, double *C){
    
	double *AB = new double[2];
    double *BC = new double[2];
    AB[0] = B[0]-A[0];
    AB[1] = B[1]-A[1];
    BC[0] = C[0]-B[0];
    BC[1] = C[1]-B[1];
    double dot = AB[0] * BC[0] + AB[1] * BC[1];
    
	return dot;
}

//---------------------------------------------------------
//Compute the cross product AB x AC
double cross(double *A, double *B, double *C){
    
	double *AB = new double[2];
    double *AC = new double[2];
    AB[0] = B[0]-A[0];
    AB[1] = B[1]-A[1];
    AC[0] = C[0]-A[0];
    AC[1] = C[1]-A[1];
    double cross = AB[0] * AC[1] - AB[1] * AC[0];
    
	return cross;
}

//---------------------------------------------------------
//Compute the distance from A to B
double distance(double *A, double *B){
    double d1 = A[0] - B[0];
    double d2 = A[1] - B[1];
    return sqrt(d1*d1+d2*d2);
}

//---------------------------------------------------------
//Compute the distance from AB to C
double linePointDist(double *A, double *B, double *C){
    double dist = cross(A,B,C) / distance(A,B);

    return fabs(dist);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SLOPE_TRANSITION 1
#define DITCH_WIDTH 1

CProfile_Cross_Sections_PDF::CProfile_Cross_Sections_PDF(){}

CProfile_Cross_Sections_PDF::~CProfile_Cross_Sections_PDF(){}

void CProfile_Cross_Sections_PDF::AddCrossSections(TSG_Point ** pCrossSections, 
									 double *pHeight,
									 TSG_Point * pRoadSection,
									 int iSections,
									 int iCrossSectionPoints,
									 int iRoadPoints){
	
	int i,j;
	std::vector<TSG_Point> ModifiedRoadSection;
	std::vector<TSG_Point> ModifiedCrossSection;
	TSG_Point  *pModifiedRoadSection;
	TSG_Point  *pModifiedCrossSection;
	int iCrossSectionPointsB;
	int iRoadPointsB;
	
	Add_Page_Title (_TL("Cross Sections"), PDF_TITLE_01, PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT);

	for (i = 0; i < iSections; i++){
		m_iOffsetY = OFFSET_Y + (int)(CROSS_SECTION_HEIGHT * 1.1 * (i % SECTIONS_PER_PAGE));
		if (m_iOffsetY == OFFSET_Y){
			Add_Page();
		}//if
		
		iCrossSectionPointsB = iCrossSectionPoints;
		iRoadPointsB = iRoadPoints;
		AdjustSections(pCrossSections[i], pRoadSection, ModifiedCrossSection, ModifiedRoadSection,
						pHeight[i], iCrossSectionPointsB, iRoadPointsB);

		pModifiedCrossSection = new TSG_Point [iCrossSectionPointsB];
		pModifiedRoadSection = new TSG_Point [iRoadPointsB];
		for (j = 0; j < iCrossSectionPointsB; j++){
			pModifiedCrossSection[j] = ModifiedCrossSection[j];
		}//for
		for (j = 0; j < iRoadPointsB; j++){
			pModifiedRoadSection[j] = ModifiedRoadSection[j];
		}//for
		
		AddCrossSection(pModifiedCrossSection, pModifiedRoadSection,
						 iCrossSectionPointsB, iRoadPointsB);

		delete [] pModifiedRoadSection;
		delete [] pModifiedCrossSection;
	}//for
	
}//method

void CProfile_Cross_Sections_PDF::AddCrossSection(TSG_Point * pCrossSection,
									 TSG_Point * pRoadSection,
									 int iCrossSectionPoints,
									 int iRoadPoints){

	int i;	
    int iMag;
	int iNumLines;
	int iFirstPoint, iLastPoint;
    double fMinLine = 0;
	double fMaxY, fMinY;
	double fMaxX, fMinX;
	double fStep;
    double fX, fY;
	double fOffsetX;
	double fDifX, fDifY;
	double fRatio;
	double fHeight;
	CSG_String sValue;
	CSG_Points Points;

	iFirstPoint = 0;
	iLastPoint = iCrossSectionPoints - 1;

	for (i = 1; i < iCrossSectionPoints; i++) {
		if (pCrossSection[i].x > pRoadSection[0].x){
			iFirstPoint = i - 1;
			break;
		}//if
	}//for
	for (i = 1; i < iCrossSectionPoints; i++) {
		if (pCrossSection[i].x > pRoadSection[iRoadPoints - 1].x){
			iLastPoint = i;
			break;
		}//if
	}//for
    
	fMinX = pCrossSection[iFirstPoint].x;
	fMaxX = pCrossSection[iLastPoint].x;

	fDifX = fMaxX - fMinX;

	fMaxY  = pCrossSection[0].y;
	fMinY  = pCrossSection[0].y;
		    		    
	for (i = iFirstPoint ; i < iLastPoint + 1; i++) {
	    if (pCrossSection[i].y > fMaxY) {
	        fMaxY = pCrossSection[i].y;
	    }// if
	    if (pCrossSection[i].y < fMinY) {
	        fMinY = pCrossSection[i].y;
	    }// if
    }//for    
	
	for (i = 0; i < iRoadPoints; i++) {
	    if (pRoadSection[i].y > fMaxY) {
	        fMaxY = pRoadSection[i].y;
	    }// if
	    if (pRoadSection[i].y < fMinY) {
	        fMinY = pRoadSection[i].y;
	    }// if
    }//for 

	fDifY = fMaxY - fMinY;

	fRatio = (fDifX / fDifY) / (CROSS_SECTION_WIDTH / CROSS_SECTION_HEIGHT);
	if (fRatio > 1){
		fMaxY = fMaxY + fDifY / 2. * (fRatio - 1.);
		fMinY = fMinY - fDifY / 2. * (fRatio - 1.);

	}//if
	else{
		fMaxX = fMaxX + fDifX / 2. * (1. / fRatio - 1.);
		fMinX = fMinX - fDifX / 2. * (1. / fRatio - 1.);
	}//else

	fMaxX = fMaxX + fDifX * .05;
	fMinX = fMinX - fDifX * .05;
	fMaxY = fMaxY + fDifY * .05;
	fMinY = fMinY - fDifY * .05;

	iMag = (int) (log(fMaxY - fMinY) / log(10.0));
	fStep = (double) ((int) pow(10.0, (double) iMag));                
	if (fStep == 0){
		fStep = 1.;
	}//if
	if (fMinY < 0) {        	
		iMag = (int) (log(fabs(fMinY)) / log(10.0));
		fMinLine = -(int) pow(10.0, (double) iMag);
	}// if
	else {
		fMinLine=(int)(((int)(fMinY/fStep)) *fStep);
	}//else

	iNumLines = (int) ((fMaxY - fMinY) / fStep);

	while (iNumLines < 8) {
		fStep = fStep / 2.0;
		iNumLines = (int) ((fMaxY - fMinY) / fStep);
	}// while
	iNumLines = (int) ((fMaxY - fMinLine) / fStep);

	fHeight = Get_Size_Page().Get_YMax() - Get_Size_Page().Get_YMin();

	//horizontal lines

	for (i = 0; i < iNumLines; i++) {
        fY = m_iOffsetY + CROSS_SECTION_HEIGHT
                - ((fMinLine + fStep * i - fMinY) / (fMaxY - fMinY)) * CROSS_SECTION_HEIGHT;		
        if (fY <= m_iOffsetY + CROSS_SECTION_HEIGHT && fY >= m_iOffsetY) {
			fY = fHeight - fY;
			sValue = SG_Get_String(fMinLine + fStep * (double) i);			
			Draw_Text(OFFSET_X - 10, fY, SG_Get_String(fMinLine + fStep * (double) i).c_str(),
					9, PDF_STYLE_TEXT_ALIGN_H_RIGHT);
			Draw_Line(OFFSET_X, fY, OFFSET_X - 5, fY, 0);
        }// if
    }// for	

	iMag = (int) (log(fMaxX - fMinX) / log(10.0));
	fStep = (double) pow(10.0, (double) iMag);                

	iNumLines = (int) ((fMaxX-fMinX) / fStep);
	
	while (iNumLines < 3) {
		fStep = fStep / 2.0;
		iNumLines = (int) ((fMaxX-fMinX) / fStep);
	}// while    
	fMinLine = (int)(((int)(fMinX/fStep)) * fStep);          
	while (fMinLine<fMinX){
		fMinLine+=fStep;
	}//while
	fOffsetX = fMinX-fMinLine;

	//vertical lines

	for (i = 0; i < iNumLines; i++) {
		fX = OFFSET_X + ((fStep * (double) i-fOffsetX) / (fMaxX-fMinX)) * CROSS_SECTION_WIDTH;
		if (fX >= OFFSET_X && fX < OFFSET_X + CROSS_SECTION_WIDTH){
			if (fabs(fStep * (double)i+fMinLine) > 100000) {
				sValue = SG_Get_String(fStep * (double)i+fMinLine);
			}// if 
			else {
				sValue = SG_Get_String(fStep * (double)i+fMinLine, 2, true);
			}// else
			Draw_Line (fX, fHeight - m_iOffsetY - CROSS_SECTION_HEIGHT,
						fX, fHeight - m_iOffsetY - CROSS_SECTION_HEIGHT - 10);
			Draw_Text(fX, fHeight - m_iOffsetY - CROSS_SECTION_HEIGHT - 15, 
					SG_Get_String(fMinLine + fStep * (double) i).c_str(), 9, 
					PDF_STYLE_TEXT_ALIGN_H_CENTER | PDF_STYLE_TEXT_ALIGN_V_TOP );
		}//if
	}// for
	
	//axis
	
	Draw_Line (OFFSET_X, fHeight - m_iOffsetY, 
				OFFSET_X, fHeight - m_iOffsetY - CROSS_SECTION_HEIGHT, 2);
	Draw_Line (OFFSET_X, fHeight - m_iOffsetY - CROSS_SECTION_HEIGHT, 
				OFFSET_X + CROSS_SECTION_WIDTH, 
				fHeight - m_iOffsetY - CROSS_SECTION_HEIGHT, 2);

	//cross sections
	
	fY = m_iOffsetY + CROSS_SECTION_HEIGHT 
			- CROSS_SECTION_HEIGHT * ((pCrossSection[iFirstPoint].y - fMinY) / (fMaxY - fMinY));
	fY = fHeight - fY;
	fX = OFFSET_X + CROSS_SECTION_WIDTH * ((pCrossSection[iFirstPoint].x - fMinX) / (fMaxX - fMinX));

	Points.Add(fX,fY);
	
	for (i = iFirstPoint + 1; i < iLastPoint + 1; i++) {
		fY = m_iOffsetY + CROSS_SECTION_HEIGHT 
			- CROSS_SECTION_HEIGHT * ((pCrossSection[i].y - fMinY) / (fMaxY - fMinY));
		fY = fHeight - fY;
		fX = OFFSET_X + CROSS_SECTION_WIDTH * ((pCrossSection[i].x - fMinX) / (fMaxX - fMinX));
		Points.Add(fX,fY);
		if (pCrossSection[i].x > fMaxX){
			break;
		}//if
	}// for
	Draw_Line(Points, 2, 0x00bb00);

	fY = m_iOffsetY + CROSS_SECTION_HEIGHT 
			- CROSS_SECTION_HEIGHT * ((pRoadSection[0].y - fMinY) / (fMaxY - fMinY));
	fY = fHeight - fY;
	fX = OFFSET_X + CROSS_SECTION_WIDTH * ((pRoadSection[0].x - fMinX) / (fMaxX - fMinX));

	Points.Clear();
	Points.Add(fX,fY);	        
	
	for (i = 1; i < iRoadPoints; i++) {
		fY = m_iOffsetY + CROSS_SECTION_HEIGHT 
			- CROSS_SECTION_HEIGHT * ((pRoadSection[i].y - fMinY) / (fMaxY - fMinY));
		fY = fHeight - fY;
		fX = OFFSET_X + CROSS_SECTION_WIDTH * ((pRoadSection[i].x - fMinX) / (fMaxX - fMinX));
		Points.Add(fX,fY);	
	}// for
	Draw_Line(Points, 0, 0x0000bb);


}//method

void CProfile_Cross_Sections_PDF::AddVolumesTable(TSG_Point * pProfile,
									TSG_Point ** pCrossSections, 
									 double *pHeight,
									 TSG_Point * pRoadSection,
									 int iSections,
									 int iCrossSectionPoints,
									 int iRoadPoints){


	int i,j;
	int iRow;
	int iRowsWritten = 0;
	double fDist;
	double fX,fY;
	double fXBox, fYBox;
	double fIn, fOut;
	double fInPrev, fOutPrev;
	double fVolume;
	double fHeight;
	CSG_String sHeader[] = {_TL("Section"),_TL("Dist."),_TL("Area +"),_TL("Area -"),_TL("Partial D."),_TL("Vol. +"), _TL("Vol. -")};
			
	Add_Page_Title(_TL("Volumes"), PDF_TITLE_01);

	fHeight = Get_Size_Page().Get_YMax() - Get_Size_Page().Get_YMin();
	
	for (i = 0; i < iSections; i++){
		iRow = iRowsWritten % VOLUMES_TABLE_MAX_ROWS;
		if (iRow == 0){
			Add_Page();
			Draw_Rectangle(OFFSET_X, fHeight - OFFSET_Y,
							OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * VOLUMES_TABLE_COLUMNS,
							fHeight - OFFSET_Y - VOLUMES_TABLE_ROW_HEIGHT);			
			fY = OFFSET_Y + VOLUMES_TABLE_ROW_HEIGHT * .6;
			fY = fHeight - fY;
			for (j = 0; j < VOLUMES_TABLE_COLUMNS; j++){
				fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * j + VOLUMES_TABLE_COLUMN_WIDTH / 2.;				
				Draw_Text(fX, fY, sHeader[j].c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);

				fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * (j + 1);
				Draw_Line(fX, fHeight - OFFSET_Y, 
							fX, fHeight - (OFFSET_Y + VOLUMES_TABLE_ROW_HEIGHT),
							0);
			}//for
			if (i){
				i--;
			}//if
		}//if
		for (j = 0; j < 4; j++){
			fYBox = OFFSET_Y + VOLUMES_TABLE_HEADER_SEPARATION + VOLUMES_TABLE_ROW_HEIGHT * iRow;
			fYBox = fHeight - fYBox;
			fXBox = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * j;			
			Draw_Rectangle(fXBox, fYBox, fXBox + VOLUMES_TABLE_COLUMN_WIDTH, 
							fYBox - VOLUMES_TABLE_ROW_HEIGHT);			
		}//for
		fDist = pProfile[i].x;		
		fY = OFFSET_Y + VOLUMES_TABLE_HEADER_SEPARATION + VOLUMES_TABLE_ROW_HEIGHT * (iRow + .6);
		fY = fHeight - fY;
		fX = OFFSET_X + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
		Draw_Text(fX, fY, SG_Get_String(i,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);

		fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
		Draw_Text(fX, fY, SG_Get_String(fDist,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);

		CalculateAreas(pCrossSections[i], pRoadSection, pHeight[i], iCrossSectionPoints, 
						iRoadPoints,fIn, fOut);

		fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * 2 + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
		Draw_Text(fX, fY, SG_Get_String(fIn,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);

		fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * 3 + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
		Draw_Text(fX, fY, SG_Get_String(fOut,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);

		if (iRow > 0){
			for (j = 4; j < 7; j++){
				fYBox = OFFSET_Y + VOLUMES_TABLE_HEADER_SEPARATION + VOLUMES_TABLE_ROW_HEIGHT * (iRow - .5);
				fYBox = fHeight - fYBox;
				fXBox = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * j;			
				Draw_Rectangle(fXBox, fYBox, fXBox + VOLUMES_TABLE_COLUMN_WIDTH, 
								fYBox - VOLUMES_TABLE_ROW_HEIGHT);				
			}//for
			fY = OFFSET_Y + VOLUMES_TABLE_HEADER_SEPARATION + VOLUMES_TABLE_ROW_HEIGHT * (iRow + .1) ;
			fY = fHeight - fY;

			fDist = (pProfile[i].x - pProfile[i-1].x);
			fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * 4 + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
			Draw_Text(fX, fY, SG_Get_String(fDist,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);

			fVolume = (pProfile[i].x - pProfile[i-1].x) * (fIn + fInPrev) / 2.;
			fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * 5 + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
			Draw_Text(fX, fY, SG_Get_String(fVolume,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);
			
			fVolume = (pProfile[i].x - pProfile[i-1].x) * (fOut + fOutPrev) / 2.;
			fX = OFFSET_X + VOLUMES_TABLE_COLUMN_WIDTH * 6 + (VOLUMES_TABLE_COLUMN_WIDTH) / 2.;
			Draw_Text(fX, fY, SG_Get_String(fVolume,0).c_str(), 10, PDF_STYLE_TEXT_ALIGN_H_CENTER);
		}//if

		fInPrev = fIn;
		fOutPrev = fOut;
		iRowsWritten++;

	}//for

}//Method

void CProfile_Cross_Sections_PDF::AdjustSections(TSG_Point * pCrossSection,
									TSG_Point * pRoadSection,
									std::vector<TSG_Point > &ModifiedCrossSection,
									std::vector<TSG_Point > &ModifiedRoadSection,
									double fHeight,
									int &iCrossSectionPoints, 
									int &iRoadPoints){

	bool bDitchAdded;
	int i,j;
	int iSgn;
	int iRoadPointsToAdd = 0;
	int iCrossSectionPointsToAdd = 0;
	double fX, fY;
	TSG_Point  P;

	ModifiedCrossSection.clear();
	ModifiedRoadSection.clear();

	//left half

	for (i = 0 ; i < iCrossSectionPoints - 1; i++){
		if (pCrossSection[i].x < pRoadSection[0].x && 
				pCrossSection[i+1].x >= pRoadSection[0].x){
			break;
		}//if
	}//for

	if (i == iCrossSectionPoints - 1){
		if (pRoadSection[0].y  + fHeight < pCrossSection[0].y){
			P.x = pRoadSection[0].x - 2* DITCH_WIDTH * SLOPE_TRANSITION - DITCH_WIDTH 
				- fabs(pCrossSection[0].y - pRoadSection[0].y - fHeight) / SLOPE_RATIO;
			P.y = pCrossSection[0].y - fHeight;
			ModifiedRoadSection.push_back(P);
			P.y = pCrossSection[0].y;
			ModifiedCrossSection.push_back(P);
			iCrossSectionPointsToAdd++;
			iRoadPointsToAdd++;			
			
			P.x = pRoadSection[0].x - 2* DITCH_WIDTH * SLOPE_TRANSITION - DITCH_WIDTH;
			P.y = pRoadSection[0].y;
			ModifiedRoadSection.push_back(P);
			iRoadPointsToAdd++;

			P.x = pRoadSection[0].x - DITCH_WIDTH * SLOPE_TRANSITION - DITCH_WIDTH;	
			P.y = pRoadSection[0].y - DITCH_WIDTH / SLOPE_TRANSITION;
			ModifiedRoadSection.push_back(P);
			iRoadPointsToAdd++;

			P.x = pRoadSection[0].x - DITCH_WIDTH * SLOPE_TRANSITION;
			ModifiedRoadSection.push_back(P);
			iRoadPointsToAdd++;
		}//if		
		P.x = pRoadSection[0].x - fabs(pCrossSection[0].y - pRoadSection[0].y - fHeight) / SLOPE_RATIO;
		P.y = pCrossSection[0].y - fHeight;
		ModifiedRoadSection.push_back(P);
		P.y = pCrossSection[0].y;
		ModifiedCrossSection.push_back(P);
		iCrossSectionPointsToAdd++;
		iRoadPointsToAdd++;
	}//if
	else{
		bDitchAdded = false;
		for (j = 0; j < 4; j++){ //4 dummy points
			ModifiedRoadSection.push_back(pRoadSection[0]);
		}//for	
		for (j = i ; j >= 0; j--){
			if (ModifiedRoadSection[1].y  + fHeight > pCrossSection[j+1].y){
				iSgn = -1;
			}//if
			else{
				iSgn = 1;
			}//else
			Intersect_Lines(ModifiedRoadSection[1].x, ModifiedRoadSection[1].y + fHeight, 
							ModifiedRoadSection[1].x - LONG_SEGMENT / SLOPE_RATIO,
							ModifiedRoadSection[1].y + LONG_SEGMENT * iSgn + fHeight,
							pCrossSection[j].x, pCrossSection[j].y,
							pCrossSection[j+1].x, pCrossSection[j+1].y,
							fX,fY);
			if (pCrossSection[j].x < fX && pCrossSection[j+1].x >= fX){
				if (iSgn == 1){
					if(!bDitchAdded){
						ModifiedRoadSection[1].x = pRoadSection[0].x - 2* DITCH_WIDTH * SLOPE_TRANSITION - DITCH_WIDTH;
						ModifiedRoadSection[1].y = pRoadSection[0].y;						
						iRoadPointsToAdd++;

						ModifiedRoadSection[2].x = pRoadSection[0].x - DITCH_WIDTH * SLOPE_TRANSITION - DITCH_WIDTH;	
						ModifiedRoadSection[2].y = pRoadSection[0].y - DITCH_WIDTH / SLOPE_TRANSITION;						
						iRoadPointsToAdd++;

						ModifiedRoadSection[3].x = pRoadSection[0].x - DITCH_WIDTH * SLOPE_TRANSITION;
						ModifiedRoadSection[3].y = pRoadSection[0].y - DITCH_WIDTH / SLOPE_TRANSITION;						
						iRoadPointsToAdd++;
						
						j = i + 1;
						bDitchAdded = true;
					}//if
					else{
						ModifiedRoadSection[0].x = fX;
						ModifiedRoadSection[0].y = fY - fHeight;
						iRoadPointsToAdd++;
						break;
					}//else
				}//if
				else{
					ModifiedRoadSection[0].x = fX;
					ModifiedRoadSection[0].y = fY - fHeight;
					iRoadPointsToAdd++;
					break;
				}//else
			}//if
		}//for
		if (j < 0){
			ModifiedRoadSection[0].x = pRoadSection[0].x - fabs(pCrossSection[0].y - pRoadSection[0].y - fHeight) / SLOPE_RATIO;
			ModifiedRoadSection[0].y = pCrossSection[0].y - fHeight;
			P.x = ModifiedRoadSection[0].x;
			P.y = pCrossSection[0].y;
			ModifiedCrossSection.push_back(P);
			iCrossSectionPointsToAdd++;
			iRoadPointsToAdd++;
		}//if*/
	}//else

	//add the central points

	for (i = 0; i < iCrossSectionPoints; i++){
		ModifiedCrossSection.push_back(pCrossSection[i]);
	}//for
	
	ModifiedRoadSection.resize(iRoadPoints + iRoadPointsToAdd);
	for (i = 0; i < iRoadPoints; i++){
		ModifiedRoadSection[i + iRoadPointsToAdd].x = pRoadSection[i].x;
		ModifiedRoadSection[i + iRoadPointsToAdd].y = pRoadSection[i].y;
	}//for	
	for (i = 0; i < iRoadPoints + iRoadPointsToAdd; i++){
		ModifiedRoadSection[i].x = ModifiedRoadSection[i].x;
		ModifiedRoadSection[i].y = ModifiedRoadSection[i].y + fHeight;
	}//for

	iRoadPoints += iRoadPointsToAdd;
	iCrossSectionPoints += iCrossSectionPointsToAdd;

	// right half

	for (i = 0 ; i < iCrossSectionPoints - 1; i++){
		if (ModifiedCrossSection[i].x < ModifiedRoadSection[iRoadPoints-1].x && 
				ModifiedCrossSection[i+1].x >= ModifiedRoadSection[iRoadPoints-1].x){
			break;
		}//if
	}//for
	if (i == iCrossSectionPoints - 1){
		if (ModifiedRoadSection[iRoadPoints-1].y  + fHeight < ModifiedCrossSection[iCrossSectionPoints-1].y){
			P.x = ModifiedRoadSection[iRoadPoints-1].x + DITCH_WIDTH * SLOPE_TRANSITION;
			P.y = ModifiedRoadSection[iRoadPoints-1].y - DITCH_WIDTH / SLOPE_TRANSITION;
			ModifiedRoadSection.push_back(P);
			iRoadPoints++;

			P.x = ModifiedRoadSection[iRoadPoints-2].x + DITCH_WIDTH * SLOPE_TRANSITION + DITCH_WIDTH;	
			ModifiedRoadSection.push_back(P);
			iRoadPoints++;

			P.x = ModifiedRoadSection[iRoadPoints-3].x + 2* DITCH_WIDTH * SLOPE_TRANSITION + DITCH_WIDTH;
			P.y = ModifiedRoadSection[iRoadPoints-3].y;
			ModifiedRoadSection.push_back(P);
			iRoadPoints++;
		}//if		
		P.x = ModifiedRoadSection[iRoadPoints-1].x 
				+ fabs(ModifiedCrossSection[iCrossSectionPoints-1].y 
				- ModifiedRoadSection[iRoadPoints-1].y) / SLOPE_RATIO;
		P.y = ModifiedCrossSection[iCrossSectionPoints-1].y ;
		ModifiedRoadSection.push_back(P);
		ModifiedCrossSection.push_back(P);
		iCrossSectionPoints++;
		iRoadPoints++;
	}//if
	else{
		bDitchAdded = false;
		for (j = i ; j < iCrossSectionPoints - 1; j++){
			if (ModifiedRoadSection[iRoadPoints-1].y > ModifiedCrossSection[j].y){
				iSgn = -1;
			}//if
			else{
				iSgn = 1;
			}//else
			if (iSgn == 1 && !bDitchAdded){
				P.x = ModifiedRoadSection[iRoadPoints-1].x + DITCH_WIDTH * SLOPE_TRANSITION;
				P.y = ModifiedRoadSection[iRoadPoints-1].y - DITCH_WIDTH / SLOPE_TRANSITION;
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;

				P.x = ModifiedRoadSection[iRoadPoints-2].x + DITCH_WIDTH * SLOPE_TRANSITION + DITCH_WIDTH;	
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;

				P.x = ModifiedRoadSection[iRoadPoints-3].x + 2* DITCH_WIDTH * SLOPE_TRANSITION + DITCH_WIDTH;
				P.y = ModifiedRoadSection[iRoadPoints-3].y;
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;
				
				if (ModifiedRoadSection[iRoadPoints-1].y > ModifiedCrossSection[j].y){
					iSgn = -1;
				}//if
				else{
					iSgn = 1;
				}//else

				bDitchAdded = true;

			}//if

			Intersect_Lines(ModifiedRoadSection[iRoadPoints-1].x, ModifiedRoadSection[iRoadPoints-1].y, 
							ModifiedRoadSection[iRoadPoints-1].x + LONG_SEGMENT / SLOPE_RATIO,
							ModifiedRoadSection[iRoadPoints-1].y + LONG_SEGMENT * iSgn,
							ModifiedCrossSection[j].x, ModifiedCrossSection[j].y,
							ModifiedCrossSection[j+1].x, ModifiedCrossSection[j+1].y,
							fX,fY);

			if (ModifiedCrossSection[j].x < fX && ModifiedCrossSection[j+1].x >= fX){
				P.x = fX;
				P.y = fY;
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;
				break;
			}//if
		}//for
		if (j == iCrossSectionPoints - 1){
			if (ModifiedRoadSection[iRoadPoints-1].y  + fHeight < ModifiedCrossSection[iCrossSectionPoints-1].y
					&& !bDitchAdded){
				
				P.x = ModifiedRoadSection[iRoadPoints-1].x + DITCH_WIDTH * SLOPE_TRANSITION;
				P.y = ModifiedRoadSection[iRoadPoints-1].y - DITCH_WIDTH / SLOPE_TRANSITION;
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;

				P.x = ModifiedRoadSection[iRoadPoints-2].x + DITCH_WIDTH * SLOPE_TRANSITION + DITCH_WIDTH;	
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;

				P.x = ModifiedRoadSection[iRoadPoints-3].x + 2* DITCH_WIDTH * SLOPE_TRANSITION + DITCH_WIDTH;
				P.y = ModifiedRoadSection[iRoadPoints-3].y;
				ModifiedRoadSection.push_back(P);
				iRoadPoints++;

			}//if		
			P.x = ModifiedRoadSection[iRoadPoints-1].x 
					+ fabs(ModifiedCrossSection[iCrossSectionPoints-1].y 
					- ModifiedRoadSection[iRoadPoints-1].y) / SLOPE_RATIO;
			P.y = ModifiedCrossSection[iCrossSectionPoints-1].y ;
			ModifiedRoadSection.push_back(P);
			ModifiedCrossSection.push_back(P);
			iCrossSectionPoints++;
			iRoadPoints++;

		}//if*/
	}//else

}//method

void CProfile_Cross_Sections_PDF::Intersect_Lines(double x0,double y0,double x1,double y1,
									double x2,double y2,double x3,double y3,
									double &xi,double &yi){

	double a1,b1,c1, 
		  a2,b2,c2,
		  det_inv,  
		  m1,m2;    

	if ((x1-x0)!=0)
	   m1 = (y1-y0)/(x1-x0);
	else
	   m1 = (double)1e+10;   

	if ((x3-x2)!=0)
	   m2 = (y3-y2)/(x3-x2);
	else
	   m2 = (double)1e+10;   

	a1 = m1;
	a2 = m2;

	b1 = -1;
	b2 = -1;

	c1 = (y0-m1*x0);
	c2 = (y2-m2*x2);

	det_inv = 1/(a1*b2 - a2*b1);

	xi=((b1*c2 - b2*c1)*det_inv);
	yi=((a2*c1 - a1*c2)*det_inv);

} //method

void CProfile_Cross_Sections_PDF::CalculateAreas(TSG_Point * pCrossSection,
									TSG_Point * pRoadSection,
									double fHeight,
									int iCrossSectionPoints, 
									int iRoadPoints,
									double &fPositiveArea,
									double &fNegativeArea){

	int i;
	CSG_Shapes *pRoadShapes, *pCrossSectionShapes, *pResultShapes;
	CSG_Shape *pRoadShape, *pCrossSectionShape, *pResultShape;

	pResultShapes = new CSG_Shapes;
	pResultShapes->Create(SHAPE_TYPE_Polygon);
	pResultShape = pResultShapes->Add_Shape();
	pRoadShapes = new CSG_Shapes;
	pRoadShapes->Create(SHAPE_TYPE_Polygon);
	pRoadShape = pRoadShapes->Add_Shape();
	pCrossSectionShapes = new CSG_Shapes;
	pCrossSectionShapes->Create(SHAPE_TYPE_Polygon);
	pCrossSectionShape = pCrossSectionShapes->Add_Shape();
	
	for (i = 0; i < iRoadPoints; i++){
		pRoadShape->Add_Point(pRoadSection[i].x, pRoadSection[i].y + fHeight);
	}//for
	pRoadShape->Add_Point(pRoadSection[iRoadPoints-1].x + LONG_SEGMENT / SLOPE_RATIO,
						pRoadSection[iRoadPoints-1].y + fHeight + LONG_SEGMENT);
	
	pRoadShape->Add_Point(pRoadSection[0].x - LONG_SEGMENT / SLOPE_RATIO,
						pRoadSection[0].y + fHeight + LONG_SEGMENT);
						
	for (i = 0; i < iCrossSectionPoints; i++){
		pCrossSectionShape->Add_Point(pCrossSection[i].x, pCrossSection[i].y);
	}//for
	pCrossSectionShape->Add_Point(pCrossSection[iCrossSectionPoints-1].x + LONG_SEGMENT / SLOPE_RATIO,
						pCrossSection[iCrossSectionPoints-1].y - LONG_SEGMENT);
	
	pCrossSectionShape->Add_Point(pCrossSection[0].x - LONG_SEGMENT / SLOPE_RATIO,
						pCrossSection[0].y - LONG_SEGMENT);		

	if (GPC_Intersection(pCrossSectionShape, pRoadShape, pResultShape)){
		fNegativeArea = ((CSG_Shape_Polygon*)pResultShape)->Get_Area();
	}//if
	else{
		fNegativeArea = 0;
	}//else

	pCrossSectionShape->Del_Parts();
	pRoadShape->Del_Parts();
	pResultShape->Del_Parts();
	
	for (i = 0; i < iRoadPoints; i++){
		pRoadShape->Add_Point(pRoadSection[i].x, pRoadSection[i].y + fHeight);
	}//for
	pRoadShape->Add_Point(pRoadSection[iRoadPoints-1].x + LONG_SEGMENT / SLOPE_RATIO,
						pRoadSection[iRoadPoints-1].y + fHeight - LONG_SEGMENT);
	
	pRoadShape->Add_Point(pRoadSection[0].x - LONG_SEGMENT / SLOPE_RATIO,
						pRoadSection[0].y + fHeight - LONG_SEGMENT);
						
	for (i = 0; i < iCrossSectionPoints; i++){
		pCrossSectionShape->Add_Point(pCrossSection[i].x, pCrossSection[i].y);
	}//for
	pCrossSectionShape->Add_Point(pCrossSection[iCrossSectionPoints-1].x + LONG_SEGMENT / SLOPE_RATIO,
						pCrossSection[iCrossSectionPoints-1].y + LONG_SEGMENT);
	
	pCrossSectionShape->Add_Point(pCrossSection[0].x - LONG_SEGMENT / SLOPE_RATIO,
						pCrossSection[0].y + LONG_SEGMENT);		
	
	if (GPC_Intersection(pCrossSectionShape, pRoadShape, pResultShape)){
		fPositiveArea = ((CSG_Shape_Polygon*)pResultShape)->Get_Area();
	}//if
	else{
		fPositiveArea = 0;
	}//else

	delete pRoadShapes;
	delete pCrossSectionShapes;
	delete pResultShapes;
				
}//method

void CProfile_Cross_Sections_PDF::AddLongitudinalProfile(TSG_Point * pProfile,
											double *pHeight,
											int iNumPoints){

	int i;	
    int iMag;
	int iNumLines;
    double fMinLine = 0;
	double fMaxY, fMinY;
	double fMaxX, fMinX;
	double fStep;
    double fX, fY;
	double fOffsetX;
	CSG_String sValue;
	CSG_Points Points;
	double fHeight;
	
	Add_Page_Title(_TL("Longitudinal Profile"), PDF_TITLE_01);
	
	Add_Page(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE);

	fHeight = Get_Size_Page().Get_YMax() - Get_Size_Page().Get_YMin();

	m_iOffsetY = 50;

	fMaxY  = pProfile[0].y;
	fMinY  = pProfile[0].y;
		    		    
	for (i = 0; i < iNumPoints; i++) {
	    if (pProfile[i].y > fMaxY) {
	        fMaxY = pProfile[i].y;
	    }// if
	    if (pProfile[i].y < fMinY) {
	        fMinY = pProfile[i].y;
	    }// if
		if (pHeight[i] > fMaxY) {
	        fMaxY = pHeight[i];
	    }// if
	    if (pHeight[i] < fMinY) {
	        fMinY = pHeight[i];
	    }// if
    }//for    
	
	iMag = (int) (log(fMaxY - fMinY) / log(10.0));
	fStep = (double) ((int) pow(10.0, (double) iMag));                
	if (fStep == 0){
		fStep = 1.;
	}//if
	if (fMinY < 0) {        	
		iMag = (int) (log(fabs(fMinY)) / log(10.0));
		fMinLine = -(int) pow(10.0, (double) iMag);
	}// if
	else {
		fMinLine=(int)(((int)(fMinY/fStep)) *fStep);
	}//else

	iNumLines = (int) ((fMaxY - fMinY) / fStep);

	while (iNumLines < 8) {
		fStep = fStep / 2.0;
		iNumLines = (int) ((fMaxY - fMinY) / fStep);
	}// while
	iNumLines = (int) ((fMaxY - fMinLine) / fStep);

	fHeight = Get_Size_Page().Get_YMax() - Get_Size_Page().Get_YMin();

	for (i = 0; i < iNumLines; i++) {
        fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION;
		fY = m_iOffsetY + PROFILE_HEIGHT
                - ((fMinLine + fStep * i - fMinY) / (fMaxY - fMinY)) * PROFILE_HEIGHT;		
        if (fY <= m_iOffsetY + PROFILE_HEIGHT && fY >= m_iOffsetY) {
			fY = fHeight - fY;
			sValue = SG_Get_String(fMinLine + fStep * (double) i);
			Draw_Text(fX - 10, fY, SG_Get_String(fMinLine + fStep * (double) i).c_str(), 9,
				PDF_STYLE_TEXT_ALIGN_H_RIGHT);
			Draw_Line(fX, fY, fX - 5, fY, 0);
        }// if
    }// for	

	fMinX = pProfile[0].x;
	fMaxX = pProfile[iNumPoints - 1].x;

	iMag = (int) (log(fMaxX - fMinX) / log(10.0));
	fStep = (double) pow(10.0, (double) iMag);                

	iNumLines = (int) ((fMaxX-fMinX) / fStep);
	
	while (iNumLines < 3) {
		fStep = fStep / 2.0;
		iNumLines = (int) ((fMaxX-fMinX) / fStep);
	}// while    
	fMinLine = (int)(((int)(fMinX/fStep)) * fStep);          
	while (fMinLine<fMinX){
		fMinLine+=fStep;
	}//while
	fOffsetX = fMinX-fMinLine;

	for (i = 0; i < iNumLines; i++) {
		fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
				+ ((fStep * (double) i-fOffsetX) / (fMaxX-fMinX)) * PROFILE_WIDTH;
		if (fX>=OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
				&& fX < OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION + PROFILE_WIDTH){
			if (fabs(fStep * (double)i+fMinLine) > 100000) {
				sValue = SG_Get_String(fStep * (double)i+fMinLine);
			}// if 
			else {
				sValue = SG_Get_String(fStep * (double)i+fMinLine, 2, true);
			}// else
			Draw_Text(fX, fHeight - m_iOffsetY - PROFILE_HEIGHT - 15,
				SG_Get_String(fMinLine + fStep * (double) i).c_str(), 9,
				PDF_STYLE_TEXT_ALIGN_H_CENTER | PDF_STYLE_TEXT_ALIGN_V_TOP);
			Draw_Line(fX, fHeight - m_iOffsetY - PROFILE_HEIGHT,
					fX, fHeight - m_iOffsetY - PROFILE_HEIGHT - 10, 0);
		}//if
	}// for
	
	Draw_Line(OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION, 	fHeight - m_iOffsetY,
			OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION, 
			fHeight - m_iOffsetY - PROFILE_HEIGHT, 2);
	Draw_Line(OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION, 
				fHeight - m_iOffsetY - PROFILE_HEIGHT,
				OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION + PROFILE_WIDTH, 
				fHeight - m_iOffsetY - PROFILE_HEIGHT, 2);
		
	fY = m_iOffsetY + PROFILE_HEIGHT 
			- PROFILE_HEIGHT * ((pProfile[0].y - fMinY) / (fMaxY - fMinY));
	fY = fHeight - fY;
	fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
		+ PROFILE_WIDTH * ((pProfile[0].x - fMinX) / (fMaxX - fMinX));
	Points.Add(fX, fY);	        
	for (i = 1; i < iNumPoints; i++) {
		fY = m_iOffsetY + PROFILE_HEIGHT 
			- PROFILE_HEIGHT * ((pProfile[i].y - fMinY) / (fMaxY - fMinY));
		fY = fHeight - fY;
		fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
			+ PROFILE_WIDTH * ((pProfile[i].x - fMinX) / (fMaxX - fMinX));
		Points.Add(fX, fY);
	}// for
	Draw_Line(Points, 2, 0x00bb00);

	Points.Clear();
	
	fY = m_iOffsetY + PROFILE_HEIGHT 
			- PROFILE_HEIGHT * ((pHeight[0] - fMinY) / (fMaxY - fMinY));
	fY = fHeight - fY;
	fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
		+ PROFILE_WIDTH * ((pProfile[0].x - fMinX) / (fMaxX - fMinX));
	Points.Add(fX,fY);
	for (i = 1; i < iNumPoints; i++) {
		fY = m_iOffsetY + PROFILE_HEIGHT 
			- PROFILE_HEIGHT * ((pHeight[i] - fMinY) / (fMaxY - fMinY));
		fY = fHeight - fY;
		fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
			+ PROFILE_WIDTH * ((pProfile[i].x - fMinX) / (fMaxX - fMinX));
		Points.Add(fX, fY);
	}// for
	Draw_Line(Points, 2, 0x0000bb);

	DrawGuitar(pProfile, pHeight, iNumPoints);

}//method

void CProfile_Cross_Sections_PDF::DrawGuitar(TSG_Point *pProfile, 
							   double *pHeight, 
							   int iNumPoints){

	int i;
	double fY;
	double fMinX, fMaxX;
	double fDif;
	CSG_String sValue;
	CSG_String sField [] = {"Distancia","Dist. Parcial.", "Elevacion del terreno", "Elevación de la calzada", " ", " "};
	double fHeight;

	fMinX = pProfile[0].x;
	fMaxX = pProfile[iNumPoints - 1].x;

	fHeight = Get_Size_Page().Get_YMax() - Get_Size_Page().Get_YMin();
	
	for (i = 0; i < 7; i++){
		fY = m_iOffsetY + PROFILE_HEIGHT + SEPARATION + GUITAR_LEYEND_HEIGHT / 6 * i;
		fY = fHeight - fY;
		Draw_Line(OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION, fY,
				OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION + GUITAR_WIDTH, fY, 0, 0x880000);
		if (i){
			Draw_Text(OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION - 10, 
						fY + GUITAR_LEYEND_HEIGHT / 13, 
						sField[i-1].c_str(),
						9,
						PDF_STYLE_TEXT_ALIGN_V_CENTER | PDF_STYLE_TEXT_ALIGN_H_RIGHT);
		
		}//if
	}//for
	
	AddGuitarValue(0, 0, fMinX, fMaxX, 1);
	for (i = 0; i < iNumPoints; i++) {
		AddGuitarValue(pProfile[i].x, pProfile[i].x / 1000, fMinX, fMaxX, 0);
		if (i){
			AddGuitarValue(pProfile[i].x, pProfile[i].x / 1000 - pProfile[i-1].x / 1000, 
							fMinX, fMaxX, 1);
		}//if
		fDif = pProfile[i].y - pHeight[i];
		AddGuitarValue(pProfile[i].x, pProfile[i].y, fMinX, fMaxX, 2);
		AddGuitarValue(pProfile[i].x, pHeight[i], fMinX, fMaxX, 3);
		if (fDif > 0){
			AddGuitarValue(pProfile[i].x, fDif, fMinX, fMaxX, 4);
		}//if
		else{
			AddGuitarValue(pProfile[i].x, fDif, fMinX, fMaxX, 5);
		}//else


	}// for

}//method

void CProfile_Cross_Sections_PDF::AddGuitarValue(double fXValue,
								   double fValue,
								   double fMinX,
								   double fMaxX,								   
								   int iLine){

	double fX,fY;
	double fHeight;
	
	fHeight = Get_Size_Page().Get_YMax() - Get_Size_Page().Get_YMin();
	
	fX = OFFSET_Y + GUITAR_LEYEND_WIDTH + SEPARATION 
		+ PROFILE_WIDTH * ((fXValue - fMinX) / (fMaxX - fMinX));
	fY = m_iOffsetY + PROFILE_HEIGHT + SEPARATION + GUITAR_LEYEND_HEIGHT / 6 * (iLine + 1);
	fY = fHeight - fY;

	Draw_Line(fX, fY, fX, fY + 3, 0, 0x880000);

	fY += 3;
	fX += 3;
	
	Draw_Text(fX, fY, SG_Get_String(fValue, 2).c_str(), 8, 
			0, 
			3.14159/2.);
		
	fY = m_iOffsetY + PROFILE_HEIGHT + SEPARATION + GUITAR_LEYEND_HEIGHT / 6 * iLine;
	fY = fHeight - fY;
	fX -= 3;

	Draw_Line(fX, fY, fX, fY - 3, 0, 0x880000);

}//method


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CProfile_Cross_Sections::CProfile_Cross_Sections(void){

	CSG_Parameter *pNode;

	Parameters.Set_Name(_TL("Cross Sections"));
	Parameters.Set_Description(_TW("(c) 2004 Victor Olaya. Cross Sections"));

	Parameters.Add_Grid(NULL,
						"DEM",
						_TL("DEM"),
						_TL("Digital Terrain Model"),
						PARAMETER_INPUT);

	Parameters.Add_Shapes(NULL,
						"LINES",
						_TL("Path"),
						_TL(""),
						PARAMETER_INPUT, 
						SHAPE_TYPE_Line);

	Parameters.Add_Shapes(NULL,
						"SECTIONS",
						_TL("Cross Sections"),
						_TL(""),
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL,
						"NUMPOINTS",
						_TL("Number of points"),
						_TL("Number of points on each side of the section"),
						PARAMETER_TYPE_Int,
						10,
						1,
						true);

	Parameters.Add_Value(NULL,
						"INTERVAL",
						_TL("Interval"),
						_TL("Interval between points (in grid units)."),
						PARAMETER_TYPE_Double,
						10);

	Parameters.Add_Value(NULL,
						"STEP",
						_TL("n"),
						_TL("Draw a section each n points"),
						PARAMETER_TYPE_Int,
						2);

	pNode = Parameters.Add_Node(NULL,
						"PDFNODE",
						_TL("PDF Documents"),
						_TL(""));

	Parameters.Add_Value(pNode,
						"PDF",
						_TL("Create PDF Documents"),
						_TL(""),
						PARAMETER_TYPE_Bool,
						true);	

	Parameters.Add_FilePath(pNode,
							"OUTPUTPATH",
							_TL("Folder for PDF Files"),
							_TL(""),
							_TL(""),
							_TL(""),
							true,
							true);

	Parameters.Add_Value(pNode,
						"WIDTH",
						_TL("Road Width"),
						_TL(""),
						PARAMETER_TYPE_Double,
						5.);

	Parameters.Add_Value(pNode,
						"SLOPE",
						_TL("Side Slope"),
						_TL(""),
						PARAMETER_TYPE_Double,
						.5);

	Parameters.Add_Value(pNode,
						"THRESHOLD",
						_TL("Height Threshold"),
						_TL("Height threshold for optimal profile calculation"),
						PARAMETER_TYPE_Double,
						.5);
}//constructor


CProfile_Cross_Sections::~CProfile_Cross_Sections(void){}

bool CProfile_Cross_Sections::On_Execute(void){

	CSG_Shapes *pLines;
	CSG_Shape *pShape, *pSection;
	CSG_Grid *pDEM;
	TSG_Point	Point, Point2;
	double fInterval;
	double fStepX, fStepY;
	int iNumPoints;
	int i,j,k;
	int iPoint;
	int iField = 0;
	int iStep;
	double dX, dY;
	double dX2, dY2;
	double dDistance;
	double dHeight;
	bool bCopy;

	m_pSections = Parameters("SECTIONS")->asShapes();
	pLines = Parameters("LINES")->asShapes();
	pDEM = Parameters("DEM")->asGrid();
	fInterval = (double) Parameters("INTERVAL")->asDouble();
	iNumPoints = Parameters("NUMPOINTS")->asInt();
	iStep = Parameters("STEP")->asInt();

	if(pLines == m_pSections){
		bCopy = true;
		m_pSections	= SG_Create_Shapes();
	}//if
	else{
		bCopy = false;
	}//else

	m_pSections->Create(SHAPE_TYPE_Line, _TL("Cross Sections"));
	for (i = iNumPoints; i > 0; i--){
		m_pSections->Add_Field(CSG_String::Format(SG_T("-%s"), SG_Get_String(fInterval * i, 2).c_str()),
										SG_DATATYPE_Double);
	}//for
	m_pSections->Add_Field("0", SG_DATATYPE_Double);
	for (i = 1; i < iNumPoints +1; i++){
		m_pSections->Add_Field(SG_Get_String(fInterval * i).c_str(), SG_DATATYPE_Double);
	}//for
	for(i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++){
		pShape = pLines->Get_Shape(i);
		for(j=0; j<pShape->Get_Part_Count(); j++){
			for(k=0; k<pShape->Get_Point_Count(j)-1; k+=iStep){
				Point = pShape->Get_Point(k,j);
				Point2 = pShape->Get_Point(k+1,j);
				dDistance = sqrt(pow(Point2.x - Point.x,2) + pow(Point2.y - Point.y,2));
				dX = (Point.x + Point2.x) / 2.;
				dY = (Point.y + Point2.y) / 2.;
				fStepX = (Point2.y - Point.y) / dDistance * fInterval;
				fStepY = -(Point2.x - Point.x) / dDistance * fInterval;
				pSection = m_pSections->Add_Shape();
				pSection->Add_Point(dX + iNumPoints * fStepX,
								dY + iNumPoints * fStepY);
				pSection->Add_Point(dX - iNumPoints * fStepX,
								dY - iNumPoints * fStepY);
				iField = 0;
				for (iPoint = iNumPoints; iPoint > 0; iPoint--){
					dX2 = dX - iPoint * fStepX;
					dY2 = dY - iPoint * fStepY;
					dHeight = pDEM->Get_Value(dX2, dY2);
					pSection->Set_Value(iField, dHeight);
					iField++;
				}//for
				dHeight = pDEM->Get_Value(dX, dY);
				pSection->Set_Value(iField, dHeight);
				iField++;
				for (iPoint = 1; iPoint < iNumPoints +1; iPoint++){
					dX2 = dX + iPoint * fStepX;
					dY2 = dY + iPoint * fStepY;
					dHeight = pDEM->Get_Value(dX2, dY2);
					pSection->Set_Value(iField, dHeight);
					iField++;
				}//for*/
			}//for
		}//for
	}//for

	if (Parameters("PDF")->asBool()){
		CreatePDFDocs();
	}//if

	if(bCopy){
		pLines->Assign(m_pSections);
		delete(m_pSections);
	}//if

	return true;

}// method

void CProfile_Cross_Sections::CreatePDFDocs(){

	CSG_String sFilePath;
			
	if (Parameters("OUTPUTPATH")->asString()){
		m_DocEngine.Open(_TL("SECTIONS"));
		sFilePath = SG_File_Make_Path(Parameters("OUTPUTPATH")->asString(), _TL("Sections"), _TL("pdf"));
	}//if
	else{
		Message_Add(_TL("\n** Error : Invalid parameters **\n"));
		return;
	}//else

	AddLongitudinalProfiles();
	AddCrossSections();

	if (m_DocEngine.Save(sFilePath)){
		if (!m_DocEngine.Close()){
			Message_Add(_TL("\n\n ** Error : Could not close PDF engine ** \n\n"));
		}
	}//if
	else{
		Message_Add(_TL("\n\n ** Error : Could not save PDF file ** \n\n"));
	}//else


}//method


void CProfile_Cross_Sections::AddLongitudinalProfiles(){

	int i,j,k;
	int iIndex = 0;	
	int iNumPoints = Parameters("NUMPOINTS")->asInt();
	int iStep = Parameters("STEP")->asInt();
	int iFirstPoint = 0;
	int iLastPoint;
	int iSections;
	double fX1, fY1, fX2, fY2;
	double fDistance, fDistanceSum = 0;
	double fDist;
	double fThreshold = (double)Parameters("THRESHOLD")->asDouble();
	bool bValid;
	CSG_Shape *pShape;
	CSG_Table *pTable;
	TSG_Point	Point, Point2;
	CSG_Shapes* pLines = Parameters("LINES")->asShapes();

	pTable = m_pSections;
	iSections = pTable->Get_Record_Count();

	m_pProfile = new TSG_Point[iSections];

	for(i=0; i<pLines->Get_Count() && Set_Progress(i, pLines->Get_Count()); i++){
		pShape = pLines->Get_Shape(i);
		for(j=0; j<pShape->Get_Part_Count(); j++){
			Point = pShape->Get_Point(0,j);
			for(k=0; k<pShape->Get_Point_Count(j)-1; k+=iStep){
				Point2 = pShape->Get_Point(k,j);
				fDistance = sqrt(pow(Point2.x - Point.x,2) + pow(Point2.y - Point.y,2));
				m_pProfile[iIndex].x = fDistanceSum + fDistance / 2.;
				m_pProfile[iIndex].y = pTable->Get_Record(iIndex)->asFloat(iNumPoints);
				fDistanceSum += fDistance;
				iIndex++;
				Point.x = Point2.x;
				Point.y = Point2.y;
			}//for
		}//for
	}//for*/

	m_pHeight = new double[iSections];

	for (i = 0; i < iSections; i++){
		m_pHeight[i] = 1500;
	}
	
	iLastPoint = iSections - 1;
	while(iFirstPoint != iSections - 1){
		fX1 = m_pProfile[iFirstPoint].x;
		fY1 = m_pProfile[iFirstPoint].y;
		fX2 = m_pProfile[iLastPoint].x;
		fY2 = m_pProfile[iLastPoint].y;
		bValid = true;
		for (i = iFirstPoint; i < iLastPoint; i++){			
			fDist = CalculatePointLineDist(fX1,fY1,fX2,fY2, m_pProfile[i].x, m_pProfile[i].y);
			if (fDist > fThreshold){
				iLastPoint--;
				bValid = false;
				break;
			}//if
		}//for
		if (bValid){
			for (j = iFirstPoint; j < iLastPoint + 1; j++){
				m_pHeight[j] = m_pProfile[iFirstPoint].y + 
					(m_pProfile[j].x - m_pProfile[iFirstPoint].x) 
					/ (m_pProfile[iLastPoint].x - m_pProfile[iFirstPoint].x)
					* (m_pProfile[iLastPoint].y - m_pProfile[iFirstPoint].y);
			}//for			
			iFirstPoint = iLastPoint;
			iLastPoint = iSections - 1;			
		}//if
	}//while
	
	
	m_DocEngine.AddLongitudinalProfile(m_pProfile, m_pHeight, iSections);

}//method

double CProfile_Cross_Sections::CalculatePointLineDist(double fX1, double fY1, double fX2, double fY2, double fPX, double fPY){

	double *pA, *pB, *pC;

	pA = new double[2];
	pA[0] = fX1;
	pA[1] = fY1;
	pB = new double[2];
	pB[0] = fX2;
	pB[1] = fY2;
	pC = new double[2];
	pC[0] = fPX;
	pC[1] = fPY;
	
	return linePointDist(pA, pB, pC);

}//method

void CProfile_Cross_Sections::AddCrossSections(){

	int i,j;
	int iRoadPoints;
	int iNumPoints = Parameters("NUMPOINTS")->asInt();
	double fInterval = (double) Parameters("INTERVAL")->asDouble();
	CSG_Table *pTable;
	TSG_Point *pRoadSection;
	TSG_Point **pCrossSections;
	double dWidth = Parameters("WIDTH")->asDouble();

	pRoadSection = new TSG_Point [2];
	pRoadSection[0].x = -dWidth / 2.;
	pRoadSection[0].y = 0;
	pRoadSection[1].x = dWidth / 2.;
	pRoadSection[1].y = 0;

	pTable = m_pSections;
	pCrossSections = new TSG_Point *[pTable->Get_Record_Count()];

	for (i = 0; i < pTable->Get_Record_Count(); i++){
		pCrossSections[i] = new TSG_Point [pTable->Get_Field_Count()];
		for (j = 0; j < pTable->Get_Field_Count(); j++){
			pCrossSections[i][j].x = -fInterval * iNumPoints + fInterval * j;
			pCrossSections[i][j].y = pTable->Get_Record(i)->asFloat(j);
		}//for
	}//for

	iRoadPoints = 2;

	m_DocEngine.AddCrossSections(pCrossSections, m_pHeight, pRoadSection, pTable->Get_Record_Count(),
								pTable->Get_Field_Count(), iRoadPoints);
	m_DocEngine.AddVolumesTable(m_pProfile, pCrossSections, m_pHeight, pRoadSection,
								pTable->Get_Record_Count(),	pTable->Get_Field_Count(), iRoadPoints);

}//method

///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
