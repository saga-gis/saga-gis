
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
#include "Shapes_Create_Empty.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_ID_NAME(i)	CSG_String::Format("NAME%d", i)
#define GET_ID_TYPE(i)	CSG_String::Format("TYPE%d", i)


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Create_Empty::CShapes_Create_Empty(void)
{
	Set_Name		(_TL("Create New Shapes Layer"));

	Set_Author		("O.Conrad (c) 2008");

	CSG_String Types;

	for(int i=0; i<SG_DATATYPE_Undefined; i++)
	{
		if( SG_DATATYPES_Table & SG_Data_Type_Get_Flag((TSG_Data_Type)i) )
		{
			Types += "<li>" + SG_Data_Type_Get_Name((TSG_Data_Type)i) + "</li>";
		}
	}

	Set_Description	(CSG_String::Format(_TW(
		"Creates a new empty shapes layer of given type, "
		"which might be either point, multipoint, line or polygon.\n\n"
		"Possible field types for the attributes table are:\n<ul>%s</ul>"), Types.c_str()
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES" , _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_String("",
		"NAME"   , _TL("Name"),
		_TL(""),
		_TL("New Shapes Layer")
	);

	Parameters.Add_Choice("",
		"TYPE"   , _TL("Geometry Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Point"),
			_TL("Multipoint"),
			_TL("Lines"),
			_TL("Polygon")
		)
	);

	Parameters.Add_Choice("",
		"VERTEX" , _TL("Vertex Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			SG_T("x, y"),
			SG_T("x, y, z"),
			SG_T("x, y, z, m")
		)
	);

	//-----------------------------------------------------
	m_CRS.Create(Parameters, "SHAPES");

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"NFIELDS", _TL("Number of Attributes"),
		_TL(""),
		2, 1, true
	);

	CSG_Parameters *pFields = Parameters.Add_Parameters("",
		"FIELDS" , _TL("Attributes"),
		_TL("")
	)->asParameters();

	Set_Field_Count(pFields, Parameters("NFIELDS")->asInt());

	(*pFields)(GET_ID_NAME(0))->Set_Value("ID"  );
	(*pFields)(GET_ID_TYPE(0))->Set_Value( 3    );

	(*pFields)(GET_ID_NAME(1))->Set_Value("Name");
	(*pFields)(GET_ID_TYPE(1))->Set_Value( 0    );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Create_Empty::Set_Field_Count(CSG_Parameters *pFields, int nFields)
{
	if( pFields && nFields > 0 )
	{
		int nCurrent = pFields->Get_Count() / 2;

		if( nCurrent < nFields )
		{
			for(int i=nCurrent; i<nFields; i++)
			{
				CSG_String Name(CSG_String::Format("%s %d", _TL("Field"), i + 1));

				pFields->Add_String   (""            , GET_ID_NAME(i),      Name  , _TL("Name"), Name);
				pFields->Add_Data_Type(GET_ID_NAME(i), GET_ID_TYPE(i), _TL("Type"), _TL("Type"), SG_DATATYPES_Table);
			}
		}
		else if( nCurrent > nFields )
		{
			for(int i=nCurrent, j=2*nCurrent; i>nFields; i--)
			{
				pFields->Del_Parameter(--j);
				pFields->Del_Parameter(--j);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Empty::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	if( has_GUI() )
	{
		Parameters.Set_Parameter("SHAPES", DATAOBJECT_CREATE);
	}

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CShapes_Create_Empty::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CShapes_Create_Empty::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS.On_Parameter_Changed(pParameters, pParameter);

	if( pParameter->Cmp_Identifier("NFIELDS") )
	{
		Set_Field_Count((*pParameters)("FIELDS")->asParameters(), pParameter->asInt());
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Empty::On_Execute(void)
{
	TSG_Vertex_Type Vertex;

	switch( Parameters("VERTEX")->asInt() )
	{
	default: Vertex = SG_VERTEX_TYPE_XY  ; break;
	case  1: Vertex = SG_VERTEX_TYPE_XYZ ; break;
	case  2: Vertex = SG_VERTEX_TYPE_XYZM; break;
	}

	//-----------------------------------------------------
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes();

	switch( Parameters("TYPE")->asInt() )
	{
	default: pShapes->Create(SHAPE_TYPE_Point  , Parameters("NAME")->asString(), NULL, Vertex); break;
	case  1: pShapes->Create(SHAPE_TYPE_Points , Parameters("NAME")->asString(), NULL, Vertex); break;
	case  2: pShapes->Create(SHAPE_TYPE_Line   , Parameters("NAME")->asString(), NULL, Vertex); break;
	case  3: pShapes->Create(SHAPE_TYPE_Polygon, Parameters("NAME")->asString(), NULL, Vertex); break;
	}

	m_CRS.Get_CRS(pShapes->Get_Projection(), true);

	//-----------------------------------------------------
	CSG_Parameters &Fields = *Parameters("FIELDS")->asParameters();

	for(int i=0; i<Parameters("NFIELDS")->asInt(); i++)
	{
		pShapes->Add_Field(
			Fields(GET_ID_NAME(i))->asString(),
			Fields(GET_ID_TYPE(i))->asDataType()->Get_Data_Type()
		);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Create_Copy::CShapes_Create_Copy(void)
{
	Set_Name		(_TL("Copy Shapes"));

	Set_Author		("O.Conrad (c) 2017");

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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Copy::On_Execute(void)
{
	CSG_Shapes *pCopy = Parameters("COPY")->asShapes();

	return( pCopy->Create(*Parameters("SHAPES")->asShapes()) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
