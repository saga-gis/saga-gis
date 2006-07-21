/*******************************************************************************
    PDFDocEngine_CrossSections.h
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

#include <vector>
#include <saga_api/doc_pdf.h>
#include "Intersection_GPC.h"

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

class CPDFDocEngine_CrossSections : public CDoc_PDF   
{

public:
		
	CPDFDocEngine_CrossSections();
	~CPDFDocEngine_CrossSections();
	void AddCrossSections(TSG_Point **,float*,TSG_Point *,int,int,int);	
	void AddLongitudinalProfile(TSG_Point *, float*, int);
	void AddVolumesTable(TSG_Point *,TSG_Point **,float*,TSG_Point *,int,int,int);	

private:

	char *m_pPath;
	int m_iCanvasHeight;
	int m_iTableWidth;
	int m_iOffsetY;
	CSG_String m_sName;

	void AddCrossSection(TSG_Point *,TSG_Point *,int,int);
	void DrawGuitar(TSG_Point *,float*, int);
	void AddGuitarValue(float,float,float,float,int);
	void AdjustSections(TSG_Point *,TSG_Point *,std::vector<TSG_Point >&,std::vector<TSG_Point >&,float,int&,int&);
	void Intersect_Lines(float,float,float,float,float,float,float,float,float&,float&);
	void CalculateAreas(TSG_Point*,TSG_Point*,float,int,int,float&,float&);
	bool GPC_Intersection(CShape *pShape_A, CShape *pShape_B, CShape *pShape_AB);
	bool GPC_Create_Polygon(CShape *pShape, gpc_polygon *pPolygon);

};


