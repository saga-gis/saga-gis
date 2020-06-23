
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              shapes_split_by_attribute.cpp            //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes_split_by_attribute.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Split_by_Attribute::CShapes_Split_by_Attribute(void)
{
	Set_Name		(_TL("Split Table/Shapes by Attribute"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Split a table's records or a shapes layer's features "
		"attribute-wise. "
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"	, _TL("Table / Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"	, _TL("Attribute"),
		_TL(""),
		false
	);

	Parameters.Add_Table_List("",
		"CUTS"	, _TL("Cuts"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Split_by_Attribute::On_Execute(void)
{
	CSG_Table	&Table	= *Parameters("TABLE")->asTable();

	if( !Table.is_Valid() || Table.Get_Count() < 1 )
	{
		Error_Set(_TL("invalid or empty table"));

		return( false );
	}

	//-----------------------------------------------------
	int	Field	= Parameters("FIELD")->asInt();

	CSG_Index	Index;

	if( !Table.Set_Index(Index, Field) )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	Parameters("CUTS")->asTableList()->Del_Items();

	//-----------------------------------------------------
	CSG_Table	*pSplit	= NULL;	CSG_String	Value;

	for(int i=0; i<Table.Get_Count() && Set_Progress(i, Table.Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= Table.Get_Record(Index[i]);

		if( !pSplit || Value.Cmp(pRecord->asString(Field)) )
		{
			Value	= pRecord->asString(Field);

			pSplit	= Table.Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
					? SG_Create_Shapes(Table.asShapes()->Get_Type(), NULL, &Table)
					: SG_Create_Table(&Table);

			pSplit->Fmt_Name("%s [%s = %s]",
				Table.Get_Name(), Table.Get_Field_Name(Field), Value.c_str()
			);

			Parameters("CUTS")->asTableList()->Add_Item(pSplit);
		}

		pSplit->Add_Record(pRecord);
	}

	return( pSplit != NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
