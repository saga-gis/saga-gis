/**********************************************************
 * Version $Id: ProtectionIndex.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    ProtectionIndex.h
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
#include "MLB_Interface.h"

class CProtectionIndex : public CSG_Module_Grid
{
public:
	CProtectionIndex(void);
	virtual ~CProtectionIndex(void);

protected:

	bool On_Execute(void);

private:

	double getProtectionIndex(int x, int y);

	CSG_Grid *m_pDEM;
	double m_dRadius;

};