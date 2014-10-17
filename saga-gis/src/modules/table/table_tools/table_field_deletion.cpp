/**********************************************************
 * Version $Id: table_field_deletion.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     table_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                table_field_deletion.cpp               //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "table_field_deletion.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Field_Deletion::CTable_Field_Deletion(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Delete Fields"));

	Set_Author	(_TL("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Deletes selected fields from a table or shapefile. "
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL("Input table or shapefile"),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Fields"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "OUT_TABLE"	, _TL("Output table with field(s) deleted"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "OUT_SHAPES"	, _TL("Output shapes with field(s) deleted"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Field_Deletion::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLE") )
	{
		CSG_Data_Object	*pObject	= pParameter->asDataObject();

		pParameters->Get_Parameter("OUT_TABLE" )->Set_Enabled(pObject &&
			pObject->Get_ObjectType() == DATAOBJECT_TYPE_Table
		);

		pParameters->Get_Parameter("OUT_SHAPES")->Set_Enabled(pObject &&
			pObject->Get_ObjectType() == DATAOBJECT_TYPE_Shapes
		);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Field_Deletion::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Table_Fields	*pFields	= Parameters("FIELDS")->asTableFields();

	if( pFields->Get_Count() <= 0 )
	{
		Error_Set(_TL("no fields in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable  = Parameters("TABLE")->asTable();

	CSG_Table	*pOutput = NULL;

	if( pTable->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
	{
		if( (pOutput = Parameters("OUT_SHAPES")->asShapes()) != NULL && pOutput != pTable )
		{
			((CSG_Shapes *)pOutput)->Create(((CSG_Shapes *)pTable)->Get_Type(), (const wchar_t*)0, (CSG_Table *)0, ((CSG_Shapes *)pTable)->Get_Vertex_Type());
		}
	}
	else // if( pTable->Get_ObjectType() == DATAOBJECT_TYPE_Table )
	{
		if( (pOutput = Parameters("OUT_TABLE"  )->asTable()) != NULL && pOutput != pTable )
		{
			pOutput->Destroy();
		}
	}

	//-----------------------------------------------------
	if( pOutput )
	{
		int		nFields		= pTable->Get_Field_Count() - pFields->Get_Count();
		int		*pFieldsOut	= new int[nFields];

		int		iField = 0;

		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			bool bDelete = false;

			for(int j=0; j<pFields->Get_Count(); j++)
			{
				if( i == pFields->Get_Index(j) )
				{
					bDelete = true;
					break;
				}
			}

			if( !bDelete )
			{
				pFieldsOut[iField] = i;
				iField++;
			}
		}


		pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pTable->Get_Name(), _TL("Changed")));

		for(iField=0; iField<nFields; iField++)
		{
			pOutput->Add_Field(pTable->Get_Field_Name(pFieldsOut[iField]), pTable->Get_Field_Type(pFieldsOut[iField]));
		}

		for(int iRecord=0; iRecord<pTable->Get_Count(); iRecord++)
		{
			CSG_Table_Record	*pOut, *pIn	= pTable->Get_Record(iRecord);

			if( pOutput->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
			{
				pOut	= ((CSG_Shapes *)pOutput)->Add_Shape(pIn, SHAPE_COPY_GEOM);

				if( ((CSG_Shapes *)pOutput)->Get_Vertex_Type() > SG_VERTEX_TYPE_XY )
				{
					for(int iPart=0; iPart<((CSG_Shape *)pIn)->Get_Part_Count(); iPart++)
					{
						for(int iPoint=0; iPoint<((CSG_Shape *)pIn)->Get_Point_Count(iPart); iPoint++)
						{
							((CSG_Shape *)pOut)->Set_Z(((CSG_Shape *)pIn)->Get_Z(iPoint, iPart), iPoint, iPart);

							if( ((CSG_Shapes *)pOutput)->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
							{
								((CSG_Shape *)pOut)->Set_M(((CSG_Shape *)pIn)->Get_M(iPoint, iPart), iPoint, iPart);
							}
						}
					}
				}
			}
			else
			{
				pOut	= pOutput->Add_Record();
			}

			for(iField=0; iField<nFields; iField++)
			{
				*pOut->Get_Value(iField)	= *pIn->Get_Value(pFieldsOut[iField]);
			}
		}

		delete[] pFieldsOut;
	}
	else
	{
		for(int iField=pFields->Get_Count()-1; iField>=0; iField--)
		{
			pTable->Del_Field(pFields->Get_Index(pFields->Get_Index(iField)));
		}

		DataObject_Update(pTable);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
