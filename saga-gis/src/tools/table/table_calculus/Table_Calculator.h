
/*******************************************************************************
    TableCalculator.h
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
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Calculator_Base : public CSG_Tool  
{
public:
	CTable_Calculator_Base(bool bShapes);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bNoData;

	int						m_Result;

	CSG_Array_Int			m_Values;

	CSG_Formula				m_Formula;


	bool					Get_Value				(CSG_Table_Record *pRecord);

	CSG_String				Get_Formula				(CSG_String Formula, CSG_Table *pTable, CSG_Array_Int &Values);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Calculator : public CTable_Calculator_Base  
{
public:
	CTable_Calculator(void) : CTable_Calculator_Base(false)	{}

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Calculator_Shapes : public CTable_Calculator_Base  
{
public:
	CTable_Calculator_Shapes(void) : CTable_Calculator_Base(true)	{}

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("A:Shapes|Table") );	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
