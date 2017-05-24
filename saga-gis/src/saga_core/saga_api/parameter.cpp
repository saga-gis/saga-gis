/**********************************************************
 * Version $Id$
 *********************************************************/

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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter::CSG_Parameter(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, int Constraint)
{
	m_pOwner		= pOwner;
	m_pParent		= pParent;

	m_Identifier	= Identifier;
	m_Name			= Name;
	m_Description	= Description;

	m_bEnabled		= true;

	//-----------------------------------------------------
	m_nChildren		= 0;
	m_Children		= NULL;

	if( m_pParent )
	{
		m_pParent->_Add_Child(this);
	}

	//-----------------------------------------------------
	m_pData;

	switch( Type )
	{
	default                              : m_pData	= NULL                                                  ;	break;

	case PARAMETER_TYPE_Node             : m_pData	= new CSG_Parameter_Node              (this, Constraint);	break;

	case PARAMETER_TYPE_Bool             : m_pData	= new CSG_Parameter_Bool              (this, Constraint);	break;
	case PARAMETER_TYPE_Int              : m_pData	= new CSG_Parameter_Int               (this, Constraint);	break;
	case PARAMETER_TYPE_Double           : m_pData	= new CSG_Parameter_Double            (this, Constraint);	break;
	case PARAMETER_TYPE_Degree           : m_pData	= new CSG_Parameter_Degree            (this, Constraint);	break;
	case PARAMETER_TYPE_Date             : m_pData	= new CSG_Parameter_Date              (this, Constraint);	break;
	case PARAMETER_TYPE_Range            : m_pData	= new CSG_Parameter_Range             (this, Constraint);	break;
	case PARAMETER_TYPE_Choice           : m_pData	= new CSG_Parameter_Choice            (this, Constraint);	break;

	case PARAMETER_TYPE_String           : m_pData	= new CSG_Parameter_String            (this, Constraint);	break;
	case PARAMETER_TYPE_Text             : m_pData	= new CSG_Parameter_Text              (this, Constraint);	break;
	case PARAMETER_TYPE_FilePath         : m_pData	= new CSG_Parameter_File_Name         (this, Constraint);	break;

	case PARAMETER_TYPE_Font             : m_pData	= new CSG_Parameter_Font              (this, Constraint);	break;
	case PARAMETER_TYPE_Color            : m_pData	= new CSG_Parameter_Color             (this, Constraint);	break;
	case PARAMETER_TYPE_Colors           : m_pData	= new CSG_Parameter_Colors            (this, Constraint);	break;
	case PARAMETER_TYPE_FixedTable       : m_pData	= new CSG_Parameter_Fixed_Table       (this, Constraint);	break;
	case PARAMETER_TYPE_Grid_System      : m_pData	= new CSG_Parameter_Grid_System       (this, Constraint);	break;
	case PARAMETER_TYPE_Table_Field      : m_pData	= new CSG_Parameter_Table_Field       (this, Constraint);	break;
	case PARAMETER_TYPE_Table_Fields     : m_pData	= new CSG_Parameter_Table_Fields      (this, Constraint);	break;

	case PARAMETER_TYPE_DataObject_Output: m_pData	= new CSG_Parameter_Data_Object_Output(this, Constraint);	break;
	case PARAMETER_TYPE_Grid             : m_pData	= new CSG_Parameter_Grid              (this, Constraint);	break;
	case PARAMETER_TYPE_Grids            : m_pData	= new CSG_Parameter_Grids             (this, Constraint);	break;
	case PARAMETER_TYPE_Table            : m_pData	= new CSG_Parameter_Table             (this, Constraint);	break;
	case PARAMETER_TYPE_Shapes           : m_pData	= new CSG_Parameter_Shapes            (this, Constraint);	break;
	case PARAMETER_TYPE_TIN              : m_pData	= new CSG_Parameter_TIN               (this, Constraint);	break;
	case PARAMETER_TYPE_PointCloud       : m_pData	= new CSG_Parameter_PointCloud        (this, Constraint);	break;

	case PARAMETER_TYPE_Grid_List        : m_pData	= new CSG_Parameter_Grid_List         (this, Constraint);	break;
	case PARAMETER_TYPE_Grids_List       : m_pData	= new CSG_Parameter_Grids_List        (this, Constraint);	break;
	case PARAMETER_TYPE_Table_List       : m_pData	= new CSG_Parameter_Table_List        (this, Constraint);	break;
	case PARAMETER_TYPE_Shapes_List      : m_pData	= new CSG_Parameter_Shapes_List       (this, Constraint);	break;
	case PARAMETER_TYPE_TIN_List         : m_pData	= new CSG_Parameter_TIN_List          (this, Constraint);	break;
	case PARAMETER_TYPE_PointCloud_List  : m_pData	= new CSG_Parameter_PointCloud_List   (this, Constraint);	break;

	case PARAMETER_TYPE_Parameters       : m_pData	= new CSG_Parameter_Parameters        (this, Constraint);	break;
	}

	//-----------------------------------------------------
	switch( Type )
	{
	case PARAMETER_TYPE_Range:
		SG_Free(m_Children);
		m_nChildren	= 0;
		m_Children	= NULL;
		break;

	default:
		break;
	}

	//-----------------------------------------------------
	if( m_pParent && m_pParent->m_pData )
	{
		Set_UseInCMD(m_pParent->do_UseInCMD());
		Set_UseInGUI(m_pParent->do_UseInGUI());
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
void CSG_Parameter::Set_UseInGUI(bool bDoUse)
{
	if( bDoUse )
	{
		m_pData->m_Constraint	&= ~PARAMETER_NOT_FOR_GUI;
	}
	else
	{
		m_pData->m_Constraint	|=  PARAMETER_NOT_FOR_GUI;
	}

	for(int i=0; i<Get_Children_Count(); i++)
	{
		Get_Child(i)->Set_UseInGUI(bDoUse);
	}
}

//---------------------------------------------------------
void CSG_Parameter::Set_UseInCMD(bool bDoUse)
{
	if( bDoUse )
	{
		m_pData->m_Constraint	&= ~PARAMETER_NOT_FOR_CMD;
	}
	else
	{
		m_pData->m_Constraint	|=  PARAMETER_NOT_FOR_CMD;
	}

	for(int i=0; i<Get_Children_Count(); i++)
	{
		Get_Child(i)->Set_UseInCMD(bDoUse);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameter::Set_Enabled(bool bEnabled)
{
	if( m_bEnabled != bEnabled )
	{
		m_bEnabled	= bEnabled;

		return( !bEnabled );
	}

	return( bEnabled );
}

//---------------------------------------------------------
bool CSG_Parameter::is_Enabled(void) const
{
	if( do_UseInGUI() == false && SG_UI_Get_Window_Main() != NULL )
	{
		return( false );
	}

	if( do_UseInCMD() == false && SG_UI_Get_Window_Main() == NULL )
	{
		return( false );
	}

	return( m_bEnabled && (Get_Parent() == NULL || Get_Parent()->is_Enabled()) );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Children_Enabled(bool bEnabled)
{
	for(int i=0; i<Get_Children_Count(); i++)
	{
		Get_Child(i)->Set_Enabled(bEnabled);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter::is_Option(void)	const
{
	if( !is_Information() )
	{
		switch( Get_Type() )
		{
		case PARAMETER_TYPE_Bool        :
		case PARAMETER_TYPE_Int         :
		case PARAMETER_TYPE_Double      :
		case PARAMETER_TYPE_Degree      :
		case PARAMETER_TYPE_Date        :
		case PARAMETER_TYPE_Range       :
		case PARAMETER_TYPE_Choice      :
		case PARAMETER_TYPE_String      :
		case PARAMETER_TYPE_Text        :
		case PARAMETER_TYPE_FilePath    :
		case PARAMETER_TYPE_Font        :
		case PARAMETER_TYPE_Color       :
		case PARAMETER_TYPE_Colors      :
		case PARAMETER_TYPE_FixedTable  :
		case PARAMETER_TYPE_Grid_System :
		case PARAMETER_TYPE_Table_Field :
		case PARAMETER_TYPE_Table_Fields:
		case PARAMETER_TYPE_Parameters  :
			return( true );

		default:
			return( false );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::is_DataObject(void)	const
{
	switch( Get_Type() )
	{
	case PARAMETER_TYPE_DataObject_Output:
	case PARAMETER_TYPE_Grid             :
	case PARAMETER_TYPE_Grids            :
	case PARAMETER_TYPE_Table            :
	case PARAMETER_TYPE_Shapes           :
	case PARAMETER_TYPE_TIN              :
	case PARAMETER_TYPE_PointCloud       :
		return( true );

	default:
		return( false );
	}
}

//---------------------------------------------------------
bool CSG_Parameter::is_DataObject_List(void)	const
{
	switch( Get_Type() )
	{
	case PARAMETER_TYPE_Grid_List      :
	case PARAMETER_TYPE_Grids_List     :
	case PARAMETER_TYPE_Table_List     :
	case PARAMETER_TYPE_Shapes_List    :
	case PARAMETER_TYPE_TIN_List       :
	case PARAMETER_TYPE_PointCloud_List:
		return( true );

	default:
		return( false );
	}
}

//---------------------------------------------------------
bool CSG_Parameter::is_Parameters(void)	const
{
	return( Get_Type() == PARAMETER_TYPE_Parameters );
}

//---------------------------------------------------------
bool CSG_Parameter::is_Serializable(void)	const
{
	switch( Get_Type() )
	{
	case PARAMETER_TYPE_Undefined        :
	case PARAMETER_TYPE_Node             :
	case PARAMETER_TYPE_DataObject_Output:
		return( false );

	case PARAMETER_TYPE_String           :
		return( ((CSG_Parameter_String *)Get_Data())->is_Password() == false );

	default:
		return( !is_Information() );
	}
}

//---------------------------------------------------------
bool CSG_Parameter::is_Compatible(CSG_Parameter *pParameter)	const
{
	if( pParameter && pParameter->Get_Type() == Get_Type() )
	{
		switch( Get_Type() )
		{
		//-------------------------------------------------
		case PARAMETER_TYPE_Choice           :
			{
				bool	bResult	= pParameter->asChoice()->Get_Count() == asChoice()->Get_Count();

				for(int i=0; bResult && i<asChoice()->Get_Count(); i++)
				{
					bResult	= SG_STR_CMP(pParameter->asChoice()->Get_Item(i), asChoice()->Get_Item(i)) == 0;
				}

				return( bResult );
			}

		//-------------------------------------------------
		case PARAMETER_TYPE_FixedTable       :	return( pParameter->asTable()->is_Compatible(asTable()) );

		//-------------------------------------------------
		case PARAMETER_TYPE_Parameters       :
			{
				bool	bResult	= pParameter->asParameters()->Get_Count() == asParameters()->Get_Count();

				for(int i=0; bResult && i<asParameters()->Get_Count(); i++)
				{
					bResult	= pParameter->asParameters()->Get_Parameter(i)->is_Compatible(asParameters()->Get_Parameter(i));
				}

				return( bResult );
			}

		//-------------------------------------------------
		default:
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::is_Value_Equal(CSG_Parameter *pParameter)	const
{
	if( pParameter && pParameter->Get_Type() == Get_Type() )
	{
		switch( Get_Type() )
		{
		case PARAMETER_TYPE_Node             :	return( true );

		//-------------------------------------------------
		case PARAMETER_TYPE_Bool             :	return( pParameter->asBool  () == asBool  () );
		case PARAMETER_TYPE_Table_Field      :
		case PARAMETER_TYPE_Choice           :
		case PARAMETER_TYPE_Color            :
		case PARAMETER_TYPE_Int              :	return( pParameter->asInt   () == asInt   () );
		case PARAMETER_TYPE_Date             :
		case PARAMETER_TYPE_Degree           :
		case PARAMETER_TYPE_Double           :	return( pParameter->asDouble() == asDouble() );

		case PARAMETER_TYPE_Table_Fields     :
		case PARAMETER_TYPE_Font             :
		case PARAMETER_TYPE_Text             :
		case PARAMETER_TYPE_FilePath         :
		case PARAMETER_TYPE_String           :	return( SG_STR_CMP(pParameter->asString(), asString()) == 0 );

		case PARAMETER_TYPE_Range            :	return( pParameter->asRange()->Get_LoVal() == asRange()->Get_LoVal()
													&&  pParameter->asRange()->Get_HiVal() == asRange()->Get_HiVal() );

		case PARAMETER_TYPE_Grid_System      :	return( pParameter->asGrid_System()->is_Equal(*asGrid_System()) );

		//-------------------------------------------------
		case PARAMETER_TYPE_Colors           :
			{
				bool	bResult	= pParameter->asColors()->Get_Count() == asColors()->Get_Count();

				for(int i=0; bResult && i<asColors()->Get_Count(); i++)
				{
					bResult	= pParameter->asColors()->Get_Color(i) == asColors()->Get_Color(i);
				}

				return( bResult );
			}

		//-------------------------------------------------
		case PARAMETER_TYPE_FixedTable       :
			{
				bool	bResult	= pParameter->asTable()->is_Compatible(asTable()) && pParameter->asTable()->Get_Count() == asTable()->Get_Count();

				for(int i=0; bResult && i<asTable()->Get_Count(); i++)
				{
					CSG_Table_Record *pA = pParameter->asTable()->Get_Record(i), *pB = asTable()->Get_Record(i);

					for(int j=0; bResult && j<asTable()->Get_Field_Count(); j++)
					{
						bResult	= SG_STR_CMP(pA->asString(j), pB->asString(j)) == 0;
					}
				}

				return( bResult );
			}

		//-------------------------------------------------
		case PARAMETER_TYPE_DataObject_Output:
		case PARAMETER_TYPE_Grid             :
		case PARAMETER_TYPE_Grids            :
		case PARAMETER_TYPE_Table            :
		case PARAMETER_TYPE_Shapes           :
		case PARAMETER_TYPE_TIN              :
		case PARAMETER_TYPE_PointCloud       :	return( pParameter->asDataObject() == asDataObject() );

		//-------------------------------------------------
		case PARAMETER_TYPE_Grid_List        :
		case PARAMETER_TYPE_Grids_List       :
		case PARAMETER_TYPE_Table_List       :
		case PARAMETER_TYPE_Shapes_List      :
		case PARAMETER_TYPE_TIN_List         :
		case PARAMETER_TYPE_PointCloud_List  :
			{
				bool	bResult	= pParameter->asList()->Get_Item_Count() == asList()->Get_Item_Count();

				for(int i=0; bResult && i<asList()->Get_Item_Count(); i++)
				{
					bResult	= pParameter->asList()->Get_Item(i) == asList()->Get_Item(i);
				}

				return( bResult );
			}

		//-------------------------------------------------
		case PARAMETER_TYPE_Parameters       :
			{
				bool	bResult	= pParameter->asParameters()->Get_Count() == asParameters()->Get_Count();

				for(int i=0; bResult && i<asParameters()->Get_Count(); i++)
				{
					bResult	= pParameter->asParameters()->Get_Parameter(i)->is_Value_Equal(asParameters()->Get_Parameter(i));
				}

				return( bResult );
			}

		//-------------------------------------------------
		default:	break;
		}
	}

	return( false );
}

//---------------------------------------------------------
TSG_Data_Object_Type CSG_Parameter::Get_DataObject_Type(void)	const
{
	switch( Get_Type() )
	{
	default                              :	return( SG_DATAOBJECT_TYPE_Undefined  );
	case PARAMETER_TYPE_Grid             :
	case PARAMETER_TYPE_Grid_List        :	return( SG_DATAOBJECT_TYPE_Grid       );
	case PARAMETER_TYPE_Grids            :
	case PARAMETER_TYPE_Grids_List       :	return( SG_DATAOBJECT_TYPE_Grids      );
	case PARAMETER_TYPE_Table            :
	case PARAMETER_TYPE_Table_List       :	return( SG_DATAOBJECT_TYPE_Table      );
	case PARAMETER_TYPE_Shapes           :
	case PARAMETER_TYPE_Shapes_List      :	return( SG_DATAOBJECT_TYPE_Shapes     );
	case PARAMETER_TYPE_TIN              :
	case PARAMETER_TYPE_TIN_List         :	return( SG_DATAOBJECT_TYPE_TIN        );
	case PARAMETER_TYPE_PointCloud       :
	case PARAMETER_TYPE_PointCloud_List  :	return( SG_DATAOBJECT_TYPE_PointCloud );
	case PARAMETER_TYPE_DataObject_Output:
		return( ((CSG_Parameter_Data_Object_Output *)m_pData)->Get_DataObject_Type() );
	}
}

//---------------------------------------------------------
CSG_Data_Manager * CSG_Parameter::Get_Manager(void)	const
{
	return( m_pOwner ? m_pOwner->Get_Manager() : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Parameter::Get_Identifier(void)	const
{
	return( m_Identifier );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Name(const CSG_String &Name)
{
	m_Name	= Name;

	return( true );
}

const SG_Char * CSG_Parameter::Get_Name(void)	const
{
	return( m_Name );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Description(const CSG_String &Description)
{
	m_Description	= Description;

	return( true );
}

const SG_Char * CSG_Parameter::Get_Description(void)	const
{
	return( m_Description );
}

//---------------------------------------------------------
CSG_String CSG_Parameter::Get_Description(int Flags)	const
{
	return( Get_Description(Flags, SG_T("\n")) );
}

//---------------------------------------------------------
#define SEPARATE	if( bSeparate )	s.Append(Separator);	bSeparate	= true;

//---------------------------------------------------------
CSG_String CSG_Parameter::Get_Description(int Flags, const SG_Char *Separator)	const
{
	if( !Separator || !Separator[0] )
	{
		return( Get_Description(Flags) );
	}

	bool		bSeparate	= false;
	int			i;
	CSG_String	s;

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_NAME) != 0 )
	{
		SEPARATE;	s	+= CSG_String::Format(SG_T("%s"), Get_Name());
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_TYPE) != 0 )
	{
		SEPARATE;	s	+= CSG_String::Format(SG_T("%s"), Get_Type_Name().c_str());

		if( is_DataObject() || is_DataObject_List() )
		{
			if( is_Input() )
			{
				if( is_Optional() )
					s	+= CSG_String::Format(SG_T(" (%s)"), _TL("optional input"));
				else
					s	+= CSG_String::Format(SG_T(" (%s)"), _TL("input"));
			}
			else if( is_Output() )
			{
				if( is_Optional() )
					s	+= CSG_String::Format(SG_T(" (%s)"), _TL("optional output"));
				else
					s	+= CSG_String::Format(SG_T(" (%s)"), _TL("output"));
			}
		}
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_OPTIONAL) != 0 && is_Optional() )
	{
		SEPARATE;	s	+= CSG_String::Format(SG_T("%s"), _TL("optional"));
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_PROPERTIES) != 0 )
	{
		switch( Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Choice:
			SEPARATE;	s	+= CSG_String::Format(SG_T("%s:"), _TL("Available Choices"));

			for(i=0; i<asChoice()->Get_Count(); i++)
			{
				s	+= CSG_String::Format(SG_T("%s[%d] %s"), Separator, i, asChoice()->Get_Item(i));
			}
			break;

		case PARAMETER_TYPE_Int:
			if( asValue()->has_Minimum() )
			{
				SEPARATE;	s	+= CSG_String::Format(SG_T("%s: %d"), _TL("Minimum"), (int)asValue()->Get_Minimum());
			}
			if( asValue()->has_Maximum() )
			{
				SEPARATE;	s	+= CSG_String::Format(SG_T("%s: %d"), _TL("Maximum"), (int)asValue()->Get_Maximum());
			}
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
//		case PARAMETER_TYPE_Range:
			if( asValue()->has_Minimum() )
			{
				SEPARATE;	s	+= CSG_String::Format(SG_T("%s: %f"), _TL("Minimum"), asValue()->Get_Minimum());
			}
			if( asValue()->has_Maximum() )
			{
				SEPARATE;	s	+= CSG_String::Format(SG_T("%s: %f"), _TL("Maximum"), asValue()->Get_Maximum());
			}
			break;

		case PARAMETER_TYPE_FixedTable:
			SEPARATE;	s	+= CSG_String::Format(SG_T("%d %s:%s"), asTable()->Get_Field_Count(), _TL("Fields"), Separator);

			for(i=0; i<asTable()->Get_Field_Count(); i++)
			{
				s	+= CSG_String::Format(SG_T("- %d. [%s] %s%s"), i + 1, SG_Data_Type_Get_Name(asTable()->Get_Field_Type(i)).c_str(), asTable()->Get_Field_Name(i), Separator);
			}
			break;

		case PARAMETER_TYPE_Parameters:
			SEPARATE;	s	+= CSG_String::Format(SG_T("%d %s:%s"), asParameters()->Get_Count(), _TL("Parameters"), Separator);

			for(i=0; i<asParameters()->Get_Count(); i++)
			{
				s	+= CSG_String::Format(SG_T("- %d. %s%s"), i + 1, asParameters()->Get_Parameter(i)->Get_Description(Flags, Separator).c_str(), Separator);
			}
			break;
		}

		if( !m_pData->Get_Default().is_Empty() )
		{
			SEPARATE;	s	+= CSG_String::Format(SG_T("%s: %s"), _TL("Default"), m_pData->Get_Default().c_str());
		}
	}

	//-----------------------------------------------------
	if( (Flags & PARAMETER_DESCRIPTION_TEXT) != 0 && m_Description.Length() > 0 )
	{
		SEPARATE;

		s	+= m_Description;
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
		has_Changed();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(double Value)
{
	if( m_pData->Set_Value(Value) )
	{
		has_Changed();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(const CSG_String &Value)
{
	if( m_pData->Set_Value(Value) )
	{
		has_Changed();

		return( true );
	}

	return( false );
}

bool CSG_Parameter::Set_Value(const char *Value)
{
	return( Set_Value(CSG_String(Value)) );
}

bool CSG_Parameter::Set_Value(const wchar_t *Value)
{
	return( Set_Value(CSG_String(Value)) );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(void *Value)
{
	if( m_pData->Set_Value(Value) )
	{
		has_Changed();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Set_Value(CSG_Parameter *Value)
{
	if( Value )
	{
		switch( Value->Get_Type() )
		{
		default:								return( Assign(Value) );

		case PARAMETER_TYPE_Choice:				return( Set_Value(Value->asInt()) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::has_Changed(int Check_Flags)
{
	if( m_pOwner )
	{
		return( m_pOwner->_On_Parameter_Changed(this, Check_Flags) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter::Check(bool bSilent)
{
	if( !is_Enabled() )
	{
		return( true );
	}

	//-----------------------------------------------------
	if( Get_Type() == PARAMETER_TYPE_Parameters )
	{
		return( asParameters()->DataObjects_Check(bSilent) );
	}

	//-----------------------------------------------------
	if( Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		if( m_pOwner->Get_Manager() && !m_pOwner->Get_Manager()->Exists(*asGrid_System()) )
		{
			Set_Value((void *)NULL);
		}

		return( true );	// ( false );
	}

	//-----------------------------------------------------
	if( is_DataObject() )
	{
		if( is_Input() || (is_Output() && asDataObject() != DATAOBJECT_CREATE) )
		{
			if( m_pOwner->Get_Manager() && !m_pOwner->Get_Manager()->Exists(asDataObject()) )
			{
				Set_Value(DATAOBJECT_NOTSET);
			}
		}

		return( asDataObject() || is_Optional() );
	}

	//-----------------------------------------------------
	else if( is_DataObject_List() )
	{
		for(int j=asList()->Get_Item_Count()-1; j>=0; j--)
		{
			CSG_Data_Object	*pDataObject	= asList()->Get_Item(j);

			if( !pDataObject || (m_pOwner->Get_Manager() && !m_pOwner->Get_Manager()->Exists(pDataObject)) )
			{
				asList()->Del_Item(j, false);
			}
		}

		asList()->Update_Data();

		return( is_Output() || is_Optional() || asList()->Get_Item_Count() > 0 );
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
bool CSG_Parameter::Assign(CSG_Parameter *pSource)
{
	if( pSource )
	{
		m_bEnabled	= pSource->m_bEnabled;

		return( m_pData->Assign(pSource->m_pData) );
	}

	return( false );
}

//---------------------------------------------------------
CSG_MetaData * CSG_Parameter::Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		if( !is_Information() && Get_Type() != PARAMETER_TYPE_Node && Get_Type() != PARAMETER_TYPE_Undefined )
		{
			CSG_MetaData	*pEntry	= Entry.Add_Child(
				is_Option         () ? "OPTION"    :
				is_DataObject     () ? "DATA"      :
				is_DataObject_List() ? "DATA_LIST" : "PARAMETER"
			);

			pEntry->Add_Property("type" , Get_Type_Identifier        ());
			pEntry->Add_Property("id"   , Get_Identifier             ());
			pEntry->Add_Property("name" , Get_Name                   ());
			pEntry->Add_Property("parms", Get_Owner()->Get_Identifier());

			m_pData->Serialize(*pEntry, bSave);

			return( pEntry );
		}
	}
	else
	{
		if(	Entry.Cmp_Property("type", Get_Type_Identifier())
		&&	Entry.Cmp_Property("id"  , Get_Identifier     ()) )
		{
			return( m_pData->Serialize(Entry, bSave) ? &Entry : NULL );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters_Grid_Target::CSG_Parameters_Grid_Target(void)
{
	m_pParameters	= NULL;
}

//---------------------------------------------------------
bool CSG_Parameters_Grid_Target::Create(CSG_Parameters *pParameters, bool bAddDefaultGrid, CSG_Parameter *pNode, const CSG_String &Prefix)
{
	if( pParameters == NULL )
	{
		return( false );
	}

	m_pParameters	= pParameters;
	m_Prefix		= Prefix;

	//-----------------------------------------------------
	CSG_String	ParentID(pNode ? pNode->Get_Identifier() : SG_T("")), TargetID(m_Prefix + "DEFINITION");

	m_pParameters->Add_Choice(
		ParentID, TargetID, _TL("Target Grid System"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("user defined"),
			_TL("grid or grid system")
		), 0
	);

	//-----------------------------------------------------
	m_pParameters->Add_Value     (TargetID, m_Prefix + "USER_SIZE", _TL("Cellsize"), _TL(""), PARAMETER_TYPE_Double, 1.0, 0.0, true);
	m_pParameters->Add_Value     (TargetID, m_Prefix + "USER_XMIN", _TL("Left"    ), _TL(""), PARAMETER_TYPE_Double,   0.0);
	m_pParameters->Add_Value     (TargetID, m_Prefix + "USER_XMAX", _TL("Right"   ), _TL(""), PARAMETER_TYPE_Double, 100.0);
	m_pParameters->Add_Value     (TargetID, m_Prefix + "USER_YMIN", _TL("Bottom"  ), _TL(""), PARAMETER_TYPE_Double,   0.0);
	m_pParameters->Add_Value     (TargetID, m_Prefix + "USER_YMAX", _TL("Top"     ), _TL(""), PARAMETER_TYPE_Double, 100.0);
	m_pParameters->Add_Info_Value(TargetID, m_Prefix + "USER_COLS", _TL("Columns" ), _TL(""), PARAMETER_TYPE_Int   , 100);
	m_pParameters->Add_Info_Value(TargetID, m_Prefix + "USER_ROWS", _TL("Rows"    ), _TL(""), PARAMETER_TYPE_Int   , 100);
	m_pParameters->Add_Choice    (TargetID, m_Prefix + "USER_FITS", _TL("Fit"     ), _TL(""),
		CSG_String::Format("%s|%s|",
			_TL("nodes"),
			_TL("cells")
		), 0
	);

	//-----------------------------------------------------
	m_pParameters->Add_Grid_System(TargetID, m_Prefix + "SYSTEM", _TL("Grid System"), _TL(""));

	if( !SG_UI_Get_Window_Main() )
	{
		m_pParameters->Add_Grid(m_Prefix + "SYSTEM", m_Prefix + "TEMPLATE", _TL("Target System"), _TL("use this grid's system for output grids"), PARAMETER_INPUT_OPTIONAL, false);
	}

	//-----------------------------------------------------
	if( bAddDefaultGrid )
	{
		Add_Grid(m_Prefix + "OUT_GRID", _TL("Target Grid"), false);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters_Grid_Target::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !m_pParameters || !pParameters || m_pParameters->Get_Identifier().Cmp(pParameters->Get_Identifier()) || !pParameter )
	{
		return( false );
	}

	CSG_Parameter	*pSize	= pParameters->Get_Parameter(m_Prefix + "USER_SIZE");
	CSG_Parameter	*pXMin	= pParameters->Get_Parameter(m_Prefix + "USER_XMIN");
	CSG_Parameter	*pXMax	= pParameters->Get_Parameter(m_Prefix + "USER_XMAX");
	CSG_Parameter	*pYMin	= pParameters->Get_Parameter(m_Prefix + "USER_YMIN");
	CSG_Parameter	*pYMax	= pParameters->Get_Parameter(m_Prefix + "USER_YMAX");
	CSG_Parameter	*pRows	= pParameters->Get_Parameter(m_Prefix + "USER_ROWS");
	CSG_Parameter	*pCols	= pParameters->Get_Parameter(m_Prefix + "USER_COLS");
	CSG_Parameter	*pFits	= pParameters->Get_Parameter(m_Prefix + "USER_FITS");

	double		Size	= pSize->asDouble();

	CSG_Rect	r(pXMin->asDouble(), pYMin->asDouble(), pXMax->asDouble(), pYMax->asDouble());

	if( m_bFitToCells )
	{
		r.Deflate(0.5 * Size, false);
	}

	//-----------------------------------------------------
	if(      !SG_STR_CMP(pParameter->Get_Identifier(), pFits->Get_Identifier()) )
	{
		if( m_bFitToCells != (pFits->asInt() == 1) )
		{
			m_bFitToCells	= pFits->asInt() == 1;
		}
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pSize->Get_Identifier()) )
	{
		r.m_rect.xMax	= r.Get_XMin() + Size * (int)(r.Get_XRange() / Size);
		r.m_rect.yMax	= r.Get_YMin() + Size * (int)(r.Get_YRange() / Size);
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pXMin->Get_Identifier()) )
	{
		if( r.Get_XRange() <= 0.0 )
			r.m_rect.xMin	= r.Get_XMax() - Size * pCols->asInt();
		else
			r.m_rect.xMax	= r.Get_XMin() + Size * (int)(r.Get_XRange() / Size);
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pXMax->Get_Identifier()) )
	{
		if( r.Get_XRange() <= 0.0 )
			r.m_rect.xMax	= r.Get_XMin() + Size * pCols->asInt();
		else
			r.m_rect.xMin	= r.Get_XMax() - Size * (int)(r.Get_XRange() / Size);
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pYMin->Get_Identifier()) )
	{
		if( r.Get_YRange() <= 0.0 )
			r.m_rect.yMin	= r.Get_YMax() - Size * pRows->asInt();
		else
			r.m_rect.yMax	= r.Get_YMin() + Size * (int)(r.Get_YRange() / Size);
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pYMax->Get_Identifier()) )
	{
		if( r.Get_YRange() <= 0.0 )
			r.m_rect.yMax	= r.Get_YMin() + Size * pRows->asInt();
		else
			r.m_rect.yMin	= r.Get_YMax() - Size * (int)(r.Get_YRange() / Size);
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pCols->Get_Identifier()) && pCols->asInt() > 0 )
	{
		pSize->Set_Value(Size	= r.Get_XRange() / pCols->asDouble());
	}
	else if( !SG_STR_CMP(pParameter->Get_Identifier(), pRows->Get_Identifier()) && pRows->asInt() > 0 )
	{
		pSize->Set_Value(Size	= r.Get_YRange() / pRows->asDouble());
	}
	else
	{
		return( true );	// none of the relevant parameters did change
	}

	//-----------------------------------------------------
	pCols->Set_Value(1 + (int)((r.Get_XRange()) / Size));
	pRows->Set_Value(1 + (int)((r.Get_YRange()) / Size));

	if( m_bFitToCells )
	{
		r.Inflate(0.5 * Size, false);
	}

	pXMin->Set_Value(r.Get_XMin());
	pXMax->Set_Value(r.Get_XMax());
	pYMin->Set_Value(r.Get_YMin());
	pYMax->Set_Value(r.Get_YMax());

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameters_Grid_Target::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !m_pParameters || !pParameters || m_pParameters->Get_Identifier().Cmp(pParameters->Get_Identifier()) || !pParameter )
	{
		return( false );
	}

	if( (pParameter = pParameters->Get_Parameter(m_Prefix + "DEFINITION")) == NULL )
	{
		return( false );
	}

	pParameters->Set_Enabled(m_Prefix + "USER_SIZE", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_XMIN", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_XMAX", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_YMIN", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_YMAX", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_ROWS", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_COLS", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_FITS", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "USER_OPTS", pParameter->asInt() == 0);
	pParameters->Set_Enabled(m_Prefix + "SYSTEM"   , pParameter->asInt() == 1);

	return( true );
}

//---------------------------------------------------------
/**
  * Initializes the grid system from extent and number of rows.
  * Extent calculation is done for grid nodes and will automatically
  * become adjusted to match the grid cells if necessary. If Rounding is
  * greater than zero it specifies the number of significant figures to which
  * the cell size is rounded and also adjusts the extent coordinates
  * to be a multiple of cell size.
*/
bool CSG_Parameters_Grid_Target::Set_User_Defined(CSG_Parameters *pParameters, const TSG_Rect &Extent, int Rows, int Rounding)
{
	if( !SG_UI_Get_Window_Main() )	// no cancel button, so set parameters directly
	{
		pParameters	= m_pParameters;
	}

	if( !m_pParameters || !pParameters || m_pParameters->Get_Identifier().Cmp(pParameters->Get_Identifier()) )
	{
		return( false );
	}

	if( Rows < 1 && (Rows = m_pParameters->Get_Parameter(m_Prefix + "USER_ROWS")->asInt()) < 1 )
	{
		Rows	= 100;
	}

	//-----------------------------------------------------
	CSG_Rect	r(Extent);

	if( r.Get_XRange() == 0.0 && r.Get_YRange() == 0.0 )
	{
		r.Inflate(0.5 * Rows, false);	// assume cellsize = 1.0
	}
	else if( r.Get_XRange() == 0.0 )
	{
		double	d	= 0.5 * r.Get_YRange() / Rows;	r.m_rect.xMin	-= d;	r.m_rect.xMax	+= d;	// inflate by half cellsize
	}
	else if( r.Get_YRange() == 0.0 )
	{
		double	d	= 0.5 * r.Get_XRange() / Rows;	r.m_rect.yMin	-= d;	r.m_rect.yMax	+= d;	// inflate by half cellsize
	}

	//-----------------------------------------------------
	double	Size	= r.Get_YRange() / (Rows - 1);

	if( Rounding > 0 )
	{
		Size	= SG_Get_Rounded_To_SignificantFigures(Size, Rounding);

		r.m_rect.xMin	= Size * floor(r.m_rect.xMin / Size);
		r.m_rect.yMin	= Size * floor(r.m_rect.yMin / Size);
		r.m_rect.yMax	= Size * ceil (r.m_rect.yMax / Size);
	}

	int		Cols	= 1 + (int)(r.Get_XRange() / Size);

	r.m_rect.xMax	= r.Get_XMin() + (Cols - 1) * Size;

	//-----------------------------------------------------
	if( (m_bFitToCells = pParameters->Get_Parameter(m_Prefix + "USER_FITS")->asInt() == 1) == true )
	{
		r.Inflate(0.5 * Size, false);
	}

	bool	bCallback	= pParameters->Set_Callback(false);

	pParameters->Set_Parameter(m_Prefix + "USER_SIZE", Size        );
	pParameters->Set_Parameter(m_Prefix + "USER_XMIN", r.Get_XMin());
	pParameters->Set_Parameter(m_Prefix + "USER_XMAX", r.Get_XMax());
	pParameters->Set_Parameter(m_Prefix + "USER_YMIN", r.Get_YMin());
	pParameters->Set_Parameter(m_Prefix + "USER_YMAX", r.Get_YMax());
	pParameters->Set_Parameter(m_Prefix + "USER_COLS", Cols        );
	pParameters->Set_Parameter(m_Prefix + "USER_ROWS", Rows        );

	pParameters->Set_Callback(bCallback);

	return( true );
}

//---------------------------------------------------------
/**
  * Initializes the grid system from 'pPoints' extent and fits number of columns/rows to the average point density.
  * Number of columns/rows can be increased if 'Scale' is greater than 1.
*/
bool CSG_Parameters_Grid_Target::Set_User_Defined(CSG_Parameters *pParameters, CSG_Shapes *pPoints, int Scale, int Rounding)
{
	if( !pPoints || pPoints->Get_Count() <= 0 || pPoints->Get_Extent().Get_Area() <= 0.0 )
	{
		return( false );
	}

	CSG_Rect	r	= pPoints->Get_Extent();

	double	Size	= sqrt(r.Get_Area() / pPoints->Get_Count());	// edge length of a square given as average area per point (cell size)

	int		Rows	= 1 + (int)(0.5 + r.Get_YRange() / Size);

	if( Scale > 1 )
	{
		Rows	*= Scale;
	}

	r.Inflate(Size, false);

	return( Set_User_Defined(pParameters, r, Rows, Rounding) );
}

//---------------------------------------------------------
/**
  * Initializes the grid system from 'System', if it represents a valid grid system.
*/
bool CSG_Parameters_Grid_Target::Set_User_Defined(CSG_Parameters *pParameters, const CSG_Grid_System &System)
{
	return( System.is_Valid() && Set_User_Defined(pParameters, System.Get_Extent(), System.Get_NY(), 0) );
}

//---------------------------------------------------------
/**
  * Initializes the grid system from lower left cell center coordinate, cell size and number of columns and rows (nx, ny).
*/
bool CSG_Parameters_Grid_Target::Set_User_Defined(CSG_Parameters *pParameters, double xMin, double yMin, double Cellsize, int nx, int ny)
{
	return( Set_User_Defined(pParameters, CSG_Grid_System(Cellsize, xMin, yMin, nx, ny)) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Parameters_Grid_Target::Add_Grid(const CSG_String &Identifier, const CSG_String &Name, bool bOptional)
{
	if( !m_pParameters || Identifier.Length() == 0 || m_pParameters->Get_Parameter(Identifier) != NULL )
	{
		return( false );
	}

	CSG_Parameter	*pTarget	= m_pParameters->Get_Parameter(m_Prefix + "DEFINITION");
	CSG_Parameter	*pSystem	= NULL;

	for(int i=0; i<pTarget->Get_Children_Count() && !pSystem; i++)
	{
		if( pTarget->Get_Child(i)->Get_Type() == PARAMETER_TYPE_Grid_System )
		{
			pSystem	= pTarget->Get_Child(i);
		}
	}

	m_pParameters->Add_Grid(pSystem ? pSystem->Get_Identifier() : SG_T(""), Identifier, Name, _TL(""), bOptional ? PARAMETER_OUTPUT_OPTIONAL : PARAMETER_OUTPUT, false);

	if( bOptional && SG_UI_Get_Window_Main() )
	{
		CSG_Parameter	*pNode	= m_pParameters->Get_Parameter(m_Prefix + "USER_OPTS");

		if( !pNode )
		{
			pNode	= m_pParameters->Add_Node(pTarget->Get_Identifier(), m_Prefix + "USER_OPTS", _TL("Optional Target Grids"), _TL(""));
		}

		m_pParameters->Add_Value(pNode->Get_Identifier(), Identifier + "_CREATE", Name, _TL(""), PARAMETER_TYPE_Bool, false);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System CSG_Parameters_Grid_Target::Get_System(void)
{
	CSG_Grid_System	System;

	if( m_pParameters )
	{
		if( m_pParameters->Get_Parameter(m_Prefix + "DEFINITION")->asInt() == 0 )	// user defined
		{
			double	Size	= m_pParameters->Get_Parameter(m_Prefix + "USER_SIZE")->asDouble();

			CSG_Rect	r(
				m_pParameters->Get_Parameter(m_Prefix + "USER_XMIN")->asDouble(),
				m_pParameters->Get_Parameter(m_Prefix + "USER_YMIN")->asDouble(),
				m_pParameters->Get_Parameter(m_Prefix + "USER_XMAX")->asDouble(),
				m_pParameters->Get_Parameter(m_Prefix + "USER_YMAX")->asDouble()
			);

			if( m_bFitToCells )
			{
				r.Deflate(0.5 * Size, false);
			}

			System.Assign(Size, r);
		}
		else
		{
			CSG_Parameter	*pParameter	= m_pParameters->Get_Parameter(m_Prefix + "SYSTEM");

			if( pParameter->asGrid_System() )
			{
				System.Assign(*pParameter->asGrid_System());
			}
		}
	}

	return( System );
}

//---------------------------------------------------------
CSG_Grid * CSG_Parameters_Grid_Target::Get_Grid(const CSG_String &Identifier, TSG_Data_Type Type)
{
	if( !m_pParameters )
	{
		return( NULL );
	}

	CSG_Parameter	*pParameter	= m_pParameters->Get_Parameter(Identifier);

	if( !pParameter )
	{
		return( NULL );
	}

	CSG_Grid_System	System(Get_System());

	if( !System.is_Valid() )
	{
		return( NULL );
	}

	CSG_Grid	*pGrid	= NULL;

	if( m_pParameters->Get_Parameter(m_Prefix + "DEFINITION")->asInt() == 0 )
	{
		if( m_pParameters->Get_Parameter(Identifier + "_CREATE") == NULL
		||  m_pParameters->Get_Parameter(Identifier + "_CREATE")->asBool() )
		{
			pGrid	= SG_Create_Grid(System, Type);
		}
	}
	else
	{
		pGrid	= pParameter->asGrid();

		if( (pGrid == DATAOBJECT_NOTSET && !pParameter->is_Optional()) || pGrid == DATAOBJECT_CREATE )
		{
			pGrid	= SG_Create_Grid(System, Type);
		}
	}

	if( pGrid && pGrid != pParameter->asGrid() )
	{
		pParameter->Set_Value(pGrid);
	}

	return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * CSG_Parameters_Grid_Target::Get_Grid(TSG_Data_Type Type)
{
	return( Get_Grid(m_Prefix + "OUT_GRID", Type) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
