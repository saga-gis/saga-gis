/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    GridsFromTableAndGrid.cpp
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
#include "GridsFromTableAndGrid.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridsFromTableAndGrid::CGridsFromTableAndGrid(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Grids from classified grid and table"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Creates several grids using a classified grid and a table with data values for each class."
	));

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL("The table with the (numeric) data values for each class. The tool creates a grid for each table column (besides the ID)."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ID_FIELD"	, _TL("Attribute"),
		_TL("The attribute with the class IDs, used to link the table and the grid.")
	);

	Parameters.Add_Grid(
		NULL	, "CLASSES"		, _TL("Classes"),
		_TL("The grid coded with the class IDs."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL("The output grids, one grid for each table column."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridsFromTableAndGrid::On_Execute(void)
{
	int						iField, iRecord, iAttribute, nAttributes, *Attribute;
	sLong					iCell, jCell;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Grid				*pClasses;
	CSG_Table				*pTable;

	//-----------------------------------------------------
	pClasses	= Parameters("CLASSES" )->asGrid();
	pGrids		= Parameters("GRIDS"   )->asGridList();
	pTable		= Parameters("TABLE"   )->asTable();
	iField		= Parameters("ID_FIELD")->asInt();

	pGrids->Del_Items();

	if( !pClasses->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	if( pTable->Get_Field_Count() == 0 || pTable->Get_Count() == 0 )
	{
		Message_Add(_TL("selected table contains no valid records"));

		return( false );
	}

	//-----------------------------------------------------
	if( !pTable->Set_Index(iField, TABLE_INDEX_Ascending) )
	{
		Message_Add(_TL("failed to create index for table"));

		return( false );
	}

	//-----------------------------------------------------
	Attribute	= new int[pTable->Get_Field_Count()];

	for(iAttribute=0, nAttributes=0; iAttribute<pTable->Get_Field_Count(); iAttribute++)
	{
		if( iAttribute != iField && pTable->Get_Field_Type(iAttribute) != SG_DATATYPE_String )
		{
			Attribute[nAttributes++]	= iAttribute;

			CSG_Grid	*pGrid	= SG_Create_Grid(*Get_System());

			pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pClasses->Get_Name(), pTable->Get_Field_Name(iAttribute)));

			pGrids->Add_Item(pGrid);
		}
	}

	if( nAttributes == 0 )
	{
		delete[](Attribute);

		Message_Add(_TL("selected table does not have numeric attributes"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(0);

	for(iCell=0, iRecord=0; iCell<Get_NCells() && pRecord && Set_Progress_NCells(iCell); iCell++)
	{
		if( pClasses->Get_Sorted(iCell, jCell, false, true) )
		{
			double	valClass	= pClasses->asDouble(jCell);

			while( pRecord && pRecord->asDouble(iField) < valClass )
			{
				pRecord	= pTable->Get_Record_byIndex(++iRecord);
			}

			if( !pRecord || pRecord->asDouble(iField) > valClass )
			{
				for(iAttribute=0; iAttribute<nAttributes; iAttribute++)
				{
					pGrids->Get_Grid(iAttribute)->Set_NoData(jCell);
				}
			}
			else
			{
				for(iAttribute=0; iAttribute<nAttributes; iAttribute++)
				{
					pGrids->Get_Grid(iAttribute)->Set_Value(jCell, pRecord->asDouble(Attribute[iAttribute]));
				}
			}
		}
	}

	//-----------------------------------------------------
	delete[](Attribute);

	return true;
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
