/*******************************************************************************
    CPolygonStatisticsFromPoints.cpp
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
#include "Polygon_StatisticsFromPoints.h"
#include <string>

#define VERY_LARGE_NUMBER 9999999999.
#define IS_OUTSIDE -1

CSG_String	sParamName[]	= {"[Sum]", "[Average]", "[Variance]", "[Minimum]", "[Maximum]"};

CPolygonStatisticsFromPoints::CPolygonStatisticsFromPoints(){
				   
	Parameters.Set_Name(_TL("Polygon statistics from points"));

	Parameters.Set_Description(_TW(
		"Calculates statistics using a polygon and a points layer."
		"(c) 2005 by Victor Olaya\r\nemail: volaya@ya.com")
	);

	Parameters.Add_Shapes(NULL, 
						"POLYGONS", 
						_TL("Polygons"), 
						_TL(""), 
						PARAMETER_INPUT);	
	
	Parameters.Add_Shapes(NULL, 
						"POINTS", 
						_TL("Points"), 
						_TL(""), 
						PARAMETER_INPUT);

	m_pExtraParameters	= Add_Parameters("EXTRA",
												_TL("Fields to add"),
												_TL(""));

}//constructor

CPolygonStatisticsFromPoints::~CPolygonStatisticsFromPoints(){

}//destructor

bool CPolygonStatisticsFromPoints::On_Execute(void){

	int i,j;
	m_pPoints = Parameters("POINTS")->asShapes();
	m_pPolygons = Parameters("POLYGONS")->asShapes();
	CSG_Table *pShapesTable, *pTable;
	CSG_Parameter **pExtraParameter;
	CSG_String sName;
	
	pTable = m_pPoints;
	pShapesTable = m_pPolygons;
	m_bIncludeParam = new bool [pTable->Get_Field_Count() * 5];
	pExtraParameter = new CSG_Parameter* [pTable->Get_Field_Count() * 5];

	for (i = 0; i < pTable->Get_Field_Count(); i++){
		for (j = 0; j < 5; j++){
			if (pTable->Get_Field_Type(i) > 1 && pTable->Get_Field_Type(i) < 7){ //is numeric field
				sName.Printf(SG_T("%s%s"),
					pTable->Get_Field_Name(i),
					SG_STR_MBTOSG(sParamName[j])
				);

				pExtraParameter[i * 5 + j] = m_pExtraParameters->Add_Value(
					NULL,
					SG_Get_String(i * 5 + j,0).c_str(), 
					sName.c_str(),
					_TL(""),
					PARAMETER_TYPE_Bool, 
					false
				);
			}//if
		}//for
	}//for
	if(Dlg_Parameters("EXTRA")){
		for (i = 0; i < pTable->Get_Field_Count() * 5; i++){
			sName = SG_Get_String(i);
			try{
				m_bIncludeParam[i] = Get_Parameters("EXTRA")->Get_Parameter(sName.c_str())->asBool();
			}//try
			catch(...){
				m_bIncludeParam[i] = false;
			}//catch
		}//for
	
		CalculateStatistics();
		
		m_pExtraParameters->Destroy();
		
		delete [] m_bIncludeParam;
	
		return true;	
	}//if

	m_pExtraParameters->Destroy();

	delete [] m_bIncludeParam;
	
	return false;


}//method

void CPolygonStatisticsFromPoints::CalculateStatistics(){

	int i,j;
	int iPoly;
	CSG_Shape *pPoly, *pShape;
	CSG_Table *pPointsTable, *pPolygonsTable;
	CSG_Table_Record *pRecord;
	CSG_String sName;
	float *pSum;
	float *pMin;
	float *pMax;
	float *pVar;
	float fValue;
	float fMean;
	int iLastField = -1;
	int iField;
	int iParam;

	pPointsTable = m_pPoints;

	pPolygonsTable = m_pPolygons;	
	pPolygonsTable->Add_Field("Count", TABLE_FIELDTYPE_Int);
	m_pClasses = new int[pPointsTable->Get_Record_Count()];
	m_pCount = new int[pPolygonsTable->Get_Record_Count()];
	pSum = new float[pPolygonsTable->Get_Record_Count()];
	pMax = new float[pPolygonsTable->Get_Record_Count()];
	pMin = new float[pPolygonsTable->Get_Record_Count()];
	pVar = new float[pPolygonsTable->Get_Record_Count()];

	for (i = 0; i < pPointsTable->Get_Record_Count(); i++){
		m_pClasses[i] = IS_OUTSIDE;
	}//for

	for(iPoly = 0; iPoly < m_pPolygons->Get_Count(); iPoly++){
		pPoly = m_pPolygons->Get_Shape(iPoly);
		for(i = 0; i < m_pPoints->Get_Count(); i++){			
			pShape = m_pPoints->Get_Shape(i);					
			if (((CSG_Shape_Polygon*)pPoly)->is_Containing(pShape->Get_Point(0, 0))){
				m_pClasses[i] = iPoly;
			}//if
		}//for
	}//for

	for (i = 0; i < pPolygonsTable->Get_Record_Count(); i++){
		if (m_pClasses[i] != IS_OUTSIDE){
			m_pCount[i] = 0;
		}//if
	}//for

	for (i = 0; i < pPointsTable->Get_Record_Count(); i++){
		m_pCount[m_pClasses[i]]++;
	}//for
	
	for (i = 0; i < pPolygonsTable->Get_Record_Count(); i++){
		pRecord = pPolygonsTable->Get_Record(i);
		pRecord->Set_Value(pPolygonsTable->Get_Field_Count() - 1, m_pCount[i]);
	}//for

	for (i = 0; i < pPointsTable->Get_Field_Count() * 5; i++){
		if (m_bIncludeParam[i]){
			iField = (int) (i / 5);
			iParam = i % 5;
			sName.Printf(SG_T("%s%s"), pPointsTable->Get_Field_Name(iField), SG_STR_MBTOSG(sParamName[iParam]));
			pPolygonsTable->Add_Field(sName.c_str(), TABLE_FIELDTYPE_Double); 
			if (iField != iLastField){					
				for (j = 0; j < pPolygonsTable->Get_Record_Count(); j++){
					pSum[j] = 0;
					pMax[j] = -(float)VERY_LARGE_NUMBER;
					pMin[j] =  (float)VERY_LARGE_NUMBER;
					pVar[j] = 0;
				}//for
				for (j = 0; j < pPointsTable->Get_Record_Count(); j++){
					if (m_pClasses[j] != IS_OUTSIDE){
						pRecord = pPointsTable->Get_Record(j);
						fValue = pRecord->asFloat(iField);
						pSum[m_pClasses[j]] += fValue;
						pVar[m_pClasses[j]] += (fValue * fValue);
						if (fValue > pMax[m_pClasses[j]]){
							 pMax[m_pClasses[j]] = fValue;
						}//if
						if (fValue < pMin[m_pClasses[j]]){
							 pMin[m_pClasses[j]] = fValue;
						}//if
					}//if
				}//for
			}//if
			iField = pPolygonsTable->Get_Field_Count() - 1;
			for (j = 0; j < pPolygonsTable->Get_Record_Count(); j++){
				pRecord = pPolygonsTable->Get_Record(j);
				switch (iParam){
				case 0: //sum
					pRecord->Set_Value(iField, pSum[j]);
					break;
				case 1: //average
					pRecord->Set_Value(iField, pSum[j] / (float) m_pCount[j]);
					break;
				case 2: //variance
					fMean = pSum[j] / (float) m_pCount[j];
					pRecord->Set_Value(iField, pVar[j] / (float) m_pCount[j] - fMean * fMean);
					break;
				case 3: //min
					pRecord->Set_Value(iField, pMin[j]);
					break;
				case 4: //max
					pRecord->Set_Value(iField, pMax[j]);
					break;
				default:
					break;
				}//switch
			}//for

		}//if
	
	}//for

}//method

