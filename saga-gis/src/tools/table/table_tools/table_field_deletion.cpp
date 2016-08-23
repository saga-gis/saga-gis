/**********************************************************
 * Version $Id: table_field_deletion.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
	int	*Fields	= (int *)Parameters("FIELDS")->asPointer();
	int	nFields	=        Parameters("FIELDS")->asInt    ();

	if( nFields <= 0 )
	{
		Error_Set(_TL("no fields in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pInput  = Parameters("TABLE")->asTable();
	CSG_Table	*pOutput = NULL;

	if( pInput->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
	{
		if( (pOutput = Parameters("OUT_SHAPES")->asShapes()) != NULL && pOutput != pInput )
		{
			((CSG_Shapes *)pOutput)->Create(((CSG_Shapes *)pInput)->Get_Type(), (const wchar_t*)0, (CSG_Table *)0, ((CSG_Shapes *)pInput)->Get_Vertex_Type());
		}
	}
	else // if( pInput->Get_ObjectType() == DATAOBJECT_TYPE_Table )
	{
		if( (pOutput = Parameters("OUT_TABLE" )->asTable()) != NULL && pOutput != pInput )
		{
			pOutput->Destroy();
		}
	}

	//-----------------------------------------------------
	if( pOutput == NULL || pOutput == pInput )
	{
		for(int iField=nFields-1; iField>=0; iField--)
		{
			pInput->Del_Field(Fields[iField]);
		}

		DataObject_Update(pInput);
	}

	//-----------------------------------------------------
	else
	{
		bool	*bDelete	= (bool *)SG_Calloc(pInput->Get_Field_Count(), sizeof(bool));

		int		i, j;

		for(i=0; i<nFields; i++)
		{
			bDelete[Fields[i]]	= true;
		}

		pOutput->Set_Name(CSG_String::Format("%s [%s]", pInput->Get_Name(), _TL("Changed")));

		for(i=0; i<pInput->Get_Field_Count(); i++)
		{
			if( !bDelete[i] )
			{
				pOutput->Add_Field(pInput->Get_Field_Name(i), pInput->Get_Field_Type(i));
			}
		}

		for(int iRecord=0; iRecord<pInput->Get_Count() && Set_Progress(iRecord, pInput->Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pOut, *pIn	= pInput->Get_Record(iRecord);

			if( pOutput->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
			{
				pOut	= ((CSG_Shapes *)pOutput)->Add_Shape(pIn, SHAPE_COPY_GEOM);
			}
			else
			{
				pOut	= pOutput->Add_Record();
			}

			for(i=0, j=0; i<pInput->Get_Field_Count(); i++)
			{
				if( !bDelete[i] )
				{
					*pOut->Get_Value(j++)	= *pIn->Get_Value(i);
				}
			}
		}

		SG_Free(bDelete);
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
