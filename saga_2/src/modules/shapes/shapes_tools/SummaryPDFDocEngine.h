/*******************************************************************************
    SummaryPDFDocEngine.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <saga_api/pdf_document.h>

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

class CSummaryPDFDocEngine : public CPDF_Document 
{
public:

	CSummaryPDFDocEngine();
	virtual ~CSummaryPDFDocEngine();
	void AddClassSummaryPage(CShapes*, CTable*, int, CAPI_String);
	void AddSummaryStatisticsPage(CTable* pTable);

private:

	void AddBarGraphStatistics(CAPI_dPoints &, CAPI_String*,	const CGEO_Rect &);

};

