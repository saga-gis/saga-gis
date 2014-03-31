/**********************************************************
 * Version $Id: Shapes_Summary.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    Summarize.cpp
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Shapes_Summary.h"

#define VERY_LARGE_NUMBER 9999999999.

CSG_String sParam[] = {"[Sum]", "[Mean]", "[Variance]", "[Minimum]", "[Maximum]"};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PDF_PAGE_WIDTH_A4       596
#define PDF_PAGE_HEIGHT_A4      842
#define PDF_PAGE_WIDTH_A3 PDF_PAGE_HEIGHT_A4
#define PDF_PAGE_HEIGHT_A3 (sqrt(2.) * PDF_PAGE_HEIGHT_A4)
#define OFFSET_X 50
#define OFFSET_Y 50
#define LINE_SPACEMENT 12
#define SUMMARY_LAYOUT_HEADER 50
#define MAX_SUMMARY_LAYOUT_HEIGHT (PDF_PAGE_WIDTH_A3 - 2 * OFFSET_Y - LINE_SPACEMENT)
#define MAX_SUMMARY_LAYOUT_WIDTH MAX_SUMMARY_LAYOUT_HEIGHT
#define SUMMARY_LAYOUT_SEPARATION 30
#define MAX_SUMMARY_TABLE_WIDTH (PDF_PAGE_HEIGHT_A3 - 2 * OFFSET_X - MAX_SUMMARY_LAYOUT_WIDTH - SUMMARY_LAYOUT_SEPARATION)
#define MAX_ROWS_IN_SUMMARY_LAYOUT (MAX_SUMMARY_LAYOUT_HEIGHT / TABLE_CELL_HEIGHT)
#define SUMMARY_STATISTICS_GRAPH_HEIGHT 120
#define SUMMARY_STATISTICS_GRAPH_WIDTH (PDF_PAGE_WIDTH_A4 - 2 * OFFSET_X)
#define SUMMARY_STATISTICS_GRAPH_SEPARATION 120
#define GRATICULE_SEPARATION 20
#define SUMMARY_TABLE_CELL_HEIGHT 13.0


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

CShapes_Summary_PDF::CShapes_Summary_PDF(){

}//constructor

CShapes_Summary_PDF::~CShapes_Summary_PDF(){}

void CShapes_Summary_PDF::AddClassSummaryPage(CSG_Shapes *pShapes, 
											   CSG_Table *pTable,
											   int iClass,
											   CSG_String sTitle){

	int i;
	double fPaperHeight;
	double fRealWidth, fRealHeight;
	double fX, fY;
	CSG_Table *pClassTable;
	CSG_Table_Record *pRecord;
	CSG_Rect CanvasExtent, TableExtent;
	TSG_Rect Extent;
	

	Add_Page(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE);
	Add_Outline_Item(sTitle.c_str());

	fPaperHeight = Get_Size_Page().Get_YRange();

	Draw_Text(PDF_PAGE_HEIGHT_A3 - OFFSET_X, fPaperHeight - OFFSET_Y, sTitle, 
				25, PDF_STYLE_TEXT_ALIGN_H_RIGHT, 0.0, SG_GET_RGB(0, 0, 0));

	Draw_Line(OFFSET_X, fPaperHeight - OFFSET_Y - 5, 
				PDF_PAGE_HEIGHT_A3 - OFFSET_X, fPaperHeight - OFFSET_Y - 5, 4 );

	Extent = pShapes->Get_Extent();
	fRealWidth = Extent.xMax - Extent.xMin;
	fRealHeight = Extent.yMax - Extent.yMin;

	CanvasExtent.Assign(OFFSET_X,
						OFFSET_Y,
						OFFSET_X + MAX_SUMMARY_LAYOUT_WIDTH,
						OFFSET_Y + MAX_SUMMARY_LAYOUT_HEIGHT);

	_Fit_Rectangle(CanvasExtent, fRealWidth / fRealHeight, true);

	CanvasExtent.Deflate(GRATICULE_SEPARATION, false);

	Draw_Shapes(CanvasExtent, pShapes);

	CanvasExtent.Inflate(GRATICULE_SEPARATION, false);

	/*CanvasExtent.xMin = OFFSET_X + (MAX_SUMMARY_LAYOUT_WIDTH - fWidth) / 2. - GRATICULE_SEPARATION;
	CanvasExtent.xMax = CanvasExtent.xMin + fWidth + 2 * GRATICULE_SEPARATION;
	CanvasExtent.yMin = OFFSET_Y + (MAX_SUMMARY_LAYOUT_HEIGHT - fHeight) / 2. - GRATICULE_SEPARATION;
	CanvasExtent.yMax = CanvasExtent.yMin + fHeight + 2* GRATICULE_SEPARATION;*/

	Draw_Graticule(CanvasExtent, Extent);

	pClassTable = SG_Create_Table();
	pClassTable->Add_Field(_TL("Field"), SG_DATATYPE_String );
	pClassTable->Add_Field(_TL("Value"), SG_DATATYPE_Double);

	for (i = 0; i < pTable->Get_Field_Count(); i++){
		pRecord = pClassTable->Add_Record();
		pRecord->Set_Value(0, pTable->Get_Field_Name(i));
		pRecord->Set_Value(1, pTable->Get_Record(iClass)->asDouble(i));
	}
	fY = fPaperHeight - OFFSET_Y - SUMMARY_LAYOUT_HEADER;
	fX = PDF_PAGE_HEIGHT_A3 - OFFSET_X - MAX_SUMMARY_TABLE_WIDTH;
	TableExtent.Assign(fX, fY, fX + MAX_SUMMARY_TABLE_WIDTH, OFFSET_Y);

	Draw_Table(TableExtent, pClassTable, SUMMARY_TABLE_CELL_HEIGHT, 0.0);
	
}//method

void CShapes_Summary_PDF::AddSummaryStatisticsPage(CSG_Table* pTable){

	int i,j;
	double fX, fY;
	double fPaperHeight;
	CSG_Points Data;
	CSG_String *pNames;
	CSG_Rect r;

	fPaperHeight = Get_Size_Page().Get_YRange();

	pNames = new CSG_String [pTable->Get_Record_Count()];
	for (i = 0; i < pTable->Get_Record_Count(); i++){
		pNames[i] = pTable->Get_Record(i)->asString(0);
	}//for

	for (i = 1; i < pTable->Get_Field_Count(); i++){
		if ((i-1)%3 == 0){
			Add_Page();
		}//if
		Data.Clear();
		for (j = 0; j < pTable->Get_Record_Count(); j++){
			Data.Add(0.0, pTable->Get_Record(j)->asDouble(i)); 
		}//for		
		fY = OFFSET_Y + ((i-1)%3) * (SUMMARY_STATISTICS_GRAPH_HEIGHT + SUMMARY_STATISTICS_GRAPH_SEPARATION);
		fY = fPaperHeight - fY;
		fX = OFFSET_X;		
		Draw_Text(fX, fY + 10, pTable->Get_Field_Name(i), 14);
		r.Assign(fX, fY - 25 - SUMMARY_STATISTICS_GRAPH_HEIGHT, fX + SUMMARY_STATISTICS_GRAPH_WIDTH, fY - 25 );
		AddBarGraphStatistics(Data, pNames, r);
	}//for*/


}//method

void CShapes_Summary_PDF::AddBarGraphStatistics(CSG_Points &Data,
										CSG_String *pNames,
										const CSG_Rect &r){

	int i;
    int iMag;
	int iNumLines;
    double fMinLine = 0;
	double fWidth;
	double fMax, fMin;
	double fStep;
    double fX, fY;
	double fPaperHeight;
	double fAngle;
	CSG_String sValue;
	CSG_Points Points;

	fPaperHeight = Get_Size_Page().Get_YRange();

	fMin = fMax = Data[0].y;	

	for (i = 0; i < Data.Get_Count(); i++){
		if (Data[i].y > fMax){
			fMax = Data[i].y;
		}
		if (Data[i].y < fMin){
			fMin = Data[i].y;
		}		
		fMin = M_GET_MIN(0, fMin);		
    }

	if (fMin != fMax){
		iMag = (int) (log(fMax - fMin) / log(10.0));
		fStep =  (pow(10.0, (double) iMag));
		if (fStep == 0){
			fStep = 1.;
		}
		fMinLine= (long)(((long)(fMin/fStep)) *fStep);

		iNumLines = (int) ((fMax - fMin) / fStep);

		while (iNumLines < 8){
			fStep = fStep / 2.0;
			iNumLines = (int) ((fMax - fMin) / fStep);
		}
		iNumLines = (int) ((fMax - fMinLine) / fStep);
	}
	else{
		fStep = fMin = 0;
		iNumLines = 1;
		fMinLine = fMax;
	}


	for (i = 0; i < iNumLines; i++) {
        fY = r.Get_YMin() + ((fMinLine + fStep * i - fMin) / (fMax - fMin)) * r.Get_YRange();
        if (fY <= r.Get_YMax() && fY >= r.Get_YMin()) {
			Draw_Line(r.Get_XMin(), fY, r.Get_XMax(), fY);
        }
    }

	fWidth = (double) r.Get_XRange() / (double) (Data.Get_Count());
	for (i = 0; i < Data.Get_Count(); i++)
	{
	    fX = r.Get_XMin() + i * fWidth;
		fY = r.Get_YMin();
		//fY = fPaperHeight - fY;
		Draw_Rectangle(fX, fY, fX + fWidth,
				fY + r.Get_YRange() * ((Data[i].y - fMin) / (fMax - fMin)),
				PDF_STYLE_POLYGON_FILLSTROKE, 0xaa0000);
	}

	fAngle = 3.14159/ 180 * 60;

	for (i = 0; i < Data.Get_Count(); i++)
	{
	    fX = r.Get_XMin() + i * fWidth + fWidth / 2.;
		Draw_Text(fX - 5, fY - 5, pNames[i], 9, PDF_STYLE_TEXT_ALIGN_V_TOP|PDF_STYLE_TEXT_ALIGN_H_RIGHT, fAngle);
		Draw_Line(fX, fY, fX, fY - 5, 1);
    }

	Draw_Line(r.Get_XMin(), r.Get_YMin(), r.Get_XMin(), r.Get_YMax(), 4);
	Draw_Line(r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), r.Get_YMin(), 4);

										
}//method


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Summary::CShapes_Summary(void){

	CSG_Parameter *pNode;
	CSG_String sName;

	Parameters.Set_Name(_TL("Shapes Summary Report"));
	Parameters.Set_Description(_TW("(c) 2004 by Victor Olaya. summary."));

	pNode = Parameters.Add_Shapes(NULL,
								"SHAPES",
								_TL("Shapes"),
								_TL(""),
								PARAMETER_INPUT);

	Parameters.Add_Table_Field(pNode,
								"FIELD",
								_TL("Field"),
								_TL(""));

	Parameters.Add_Table(NULL,
						"TABLE",
						_TL("Summary Table"),
						_TL(""),
						PARAMETER_OUTPUT);

	pNode = Parameters.Add_Node(NULL,
						"PDFNODE",
						_TL("PDF Docs"),
						_TL(""));

	Parameters.Add_Value(pNode,
						"PDF",
						_TL("Create PDF Docs"),
						_TL("Create PDF Docs"),
						PARAMETER_TYPE_Bool,
						true);

	Parameters.Add_FilePath(pNode,
							"OUTPUTPATH",
							_TL("Folder"),
							_TL("Folder"),
							_TL(""),
							_TL(""),
							true,
							true);

	m_pExtraParameters	= Add_Parameters("EXTRA",
										_TL("Field for Summary"),
										_TL(""));

}//constructor


CShapes_Summary::~CShapes_Summary(void)
{}

bool CShapes_Summary::On_Execute(void){

	int i,j;
	CSG_Table *pShapesTable;
	CSG_Parameter **pExtraParameter;
	CSG_String sName, sFilePath;

	m_iField = Parameters("FIELD")->asInt();
	m_pShapes = Parameters("SHAPES")->asShapes();
	m_pTable = Parameters("TABLE")->asTable();

	pShapesTable = m_pShapes;
	m_bIncludeParam = new bool [pShapesTable->Get_Field_Count() * 5];
	pExtraParameter = new CSG_Parameter* [pShapesTable->Get_Field_Count() * 5];

	for (i = 0; i < pShapesTable->Get_Field_Count(); i++){
		for (j = 0; j < 5; j++){
			if( SG_Data_Type_is_Numeric(pShapesTable->Get_Field_Type(i)) )
			{ //is numeric field
				sName = pShapesTable->Get_Field_Name(i);
				sName.Append(sParam[j]);
				pExtraParameter[i * 5 + j] = m_pExtraParameters->Add_Value(NULL,
																			SG_Get_String(i * 5 + j,0).c_str(),
																			sName.c_str(),
																			_TL(""),
																			PARAMETER_TYPE_Bool,
																			false);
				m_bIncludeParam[i * 5 + j] = true;
			}//if
			else{
				m_bIncludeParam[i * 5 + j] = false;
			}//else
		}//for
	}//for

	if(Dlg_Parameters("EXTRA")){
		for (i = 0; i < pShapesTable->Get_Field_Count() * 5; i++){
			sName = SG_Get_String(i,0);
			if (m_bIncludeParam[i]){
				m_bIncludeParam[i] = Get_Parameters("EXTRA")->Get_Parameter(sName.c_str())->asBool();
			}//if			
		}//for

		Summarize();

		if (Parameters("PDF")->asBool()){
			if (Parameters("OUTPUTPATH")->asString()){
				sName = _TL("Summary_");
				sName.Append(m_pShapes->Get_Name());
				m_DocEngine.Open(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE, sName);
				CreatePDFDocs();
				sFilePath = SG_File_Make_Path(Parameters("OUTPUTPATH")->asString(), sName, SG_T("pdf"));
				if (m_DocEngine.Save(sFilePath)){
					if (!m_DocEngine.Close()){
						Message_Add(_TL("\n\n ** Error : Could not close PDF engine ** \n\n"));
					}
				}//if
				else{
					Message_Add(_TL("\n\n ** Error : Could not save PDF file ** \n\n"));
				}//else
			}//if
		}//if

		m_pExtraParameters->Destroy();

		delete [] m_bIncludeParam;

		return true;

	}//if

	m_pExtraParameters->Destroy();

	delete [] m_bIncludeParam;

	return false;

}//method

void CShapes_Summary::CreatePDFDocs(){

	CSG_Shapes *pShapes;
	CSG_Shape *pShape;
	CSG_Table *pShapesTable;
	int i,j;
	
	m_DocEngine.Add_Page_Title (_TL("Summary"), PDF_TITLE_01, PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE);

	pShapesTable = m_pShapes;
	pShapes = new CSG_Shapes();
	for (i = 0; i < (int)m_ClassesID.size(); i++){
		Set_Progress(i,m_ClassesID.size());
		pShapes->Create(m_pShapes->Get_Type());
		for (j = 0; j < m_pShapes->Get_Count(); j++){
			if (m_pClasses[j] == i){
				pShape = pShapes->Add_Shape();
				pShape->Assign(m_pShapes->Get_Shape(j));
			}//if
		}//for
		m_DocEngine.AddClassSummaryPage(pShapes, m_pTable, i, m_ClassesID[i]);
	}//for
	
	if (m_pTable->Get_Record_Count() > 1){
		m_DocEngine.Add_Page_Title (_TL("Statistics"), PDF_TITLE_01, PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT);
		m_DocEngine.AddSummaryStatisticsPage(m_pTable);
	}//if

	delete pShapes;

}//method


void CShapes_Summary::Summarize(){

	int i,j;

	CSG_Table *pShapesTable;
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

	m_ClassesID.clear();

	pShapesTable = m_pShapes;
	m_pClasses = new int[pShapesTable->Get_Record_Count()];

	for (i = 0; i < pShapesTable->Get_Record_Count(); i++){
		pRecord = pShapesTable->Get_Record(i);
		sName = pRecord->asString(m_iField);
		for (j = 0; j < (int)m_ClassesID.size(); j++){
			if (!m_ClassesID[j].CmpNoCase(sName)){
				m_pClasses[i] = j;
				break;
			}//if
		}//for
		if (j == m_ClassesID.size()){
			m_pClasses[i] = j;
			m_ClassesID.push_back(sName);
		}//if
	}//for

	m_pTable->Create((CSG_Table*)NULL);
	m_pTable->Set_Name(_TL("Summary Table"));
	m_pTable->Add_Field(_TL("Class"), SG_DATATYPE_String);
	m_pTable->Add_Field(_TL("Count"), SG_DATATYPE_Int);

	m_pCount = new int[m_ClassesID.size()];
	pSum = new float[m_ClassesID.size()];
	pMax = new float[m_ClassesID.size()];
	pMin = new float[m_ClassesID.size()];
	pVar = new float[m_ClassesID.size()];

	for (i = 0; i < (int)m_ClassesID.size(); i++){
		m_pCount[i] = 0;
	}//for

	for (i = 0; i < pShapesTable->Get_Record_Count(); i++){
		m_pCount[m_pClasses[i]]++;
	}//for

	for (i = 0; i < (int)m_ClassesID.size(); i++){
		pRecord = m_pTable->Add_Record();
		sName = m_ClassesID[i];
		pRecord->Set_Value(0,sName.c_str());
		pRecord->Set_Value(1,m_pCount[i]);
	}//for

	for (i = 0; i < pShapesTable->Get_Field_Count() * 5; i++){
		if (m_bIncludeParam[i]){
			iField = (int) (i / 5);
			iParam = i % 5;
			sName = pShapesTable->Get_Field_Name(iField);
			sName.Append(sParam[iParam]);
			m_pTable->Add_Field(sName.c_str(), SG_DATATYPE_Double);
			if (iField != iLastField){
				for (j = 0; j < (int)m_ClassesID.size(); j++){
					pSum[j] = 0;
					pMax[j] = -(float)VERY_LARGE_NUMBER;
					pMin[j] =  (float)VERY_LARGE_NUMBER;
					pVar[j] = 0;
				}//for
				for (j = 0; j < pShapesTable->Get_Record_Count(); j++){
					pRecord = pShapesTable->Get_Record(j);
					fValue = pRecord->asFloat(iField);
					pSum[m_pClasses[j]] += fValue;
					pVar[m_pClasses[j]] += (fValue * fValue);
					if (fValue > pMax[m_pClasses[j]]){
						 pMax[m_pClasses[j]] = fValue;
					}//if
					if (fValue < pMin[m_pClasses[j]]){
						 pMin[m_pClasses[j]] = fValue;
					}//if
				}//for
			}//if
			iField = m_pTable->Get_Field_Count() - 1;
			for (j = 0; j < (int)m_ClassesID.size(); j++){
				pRecord = m_pTable->Get_Record(j);
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
