/*******************************************************************************
    Watersheds.cpp
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

#include "Watersheds_ext.h"
#include "Helper.h"
#include <string>

#define NO_EQUIVALENT_RECTANGLE -1

CWatersheds_ext::CWatersheds_ext(void){

	Parameters.Set_Name(_TL("Watershed Basins (extended)"));
	Parameters.Set_Description(_TW("(c) 2004 by Victor Olaya. Subbasin" ));

	Parameters.Add_Grid(NULL, 
						"DEM", 
						_TL("DEM"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"CHANNELS", 
						_TL("Drainage Network"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"SOILLOSS", 
						_TL("Soil Losses"), 
						_TL(""), 
						PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid(NULL, 
						"CN", 
						_TL("Curve Number"), 
						_TL(""), 
						PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid(NULL, 
						"BASINGRID", 
						_TL("Subbasins"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"BASINS", 
						_TL("Subbasins"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"HEADERS", 
						_TL("River Headers"), 
						_TL(""), 
						PARAMETER_OUTPUT);
	
	Parameters.Add_Choice(NULL, 
						"FRAGMENTATION", 
						_TL("Basin subdivision"), 
						_TL(""), 
						CSG_String::Format(SG_T("%s|%s|"),
							_TL("Only closing points on main stream"),
							_TL("All")
						),0
					);

}//constructor

CWatersheds_ext::~CWatersheds_ext(void){}

bool CWatersheds_ext::On_Execute(void){
	
	m_pDEM = Parameters("DEM")->asGrid(); 
	m_pCNGrid = Parameters("CN")->asGrid();
	m_pBasinGrid = Parameters("BASINGRID")->asGrid();
	m_pSoilLossGrid = Parameters("SOILLOSS")->asGrid();
	m_pChannelsGrid = Parameters("CHANNELS")->asGrid();
	m_pBasins = Parameters("BASINS")->asShapes();
	m_pHeaders = Parameters("HEADERS")->asShapes();
	m_iFragmentationType = Parameters("FRAGMENTATION")->asInt();

	m_pBasinGrid->Assign((double)0);
	m_pBasinGrid->Set_Name(_TL("Subbasins"));
	m_pBasinGrid->Set_Description(_TW("Subbasins"));

	CalculateBasin();
	CreateShapesLayer();

	delete[] m_fMaxDistance;
	delete[] m_fHeightDif;
	delete m_pDistanceGrid;


	return( true );

}//method


void CWatersheds_ext::CalculateBasin() {

	int i,j;
	float fMinHeight = 1000000000;
	int iNextX, iNextY;
	int iX, iY;
    bool bAddPoint;
	TSG_Point		P;
	CSG_Points	Crossings;  

	Process_Set_Text(_TL("Calculating Subbasins..."));
	
	for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			if(!m_pChannelsGrid->is_NoData(x,y)){
				if (isHeader(x,y)){
					P.x=x;
					P.y=y;
					m_Headers.Add(P);				
				}//if
				if (m_pDEM->asFloat(x,y)<fMinHeight){
					m_iClosingX = x;
					m_iClosingY = y;
					fMinHeight = m_pDEM->asFloat(x,y);
				}//if
			}//if
		}// for
	}// for
				
	for (i = 0; i < m_Headers.Get_Count(); i++) {
	    iNextX = m_Headers.Get_Point(i).x;
		iNextY = m_Headers.Get_Point(i).y;
	    do {
	        iX = iNextX;
	        iY = iNextY;
			getNextCell(m_pDEM, m_pChannelsGrid, iX, iY, iNextX, iNextY);	                
	        if (m_pChannelsGrid->asInt(iNextX,iNextY) != m_pChannelsGrid->asInt(iX,iY)) {
	            bAddPoint = true;
	            for (j = 0; j < Crossings.Get_Count(); j++) {
	                if (Crossings.Get_Point(j).x == iX && Crossings.Get_Point(j).y == iY){
	                    bAddPoint = false;
	                }//if
	            }// for
	            if (bAddPoint) {							
					P.x = iX;
					P.y = iY;
	                Crossings.Add(P);
	            }// if	
	        }// if
	    }while (!(iX == m_iClosingX && iY == m_iClosingY)
	            && (iX != iNextX || iY != iNextY));
	}// for
	
	float fMaxDistance = -1;
	float fDistance = 0;
	TSG_Point	LongestRiverHeader;
	
	for (i = 0; i < m_Headers.Get_Count(); i++) {
	    fDistance = DistanceToClosingPoint(m_Headers.Get_Point(i).x, m_Headers.Get_Point(i).y);
	    if (fDistance > fMaxDistance) {
	        fMaxDistance = fDistance;
	        LongestRiverHeader.x = m_Headers.Get_Point(i).x; 
			LongestRiverHeader.y = m_Headers.Get_Point(i).y; 
	    }// if
	}// for		

	float fHeightDif = m_pDEM->asFloat((int)LongestRiverHeader.x, (int)LongestRiverHeader.y);
		
	if (m_iFragmentationType == FRAGMENTATION_TO_MAIN) {

		CSG_Points	CrossingsNext;
		CSG_Points	CrossingsAtPrincipal;
		
	    for (i = 0; i < Crossings.Get_Count(); i++) {
	        getNextCell(m_pDEM, m_pChannelsGrid, Crossings.Get_Point(i).x,Crossings.Get_Point(i).y, iNextX,iNextY);
			P.x = iNextX;
			P.y = iNextY;
	        CrossingsNext.Add(P);
	    }// for

		iNextX = LongestRiverHeader.x;
		iNextY = LongestRiverHeader.y;
		
	    do {
	        iX = iNextX;
	        iY = iNextY;
	        getNextCell(m_pDEM, m_pChannelsGrid, iX, iY, iNextX, iNextY);
	        for (i = 0; i < CrossingsNext.Get_Count(); i++) {
	            if (iX == CrossingsNext.Get_Point(i).x && iY == CrossingsNext.Get_Point(i).y) {
	                CrossingsAtPrincipal.Add(Crossings.Get_Point(i));
	            }//if
	        }// for
	    }while (!(iX == m_iClosingX && iY == m_iClosingY)
	            && (iX != iNextX || iY != iNextY));
	    Crossings = CrossingsAtPrincipal;			
	}// if
	
	P.x = m_iClosingX;
	P.y = m_iClosingY;
	Crossings.Add(P);
	
	bool *bCalculated = new bool[Crossings.Get_Count()];
	for (i = 0; i < Crossings.Get_Count(); i++){
		bCalculated[i] = false;
	}//for

	m_fMaxDistance = new float[Crossings.Get_Count()+1];
	m_fMaxDistance[0] = fMaxDistance;
	m_fHeightDif = new float[Crossings.Get_Count()+1];
	m_fHeightDif[0] = fHeightDif;

	m_pDistanceGrid = SG_Create_Grid(m_pDEM,GRID_TYPE_Float);
	m_pDistanceGrid->Assign((double)0);

	m_iNumBasins = 1;
	for (i = 0; i < Crossings.Get_Count(); i++) {
	    if (!bCalculated[i]) {
	        if (isTopHeader(Crossings, i, bCalculated)) {
				m_fCells=0;
	            WriteBasin(Crossings.Get_Point(i).x, Crossings.Get_Point(i).y, m_iNumBasins);
	            bCalculated[i] = true;
				if (m_fCells<100){
					DeleteBasin(Crossings.Get_Point(i).x, Crossings.Get_Point(i).y, m_iNumBasins);
					m_iNumBasins--;
				}//if
	            i = 0;
				m_iNumBasins++;
	        }// if
	    }// if
	}// for
		
	m_pBasinGrid->Set_NoData_Value(0);		
	
	//DataObject_Add(m_pDistanceGrid);

	
}// method

void CWatersheds_ext::CreateShapesLayer(){ //first shape (0) is the whole basin. 

	CSG_Table *pTable;
	CSG_Table_Record *pRecord, *pRecord2;
	CSG_Shape *pSubbasin;
	TSG_Point	Point;	
	float fArea=0, fPerim=0;
	float fSide1, fSide2;
	float fConcTime;
	float fMinHeight;
	int i,j,k;	
	int x,y;
	int iNextX, iNextY;
	int iX, iY;
	int iXOrig, iYOrig;
	int iIndex;
	int iUpstreamBasin;
	int iDownstreamBasin;
	int iCode;
	float * fCN;
	float * fSoilLoss;
	float * fCNValidCells;
	float * fSoilLossValidCells;
	CSG_String sSubbasins;
	int iOffsetX[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int iOffsetY[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int iDirection;
    int iDir = 1;
    int iNewDirection[] = {5, 6, 7, 0, 1, 2, 3, 4};
		
	
	Process_Set_Text(_TL("Vectorizing headers..."));
		
	m_pHeaders->Create(SHAPE_TYPE_Point, _TL("Headers"));
	pTable=&m_pHeaders->Get_Table();
	pTable->Add_Field("Header", TABLE_FIELDTYPE_Int);
	for (i = 0; i < m_Headers.Get_Count(); i++){
		m_pHeaders->Add_Shape()->Add_Point(m_Headers.Get_Point(i).x * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin(), 
											m_Headers.Get_Point(i).y * m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
		m_pHeaders->Get_Shape(i)->Get_Record()->Set_Value(0,i);
	}//for

	Process_Set_Text(_TL("Vectorizing subbasins..."));

	m_pBasins->Create(SHAPE_TYPE_Polygon, _TL("Subbasins"));
	pTable = &m_pBasins->Get_Table();
	pTable->Add_Field(_TL("Basin Code"), TABLE_FIELDTYPE_Int);

	pSubbasin = m_pBasins->Add_Shape();		

	for (x = 0; x < m_pDEM->Get_NX(); x++) {
	    for (y = 0; y < m_pDEM->Get_NY(); y++) {
	        if (m_pBasinGrid->asInt(x,y) !=0) {
	            iX = x;
	            iY = y;
	            goto out;
	        }// if		
	    }// for
	}// for
out:
	iXOrig = iX;
	iYOrig = iY;
	pSubbasin->Add_Point(iX * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin(),
						iY* m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
	iDirection = 1;
	do {
	    if (iDirection > 7) {
	        iDirection =  iDirection % 8;
	    }//if
	    for (i = iDirection; i < iDirection + 8; i++) {
	        if (i > 7) {
	            iDir = i % 8;
	        }//if
	        else {
	            iDir = i;
	        }//else
	        if (m_pBasinGrid->asInt(iX + iOffsetX[iDir],iY + iOffsetY[iDir]) !=0) {
	            iX = iX + iOffsetX[iDir];
				iY = iY + iOffsetY[iDir];	                        
	            iDirection = iNewDirection[iDir];
	            break;
	        }// if
	    }// for
	    pSubbasin->Add_Point(iX * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin(),
							iY* m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
	}while ((iY != iYOrig) || (iX != iXOrig));
	
	pSubbasin->Get_Record()->Set_Value(0,(double)0);

	for (k = 1; k < m_iNumBasins; k++) {

		pSubbasin = m_pBasins->Add_Shape();		

	    for (x = 0; x < m_pDEM->Get_NX(); x++) {
	        for (y = 0; y < m_pDEM->Get_NY(); y++) {
	            if (m_pBasinGrid->asInt(x,y) == k) {
	                iX = x;
	                iY = y;
	                goto out2;
	            }// if		
	        }// for
	    }// for
out2:
	    iXOrig = iX;
	    iYOrig = iY;
	    pSubbasin->Add_Point(iX * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin(),
							iY* m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
	    iDirection = 1;
	    do {
	        if (iDirection > 7) {
	            iDirection =  iDirection % 8;
	        }//if
	        for (i = iDirection; i < iDirection + 8; i++) {
	            if (i > 7) {
	                iDir = i % 8;
	            }//if
	            else {
	                iDir = i;
	            }//else
	            if (m_pBasinGrid->asInt(iX + iOffsetX[iDir],iY + iOffsetY[iDir]) == k) {
	                iX = iX + iOffsetX[iDir];
					iY = iY + iOffsetY[iDir];	                        
	                iDirection = iNewDirection[iDir];
	                break;
	            }// if
	        }// for
	        pSubbasin->Add_Point(iX * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin(),
								iY* m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
	    }while ((iY != iYOrig) || (iX != iXOrig));
		pSubbasin->Get_Record()->Set_Value(0,k);
	    
	}// for

	pTable->Add_Field(_TL("Closing Pt. X"), TABLE_FIELDTYPE_Double); //1
	pTable->Add_Field(_TL("Closing Pt. Y"), TABLE_FIELDTYPE_Double); //2
	pTable->Add_Field(_TL("Perimeter (m)"), TABLE_FIELDTYPE_Double); //3
	pTable->Add_Field(_TL("Area (ha)"), TABLE_FIELDTYPE_Double); //4
	pTable->Add_Field(_TL("Avg. Curve Number"), TABLE_FIELDTYPE_Double); //5
	pTable->Add_Field(_TL("Soil losses (t/ha·year)"), TABLE_FIELDTYPE_Double); //6
	pTable->Add_Field(_TL("Concentration time (h)"), TABLE_FIELDTYPE_Double); //7 
	pTable->Add_Field(_TL("Upslope Basins"), TABLE_FIELDTYPE_String); //8
	pTable->Add_Field(_TL("Downslope basins"), TABLE_FIELDTYPE_Int); //9 
	pTable->Add_Field(_TL("Basin Type (Gravelius)"), TABLE_FIELDTYPE_String); //10
	pTable->Add_Field(_TL("Equivalente Rectangle (Side 1)(m)"), TABLE_FIELDTYPE_Double); //11
	pTable->Add_Field(_TL("Equivalente Rectangle (Side 2)(m)"), TABLE_FIELDTYPE_Double); //12
	pTable->Add_Field(_TL("Orographic coef."), TABLE_FIELDTYPE_Double); //13
	pTable->Add_Field(_TL("Massivity coef."), TABLE_FIELDTYPE_Double); //14
	pTable->Add_Field(_TL("Centroid X"), TABLE_FIELDTYPE_Double); //15
	pTable->Add_Field(_TL("Centroid Y"), TABLE_FIELDTYPE_Double); //16
	pTable->Add_Field(_TL("Máximum flow distance(m)"), TABLE_FIELDTYPE_Double); //17

	for (i = 0; i < m_pBasins->Get_Count(); i++) {								            
        pSubbasin = m_pBasins->Get_Shape(i);

		fArea = ((CSG_Shape_Polygon*)pSubbasin)->Get_Area();
		fPerim = ((CSG_Shape_Polygon*)pSubbasin)->Get_Perimeter();
		Point = ((CSG_Shape_Polygon*)pSubbasin)->Get_Centroid();
		pSubbasin->Get_Record()->Set_Value(15, Point.x);
		pSubbasin->Get_Record()->Set_Value(16, Point.y);

		EquivalentRectangle(fPerim,fArea, fSide1, fSide2);
		pSubbasin->Get_Record()->Set_Value(3, fPerim);
		pSubbasin->Get_Record()->Set_Value(4, fArea/10000.0);
		pSubbasin->Get_Record()->Set_Value(10, GraveliusType(fPerim, fArea).c_str());
		if (fSide1!=NO_EQUIVALENT_RECTANGLE){
			pSubbasin->Get_Record()->Set_Value(11, SG_Get_String(fSide1).c_str());
			pSubbasin->Get_Record()->Set_Value(12, SG_Get_String(fSide2).c_str());
		}//if
		else{
			pSubbasin->Get_Record()->Set_Value(11, SG_T("---"));
			pSubbasin->Get_Record()->Set_Value(12, SG_T("---"));
		}//else
		pSubbasin->Get_Record()->Set_Value(13, OrographicIndex(fPerim, fArea/10000));
		pSubbasin->Get_Record()->Set_Value(14, MassivityIndex(fPerim, fArea/10000));
	}//for

	//upstream and downstream basins
	
	for (i = 0; i < m_Headers.Get_Count(); i++) {
	    iNextX = m_Headers.Get_Point(i).x;
		iNextY = m_Headers.Get_Point(i).y;
	    do {
	        iX = iNextX;
	        iY = iNextY;
			getNextCell(m_pDEM, m_pChannelsGrid, iX, iY, iNextX, iNextY);
			iDownstreamBasin = m_pBasinGrid->asInt(iNextX,iNextY);
			iUpstreamBasin = m_pBasinGrid->asInt(iX,iY);
	        if (iUpstreamBasin != iDownstreamBasin && iDownstreamBasin!=0 && iUpstreamBasin !=0) {				
	            for (j = 0; j < pTable->Get_Record_Count(); j++) {
	                pRecord = pTable->Get_Record(j);
					iCode = pRecord->asInt(0);
					if (iCode == iUpstreamBasin){
						pRecord->Set_Value(9, iDownstreamBasin);
						pRecord->Set_Value(1, iX * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin());						
						pRecord->Set_Value(2, iY * m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
					}//if					
	            }// for	            
	        }// if
	    }while (!(iX == m_iClosingX && iY == m_iClosingY)
	            && (iX != iNextX || iY != iNextY));
	}// for
		
	pRecord = pTable->Get_Record(pTable->Get_Record_Count()-1);
	pRecord->Set_Value(1, iX * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin());						
	pRecord->Set_Value(2, iY * m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());

	for (i = 1; i<pTable->Get_Record_Count(); i++){
		pTable->Get_Record(i)->Set_Value(8,SG_T(" --- "));
		iCode = pTable->Get_Record(i)->asInt(0);
		for (j = 0; j<pTable->Get_Record_Count(); j++){
			iDownstreamBasin = pTable->Get_Record(j)->asInt(9);
			if (iDownstreamBasin == iCode){
				sSubbasins = CSG_String(pTable->Get_Record(i)->asString(8)) + SG_T(" ")
					+ SG_Get_String(pTable->Get_Record(j)->asInt(0));
				pTable->Get_Record(i)->Set_Value(8, sSubbasins.c_str());
			}//if
		}//for
	}//for*/

	fCN = new float[m_pBasins->Get_Count()];
	fCNValidCells = new float[m_pBasins->Get_Count()];
	fSoilLoss = new float[m_pBasins->Get_Count()];
	fSoilLossValidCells = new float[m_pBasins->Get_Count()];

	for (i = 0; i < m_pBasins->Get_Count(); i++){
		fCN[i] = 0;
		fCNValidCells[i] = 0;
		fSoilLoss[i] = 0;
		fSoilLossValidCells[i] = 0;
	}//for

	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			if (!m_pBasinGrid->is_NoData(x,y)){
				iIndex = m_pBasinGrid->asInt(x,y);
				fSoilLossValidCells[iIndex]++;
				fSoilLossValidCells[0]++;
				if (m_pSoilLossGrid){
					fSoilLoss[iIndex] += m_pSoilLossGrid->asFloat(x,y);
					fSoilLoss[0] += m_pSoilLossGrid->asFloat(x,y);
				}//if
				fCNValidCells[iIndex]++;
				fCNValidCells[0]++;
				if (m_pCNGrid){					
					fCN[iIndex] += m_pCNGrid->asFloat(x,y);
					fCN[0] += m_pCNGrid->asFloat(x,y);
				}//if
			}//if
		}//for
	}//for
	
	for (i = 0; i < m_pBasins->Get_Count(); i++){
		fCN[i] /= fCNValidCells[i];
		fSoilLoss[i] /= fSoilLossValidCells[i];		
	}//for

	for (j = 1; j < pTable->Get_Record_Count(); j++) {
		pRecord = pTable->Get_Record(j);
		iCode = pRecord->asInt(0);		
		pRecord->Set_Value(5, fCN[iCode]);
		pRecord->Set_Value(6, fSoilLoss[iCode]);
		iX = (pRecord->asInt(1) - m_pDEM->Get_XMin()) / m_pDEM->Get_Cellsize();
		iY = (pRecord->asInt(2) - m_pDEM->Get_YMin()) / m_pDEM->Get_Cellsize();
		fMinHeight = m_pDEM->asFloat(iX,iY);
		fConcTime = pow(0.87 * pow(m_fMaxDistance[iCode] / 1000.0, 3.0)
                / (m_fHeightDif[iCode]-fMinHeight),  0.385);
		pRecord->Set_Value(7, fConcTime);
		pRecord->Set_Value(17, m_fMaxDistance[iCode]);
	}// for	*/  
	
	pRecord = pTable->Get_Record(0);
	fConcTime = pow(0.87 * pow(m_fMaxDistance[0] / 1000.0, 3.0)
                / (m_fHeightDif[0]-fMinHeight),  0.385);
	pRecord->Set_Value(5, fCN[0]);
	pRecord->Set_Value(6, fSoilLoss[0]);
	pRecord->Set_Value(7, fConcTime);
	pRecord2 = pTable->Get_Record(j-1);
	pRecord->Set_Value(1, pRecord2->asInt(1));
	pRecord->Set_Value(2, pRecord2->asInt(2));
	pRecord->Set_Value(17, m_fMaxDistance[0]);

}//method

bool CWatersheds_ext::isHeader(int iX, int iY){

    int iNextX, iNextY;
	
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
			if (is_InGrid(iX+i,iY+j)){
            	if (!m_pChannelsGrid->is_NoData(iX+i,iY+j) && (i!=0 || j!=0)){
               		getNextCell(m_pDEM, m_pChannelsGrid, iX + i, iY + j, iNextX, iNextY);
                	if (iNextX == iX && iNextY == iY) {
                    	return false;
                	}//if
				}//if
            }// if
        }// for
    }// for

    return true;
		
}// method

bool CWatersheds_ext::isLastCell(int iX, int iY){

    int iNextX, iNextY;
		       
	getNextCell(m_pDEM, m_pChannelsGrid, iX, iY, iNextX, iNextY);
    if ((iNextX == iX && iNextY == iY) || 
				(m_pChannelsGrid->is_NoData(iNextX,iNextY))) {
		return true;
    }//if
	
    return false;
		
}// method
	
void CWatersheds_ext::WriteBasin(int iX, int iY, int iBasinNumber) {
	
    int iNextX, iNextY;
	float fDistance;
	
    if ((iX <= 0 || iX >= Get_NX() || iY <= 0 || iY >= Get_NY())
            || m_pDEM->is_NoData(iX,iY)) {} else {
        m_pBasinGrid->Set_Value(iX,iY, iBasinNumber);
		m_fCells++;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (!(i == 0) || !(j == 0)) {
                    if (m_pBasinGrid->asInt(iX + i, iY + j) == 0) {
                        getNextCell(m_pDEM, iX + i, iY + j, iNextX, iNextY);
                        if (iNextX == iX && iNextY == iY) {
							fDistance = m_pDistanceGrid->asDouble(iX,iY)
								+ M_GET_LENGTH(i,j)*m_pDEM->Get_Cellsize();
                            m_pDistanceGrid->Set_Value(iX+i, iY+j,fDistance) ;
							if (fDistance > m_fMaxDistance[iBasinNumber]){
								m_fMaxDistance[iBasinNumber] = fDistance;
								m_fHeightDif[iBasinNumber] = m_pDEM->asFloat(iX+i,iY+j);
							}//if
							WriteBasin(iX + i, iY + j, iBasinNumber);
                        }//if
                    }// if
                }// if
            }// for
        }// for	
    }// else

}// method

void CWatersheds_ext::DeleteBasin(int iX, int iY, int iBasinNumber) {
	
    int iNextX, iNextY;
	
    if ((iX <= 0 || iX >= Get_NX() || iY <= 0 || iY >= Get_NY())
            || m_pDEM->is_NoData(iX,iY)) {} else {
        m_pBasinGrid->Set_Value(iX,iY, 0);
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (!(i == 0) || !(j == 0)) {
                    if (m_pBasinGrid->asInt(iX + i, iY + j) == iBasinNumber) {
                        getNextCell(m_pDEM, iX + i, iY + j, iNextX, iNextY);
                        if (iNextX == iX && iNextY == iY) {
							DeleteBasin(iX + i, iY + j, iBasinNumber);
                        }//if
                    }// if
                }// if
            }// for
        }// for	
    }// else

}// method


bool CWatersheds_ext::isTopHeader(CSG_Points &Headers,
							int iIndex,							
							bool* bCalculated) {
			
    int iX, iY;
	int iNextX, iNextY;
    int iTargetX = Headers.Get_Point(iIndex).x;
	int iTargetY = Headers.Get_Point(iIndex).y;
   			
    for (int i = 0; i < Headers.Get_Count(); i++) {
        if (i != iIndex && !bCalculated[i]) {
            
			iNextX = Headers.Get_Point(i).x;
			iNextY = Headers.Get_Point(i).y;
			
            do {
                iX = iNextX;
                iY = iNextY;
                getNextCell(m_pDEM, m_pChannelsGrid, iX, iY, iNextX, iNextY);
                if (iNextX == iTargetX && iNextY == iTargetY) {
                    return false;
				}//if					
            }while (!(iX == m_iClosingX && iY == m_iClosingY)
                    && !(iNextX == iX && iNextY == iY));							
        }// if
    }// for			
	
    return true;
	
}// method

float CWatersheds_ext::DistanceToClosingPoint(int iX, int iY){ // in m
            
	float fDist = 1;
	int iNextX = iX;
	int iNextY = iY;

    if ((iX <= 0 || iX >= Get_NX() || iY <= 0 || iY >= Get_NY())
            || m_pDEM->is_NoData(iX,iY)) {
		return 0;
	}// if
	do {
		iX = iNextX;
		iY = iNextY;
		getNextCell(m_pDEM, iX, iY, iNextX, iNextY);
		if (fabs((double)iX - iNextX + iY - iNextY) == 1) {
			fDist = fDist + Get_Cellsize();
		}// if
		else {
			fDist = fDist + 1.414f * Get_Cellsize();
		}// else
		if (iX == m_iClosingX && iY == m_iClosingY) {
			return fDist;
		}// if

	}while (iX != iNextX || iY != iNextY);

	return 0;

}// method

CSG_String CWatersheds_ext::GraveliusType(float fPerimeter, //in m
									   float fArea){ //in m2
            		
	CSG_String sType;
	float fGraveliusIndex = 0.28 * fPerimeter / sqrt(fArea);
	
    if (fGraveliusIndex > 1.75) {
		sType = "Rectangular";
    }//if
    else if (fGraveliusIndex > 1.5) {
        sType = "Ovalooblonga-rectangularoblonga";
    }//else if
    else if (fGraveliusIndex > 1.5) {
        sType = "Ovaloredonda-ovalooblonga";
    }//else if
    else {
        sType = "Redonda-ovaloredonda";
    }//else

	return sType;
	    
}// method
	
void CWatersheds_ext::EquivalentRectangle(float fPerimeter, //in m
									 float fArea, // in m2
									 float &fSide1, //in m
									 float &fSide2){// in m
            
    float fDisc = pow(fPerimeter, 2) - 8.0 * fArea;

    if (fDisc > 0) {
        fSide1 = (fPerimeter +sqrt(fDisc)) / 4.0;
        fSide2 = (fPerimeter - (2 * fSide1)) / 2.0;            
    }// if
    else {
        fSide1 = NO_EQUIVALENT_RECTANGLE;
        fSide2 = NO_EQUIVALENT_RECTANGLE;
    }// if
	            	
}// method
	
float CWatersheds_ext::MassivityIndex(float fMeanHeight,//in m
								  float fArea){ //in ha
            				
	return fMeanHeight / fArea;
		
}// method
	
float CWatersheds_ext::OrographicIndex(float fMeanHeight,//in m
								   float fArea){ //in ha            
			
   return fMeanHeight * fMeanHeight / fArea;
	
}// method


