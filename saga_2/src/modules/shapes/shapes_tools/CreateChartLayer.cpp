/*******************************************************************************
    CCreateChartLayer.cpp
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

#include "CreateChartLayer.h"
#include <string>

#define TYPE_PIE 0
#define TYPE_BARS 0
#define PI2 6.283185


CCreateChartLayer::CCreateChartLayer(void){

	CSG_Parameter *pNode;
	
	Parameters.Set_Name(_TL("Create Chart Layer (bars/sectors)"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. "));
		
	pNode = Parameters.Add_Shapes(NULL, 
						"INPUT", 
						_TL("Shapes"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Table_Field(pNode,
								"SIZE",
								_TL("Size field"),
								_TL(""));

	Parameters.Add_Value(NULL, 
						"MAXSIZE", 
						_TL("Maximum size"), 
						_TL(""),
						PARAMETER_TYPE_Double, 
						100,
						0,
						true);

	Parameters.Add_Value(NULL, 
						"MINSIZE", 
						_TL("Minimum size"), 
						_TL(""),
						PARAMETER_TYPE_Double, 
						10,
						0,
						true);
	
	Parameters.Add_Choice(NULL, 
						"TYPE", 
						_TL("Type"), 
						_TL(""), 
						_TW("Sectors|"
						"Bars|"),									
						0);

	m_pExtraParameters	= Add_Parameters("EXTRA",
										_TL("Fields for diagram"),
										_TL(""));

}//constructor


CCreateChartLayer::~CCreateChartLayer(void)
{}


bool CCreateChartLayer::On_Execute(void){

	
	CSG_Shapes *pInput;
	int i=0;
	int iType;	
	int iSizeField;
	
	if (GetExtraParameters()){

		iSizeField = Parameters("SIZE")->asInt();
		m_fMaxSize = (float) Parameters("MAXSIZE")->asDouble();
		m_fMinSize = (float) Parameters("MINSIZE")->asDouble();

		if (m_fMinSize > m_fMaxSize){
			m_fMinSize = m_fMaxSize;
		}//if

		iType = Parameters("TYPE")->asInt();
		pInput = Parameters("INPUT")->asShapes();
		m_fMaxValue = pInput->Get_Maximum(iSizeField);
		m_fMinValue = pInput->Get_Minimum(iSizeField);

		if (iType == TYPE_PIE){		
			m_pOutput = SG_Create_Shapes(SHAPE_TYPE_Polygon, _TL("Chart (sectors):"));				
		}//if
		else{
			m_pOutput = SG_Create_Shapes(SHAPE_TYPE_Polygon, _TL("Chart (bars):"));
		}//else

		m_pOutput->Add_Field(_TL("Field (ID)"), TABLE_FIELDTYPE_Int);
		m_pOutput->Add_Field(_TL("Field (Name)"), TABLE_FIELDTYPE_String);


		for (i = 0; i < pInput->Get_Count(); i++){
			if (iType == TYPE_PIE){
		//		AddPieChart(pInput->Get_Shape(i),m_pOutput->Get_Type());
				AddPieChart(pInput->Get_Shape(i),pInput->Get_Type());
			}//if
			else{
		//		AddBarChart(pInput->Get_Shape(i),m_pOutput->Get_Type());
				AddBarChart(pInput->Get_Shape(i),pInput->Get_Type());
			}//else
		}//for
		
		DataObject_Add(m_pOutput, false);

		delete [] m_bIncludeParam;
		
		return true;

	}//if

	delete [] m_bIncludeParam;

	return false;

}//method

bool CCreateChartLayer::GetExtraParameters(){

	int i;
	CSG_Table *pShapesTable;
	CSG_Shapes *pInput;
	CSG_Parameter *pParam;
	CSG_String sName;
	bool bIsValidSelection = false;

	pInput = Parameters("INPUT")->asShapes();

	pShapesTable = pInput;
	m_bIncludeParam = new bool [pShapesTable->Get_Field_Count() ];

	for (i = 0; i < pShapesTable->Get_Field_Count(); i++){		
		if (pShapesTable->Get_Field_Type(i) > 1 && pShapesTable->Get_Field_Type(i) < 7){ //is numeric field
			m_pExtraParameters->Add_Value(NULL,
											SG_Get_String(i,0).c_str(),
											pShapesTable->Get_Field_Name(i),
											_TL(""),
											PARAMETER_TYPE_Bool,
											false);
		}//if
	}//for
	if(Dlg_Parameters("EXTRA")){
		for (i = 0; i < pShapesTable->Get_Field_Count(); i++){
			sName = SG_Get_String(i,0);
			if (pParam = Get_Parameters("EXTRA")->Get_Parameter(sName.c_str())){
				m_bIncludeParam[i] = pParam->asBool();
				bIsValidSelection = true;
			}//try
			else{
				m_bIncludeParam[i] = false;
			}//else
		}//for

		m_pExtraParameters->Destroy();

		return bIsValidSelection;
	}//if

	m_pExtraParameters->Destroy();

	return false;

}//method

void CCreateChartLayer::AddPieChart(CSG_Shape* pShape, int iType){

	int i,j;
	int iSteps;
	int iSizeField;
	int iField;
	float fSum = 0;
	float fPartialSum = 0;
	float fSize;	
	float fSectorSize;
	double dX, dY;
	CSG_Shape *pSector;
	CSG_Table_Record *pRecord;
	TSG_Point Point;
		
	iSizeField = Parameters("SIZE")->asInt();

	pRecord = pShape;
	for (i = 0; i < pRecord->Get_Table()->Get_Field_Count(); i++){
		if (m_bIncludeParam[i]){
			fSum += pRecord->asFloat(i);
		}//if
	}//for

	fSize = pRecord->asFloat(iSizeField);
	fSize = m_fMinSize + (m_fMaxSize - m_fMinSize)/(m_fMaxValue - m_fMinValue) * (fSize - m_fMinValue);

	switch (iType){
	case SHAPE_TYPE_Polygon:
		Point = ((CSG_Shape_Polygon*) pShape)->Get_Centroid();	
		break;
	case SHAPE_TYPE_Line:
		Point = GetLineMidPoint((CSG_Shape_Line*)pShape);
		break;
	case SHAPE_TYPE_Point:
		Point = pShape->Get_Point(0);
		break;
	default:
		break;
	}//switch
	dX = Point.x;
	dY = Point.y;

	fPartialSum = 0;
	iField = 1;
	for (i = 0; i < pRecord->Get_Table()->Get_Field_Count(); i++){
		if (m_bIncludeParam[i]){
			fSectorSize = pRecord->asFloat(i) / fSum;
			pSector = m_pOutput->Add_Shape();
			pSector->Add_Point(dX,dY);
			iSteps = (int) (fSectorSize * 200.);
			for (j = 0; j < iSteps; j++){
				pSector->Add_Point(dX + fSize * sin((fPartialSum + (float)j / 200.) * PI2),
									dY + fSize * cos((fPartialSum + (float)j / 200.) * PI2));
			}//for
			fPartialSum +=fSectorSize;
			pSector->Add_Point(dX + fSize * sin(fPartialSum * PI2),
								dY + fSize * cos(fPartialSum * PI2));		
			pSector->Set_Value(0,iField);
			pSector->Set_Value(1,pRecord->Get_Table()->Get_Field_Name(i));
			iField++;
		}//if
	}//for

}//method

void CCreateChartLayer::AddBarChart(CSG_Shape* pShape, int iType){

	int i;
	int iSizeField;
	int iField;
	int iValidFields = 0;
	float fMax;	
	float fMin;
	float fSize;	
	float fBarHeight, fBarWidth;
	double dX, dY;
	CSG_Shape *pSector;
	CSG_Table_Record *pRecord;
	TSG_Point Point;
		
	iSizeField = Parameters("SIZE")->asInt();
	pRecord = pShape;

	pRecord = pShape;
	for (i = 0; i < pRecord->Get_Table()->Get_Field_Count(); i++){
		if (m_bIncludeParam[i]){
			if (!iValidFields){
				fMin = fMax = pRecord->asFloat(i);
			}
			else{					
				if (pRecord->asFloat(i) > fMax){
					fMax = pRecord->asFloat(i);
				}//if
				if (pRecord->asFloat(i) < fMin){
					fMin = pRecord->asFloat(i);
				}//if
			}//else
			iValidFields++;
		}//if
	}//for

	if (fMax > 0 && fMin > 0){
		fMin = 0;
	}//if

	if (fMax < 0 && fMin < 0){
		fMax = 0;
	}//if
	fSize = pRecord->asFloat(iSizeField);
	fSize = m_fMinSize + (m_fMaxSize - m_fMinSize)/(m_fMaxValue - m_fMinValue) * (fSize - m_fMinValue);

	switch (iType){
	case SHAPE_TYPE_Polygon:
		Point = ((CSG_Shape_Polygon*) pShape)->Get_Centroid();	
		break;
	case SHAPE_TYPE_Line:
		Point = GetLineMidPoint((CSG_Shape_Line*)pShape);
		break;
	case SHAPE_TYPE_Point:
		Point = pShape->Get_Point(0);
		break;
	default:
		break;
	}//switch
	dX = Point.x;
	dY = Point.y;
	
	fBarWidth = fSize / (float)iValidFields;

	iField = 1;
	for (i = 0; i < pRecord->Get_Table()->Get_Field_Count(); i++){
		if (m_bIncludeParam[i]){
			fBarHeight = pRecord->asFloat(i) / (fMax - fMin) * fSize;
			pSector = m_pOutput->Add_Shape();
			pSector->Add_Point(dX - fSize / 2. + fBarWidth * (iField - 1) ,
								dY);
			pSector->Add_Point(dX - fSize / 2. + fBarWidth * iField,
								dY);
			pSector->Add_Point(dX - fSize / 2. + fBarWidth * iField,
								dY + fBarHeight);
			pSector->Add_Point(dX - fSize / 2. + fBarWidth * (iField - 1) ,
								dY + fBarHeight);
			pSector->Set_Value(0,iField);
			pSector->Set_Value(1,pRecord->Get_Table()->Get_Field_Name(i));
			iField++;
		}//if
	}//for

}//method

TSG_Point CCreateChartLayer::GetLineMidPoint(CSG_Shape_Line *pLine){

	int i;
	float fDist, fAccDist = 0;
	float fLength = pLine->Get_Length(0) / 2.;
	TSG_Point Point, Point2, ReturnPoint;

	for (i = 0; i < pLine->Get_Point_Count(0) - 1; i++){
		Point = pLine->Get_Point(i);
		Point2 = pLine->Get_Point(i+1);
		fDist = sqrt(pow(Point.x - Point2.x,2.) + pow(Point.y - Point2.y,2.));
		if (fAccDist <= fLength && fAccDist + fDist > fLength){
			ReturnPoint.x = Point.x + (Point2.x - Point.x) * (fLength - fAccDist) / fDist;
			ReturnPoint.y = Point.y + (Point2.y - Point.y) * (fLength - fAccDist) / fDist;
			return ReturnPoint;
		}//if
		fAccDist += fDist;
	}//for

	return pLine->Get_Point(pLine->Get_Point_Count(0) / 2);

}//method
