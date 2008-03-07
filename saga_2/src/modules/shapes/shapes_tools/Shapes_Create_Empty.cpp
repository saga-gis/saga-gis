
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Shapes_Create_Empty.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Shapes_Create_Empty.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Create_Empty::CShapes_Create_Empty(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create Empty Shapes Layer"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description	(_TW(
		"Creates a new empty shapes layer of given type "
		"(i.e. point, multipoint, line or polygon)."
		"Available field types for the attributes table are:\n"
		" 0 - string\n"
		" 1 - 1 byte integer\n"
		" 2 - 2 byte integer\n"
		" 3 - 4 byte integer\n"
		" 4 - 4 byte floating point\n"
		" 5 - 8 byte floating point\n"
		" 6 - color (rgb)\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FixedTable(
		NULL	, "FIELDS"		, _TL("Attributes"),
		_TL("")
	)->asTable();

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Name"),
		_TL(""),
		_TL("New shapes layer")
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Shape Type"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Point"),
			_TL("Multipoint"),
			_TL("Lines"),
			_TL("Polygon")
		)
	);


	//-----------------------------------------------------
	CSG_Table_Record	*pRecord;
	CSG_Table			*pFields	= Parameters("FIELDS")->asTable();

	pFields->Set_Name(_TL("Attributes"));

	pFields->Add_Field(_TL("Name")	, TABLE_FIELDTYPE_String);
	pFields->Add_Field(_TL("Type")	, TABLE_FIELDTYPE_Int);

	pRecord	= pFields->Add_Record();
	pRecord->Set_Value(0, SG_T("ID"));
	pRecord->Set_Value(1, 2);

	pRecord	= pFields->Add_Record();
	pRecord->Set_Value(0, _TL("Name"));
	pRecord->Set_Value(1, 0.0);
}

//---------------------------------------------------------
CShapes_Create_Empty::~CShapes_Create_Empty(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Empty::On_Execute(void)
{
	CSG_Shapes	*pShapes;
	CSG_Table	*pFields;

	pShapes	= Parameters("SHAPES")	->asShapes();
	pFields	= Parameters("FIELDS")	->asTable();

	switch( Parameters("TYPE")->asInt() )
	{
	default:		return( false );
	case 0:	pShapes->Create(SHAPE_TYPE_Point  , Parameters("NAME")->asString());	break;
	case 1:	pShapes->Create(SHAPE_TYPE_Points , Parameters("NAME")->asString());	break;
	case 2:	pShapes->Create(SHAPE_TYPE_Line   , Parameters("NAME")->asString());	break;
	case 3:	pShapes->Create(SHAPE_TYPE_Polygon, Parameters("NAME")->asString());	break;
	}

	for(int i=0; i<pFields->Get_Record_Count(); i++)
	{
		CSG_Table_Record	*pRecord	= pFields->Get_Record(i);

		switch( pRecord->asInt(1) )
		{
		default:
		case 0:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_String);	break;
		case 1:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_Char  );	break;
		case 2:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_Short );	break;
		case 3:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_Int   );	break;
		case 4:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_Float );	break;
		case 5:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_Double);	break;
		case 6:	pShapes->Get_Table().Add_Field(pRecord->asString(0), TABLE_FIELDTYPE_Color );	break;
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
