
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
//                Shapes_Assign_Table.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
//              Olaf Conrad  & Victor Olaya              //
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
#include "Shapes_Assign_Table.h"

//---------------------------------------------------------
#define METHOD_ADD		0
#define METHOD_REPLACE	1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Assign_Table::CShapes_Assign_Table(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Join a Table"));

	Set_Author		(SG_T("V. Olaya, O. Conrad (c) 2003"));

	Set_Description	(_TW(
		"Joins a table with shapes layer's attributes."
	));


	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "SHAPES_ID"	, _TL("Identifier"),
		_TL("")
	);

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "TABLE_ID"	, _TL("Identifier"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "SHAPES_OUT"	, _TL("Resulting Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Attributes"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("append"),
			_TL("replace")
		)
	);

	Parameters.Add_Choice(
		NULL	, "ADDALL"		, _TL("Copy Rule"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all shapes"),
			_TL("only shapes with table entry")
		)
	);
}

//---------------------------------------------------------
CShapes_Assign_Table::~CShapes_Assign_Table(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Assign_Table::On_Execute(void)
{
	bool			bAddAll;
	int				iShape, iPart, iPoint, iField, jField, off_Field, iRecord, Method, id_Shapes, id_Table;
	CSG_String		sID;
	CSG_Table			*pTable_A;
	CSG_Table_Record	*pRecord_A;
	CSG_Shapes			*pShapes_A, *pShapes_B;
	CSG_Shape			*pShape_A, *pShape_B;

	//-----------------------------------------------------
	pShapes_A	= Parameters("SHAPES")		->asShapes();
	pShapes_B	= Parameters("SHAPES_OUT")	->asShapes();
	pTable_A	= Parameters("TABLE")		->asTable();

	id_Shapes	= Parameters("SHAPES_ID")	->asInt();
	id_Table	= Parameters("TABLE_ID")	->asInt();

	Method		= Parameters("METHOD")		->asInt();
	bAddAll		= Parameters("ADDALL")		->asInt() == 0;

	//-----------------------------------------------------
	if(	id_Shapes >= 0 && id_Shapes < pShapes_A->Get_Field_Count() && pShapes_A->Get_Count() > 0
	&&	id_Table  >= 0 && id_Table  < pTable_A->Get_Field_Count() && pTable_A->Get_Record_Count() > 0 )
	{
		if( pShapes_A == pShapes_B || pShapes_A->Get_Type() != pShapes_B->Get_Type() )
		{
			Parameters("SHAPES_OUT")->Set_Value(pShapes_B = SG_Create_Shapes());
		}

		if( Method == METHOD_ADD )
		{
			pShapes_B->Create(pShapes_A->Get_Type(), CSG_String::Format(SG_T("%s / %s"), pShapes_A->Get_Name(), pTable_A->Get_Name()), pShapes_A);
			off_Field	= pShapes_A->Get_Field_Count();

			for(iField=0; iField<pTable_A->Get_Field_Count(); iField++)
			{
				if( iField != id_Table )
					pShapes_B->Add_Field(pTable_A->Get_Field_Name(iField), pTable_A->Get_Field_Type(iField));
			}
		}
		else	//  METHOD_REPLACE
		{
			pShapes_B->Create(pShapes_A->Get_Type(), CSG_String::Format(SG_T("%s / %s"), pShapes_A->Get_Name(), pTable_A->Get_Name()), pTable_A);
			off_Field	= 0;
		}

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes_A->Get_Count() && Set_Progress(iShape, pShapes_A->Get_Count()); iShape++)
		{
			pShape_A	= pShapes_A->Get_Shape(iShape);
			sID			= pShape_A->asString(id_Shapes);

			for(iRecord=0, pShape_B=NULL; iRecord<pTable_A->Get_Record_Count() && pShape_B==NULL; iRecord++)
			{
				pRecord_A	= pTable_A->Get_Record(iRecord);

				if( !sID.CmpNoCase(pRecord_A->asString(id_Table)) )
				{
					pShape_B	= pShapes_B->Add_Shape(Method == METHOD_ADD ? pShape_A : NULL);

					for(iField=0, jField=off_Field; iField<pTable_A->Get_Field_Count(); iField++)
					{
						if( Method != METHOD_ADD || iField != id_Table )
							pShape_B->Set_Value(jField++, pRecord_A->asString(iField));
					}
				}
			}

			if( pShape_B == NULL && bAddAll )
			{
				pShape_B	= pShapes_B->Add_Shape(Method == METHOD_ADD ? pShape_A : NULL);

				if( Method != METHOD_ADD )
					pShape_B->Set_Value(id_Table, sID);
			}

			if( pShape_B )
			{
				for(iPart=0; iPart<pShape_A->Get_Part_Count(); iPart++)
					for(iPoint=0; iPoint<pShape_A->Get_Point_Count(iPart); iPoint++)
						pShape_B->Add_Point(pShape_A->Get_Point(iPoint, iPart), iPart);
			}
		}

		//-------------------------------------------------
		return( pShapes_B->Get_Count() > 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
