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

#include "Grid_CrossSections.h"
#include "Geometry.h"

CGrid_CrossSections::CGrid_CrossSections(void){

	CParameter *pNode;

	Parameters.Set_Name(_TL("Cross Sections"));
	Parameters.Set_Description(_TL("(c) 2004 Victor Olaya. Cross Sections"));

	Parameters.Add_Grid(NULL,
						"DEM",
						_TL("DEM"),
						_TL("Digital Terrain Model"),
						PARAMETER_INPUT);

	Parameters.Add_Shapes(NULL,
						"LINES",
						_TL("Path"),
						"",
						PARAMETER_INPUT, 
						SHAPE_TYPE_Line);

	Parameters.Add_Shapes(NULL,
						"SECTIONS",
						_TL("Cross Sections"),
						"",
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
						"n",
						_TL("Draw a section each n points"),
						PARAMETER_TYPE_Int,
						2);

	pNode = Parameters.Add_Node(NULL,
						"PDFNODE",
						_TL("PDF Documents"),
						"");

	Parameters.Add_Value(pNode,
						"PDF",
						_TL("Create PDF Documents"),
						"",
						PARAMETER_TYPE_Bool,
						true);	

	Parameters.Add_FilePath(pNode,
							"OUTPUTPATH",
							_TL("Folder for PDF Files"),
							"",
							"",
							"",
							true,
							true);

	Parameters.Add_Value(pNode,
						"WIDTH",
						_TL("Road Width"),
						"",
						PARAMETER_TYPE_Double,
						5.);

	Parameters.Add_Value(pNode,
						"SLOPE",
						_TL("Side Slope"),
						"",
						PARAMETER_TYPE_Double,
						.5);

	Parameters.Add_Value(pNode,
						"THRESHOLD",
						_TL("Height Threshold"),
						_TL("Height threshold for optimal profile calculation"),
						PARAMETER_TYPE_Double,
						.5);
}//constructor


CGrid_CrossSections::~CGrid_CrossSections(void){}

bool CGrid_CrossSections::On_Execute(void){

	CShapes *pLines;
	CShape *pShape, *pSection;
	CGrid *pDEM;
	TSG_Point	Point, Point2;
	float fInterval;
	float fStepX, fStepY;
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
	fInterval = (float) Parameters("INTERVAL")->asDouble();
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
		m_pSections->Get_Table().Add_Field(CSG_String::Format("-%s", SG_Get_String(fInterval * i, 2).c_str()),
										TABLE_FIELDTYPE_Double);
	}//for
	m_pSections->Get_Table().Add_Field("0", TABLE_FIELDTYPE_Double);
	for (i = 1; i < iNumPoints +1; i++){
		m_pSections->Get_Table().Add_Field(SG_Get_String(fInterval * i).c_str(), TABLE_FIELDTYPE_Double);
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
					pSection->Get_Record()->Set_Value(iField, dHeight);
					iField++;
				}//for
				dHeight = pDEM->Get_Value(dX, dY);
				pSection->Get_Record()->Set_Value(iField, dHeight);
				iField++;
				for (iPoint = 1; iPoint < iNumPoints +1; iPoint++){
					dX2 = dX + iPoint * fStepX;
					dY2 = dY + iPoint * fStepY;
					dHeight = pDEM->Get_Value(dX2, dY2);
					pSection->Get_Record()->Set_Value(iField, dHeight);
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

void CGrid_CrossSections::CreatePDFDocs(){

	CSG_String sFilePath;
			
	if (Parameters("OUTPUTPATH")->asString()){
		m_DocEngine.Open(_TL("SECTIONS"));
		sFilePath = SG_File_Make_Path(Parameters("OUTPUTPATH")->asString(), _TL("Sections"), "pdf");
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


void CGrid_CrossSections::AddLongitudinalProfiles(){

	int i,j,k;
	int iIndex = 0;	
	int iNumPoints = Parameters("NUMPOINTS")->asInt();
	int iStep = Parameters("STEP")->asInt();
	int iFirstPoint = 0;
	int iLastPoint;
	int iSections;
	float fX1, fY1, fX2, fY2;
	float fDistance, fDistanceSum = 0;
	float fDist;
	float fThreshold = (float)Parameters("THRESHOLD")->asDouble();
	bool bValid;
	CShape *pShape;
	CTable *pTable;
	TSG_Point	Point, Point2;
	CShapes* pLines = Parameters("LINES")->asShapes();

	pTable = &m_pSections->Get_Table();
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

	m_pHeight = new float[iSections];

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

float CGrid_CrossSections::CalculatePointLineDist(float fX1, float fY1, float fX2, float fY2, float fPX, float fPY){

	float *pA, *pB, *pC;

	pA = new float[2];
	pA[0] = fX1;
	pA[1] = fY1;
	pB = new float[2];
	pB[0] = fX2;
	pB[1] = fY2;
	pC = new float[2];
	pC[0] = fPX;
	pC[1] = fPY;
	
	return linePointDist(pA, pB, pC);

}//method

void CGrid_CrossSections::AddCrossSections(){

	int i,j;
	int iRoadPoints;
	int iNumPoints = Parameters("NUMPOINTS")->asInt();
	float fInterval = (float) Parameters("INTERVAL")->asDouble();
	CTable *pTable;
	TSG_Point *pRoadSection;
	TSG_Point **pCrossSections;
	double dWidth = Parameters("WIDTH")->asDouble();

	pRoadSection = new TSG_Point [2];
	pRoadSection[0].x = -dWidth / 2.;
	pRoadSection[0].y = 0;
	pRoadSection[1].x = dWidth / 2.;
	pRoadSection[1].y = 0;

	pTable = &m_pSections->Get_Table();
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
