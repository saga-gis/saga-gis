/*******************************************************************************
    SummaryPDFDocEngine.cpp
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
#include "SummaryPDFDocEngine.h"
#include <saga_api/saga_api.h>

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

CSummaryPDFDocEngine::CSummaryPDFDocEngine(){

}//constructor

CSummaryPDFDocEngine::~CSummaryPDFDocEngine(){}

void CSummaryPDFDocEngine::AddClassSummaryPage(CSG_Shapes *pShapes, 
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

void CSummaryPDFDocEngine::AddSummaryStatisticsPage(CSG_Table* pTable){

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

void CSummaryPDFDocEngine::AddBarGraphStatistics(CSG_Points &Data,
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
