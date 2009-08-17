/*******************************************************************************
    ShapeSelector.h
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

#include <saga_api/saga_api.h>
#include <vector>

class CShapeSelector  
{
public:
	CShapeSelector(CSG_Shapes *pShapes, CSG_Shapes *pShapes2, int iCondition, bool bFromSelection = false);

	int		Get_Count				(void)	{	return( m_Selection.size() );	}
	int		Get_Index				(int i)	{	return( m_Selection[i] );		}


private:

	std::vector <int> m_Selection;

};

