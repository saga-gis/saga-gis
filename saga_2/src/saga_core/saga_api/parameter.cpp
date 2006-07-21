
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
CParameter::CParameter(CParameters *pOwner, CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, int Constraint)
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

	case PARAMETER_TYPE_Node:				m_pData	= new CParameter_Node				(this, Constraint);	break;

	case PARAMETER_TYPE_Bool:				m_pData	= new CParameter_Bool				(this, Constraint);	break;
	case PARAMETER_TYPE_Int:				m_pData	= new CParameter_Int				(this, Constraint);	break;
	case PARAMETER_TYPE_Double:				m_pData	= new CParameter_Double				(this, Constraint);	break;
	case PARAMETER_TYPE_Degree:				m_pData	= new CParameter_Degree				(this, Constraint);	break;
	case PARAMETER_TYPE_Range:				m_pData	= new CParameter_Range				(this, Constraint);	break;
	case PARAMETER_TYPE_Choice:				m_pData	= new CParameter_Choice				(this, Constraint);	break;

	case PARAMETER_TYPE_String:				m_pData	= new CParameter_String				(this, Constraint);	break;
	case PARAMETER_TYPE_Text:				m_pData	= new CParameter_Text				(this, Constraint);	break;
	case PARAMETER_TYPE_FilePath:			m_pData	= new CParameter_FilePath			(this, Constraint);	break;

	case PARAMETER_TYPE_Font:				m_pData	= new CParameter_Font				(this, Constraint);	break;
	case PARAMETER_TYPE_Color:				m_pData	= new CParameter_Color				(this, Constraint);	break;
	case PARAMETER_TYPE_Colors:				m_pData	= new CParameter_Colors				(this, Constraint);	break;
	case PARAMETER_TYPE_FixedTable:			m_pData	= new CParameter_FixedTable			(this, Constraint);	break;
	case PARAMETER_TYPE_Grid_System:		m_pData	= new CParameter_Grid_System		(this, Constraint);	break;
	case PARAMETER_TYPE_Table_Field:		m_pData	= new CParameter_Table_Field		(this, Constraint);	break;

	case PARAMETER_TYPE_DataObject_Output:	m_pData	= new CParameter_DataObject_Output	(this, Constraint);	break;
	case PARAMETER_TYPE_Grid:				m_pData	= new CParameter_Grid				(this, Constraint);	break;
	case PARAMETER_TYPE_Table:				m_pData	= new CParameter_Table				(this, Constraint);	break;
	case PARAMETER_TYPE_Shapes:				m_pData	= new CParameter_Shapes				(this, Constraint);	break;
	case PARAMETER_TYPE_TIN:				m_pData	= new CParameter_TIN				(this, Constraint);	break;

	case PARAMETER_TYPE_Grid_List:			m_pData	= new CParameter_Grid_List			(this, Constraint);	break;
	case PARAMETER_TYPE_Table_List:			m_pData	= new CParameter_Table_List			(this, Constraint);	break;
	case PARAMETER_TYPE_Shapes_List:		m_pData	= new CParameter_Shapes_List		(this, Constraint);	break;
	case PARAMETER_TYPE_TIN_List:			m_pData	= new CParameter_TIN_List			(this, Constraint);	break;

	case PARAMETER_TYPE_Parameters:			m_pData	= new CParameter_Parameters			(this, Constraint);	break;
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
CParameter::~CParameter(void)
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
void CParameter::_Add_Child(CParameter *pChild)
{
	m_Children	= (CParameter **)SG_Realloc(m_Children, (m_nChildren + 1) * sizeof(CParameter *));
	m_Children[m_nChildren++]	= pChild;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameter::is_Option(void)
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
bool CParameter::is_DataObject(void)
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
bool CParameter::is_DataObject_List(void)
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
bool CParameter::is_Parameters(void)
{
	return( Get_Type() == PARAMETER_TYPE_Parameters );
}

//---------------------------------------------------------
bool CParameter::is_Serializable(void)
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
const char * CParameter::Get_Identifier(void)
{
	return( m_Identifier );
}

//---------------------------------------------------------
const char * CParameter::Get_Name(void)
{
	return( m_Name );
}

//---------------------------------------------------------
const char * CParameter::Get_Description(void)
{
	return( m_Description );
}

//---------------------------------------------------------
#define SEPARATE	if( bSeparate )	s.Append(Separator);	bSeparate	= true;

//---------------------------------------------------------
CSG_String CParameter::Get_Description(int Flags, const char *Separator)
{
	bool		bSeparate	= false;
	int			i;
	CSG_String	s;

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_NAME) != 0 )
	{
		SEPARATE;
		s.Append(CSG_String::Format("%s", Get_Name()));
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_TYPE) != 0 )
	{
		SEPARATE;
		s.Append(CSG_String::Format("[%s]", Get_Type_Name()));
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_OPTIONAL) != 0 && is_Optional() )
	{
		SEPARATE;
		s.Append(CSG_String::Format("[%s]", LNG("optional")));
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

			s.Append(CSG_String::Format("%s:%s", LNG("Choices"), Separator));

			for(i=0; i<asChoice()->Get_Count(); i++)
			{
				s.Append(CSG_String::Format("[%d] %s%s", i, asChoice()->Get_Item(i), Separator));
			}
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			SEPARATE;

			if( asValue()->has_Minimum() && asValue()->has_Maximum() )
			{
				s.Append(CSG_String::Format("%f < x < %f", asValue()->Get_Minimum(), asValue()->Get_Maximum()));
			}
			else if( asValue()->has_Minimum() )
			{
				s.Append(CSG_String::Format("%f < x", asValue()->Get_Minimum()));
			}
			else if( asValue()->has_Maximum() )
			{
				s.Append(CSG_String::Format("%s < %f", asValue()->Get_Maximum()));
			}
			break;

		case PARAMETER_TYPE_Range:
			break;

		case PARAMETER_TYPE_FixedTable:
			SEPARATE;

			s.Append(CSG_String::Format("%d %s:%s", asTable()->Get_Field_Count(), LNG("Fields"), Separator));

			for(i=0; i<asTable()->Get_Field_Count(); i++)
			{
				s.Append(CSG_String::Format("- %d. [%s] %s%s", i + 1, TABLE_FieldType_Names[asTable()->Get_Field_Type(i)], asTable()->Get_Field_Name(i), Separator));
			}
			break;

		case PARAMETER_TYPE_Parameters:
			SEPARATE;

			s.Append(CSG_String::Format("%d %s:%s", asParameters()->Get_Count(), LNG("Parameters"), Separator));

			for(i=0; i<asParameters()->Get_Count(); i++)
			{
				s.Append(CSG_String::Format("- %d. %s%s", i + 1, asParameters()->Get_Parameter(i)->Get_Description(Flags, Separator).c_str(), Separator));
			}
			break;
		}
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_TEXT) != 0 && m_Description.Length() > 0 )
	{
		SEPARATE;
		s.Append(Separator);
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
bool CParameter::Set_Value(int Value)
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
bool CParameter::Set_Value(double Value)
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
bool CParameter::Set_Value(void *Value)
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
bool CParameter::Set_Value(const char *Value)
{
	return( Set_Value((void *)Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameter::Assign(CParameter *pSource)
{
	return( m_pData->Assign(pSource->m_pData) );
}

//---------------------------------------------------------
bool CParameter::Serialize(FILE *Stream, bool bSave)
{
	CSG_String	sLine;

	if( bSave )
	{
		fprintf(Stream, "%d\n", Get_Type());

		return( m_pData->Serialize(Stream, bSave) );
	}
	else if( SG_Read_Line(Stream, sLine) && sLine.asInt() == Get_Type() )
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
