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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Image_VI_Distance_H
#define HEADER_INCLUDED__Image_VI_Distance_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CImage_VI_Distance : public CModule_Grid {

public:
	CImage_VI_Distance(void);
	virtual ~CImage_VI_Distance(void);

	virtual const char *	Get_MenuPath	(void)	{	return( _TL("R:Image Analysis") );	}


protected:
	virtual bool On_Execute(void);

private:
	double getPVI(double,double,double,double);
	double getPVI1(double,double,double,double);
	double getPVI2(double,double,double,double);
	double getPVI3(double,double,double,double);
};

#endif // #ifndef HEADER_INCLUDED__Image_VI_Distance_H
