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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 
#include "MLB_Interface.h"
#include "PDFDocEngine_CrossSections.h"

class CGrid_CrossSections : public CModule_Grid
{
public:
	CGrid_CrossSections(void);
	virtual ~CGrid_CrossSections(void);

protected:

	bool On_Execute(void);

private:

	CPDFDocEngine_CrossSections m_DocEngine;
	CShapes *m_pSections;
	float *m_pHeight;
	TSG_Point *m_pProfile;

	void CreatePDFDocs();
	void AddLongitudinalProfiles();
	void AddCrossSections();
	float CalculatePointLineDist(float,float,float,float,float,float);

};
