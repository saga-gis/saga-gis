/**********************************************************
 * Version $Id: EdgeContamination.h 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/
/*******************************************************************************
    EdgeContamination.h
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
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CEdgeContamination : public CSG_Tool_Grid
{
public:
	CEdgeContamination(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("Flow Accumulation" ));	}


protected:

	virtual bool			On_Execute			(void);


private:

	CSG_Grid				*m_pDEM, *m_pEffect;


	int						Get_D8				(int x, int y);
	int						Set_D8				(int x, int y);

	bool					Get_MFD				(int x, int y, double dz[8]);
	int						Set_MFD				(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
