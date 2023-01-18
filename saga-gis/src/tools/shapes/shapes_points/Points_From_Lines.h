
/*******************************************************************************
    Points_From_Lines.h
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
#ifndef HEADER_INCLUDED__Points_From_Lines_H
#define HEADER_INCLUDED__Points_From_Lines_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPoints_From_Lines : public CSG_Tool
{
public:
	CPoints_From_Lines(void);

	virtual CSG_String	Get_MenuPath				(void)	{	return( _TL("A:Shapes|Conversion") );	}


protected:

	virtual int			On_Parameters_Enable		(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute					(void);


private:

	void				Convert						(CSG_Shapes *pLines, CSG_Shapes *pPoints, bool bOrder);
	void				Convert_Add_Points_Segment	(CSG_Shapes *pLines, CSG_Shapes *pPoints, bool bOrder, double Distance);
	void				Convert_Add_Points_Line		(CSG_Shapes *pLines, CSG_Shapes *pPoints, bool bOrder, double Distance);
	void				Convert_Add_Points_Center	(CSG_Shapes *pLines, CSG_Shapes *pPoints, bool bOrder, double Distance);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPoints_From_MultiPoints : public CSG_Tool
{
public:
	CPoints_From_MultiPoints(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("A:Shapes|Conversion") );	}


protected:

	virtual bool		On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Points_From_Lines_H
