/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Merge.cpp
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

/////////////////////////////////////////////////////////////////////////////////////
// This module is based on Olaf Conrad's Assign_Table module, which can be found
// as part of the official SAGA ditribution. I've modified it to resemble the ArcView
// join table command
/////////////////////////////////////////////////////////////////////////////////////

#include "JoinTables.h"

CJoinTables::CJoinTables(void){

	CSG_Parameter	*pNode_0, *pNode_1;

	Parameters.Set_Name(_TL("Join Table"));

	Parameters.Set_Description(_TW("Join table to shapes attributes table."));

	pNode_0	= Parameters.Add_Shapes(NULL,
									"SHAPES",
									_TL("Shapes"),
									_TL(""),
									PARAMETER_INPUT);

	pNode_1	= Parameters.Add_Table_Field(pNode_0	,
									"SHAPES_ID",
									_TL("Field"),
									_TL(""));

	pNode_0	= Parameters.Add_Table(	NULL,
									"TABLE",
									_TL("Table"),
									_TL(""),
									PARAMETER_INPUT);

	pNode_1	= Parameters.Add_Table_Field(pNode_0,
									"TABLE_ID",
									_TL("Field"),
									_TL(""));

	pNode_0	= Parameters.Add_Shapes(NULL,
									"SHAPES_OUT",
									_TL("Resulting shapes"),
									_TL(""),
									PARAMETER_OUTPUT);

}//constructor


CJoinTables::~CJoinTables(void){}


bool CJoinTables::On_Execute(void){

	bool			bCopy;
	int				iShape, iPart, iPoint, iRecord, id_Shapes, id_Table, iFieldOut;
	double			Value;
	CSG_Table			*pTable, *pShapesTable, *pShapesOutTable;
	CSG_Table_Record	*pRecord;
	CSG_Shapes			*pShapes, *pShapes_Out;
	CSG_Shape			*pShape, *pShape_Out;

	pShapes		= Parameters("SHAPES")		->asShapes();
	id_Shapes	= Parameters("SHAPES_ID")	->asInt();

	pTable		= Parameters("TABLE")		->asTable();
	id_Table	= Parameters("TABLE_ID")	->asInt();

	if(	id_Shapes >= 0 && id_Shapes < pShapes->Get_Table().Get_Field_Count()
			&&	id_Table  >= 0 && id_Table  < pTable->Get_Field_Count() ){

		bCopy		= Parameters("SHAPES")->asShapes() == Parameters("SHAPES_OUT")->asShapes();
		pShapes_Out	= bCopy ? SG_Create_Shapes() : Parameters("SHAPES_OUT")->asShapes();

		pShapes_Out->Create(pShapes->Get_Type());
		pShapes_Out->Set_Name(CSG_String::Format(SG_T("%s / %s"), pShapes->Get_Name(), pTable->Get_Name()));
		pShapes_Out->Get_Table().Create(pTable);

		pShapesTable = &pShapes->Get_Table();

		pShapesOutTable = &pShapes_Out->Get_Table();
		pShapesOutTable->Create(pShapesTable);
		for (int i=0; i<pTable->Get_Field_Count();i++){
			if (i!=id_Table){
				pShapesOutTable->Add_Field(pTable->Get_Field_Name(i),pTable->Get_Field_Type(i));
			}//if
		}//for

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)		{
			pShape	= pShapes->Get_Shape(iShape);
			Value	= pShape->Get_Record()->asDouble(id_Shapes);
			for(iRecord=0, pShape_Out=NULL; iRecord<pTable->Get_Record_Count() && !pShape_Out; iRecord++)			{
				pRecord	= pTable->Get_Record(iRecord);
				if( Value == pRecord->asDouble(id_Table))				{
					pShape_Out = pShapes_Out->Add_Shape(pShapesTable->Get_Record(iShape));
					for (int iField=0; iField<pTable->Get_Field_Count();iField++){
						iFieldOut = pShapesTable->Get_Field_Count();
						if (iField!=id_Table){
							if (pTable->Get_Field_Type(iField)==SG_DATATYPE_String ){
								pShape_Out->Get_Record()->Set_Value(iFieldOut, pRecord->asString(iField));
							}//if
							else{
								pShape_Out->Get_Record()->Set_Value(iFieldOut, pRecord->asDouble(iField));
							}//else
							iFieldOut++;
						}//if
					}//for
					for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++){
						for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++){
							pShape_Out->Add_Point(pShape->Get_Point(iPoint, iPart), iPart);
						}//for
					}//for
				}//for
			}//for
		}//for

		if ( bCopy ){
			pShapes->Assign(pShapes_Out);
			delete(pShapes_Out);
		}//if

		return( true );
	}//if

	return( false );

}//method

