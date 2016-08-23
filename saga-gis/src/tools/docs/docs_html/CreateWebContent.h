/**********************************************************
 * Version $Id: CreateWebContent.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    Photolinks.h
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

#ifndef HEADER_INCLUDED__CreateWebContent_H
#define HEADER_INCLUDED__CreateWebContent_H

#include "doc_html.h"
#include <vector>

class CCreateWebContent : public CSG_Tool_Interactive
{
public:

	CCreateWebContent(void);
	virtual ~CCreateWebContent(void);

protected:

	virtual bool				On_Execute(void);
	virtual bool				On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);
	virtual bool				On_Execute_Finish(void);

private:

	bool						m_bDown;
	CSG_Point					m_ptDown;
	CSG_Shapes						*m_pShapes;
	int							m_iField, m_iNameField;
	CSG_String					m_sOutputPath;

	std::vector<CSG_String>	*m_Pictures;
	std::vector<CSG_String>	*m_Links;
	std::vector<CSG_String>	*m_LinksDescription;

};

#endif // #ifndef HEADER_INCLUDED__CreateWebContent_H
