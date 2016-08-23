/**********************************************************
 * Version $Id: SlopeLength.h 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/
/*******************************************************************************
    SlopeLength.h
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
class CSlopeLength : public CSG_Tool_Grid
{
public:
	CSlopeLength(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Miscellaneous") );	}


protected:
	
	virtual bool			On_Execute		(void);


private:

	CSG_Grid				*m_pDEM, *m_pLength, m_Slope;


	void					Get_Length		(int x, int y);	

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
