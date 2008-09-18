/*******************************************************************************
    Points_From_Table.cpp
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

#include <string.h>

#include "Points_From_Table.h"


CPoints_From_Table::CPoints_From_Table(void)
{
	CSG_Parameter	*pNode;

	Set_Name(_TL("Convert a Table to Points"));

	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Create Point Theme From Table"));

	Parameters.Add_Shapes(
		NULL	, "POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "X"		, _TL("X"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "Y"		, _TL("Y"),
		_TL("")
	);
}//constructor

CPoints_From_Table::~CPoints_From_Table(void)
{}

bool CPoints_From_Table::On_Execute(void)
{
	int				iRecord, iXField, iYField;
	double			dX, dY;
	CSG_Table			*pTable;
	CSG_Table_Record	*pRecord;
	CSG_Shapes			*pShapes;
	CSG_Shape			*pShape;

	pTable	= Parameters("TABLE")	->asTable();
	pShapes	= Parameters("POINTS")	->asShapes();

	iXField	= Parameters("X")		->asInt();
	iYField	= Parameters("Y")		->asInt();

	pShapes->Create(SHAPE_TYPE_Point, pTable->Get_Name(), pTable);

	if( pTable->Get_Field_Count() > 1 && pTable->Get_Record_Count() > 0 )
	{
		for(iRecord=0; iRecord<pTable->Get_Record_Count() && Set_Progress(iRecord, pTable->Get_Record_Count()); iRecord++)
		{
			pRecord	= pTable->Get_Record(iRecord);

			dX		= pRecord->asDouble(iXField);
			dY		= pRecord->asDouble(iYField);

			pShape	= pShapes->Add_Shape();

			pShape->Get_Record()->Assign(pRecord);
			pShape->Add_Point(dX, dY);
		}//for

		return( true );
	}//if

	return( false );
}//method
