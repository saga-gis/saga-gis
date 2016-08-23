/**********************************************************
 * Version $Id: idw.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    Isochrones.cpp
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
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <saga_api/saga_api.h>

class CIDW  
{
public:
	
	CIDW();
	virtual ~CIDW();
	void setParameters(CSG_Grid*,CSG_Shapes*,int);
	void Interpolate();

private:

	CSG_Grid* m_pGrid;
	CSG_Shapes *m_pPoints;
	int m_iField;

	bool Get_Grid_Value(int,int);

};

