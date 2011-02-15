/**********************************************************
 * Version $Id$
 *********************************************************/

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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Points_From_Table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_From_Table::CPoints_From_Table(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Convert Table to Points"));

	Set_Author		(_TL("Victor Olaya (c) 2004"));

	Set_Description	(_TW(
		"Create Point Theme From Table"
	));

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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_Table::On_Execute(void)
{
	int			xField, yField;
	CSG_Table	*pTable;
	CSG_Shapes	*pShapes;

	pTable	= Parameters("TABLE")	->asTable();
	pShapes	= Parameters("POINTS")	->asShapes();
	xField	= Parameters("X")		->asInt();
	yField	= Parameters("Y")		->asInt();

	if( pTable->Get_Field_Count() < 1 || pTable->Get_Record_Count() <= 0 )
	{
		return( false );
	}

	pShapes->Create(SHAPE_TYPE_Point, pTable->Get_Name(), pTable);

	for(int iRecord=0; iRecord<pTable->Get_Record_Count() && Set_Progress(iRecord, pTable->Get_Record_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		if( !pRecord->is_NoData(xField) && !pRecord->is_NoData(yField) )
		{
			CSG_Shape	*pShape	= pShapes->Add_Shape(pRecord, SHAPE_COPY_ATTR);

			pShape->Add_Point(pRecord->asDouble(xField), pRecord->asDouble(yField));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
