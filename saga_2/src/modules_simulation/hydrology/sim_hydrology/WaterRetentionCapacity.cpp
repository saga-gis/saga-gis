/*******************************************************************************
    WaterRetentionCapacity.cpp
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

#include "idw.h"
#include "WaterRetentionCapacity.h"

CWaterRetentionCapacity::CWaterRetentionCapacity(void){

	Parameters.Set_Name(_TL("Water Retention Capacity"));
	Parameters.Set_Description(_TL(
		"(c) 2004 Victor Olaya. Water Retention Capacity (Gandullo, 1994)"));

	Parameters.Add_Shapes(NULL,
						"SHAPES",
						_TL("Plot Holes"),
						"",
						PARAMETER_INPUT);

	Parameters.Add_Shapes(NULL,
						"OUTPUT",
						_TL("Final Parameters"),
						"",
						PARAMETER_OUTPUT);
	
	Parameters.Add_Grid(NULL,
						"DEM",
						_TL("DEM"),
						_TL("DEM"),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"RETENTION",
						_TL("Water Retention Capacity"),
						"",
						PARAMETER_OUTPUT);

}//constructor

CWaterRetentionCapacity::~CWaterRetentionCapacity(void)
{}

bool CWaterRetentionCapacity::On_Execute(void){

	int i,j;
	int x,y;
	int iField;
	int iShape;
	int iRows;
	float fValue = 0;
	float **pData;
	int iX, iY;
	float fC;
	double dSlope,dAspect;	
	CShape* pShape;
	CShapes* pShapes = Parameters("SHAPES")->asShapes();
	CGrid* pDEM = Parameters("DEM")->asGrid();
	
	m_pRetention = Parameters("RETENTION")->asGrid();
	m_pSlope = SG_Create_Grid(pDEM);
	m_pOutput = Parameters("OUTPUT")->asShapes();

	m_pOutput->Assign(pShapes);
	m_pOutput->Get_Table().Add_Field("CCC", TABLE_FIELDTYPE_Double);
	m_pOutput->Get_Table().Add_Field("CIL", TABLE_FIELDTYPE_Double);
	m_pOutput->Get_Table().Add_Field(_TL("Permeability"), TABLE_FIELDTYPE_Double);
	m_pOutput->Get_Table().Add_Field(_TL("Equivalent Moisture"), TABLE_FIELDTYPE_Double);
	m_pOutput->Get_Table().Add_Field(_TL("Water Retention Capacity"), TABLE_FIELDTYPE_Double);


	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			if( pDEM->Get_Gradient(x, y, dSlope, dAspect) ){
				m_pSlope->Set_Value(x, y, dSlope);				
			}
			else{
				m_pSlope->Set_NoData(x, y);				
			}
		}
	}

	iRows = pShapes->Get_Table().Get_Field_Count() / 5;
	pData = new float*[iRows];

	for (iShape = 0; iShape < pShapes->Get_Count(); iShape++){
		pShape = pShapes->Get_Shape(iShape);
		for (i = 0; i< iRows; i++){
			pData[i] = new float[5];
			for (j = 0; j < 5; j++){
				pData[i][j] = 0;
				try{
					pData[i][j] = pShape->Get_Record()->asFloat(j+i*5);
				}//try
				catch(...){}
			}//for
		}//for
		iX = (int)((pShape->Get_Point(0).x - pDEM->Get_XMin())/pDEM->Get_Cellsize());
		iY = (int)((pShape->Get_Point(0).y - pDEM->Get_YMin())/pDEM->Get_Cellsize());
		fC = (float)(1. - tan(m_pSlope->asFloat(iX,iY,false)));
		pShape = m_pOutput->Get_Shape(iShape);
		CalculateWaterRetention(pData, iRows, fC, pShape->Get_Record());
	}//for

	iField = m_pOutput->Get_Table().Get_Field_Count()-1;

	CIDW IDW;

	IDW.setParameters(m_pRetention, m_pOutput, iField);
	IDW.Interpolate();

	CorrectWithSlope();

	return true;

}//method

void CWaterRetentionCapacity::CalculateWaterRetention(float **pData, 
													   int iNumHorizons, 
													   float fC,
													   CTable_Record *pRecord){

	int i;
	int iField;
	float *pCCC = new float[iNumHorizons];
	float *pCIL = new float[iNumHorizons];
	float *pK = new float[iNumHorizons];
	int *pPerm = new int[iNumHorizons];
	float *pHe = new float[iNumHorizons];
	float *pCRA = new float[iNumHorizons];
	float fTotalDepth = 0;
	float fWaterRetention = 0;
	float fPerm = 0;
	float fHe = 0;
	float fK = 0;
	float fCCC = 0;
	float fCIL = 0;

	pK[0] = 0;
	for (i = 0; i < iNumHorizons; i++){
		pCCC[i] = CalculateCCC(pData[i]);
		pCIL[i] = CalculateCIL(pData[i]);
		pPerm[i] = CalculatePermeability(pCCC[i], pCIL[i]);
		pHe[i] = CalculateHe(pData[i]);
		if (i){
			pK[i] = CalculateK(pPerm[i-1], pPerm[i], fC);
		}//if
		pCRA[i] = (float)((12.5 * pHe[i] + 12.5 * (50. - pHe[i]) * pK[i] / 2.) * pData[i][1] / 100.);
		fTotalDepth += pData[i][0];
	}//for
	for (i = 0; i < iNumHorizons; i++){
		fWaterRetention += pData[i][0] / fTotalDepth * pCRA[i];
		fCCC += pData[i][0] / fTotalDepth * pCCC[i];
		fCIL += pData[i][0] / fTotalDepth * pCIL[i];
		fPerm += pData[i][0] / fTotalDepth * pPerm[i];
		fHe += pData[i][0] / fTotalDepth * pHe[i];
		fK += pData[i][0] / fTotalDepth * pK[i];
	}//for

	iField = pRecord->Get_Owner()->Get_Field_Count() - 1;

	pRecord->Set_Value(iField - 4, fCCC);
	pRecord->Set_Value(iField - 3, fCIL);
	pRecord->Set_Value(iField - 2, fPerm);
	pRecord->Set_Value(iField - 1, fHe);
	pRecord->Set_Value(iField, fWaterRetention);

}//method

void CWaterRetentionCapacity::CorrectWithSlope(){

	int x,y;
	float fC;

	for (x = 0; x < m_pRetention->Get_NX(); x++) {
		for (y = 0; y < m_pRetention->Get_NY(); y++) {
			fC = (float)(1. - tan(m_pSlope->asFloat(x,y,false)));
			if (fC < 0.){
				fC = 0.;
			}//if
			m_pRetention->Set_Value(x,y,m_pRetention->asFloat(x,y) * fC);
		}//for
	}//for

}//method

float CWaterRetentionCapacity::CalculateHe(float* pHorizon){

	float fL = pHorizon[2];
	float fTF = pHorizon[1];
	float fAr = pHorizon[3];
	float fMO = pHorizon[4];

	float fHe = (float)(4.6 + 0.43 * fAr + 0.25 * fL + 1.22 * fMO);
	return fHe;

}//method


float CWaterRetentionCapacity::CalculateCIL(float* pHorizon){

	float fL = pHorizon[2];
	float fTF = pHorizon[1];

	float fCIL = (float)((fL * fTF) /10000.);
	return fCIL;

}//method

float CWaterRetentionCapacity::CalculateCCC(float* pHorizon){

	float fL = pHorizon[2];
	float fTF = pHorizon[1];
	float fAr = pHorizon[3];
	float fMO = pHorizon[4];

	float fCCC = (float)((fAr - 4.*fMO) / fTF);
	return fCCC;

}//method

float CWaterRetentionCapacity::CalculateK(int iPermI, int iPermS, float fC){

	float fAi = (float)((iPermI - 1) * .2);
	float fAs = (float)((iPermS - 1) * .2);

	return (float)((1. - fAi - (1 + fAs) * (1. - fC)));

}//method


int CWaterRetentionCapacity::CalculatePermeability(float fCCC, float fCIL){

	int iPerm;
	if (fCCC < 0.15){
		iPerm = 5 - (int)((fCIL-0.1) / 0.15);
	}//if
	else if (fCIL < 0.2){
		iPerm = 5 - (int)(fCCC / 0.15);
	}//else if
	else{
		iPerm = (int)(5 - (fCCC + fCIL - 0.1) / 0.15);
	}//else

	if (iPerm < 1){
		iPerm = 1;
	}//if

	return iPerm;

}//method