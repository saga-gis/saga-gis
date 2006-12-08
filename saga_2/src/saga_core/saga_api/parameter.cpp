
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     parameter.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter::CSG_Parameter(CSG_Parameters *pOwner, CSG_Parameter *pParent, const SG_Char *Identifier, const SG_Char *Name, const SG_Char *Description, TSG_Parameter_Type Type, int Constraint)
{
	m_pOwner		= pOwner;
	m_pParent		= pParent;

	m_Identifier	= Identifier;
	m_Name			= Name;
	m_Description	= Description;

	//-----------------------------------------------------
	m_nChildren		= 0;
	m_Children		= NULL;

	if( m_pParent )
	{
		m_pParent->_Add_Child(this);
	}

	//-----------------------------------------------------
	switch( Type )
	{
	default:								m_pData	= NULL;													break;

	case PARAMETER_TYPE_Node:				m_pData	= new CSG_Parameter_Node				(this, Constraint);	break;

	case PARAMETER_TYPE_Bool:				m_pData	= new CSG_Parameter_Bool				(this, Constraint);	break;
	case PARAMETER_TYPE_Int:				m_pData	= new CSG_Parameter_Int				(this, Constraint);	break;
	case PARAMETER_TYPE_Double:				m_pData	= new CSG_Parameter_Double				(this, Constraint);	break;
	case PARAMETER_TYPE_Degree:				m_pData	= new CSG_Parameter_Degree				(this, Constraint);	break;
	case PARAMETER_TYPE_Range:				m_pData	= new CSG_Parameter_Range				(this, Constraint);	break;
	case PARAMETER_TYPE_Choice:				m_pData	= new CSG_Parameter_Choice				(this, Constraint);	break;

	case PARAMETER_TYPE_String:				m_pData	= new CSG_Parameter_String				(this, Constraint);	break;
	case PARAMETER_TYPE_Text:				m_pData	= new CSG_Parameter_Text				(this, Constraint);	break;
	case PARAMETER_TYPE_FilePath:			m_pData	= new CSG_Parameter_File_Name			(this, Constraint);	break;

	case PARAMETER_TYPE_Font:				m_pData	= new CSG_Parameter_Font				(this, Constraint);	break;
	case PARAMETER_TYPE_Color:				m_pData	= new CSG_Parameter_Color				(this, Constraint);	break;
	case PARAMETER_TYPE_Colors:				m_pData	= new CSG_Parameter_Colors				(this, Constraint);	break;
	case PARAMETER_TYPE_FixedTable:			m_pData	= new CSG_Parameter_Fixed_Table			(this, Constraint);	break;
	case PARAMETER_TYPE_Grid_System:		m_pData	= new CSG_Parameter_Grid_System		(this, Constraint);	break;
	case PARAMETER_TYPE_Table_Field:		m_pData	= new CSG_Parameter_Table_Field		(this, Constraint);	break;

	case PARAMETER_TYPE_DataObject_Output:	m_pData	= new CSG_Parameter_Data_Object_Output	(this, Constraint);	break;
	case PARAMETER_TYPE_Grid:				m_pData	= new CSG_Parameter_Grid				(this, Constraint);	break;
	case PARAMETER_TYPE_Table:				m_pData	= new CSG_Parameter_Table				(this, Constraint);	break;
	case PARAMETER_TYPE_Shapes:				m_pData	= new CSG_Parameter_Shapes				(this, Constraint);	break;
	case PARAMETER_TYPE_TIN:				m_pData	= new CSG_Parameter_TIN				(this, Constraint);	break;

	case PARAMETER_TYPE_Grid_List:			m_pData	= new CSG_Parameter_Grid_List			(this, Constraint);	break;
	case PARAMETER_TYPE_Table_List:			m_pData	= new CSG_Parameter_Table_List			(this, Constraint);	break;
	case PARAMETER_TYPE_Shapes_List:		m_pData	= new CSG_Parameter_Shapes_List		(this, Constraint);	break;
	case PARAMETER_TYPE_TIN_List:			m_pData	= new CSG_Parameter_TIN_List			(this, Constraint);	break;

	case PARAMETER_TYPE_Parameters:			m_pData	= new CSG_Parameter_Parameters			(this, Constraint);	break;
	}

	//-----------------------------------------------------
	switch( Type )
	{
	default:
		break;

	case PARAMETER_TYPE_Range:
		SG_Free(m_Children);
		m_nChildren		= 0;
		m_Children		= NULL;
		break;
	}
}

//---------------------------------------------------------
CSG_Parameter::~CSG_Parameter(void)
{
	if( m_Children )
	{
		SG_Free(m_Children);
	}

	if( m_pData )
	{
		delete(m_pData);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Parameter::_Add_Child(CSG_Parameter *pChild)
{
	m_Children	= (CSG_Parameter **)SG_Realloc(m_Children, (m_nChildren + 1) * sizeof(CSG_Parameter *));
	m_Children[m_nChildren++]	= pChild;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameter::is_Option(void)
{
	if( !is_Information() )
	{
		switch( Get_Type() )
		{
		default:
			return( false );

		case PARAMETER_TYPE_Bool:
		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
		case PARAMETER_TYPE_Range:
		case PARAMETER_TYPE_Choice:

		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_FilePath:

		case PARAMETER_TYPE_Font:

		case PARAMETER_TYPE_Color:
		case PARAMETER_TYPE_Colors:

		case PARAMETER_TYPE_FixedTable:

		case PARAMETER_TYPE_Grid_System:
		case PARAMETER_TYPE_Table_Field:

		case PARAMETER_TYPE_Parameters:

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::is_DataObject(void)
{
	switch( Get_Type() )
	{
	default:
		return( false );

	case PARAMETER_TYPE_DataObject_Output:
	case PARAMETER_TYPE_Grid:
	case PARAMETER_TYPE_Table:
	case PARAMETER_TYPE_Shapes:
	case PARAMETER_TYPE_TIN:

		return( true );
	}
}

//---------------------------------------------------------
bool CSG_Parameter::is_DataObject_List(void)
{
	switch( Get_Type() )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Grid_List:
	case PARAMETER_TYPE_Table_List:
	case PARAMETER_TYPE_Shapes_List:
	case PARAMETER_TYPE_TIN_List:

		return( true );
	}
}

//---------------------------------------------------------
bool CSG_Parameter::is_Parameters(void)
{
	return( Get_Type() == PARAMETER_TYPE_Parameters );
}

//---------------------------------------------------------
bool CSG_Parameter::is_Serializable(void)
{
	switch( Get_Type() )
	{
	case PARAMETER_TYPE_Undefined:
	case PARAMETER_TYPE_Node:
	case PARAMETER_TYPE_DataObject_Output:
		return( false );

	default:
		return( !is_Information() );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Parameter::Get_Identifier(void)
{
	return( m_Identifier );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter::Get_Name(void)
{
	return( m_Name );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter::Get_Description(void)
{
	return( m_Description );
}

//---------------------------------------------------------
#define SEPARATE	if( bSeparate )	s.Append(Separator);	bSeparate	= true;

//---------------------------------------------------------
CSG_String CSG_Parameter::Get_Description(int Flags, const SG_Char *Separator)
{
	bool		bSeparate	= false;
	int			i;
	CSG_String	s;

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_NAME) != 0 )
	{
		SEPARATE;
		s.Append(CSG_String::Format(SG_T("%s"), Get_Name()));
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_TYPE) != 0 )
	{
		SEPARATE;
		s.Append(CSG_String::Format(SG_T("%s"), Get_Type_Name()));

		if( is_DataObject() || is_DataObject_List() )
		{
			if( is_Input() )
			{
				if( is_Optional() )
					s.Append(CSG_String::Format(SG_T(" (%s)"), LNG("optional input")));
				else
					s.Append(CSG_String::Format(SG_T(" (%s)"), LNG("input")));
			}
			else if( is_Output() )
			{
				if( is_Optional() )
					s.Append(CSG_String::Format(SG_T(" (%s)"), LNG("optional output")));
				else
					s.Append(CSG_String::Format(SG_T(" (%s)"), LNG("output")));
			}
		}
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_OPTIONAL) != 0 && is_Optional() )
	{
		SEPARATE;
		s.Append(CSG_String::Format(SG_T("%s"), LNG("optional")));
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_PROPERTIES) != 0 )
	{
		switch( Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Choice:
			SEPARATE;

			s.Append(CSG_String::Format(SG_T("%s:"), LNG("Available Choices")));

			for(i=0; i<asChoice()->Get_Count(); i++)
			{
				s.Append(CSG_String::Format(SG_T("%s[%d] %s"), Separator, i, asChoice()->Get_Item(i)));
			}
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
//		case PARAMETER_TYPE_Range:
			if( asValue()->has_Minimum() && asValue()->has_Maximum() )
			{
				SEPARATE;
				s.Append(CSG_String::Format(SG_T("%s: "), LNG("Limits")));
				s.Append(CSG_String::Format(SG_T("%f < x < %f"), asValue()->Get_Minimum(), asValue()->Get_Maximum()));
			}
			else if( asValue()->has_Minimum() )
			{
				SEPARATE;
				s.Append(CSG_String::Format(SG_T("%s: "), LNG("Limit")));
				s.Append(CSG_String::Format(SG_T("%f < x"), asValue()->Get_Minimum()));
			}
			else if( asValue()->has_Maximum() )
			{
				SEPARATE;
				s.Append(CSG_String::Format(SG_T("%s: "), LNG("Limit")));
				s.Append(CSG_String::Format(SG_T("%s < %f"), asValue()->Get_Maximum()));
			}
			break;

		case PARAMETER_TYPE_FixedTable:
			SEPARATE;

			s.Append(CSG_String::Format(SG_T("%d %s:%s"), asTable()->Get_Field_Count(), LNG("Fields"), Separator));

			for(i=0; i<asTable()->Get_Field_Count(); i++)
			{
				s.Append(CSG_String::Format(SG_T("- %d. [%s] %s%s"), i + 1, gSG_Table_Field_Type_Names[asTable()->Get_Field_Type(i)], asTable()->Get_Field_Name(i), Separator));
			}
			break;

		case PARAMETER_TYPE_Parameters:
			SEPARATE;

			s.Append(CSG_String::Format(SG_T("%d %s:%s"), asParameters()->Get_Count(), LNG("Parameters"), Separator));

			for(i=0; i<asParameters()->Get_Count(); i++)
			{
				s.Append(CSG_String::Format(SG_T("- %d. %s%s"), i + 1, asParameters()->Get_Parameter(i)->Get_Description(Flags, Separator).c_str(), Separator));
			}
			break;
		}
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_TEXT) != 0 && m_Description.Length() > 0 )
	{
		SEPARATE;
		s.Append(m_Description.c_str());
	}

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(int Value)
{
	if( m_pData->Set_Value(Value) )
	{
		if( m_pOwner )
		{
			m_pOwner->_On_Parameter_Changed(this);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(double Value)
{
	if( m_pData->Set_Value(Value) )
	{
		if( m_pOwner )
		{
			m_pOwner->_On_Parameter_Changed(this);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(void *Value)
{
	if( m_pData->Set_Value(Value) )
	{
		if( m_pOwner )
		{
			m_pOwner->_On_Parameter_Changed(this);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(const SG_Char *Value)
{
	return( Set_Value((void *)Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameter::Assign(CSG_Parameter *pSource)
{
	return( m_pData->Assign(pSource->m_pData) );
}

//---------------------------------------------------------
bool CSG_Parameter::Serialize(CSG_File &Stream, bool bSave)
{
	CSG_String	sLine;

	if( bSave )
	{
		Stream.Printf(SG_T("%d\n"), Get_Type());

		return( m_pData->Serialize(Stream, bSave) );
	}
	else if( Stream.Read_Line(sLine) && sLine.asInt() == Get_Type() )
	{
		return( m_pData->Serialize(Stream, bSave) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
