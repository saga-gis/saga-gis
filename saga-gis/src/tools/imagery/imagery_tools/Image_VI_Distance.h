/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Image_VI_Distance.h
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
#ifndef HEADER_INCLUDED__Image_VI_Distance_H
#define HEADER_INCLUDED__Image_VI_Distance_H


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
class CImage_VI_Distance : public CSG_Tool_Grid
{
public:
	CImage_VI_Distance(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Vegetation Indices") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	double					m_Intercept, m_Slope;


	bool					Get_PVI0		(double R, double NIR, double &Value);
	bool					Get_PVI1		(double R, double NIR, double &Value);
	bool					Get_PVI2		(double R, double NIR, double &Value);
	bool					Get_PVI3		(double R, double NIR, double &Value);
	bool					Get_TSAVI		(double R, double NIR, double &Value);
	bool					Get_ATSAVI		(double R, double NIR, double &Value);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Image_VI_Distance_H
