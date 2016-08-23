/**********************************************************
 * Version $Id: Profile_Cross_Sections.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    CrossSections.h
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

//---------------------------------------------------------
#include "doc_pdf.h"

#include <vector>


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CProfile_Cross_Sections_PDF : public CSG_Doc_PDF   
{
public:
	CProfile_Cross_Sections_PDF();
	~CProfile_Cross_Sections_PDF();
	void AddCrossSections(TSG_Point **,double*,TSG_Point *,int,int,int);	
	void AddLongitudinalProfile(TSG_Point *, double*, int);
	void AddVolumesTable(TSG_Point *,TSG_Point **,double*,TSG_Point *,int,int,int);	

private:
	char *m_pPath;
	int m_iCanvasHeight;
	int m_iTableWidth;
	int m_iOffsetY;
	CSG_String m_sName;

	void AddCrossSection(TSG_Point *,TSG_Point *,int,int);
	void DrawGuitar(TSG_Point *,double*, int);
	void AddGuitarValue(double,double,double,double,int);
	void AdjustSections(TSG_Point *,TSG_Point *,std::vector<TSG_Point >&,std::vector<TSG_Point >&,double,int&,int&);
	void Intersect_Lines(double,double,double,double,double,double,double,double,double&,double&);
	void CalculateAreas(TSG_Point*,TSG_Point*,double,int,int,double&,double&);
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CProfile_Cross_Sections : public CSG_Tool_Grid
{
public:
	CProfile_Cross_Sections(void);
	virtual ~CProfile_Cross_Sections(void);

protected:

	bool On_Execute(void);

private:

	CProfile_Cross_Sections_PDF m_DocEngine;
	CSG_Shapes *m_pSections;
	double *m_pHeight;
	TSG_Point *m_pProfile;

	void CreatePDFDocs();
	void AddLongitudinalProfiles();
	void AddCrossSections();
	double CalculatePointLineDist(double,double,double,double,double,double);

};

///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
