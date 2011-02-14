
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
#include <string.h>

#include "Shapes_Create_Empty.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_NODE(i)	CSG_String::Format(SG_T("NODE%03d"), i)
#define GET_NAME(i)	CSG_String::Format(SG_T("NAME%03d"), i)
#define GET_TYPE(i)	CSG_String::Format(SG_T("TYPE%03d"), i)

//---------------------------------------------------------
CSG_String CShapes_Create_Empty::m_Types	= SG_T("");


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

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		"Creates a new empty shapes layer of given type, "
		"which might be either point, multipoint, line or polygon."
		"Available field types for the attributes table are:\n"
		" - string\n"
		" - 1 byte integer\n"
		" - 2 byte integer\n"
		" - 4 byte integer\n"
		" - 4 byte floating point\n"
		" - 8 byte floating point\n"
//		" - 32 bit true color (RGB)\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Shapes_Output(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Name"),
		_TL(""),
		_TL("New Shapes Layer")
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

	Parameters.Add_Value(
		NULL	, "NFIELDS"		, _TL("Number of Attributes"),
		_TL(""),
		PARAMETER_TYPE_Int		, 2, 1, true
	);

	Parameters.Add_Parameters(
		NULL	, "FIELDS"		, _TL("Attributes"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Types.Clear();
	m_Types	+= "text";					m_Types	+= "|";
	m_Types	+= "1 byte integer";		m_Types	+= "|";
	m_Types	+= "2 byte integer";		m_Types	+= "|";
	m_Types	+= "4 byte integer";		m_Types	+= "|";
	m_Types	+= "4 byte floating point";	m_Types	+= "|";
	m_Types	+= "8 byte floating point";	m_Types	+= "|";
//	m_Types	+= "true color (RGB)";		m_Types	+= "|";

	//-----------------------------------------------------
	CSG_Parameters	*pAttributes	= Parameters("FIELDS")->asParameters();

	_Set_Field_Count(pAttributes, Parameters("NFIELDS")->asInt());

	pAttributes->Get_Parameter(GET_NAME(0))->Set_Value(_TL("ID"));
	pAttributes->Get_Parameter(GET_TYPE(0))->Set_Value(3);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Create_Empty::_Set_Field_Count(CSG_Parameters *pAttributes, int nAttributes)
{
	if( pAttributes && nAttributes > 0 )
	{
		int		nCurrent	= pAttributes->Get_Count() / 3;

		if( nCurrent < nAttributes )
		{
			for(int i=nCurrent; i<nAttributes; i++)
			{
				CSG_Parameter	*pNode	= pAttributes->Add_Node(
					NULL	, GET_NODE(i), CSG_String::Format(SG_T("%i. %s"), i + 1, _TL("Attribute")), _TL("")
				);

				pAttributes->Add_String(
					pNode	, GET_NAME(i), _TL("Name"), _TL(""), _TL("Name")
				);

				pAttributes->Add_Choice(
					pNode	, GET_TYPE(i), _TL("Type"), _TL(""), m_Types
				);
			}
		}
		else if( nCurrent > nAttributes )
		{
			CSG_Parameters	Tmp;
			Tmp.Assign(pAttributes);
			pAttributes->Destroy();
			pAttributes->Set_Name(Tmp.Get_Name());

			for(int i=0; i<nAttributes; i++)
			{
				CSG_Parameter	*pNode	= pAttributes->Add_Node(
					NULL	, GET_NODE(i), CSG_String::Format(SG_T("%i. %s"), i + 1, _TL("Attribute")), _TL("")
				);

				pAttributes->Add_String(
					pNode	, GET_NAME(i), _TL("Name"), _TL(""), Tmp(GET_NAME(i)) ? Tmp(GET_NAME(i))->asString() : _TL("Name")
				);

				pAttributes->Add_Choice(
					pNode	, GET_TYPE(i), _TL("Type"), _TL(""), m_Types, Tmp(GET_TYPE(i)) ? Tmp(GET_TYPE(i))->asInt() : 0
				);
			}
		}
	}
}

//---------------------------------------------------------
int CShapes_Create_Empty::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NFIELDS")) )
	{
		_Set_Field_Count(pParameters->Get_Parameter("FIELDS")->asParameters(), pParameter->asInt());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Create_Empty::On_Execute(void)
{
	CSG_Shapes	*pShapes;

	switch( Parameters("TYPE")->asInt() )
	{
	default:	pShapes	= NULL;	break;
	case 0:		pShapes	= SG_Create_Shapes(SHAPE_TYPE_Point  , Parameters("NAME")->asString());	break;
	case 1:		pShapes	= SG_Create_Shapes(SHAPE_TYPE_Points , Parameters("NAME")->asString());	break;
	case 2:		pShapes	= SG_Create_Shapes(SHAPE_TYPE_Line   , Parameters("NAME")->asString());	break;
	case 3:		pShapes	= SG_Create_Shapes(SHAPE_TYPE_Polygon, Parameters("NAME")->asString());	break;
	}

	if( pShapes )
	{
		int						i, n;
		TSG_Data_Type	Type;
		CSG_Parameters			*pAttributes;

		pAttributes	= Parameters("FIELDS")->asParameters();
		n			= pAttributes->Get_Count() / 3;

		for(i=0; i<n; i++)
		{
			switch( pAttributes->Get_Parameter(GET_TYPE(i))->asInt() )
			{
			default:
			case 0:	Type	= SG_DATATYPE_String;	break;
			case 1:	Type	= SG_DATATYPE_Char  ;	break;
			case 2:	Type	= SG_DATATYPE_Short ;	break;
			case 3:	Type	= SG_DATATYPE_Int   ;	break;
			case 4:	Type	= SG_DATATYPE_Float ;	break;
			case 5:	Type	= SG_DATATYPE_Double;	break;
			case 6:	Type	= SG_DATATYPE_Color ;	break;
			}

			pShapes->Add_Field(pAttributes->Get_Parameter(GET_NAME(i))->asString(), Type);
		}

		Parameters("SHAPES")->Set_Value(pShapes);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
