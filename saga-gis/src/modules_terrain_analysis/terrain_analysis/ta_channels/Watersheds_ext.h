/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Watersheds.h
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

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWatersheds_ext : public CSG_Module_Grid
{
public:
	CWatersheds_ext(void);


protected:

	virtual bool		On_Execute		(void);


private:

	CSG_Grid			*m_pDEM, *m_pChannels, m_Distance, m_Direction;


	bool				Get_Basin		(CSG_Grid *pBasins, CSG_Shapes *pPolygons, int xMouth, int yMouth, int Main_ID);
	CSG_Shape *			Get_Basin		(CSG_Grid *pBasins, CSG_Shapes *pPolygons);

	bool				is_Outlet		(int x, int y);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
