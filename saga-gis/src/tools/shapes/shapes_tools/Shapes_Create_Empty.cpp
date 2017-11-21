/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#define GET_ID_NAME(i)	CSG_String::Format("NAME%d", i)
#define GET_ID_TYPE(i)	CSG_String::Format("TYPE%d", i)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Create_Empty::CShapes_Create_Empty(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create New Shapes Layer"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(CSG_String::Format(_TW(
		"Creates a new empty shapes layer of given type, "
		"which might be either point, multipoint, line or polygon.\n\n"
		"Possible field types for the attributes table are:\n"
		"- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n- %s\n"),
		SG_Data_Type_Get_Name(SG_DATATYPE_String).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Date  ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Color ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Char  ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Word  ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Short ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_DWord ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Int   ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_ULong ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Long  ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Float ).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Double).c_str(),
		SG_Data_Type_Get_Name(SG_DATATYPE_Binary).c_str()
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_Output("",
		"SHAPES"	, _TL("Shapes"),
		_TL("")
	);

	Parameters.Add_String("",
		"NAME"		, _TL("Name"),
		_TL(""),
		_TL("New Shapes Layer")
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Shape Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Point"),
			_TL("Multipoint"),
			_TL("Lines"),
			_TL("Polygon")
		)
	);

	Parameters.Add_Choice("",
		"VERTEX"	, _TL("Vertex Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("x, y"),
			_TL("x, y, z"),
			_TL("x, y, z, m")
		)
	);

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"NFIELDS"	, _TL("Number of Attributes"),
		_TL(""),
		2, 1, true
	);

	Parameters.Add_Parameters("",
		"FIELDS"	, _TL("Attributes"),
		_TL("")
	);

	Set_Field_Count(Parameters("FIELDS")->asParameters(), Parameters("NFIELDS")->asInt());

	Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_NAME(0))->Set_Value("ID");
	Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_TYPE(0))->Set_Value( 3  );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Create_Empty::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NFIELDS") )
	{
		Set_Field_Count(pParameters->Get_Parameter("FIELDS")->asParameters(), pParameter->asInt());
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Create_Empty::Set_Field_Count(CSG_Parameters *pFields, int nFields)
{
	static const CSG_String	Types(
		SG_Data_Type_Get_Name(SG_DATATYPE_String) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Date  ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Color ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Char  ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Word  ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Short ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_DWord ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Int   ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_ULong ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Long  ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Float ) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Double) + "|" +
		SG_Data_Type_Get_Name(SG_DATATYPE_Binary) + "|"
	);

	if( pFields && nFields > 0 )
	{
		int		nCurrent	= pFields->Get_Count() / 2;

		if( nCurrent < nFields )
		{
			for(int i=nCurrent; i<nFields; i++)
			{
				pFields->Add_String(""            , GET_ID_NAME(i), _TL("Name"), _TL(""), _TL("Name"));
				pFields->Add_Choice(GET_ID_NAME(i), GET_ID_TYPE(i), _TL("Type"), _TL(""), Types);
			}
		}
		else if( nCurrent > nFields )
		{
			for(int i=nCurrent-1; i>=nFields; i--)
			{
				pFields->Del_Parameter(i);
			}
		}
	}
}

//---------------------------------------------------------
CSG_String CShapes_Create_Empty::Get_Field_Name(int iField)
{
	return( Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_NAME(iField))->asString() );
}

//---------------------------------------------------------
TSG_Data_Type CShapes_Create_Empty::Get_Field_Type(int iField)
{
	switch( Parameters("FIELDS")->asParameters()->Get_Parameter(GET_ID_TYPE(iField))->asInt() )
	{
	default: return( SG_DATATYPE_String );
	case  1: return( SG_DATATYPE_Date   );
	case  2: return( SG_DATATYPE_Color  );
	case  3: return( SG_DATATYPE_Byte   );
	case  4: return( SG_DATATYPE_Char   );
	case  5: return( SG_DATATYPE_Word   );
	case  6: return( SG_DATATYPE_Short  );
	case  7: return( SG_DATATYPE_DWord  );
	case  8: return( SG_DATATYPE_Int    );
	case  9: return( SG_DATATYPE_ULong  );
	case 10: return( SG_DATATYPE_Long   );
	case 11: return( SG_DATATYPE_Float  );
	case 12: return( SG_DATATYPE_Double );
	case 13: return( SG_DATATYPE_Binary );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Empty::On_Execute(void)
{
	TSG_Vertex_Type	Vertex;

	switch( Parameters("VERTEX")->asInt() )
	{
	default: Vertex = SG_VERTEX_TYPE_XY  ;	break;
	case  1: Vertex = SG_VERTEX_TYPE_XYZ ;	break;
	case  2: Vertex = SG_VERTEX_TYPE_XYZM;	break;
	}

	//-----------------------------------------------------
	CSG_Shapes	*pShapes;

	switch( Parameters("TYPE")->asInt() )
	{
	default: pShapes = SG_Create_Shapes(SHAPE_TYPE_Point  , Parameters("NAME")->asString(), NULL, Vertex);	break;
	case  1: pShapes = SG_Create_Shapes(SHAPE_TYPE_Points , Parameters("NAME")->asString(), NULL, Vertex);	break;
	case  2: pShapes = SG_Create_Shapes(SHAPE_TYPE_Line   , Parameters("NAME")->asString(), NULL, Vertex);	break;
	case  3: pShapes = SG_Create_Shapes(SHAPE_TYPE_Polygon, Parameters("NAME")->asString(), NULL, Vertex);	break;
	}

	//-----------------------------------------------------
	for(int i=0; i<Parameters("NFIELDS")->asInt(); i++)
	{
		pShapes->Add_Field(Get_Field_Name(i), Get_Field_Type(i));
	}

	//-----------------------------------------------------
	Parameters("SHAPES")->Set_Value(pShapes);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Create_Copy::CShapes_Create_Copy(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Copy Shapes"));

	Set_Author		("O. Conrad (c) 2017");

	Set_Description	(_TW(
		"Creates a copy of a shapes layer."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"COPY"		, _TL("Copy"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Copy::On_Execute(void)
{
	CSG_Shapes	*pCopy	= Parameters("COPY")->asShapes();

	return( pCopy->Create(*Parameters("SHAPES")->asShapes()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
