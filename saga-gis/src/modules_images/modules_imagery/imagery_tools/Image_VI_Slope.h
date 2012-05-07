/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Image_VI_Slope.h
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
#ifndef HEADER_INCLUDED__Image_VI_Slope_H
#define HEADER_INCLUDED__Image_VI_Slope_H


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
class CImage_VI_Slope : public CSG_Module_Grid
{
public:
	CImage_VI_Slope(void);

	virtual const SG_Char *	Get_MenuPath	(void)	{	return( _TL("Vegetation Indices") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	double					m_Soil, m_Offset, m_Gain;


	bool					Get_DVI			(double R, double NIR, double &Value);
	bool					Get_NDVI		(double R, double NIR, double &Value);
	bool					Get_RVI			(double R, double NIR, double &Value);
	bool					Get_NRVI		(double R, double NIR, double &Value);
	bool					Get_TVI			(double R, double NIR, double &Value);
	bool					Get_CTVI		(double R, double NIR, double &Value);
	bool					Get_TTVI		(double R, double NIR, double &Value);
	bool					Get_SAVI		(double R, double NIR, double &Value);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Image_VI_Slope_H
