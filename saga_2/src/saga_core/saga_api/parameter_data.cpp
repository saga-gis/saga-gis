
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
//                  parameter_data.cpp                   //
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
#include <string.h>

#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ENTRY_TEXT_END				SG_T("[TEXT_ENTRY_END]")
#define ENTRY_DATAOBJECT_CREATE		SG_T("[ENTRY_DATAOBJECT_CREATE]")
#define ENTRY_DATAOBJECTLIST_END	SG_T("[ENTRY_DATAOBJECTLIST_END]")


///////////////////////////////////////////////////////////
//														 //
//						Base Class						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data::CSG_Parameter_Data(CSG_Parameter *pOwner, long Constraint)
{
	m_pOwner		= pOwner;
	m_Constraint	= Constraint;
}

//---------------------------------------------------------
CSG_Parameter_Data::~CSG_Parameter_Data(void)
{
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Data::Get_Type_Name(void)
{
	switch( Get_Type() )
	{
	default:								return( LNG("[PRM] Parameter") );

	case PARAMETER_TYPE_Node:				return( LNG("[PRM] Node") );

	case PARAMETER_TYPE_Bool:				return( LNG("[PRM] Boolean") );
	case PARAMETER_TYPE_Int:				return( LNG("[PRM] Integer") );
	case PARAMETER_TYPE_Double:				return( LNG("[PRM] Floating point") );
	case PARAMETER_TYPE_Degree:				return( LNG("[PRM] Degree") );
	case PARAMETER_TYPE_Range:				return( LNG("[PRM] Value range") );
	case PARAMETER_TYPE_Choice:				return( LNG("[PRM] Choice") );

	case PARAMETER_TYPE_String:				return( LNG("[PRM] Text") );
	case PARAMETER_TYPE_Text:				return( LNG("[PRM] Long text") );
	case PARAMETER_TYPE_FilePath:			return( LNG("[PRM] File path") );

	case PARAMETER_TYPE_Font:				return( LNG("[PRM] Font") );
	case PARAMETER_TYPE_Color:				return( LNG("[PRM] Color") );
	case PARAMETER_TYPE_Colors:				return( LNG("[PRM] Colors") );
	case PARAMETER_TYPE_FixedTable:			return( LNG("[PRM] Static table") );
	case PARAMETER_TYPE_Grid_System:		return( LNG("[PRM] Grid system") );
	case PARAMETER_TYPE_Table_Field:		return( LNG("[PRM] Table field") );

	case PARAMETER_TYPE_DataObject_Output:	return( LNG("[PRM] Data Object") );
	case PARAMETER_TYPE_Grid:				return( LNG("[PRM] Grid") );
	case PARAMETER_TYPE_Table:				return( LNG("[PRM] Table") );
	case PARAMETER_TYPE_Shapes:				return( LNG("[PRM] Shapes") );
	case PARAMETER_TYPE_TIN:				return( LNG("[PRM] TIN") );

	case PARAMETER_TYPE_Grid_List:			return( LNG("[PRM] Grid list") );
	case PARAMETER_TYPE_Table_List:			return( LNG("[PRM] Table list") );
	case PARAMETER_TYPE_Shapes_List:		return( LNG("[PRM] Shapes list") );
	case PARAMETER_TYPE_TIN_List:			return( LNG("[PRM] TIN list") );

	case PARAMETER_TYPE_Parameters:			return( LNG("[PRM] Parameters") );
	}
}

//---------------------------------------------------------
bool CSG_Parameter_Data::Set_Value(int Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(double Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(void *Value)
{
	return( false );
}

//---------------------------------------------------------
int CSG_Parameter_Data::asInt(void)
{
	return( 0 );
}

double CSG_Parameter_Data::asDouble(void)
{
	return( 0.0 );
}

void * CSG_Parameter_Data::asPointer(void)
{
	return( NULL );
}

const SG_Char * CSG_Parameter_Data::asString(void)
{
	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Data::Assign(CSG_Parameter_Data *pSource)
{
	if( pSource && Get_Type() == pSource->Get_Type() )
	{
		On_Assign(pSource);

		return( true );
	}

	return( false );
}

void CSG_Parameter_Data::On_Assign(CSG_Parameter_Data *pSource)
{}

//---------------------------------------------------------
bool CSG_Parameter_Data::Serialize(CSG_File &Stream, bool bSave)
{
	return( On_Serialize(Stream, bSave) );
}

bool CSG_Parameter_Data::On_Serialize(CSG_File &Stream, bool bSave)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Node							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Node::CSG_Parameter_Node(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

CSG_Parameter_Node::~CSG_Parameter_Node(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Bool							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Bool::CSG_Parameter_Bool(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Value		= false;
}

CSG_Parameter_Bool::~CSG_Parameter_Bool(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Bool::Set_Value(int Value)
{
	bool	bValue = Value != 0 ? true : false;

	if( m_Value != bValue )
	{
		m_Value		= bValue;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Bool::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Bool::asString(void)
{
	return( m_Value ? LNG("[VAL] yes") : LNG("[VAL] no") );
}

//---------------------------------------------------------
void CSG_Parameter_Bool::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Value		= ((CSG_Parameter_Bool *)pSource)->m_Value;
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::On_Serialize(CSG_File &Stream, bool bSave)
{
    int		i;
    
	if( bSave )
	{
	    i		= m_Value;
		Stream.Printf(SG_T("%d\n"),  i);
	}
	else
	{
		SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%d"), &i);
		m_Value	= i != 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Value							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Value::CSG_Parameter_Value(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Minimum	= 0.0;
	m_bMinimum	= false;

	m_Maximum	= 0.0;
	m_bMaximum	= false;
}

CSG_Parameter_Value::~CSG_Parameter_Value(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Value::Set_Range(double Minimum, double Maximum)
{
	if( 1 )
	{
		m_Minimum		= Minimum;
		m_Maximum		= Maximum;

		switch( Get_Type() )
		{
		default:
			return( false );

		case PARAMETER_TYPE_Int:
			Set_Value(asInt());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Set_Value(asDouble());
			break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_Value::Set_Minimum(double Minimum, bool bOn)
{
	if( bOn == false || (m_bMaximum && Minimum >= m_Maximum) )
	{
		m_bMinimum	= false;
	}
	else
	{
		m_bMinimum	= true;

		Set_Range(Minimum, m_Maximum);
	}
}

void CSG_Parameter_Value::Set_Maximum(double Maximum, bool bOn)
{
	if( bOn == false || (m_bMaximum && Maximum <= m_Minimum) )
	{
		m_bMaximum	= false;
	}
	else
	{
		m_bMaximum	= true;

		Set_Range(m_Minimum, Maximum);
	}
}

//---------------------------------------------------------
void CSG_Parameter_Value::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Minimum	= ((CSG_Parameter_Value *)pSource)->m_Minimum;
	m_bMinimum	= ((CSG_Parameter_Value *)pSource)->m_bMinimum;

	m_Maximum	= ((CSG_Parameter_Value *)pSource)->m_Maximum;
	m_bMaximum	= ((CSG_Parameter_Value *)pSource)->m_bMaximum;
}


///////////////////////////////////////////////////////////
//														 //
//						Int								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Int::CSG_Parameter_Int(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Value(pOwner, Constraint)
{
	m_Value		= 0;
}

CSG_Parameter_Int::~CSG_Parameter_Int(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Int::Set_Value(int Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( Set_Value((int)m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( Set_Value((int)m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Int::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

bool CSG_Parameter_Int::Set_Value(void *Value)
{
	int		val;

	if( Value )
	{
		m_String.Printf((SG_Char *)Value);

		if( m_String.asInt(val) )
		{
			return( Set_Value(val) );
		}
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Int::asString(void)
{
	m_String.Printf(SG_T("%d"), m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Int::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Value::On_Assign(pSource);

	Set_Value(((CSG_Parameter_Value *)pSource)->asInt());
}

//---------------------------------------------------------
bool CSG_Parameter_Int::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		Stream.Printf(SG_T("%d\n"), m_Value);
	}
	else
	{
		SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%d"), &m_Value);
		Set_Value(m_Value);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Double							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Double::CSG_Parameter_Double(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Value(pOwner, Constraint)
{
	m_Value		= 0.0;
}

CSG_Parameter_Double::~CSG_Parameter_Double(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Double::Set_Value(int Value)
{
	return( Set_Value((double)Value) );
}

bool CSG_Parameter_Double::Set_Value(double Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( Set_Value(m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( Set_Value(m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Double::Set_Value(void *Value)
{
	double	val;

	if( Value )
	{
		m_String.Printf((SG_Char *)Value);

		if( m_String.asDouble(val) )
		{
			return( Set_Value(val) );
		}
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Double::asString(void)
{
	m_String.Printf(SG_T("%lf"), m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Double::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Value::On_Assign(pSource);

	Set_Value(((CSG_Parameter_Value *)pSource)->asDouble());
}

//---------------------------------------------------------
bool CSG_Parameter_Double::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		Stream.Printf(SG_T("%lf\n"), m_Value);
	}
	else
	{
		SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf"), &m_Value);
		Set_Value(m_Value);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Degree							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Degree::CSG_Parameter_Degree(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Double(pOwner, Constraint)
{}

CSG_Parameter_Degree::~CSG_Parameter_Degree(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Degree::Set_Value(void *Value)
{
	return( CSG_Parameter_Double::Set_Value(SG_Degree_To_Double((const SG_Char *)Value)) );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Degree::asString(void)
{
	m_String	= SG_Double_To_Degree(asDouble());

	return( m_String );
}


///////////////////////////////////////////////////////////
//														 //
//						Range							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Range::CSG_Parameter_Range(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	pRange	= new CSG_Parameters;

	if( (m_Constraint & PARAMETER_INFORMATION) != 0 )
	{
		pLo		= pRange->Add_Info_Value(m_pOwner, SG_T("MIN"), SG_T("Minimum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		pHi		= pRange->Add_Info_Value(m_pOwner, SG_T("MAX"), SG_T("Maximum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
	else
	{
		pLo		= pRange->Add_Value		(m_pOwner, SG_T("MIN"), SG_T("Minimum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		pHi		= pRange->Add_Value		(m_pOwner, SG_T("MAX"), SG_T("Maximum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
}

CSG_Parameter_Range::~CSG_Parameter_Range(void)
{
	delete(pRange);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Range::asString(void)
{
	m_String.Printf(SG_T("[%lf] - [%lf]"),
		Get_LoParm()->asDouble(),
		Get_HiParm()->asDouble()
	);

	return( m_String );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_Range(double loVal, double hiVal)
{
	bool	bResult;

	if( loVal > hiVal )
	{
		bResult	 = pLo->Set_Value(hiVal);
		bResult	|= pHi->Set_Value(loVal);
	}
	else
	{
		bResult	 = pLo->Set_Value(loVal);
		bResult	|= pHi->Set_Value(hiVal);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_LoVal(double newValue)
{
	return( pLo->Set_Value(newValue) );
}

double CSG_Parameter_Range::Get_LoVal(void)
{
	return( pLo->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_HiVal(double newValue)
{
	return( pHi->Set_Value(newValue) );
}

double CSG_Parameter_Range::Get_HiVal(void)
{
	return( pHi->asDouble() );
}

//---------------------------------------------------------
void CSG_Parameter_Range::On_Assign(CSG_Parameter_Data *pSource)
{
	pLo->Assign(((CSG_Parameter_Range *)pSource)->pLo);
	pHi->Assign(((CSG_Parameter_Range *)pSource)->pHi);
}

//---------------------------------------------------------
bool CSG_Parameter_Range::On_Serialize(CSG_File &Stream, bool bSave)
{
	double	loVal, hiVal;

	if( bSave )
	{
		Stream.Printf(SG_T("%lf %lf\n"), Get_LoVal(), Get_HiVal());

		return( true );
	}
	else
	{
		SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf %lf"), &loVal, &hiVal);

		return( Set_Range(loVal, hiVal) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Choice::CSG_Parameter_Choice(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{
	Items		= NULL;
	nItems		= 0;
}

CSG_Parameter_Choice::~CSG_Parameter_Choice(void)
{
	Del_Items();
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::asString(void)
{
	if( m_Value >= 0 && m_Value < nItems )
	{
		return( Items[m_Value]->c_str() );
	}

	return( LNG("[VAL] [no choice available]") );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::Del_Items(void)
{
	int		i;

	if( nItems > 0 )
	{
		for(i=0; i<nItems; i++)
		{
			delete(Items[i]);
		}

		SG_Free(Items);

		nItems		= 0;
		Items		= NULL;
	}

	m_Value		= 0;
}

//---------------------------------------------------------
void CSG_Parameter_Choice::Set_Items(const SG_Char *String)
{
	const SG_Char	*s;
	int			n;
	CSG_String	sItem;

	Del_Items();

	//-----------------------------------------------------
	s	= String;

	while( s && *s > 0 )
	{
		sItem	= s;
		sItem	= sItem.BeforeFirst('|');
		n		= sItem.Length();

		Items			= (CSG_String **)SG_Realloc(Items, (nItems + 1) * sizeof(CSG_String *));
		Items[nItems]	= new CSG_String(sItem);
		nItems++;

		s	+= n + 1;
	}

	//-----------------------------------------------------
	if( nItems == 0 )
	{
		nItems		= 1;
		Items		= (CSG_String **)SG_Malloc(nItems * sizeof(CSG_String *));
		Items[0]	= new CSG_String(LNG("[VAL] [not set]"));
	}

	Set_Minimum(0, true);
	Set_Maximum(nItems - 1, true);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::Get_Item(int Index)
{
	if( Index >= 0 && Index < nItems )
	{
		return( Items[Index]->c_str() );
	}

	return( NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::On_Assign(CSG_Parameter_Data *pSource)
{
	int		i;

	Del_Items();

	if( ((CSG_Parameter_Choice *)pSource)->nItems > 0 )
	{
		nItems		= ((CSG_Parameter_Choice *)pSource)->nItems;
		Items		= (CSG_String **)SG_Malloc(nItems * sizeof(CSG_String *));

		for(i=0; i<nItems; i++)
		{
			Items[i]	= new CSG_String(((CSG_Parameter_Choice *)pSource)->Items[i]->c_str());
		}
	}

	CSG_Parameter_Int::On_Assign(pSource);
}


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_String::CSG_Parameter_String(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	bPassword	= false;
}

CSG_Parameter_String::~CSG_Parameter_String(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_String::is_Valid(void)
{
	return( m_String.Length() > 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_String::is_Password(void)
{
	return( bPassword );
}

void CSG_Parameter_String::Set_Password(bool bOn)
{
	bPassword	= bOn;
}

//---------------------------------------------------------
bool CSG_Parameter_String::Set_Value(void *Value)
{
	if( Value )
	{
		if( m_String.Cmp((SG_Char *)Value) )
		{
			m_String.Printf((SG_Char *)Value);

			return( true );
		}
	}
	else if( m_String.Length() > 0 )
	{
		m_String.Clear();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_String::asString(void)
{
	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_String::On_Assign(CSG_Parameter_Data *pSource)
{
	m_String.Printf(((CSG_Parameter_String *)pSource)->m_String.c_str());

	bPassword	= ((CSG_Parameter_String *)pSource)->bPassword;
}

//---------------------------------------------------------
bool CSG_Parameter_String::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		if( is_Valid() )
		{
			Stream.Printf(SG_T("%s\n"), m_String.c_str());
		}
		else
		{
			Stream.Printf(SG_T("\n"));
		}
	}
	else
	{
		return( Stream.Read_Line(m_String) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Text::CSG_Parameter_Text(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_String(pOwner, Constraint)
{}

CSG_Parameter_Text::~CSG_Parameter_Text(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Text::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		if( is_Valid() )
		{
			Stream.Printf(SG_T("%s\n"), m_String.c_str());
		}

		Stream.Printf(SG_T("%s\n"), ENTRY_TEXT_END);
	}
	else
	{
		CSG_String	sLine;

		m_String.Clear();

		while( Stream.Read_Line(sLine) && sLine.Cmp(ENTRY_TEXT_END) )
		{
			if( m_String.Length() > 0 )
			{
				m_String.Append(SG_T("\n"));
			}

			m_String.Append(sLine);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						FilePath						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_File_Name::CSG_Parameter_File_Name(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_String(pOwner, Constraint)
{
	m_Filter.Printf(SG_T("%s|*.*"), LNG("All Files"));

	m_bSave			= false;
	m_bMultiple		= false;
	m_bDirectory	= false;
}

CSG_Parameter_File_Name::~CSG_Parameter_File_Name(void)
{
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Filter(const SG_Char *Filter)
{
	if( Filter )
	{
		m_Filter	= Filter;
	}
	else
	{
		m_Filter.Printf(SG_T("%s|*.*"), LNG("All Files"));
	}
}

const SG_Char *  CSG_Parameter_File_Name::Get_Filter(void)
{
	return( m_Filter.c_str() );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Flag_Save(bool bFlag)
{
	m_bSave			= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Multiple(bool bFlag)
{
	m_bMultiple		= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Directory(bool bFlag)
{
	m_bDirectory	= bFlag;
}

//---------------------------------------------------------
bool CSG_Parameter_File_Name::Get_FilePaths(CSG_Strings &FilePaths)
{
	FilePaths.Clear();

	if( m_String.Length() > 0 )
	{
		if( m_String[0] != '\"' )
		{
			FilePaths.Add(m_String);
		}
		else
		{
			CSG_String	s(m_String), sTmp;

			while( s.Length() > 2 )
			{
				sTmp	= s.AfterFirst('\"');
				s		= sTmp;

				FilePaths.Add(s.BeforeFirst('\"'));
			}
		}
	}

	return( FilePaths.Get_Count() > 0 );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_String::On_Assign(pSource);

	Set_Filter(((CSG_Parameter_File_Name *)pSource)->m_Filter.c_str());

	m_bSave			= ((CSG_Parameter_File_Name *)pSource)->m_bSave;
	m_bMultiple		= ((CSG_Parameter_File_Name *)pSource)->m_bMultiple;
	m_bDirectory	= ((CSG_Parameter_File_Name *)pSource)->m_bDirectory;
}


///////////////////////////////////////////////////////////
//														 //
//						Font							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/font.h>

//---------------------------------------------------------
CSG_Parameter_Font::CSG_Parameter_Font(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pFont		= new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
	m_Color		= 0;
}

CSG_Parameter_Font::~CSG_Parameter_Font(void)
{
	delete(m_pFont);
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Set_Value(int Value)
{
	if( m_Color != Value )
	{
		m_Color		= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Font::Set_Value(void *Value)
{
	if( Value )
	{
		*m_pFont	= *((class wxFont *)Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Font::asString(void)
{
	m_String.Printf(SG_T("%s, %dpt"), m_pFont->GetFaceName().c_str(), m_pFont->GetPointSize());

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Font::On_Assign(CSG_Parameter_Data *pSource)
{
	Set_Value(pSource->asPointer());
	Set_Value(pSource->asInt());
}

//---------------------------------------------------------
bool CSG_Parameter_Font::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
//		GetNativeFontInfoDesc
//		fwrite(&logFont	, 1, sizeof(API_LOGFONT), Stream);

		Stream.Write(&m_Color	, sizeof(m_Color));

		Stream.Printf(SG_T("\n"));
	}
	else
	{
//		SetNativeFontInfo
//		fread(&logFont	, 1, sizeof(API_LOGFONT), Stream);

		Stream.Read(&m_Color	, sizeof(m_Color));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Color							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Color::CSG_Parameter_Color(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{}

CSG_Parameter_Color::~CSG_Parameter_Color(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Colors::CSG_Parameter_Colors(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

CSG_Parameter_Colors::~CSG_Parameter_Colors(void)
{}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Colors::asString(void)
{
	m_String.Printf(SG_T("%d %s"), m_Colors.Get_Count(), LNG("colors"));

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Colors::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Colors.Assign(&((CSG_Parameter_Colors *)pSource)->m_Colors);
}

//---------------------------------------------------------
bool CSG_Parameter_Colors::On_Serialize(CSG_File &Stream, bool bSave)
{
	return( m_Colors.Serialize(Stream, bSave, false) );
}


///////////////////////////////////////////////////////////
//														 //
//						FixedTable						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Fixed_Table::CSG_Parameter_Fixed_Table(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Table.Set_Name(LNG("Table"));
}

CSG_Parameter_Fixed_Table::~CSG_Parameter_Fixed_Table(void)
{}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Fixed_Table::asString(void)
{
	m_String.Printf(SG_T("%s (%s: %d, %s: %d)"), m_Table.Get_Name(), LNG("columns"), m_Table.Get_Field_Count(), LNG("rows"), m_Table.Get_Record_Count());

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Fixed_Table::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Table.Assign(&((CSG_Parameter_Fixed_Table *)pSource)->m_Table);
}

//---------------------------------------------------------
bool CSG_Parameter_Fixed_Table::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		return( m_Table.Serialize(Stream, bSave) );
	}
	else
	{
		CSG_Table	t;

		if( t.Serialize(Stream, bSave) )
		{
			m_Table.Assign_Values(&t);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid_System::CSG_Parameter_Grid_System(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

CSG_Parameter_Grid_System::~CSG_Parameter_Grid_System(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::Set_Value(void *Value)
{
	int						i, j;
	CSG_Grid					*pGrid;
	CSG_Grid_System			Invalid;
	CSG_Parameters				*pParameters;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	if( Value == NULL )
	{
		Value	= &Invalid;
	}

	//-----------------------------------------------------
	if( !m_System.is_Equal(*((CSG_Grid_System *)Value)) )
	{
		m_System.Assign(*((CSG_Grid_System *)Value));

		//-------------------------------------------------
		pParameters	= m_pOwner->Get_Owner();

		for(i=0; i<pParameters->Get_Count(); i++)
		{
			if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner )
			{
				switch( pParameters->Get_Parameter(i)->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Grid:
					pGrid	= pParameters->Get_Parameter(i)->asGrid();

					if(	!SG_UI_DataObject_Check(pGrid, DATAOBJECT_TYPE_Grid) || (pGrid != DATAOBJECT_NOTSET && pGrid != DATAOBJECT_CREATE && !m_System.is_Equal(pGrid->Get_System())) )
					{
						pParameters->Get_Parameter(i)->Set_Value(DATAOBJECT_NOTSET);
					}
					break;

				case PARAMETER_TYPE_Grid_List:
					pGrids	= (CSG_Parameter_Grid_List *)pParameters->Get_Parameter(i)->Get_Data();

					for(j=pGrids->Get_Count()-1; j>=0; j--)
					{
						if( !SG_UI_DataObject_Check(pGrids->asGrid(j), DATAOBJECT_TYPE_Grid) || m_System.is_Equal(pGrids->asGrid(j)->Get_System()) == false )
						{
							pGrids->Del_Item(j);
						}
					}
					break;
				}
			}
		}

		//-------------------------------------------------
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Grid_System::asString(void)
{
	return( m_System.Get_Name() );
}

//---------------------------------------------------------
void CSG_Parameter_Grid_System::On_Assign(CSG_Parameter_Data *pSource)
{
	m_System	= ((CSG_Parameter_Grid_System *)pSource)->m_System;
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::On_Serialize(CSG_File &Stream, bool bSave)
{
	double		Cellsize;
	TSG_Rect	Extent;

	if( bSave )
	{
		Cellsize	= m_System.Get_Cellsize();
		Extent		= m_System.Get_Extent().m_rect;

		Stream.Write(&Cellsize, sizeof(Cellsize));
		Stream.Write(&Extent  , sizeof(Extent));
		Stream.Printf(SG_T("\n"));
	}
	else
	{
		Stream.Read(&Cellsize, sizeof(Cellsize));
		Stream.Read(&Extent  , sizeof(Extent));

		m_System.Assign(Cellsize, CSG_Rect(Extent));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Table Field							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table_Field::CSG_Parameter_Table_Field(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{
}

CSG_Parameter_Table_Field::~CSG_Parameter_Table_Field(void)
{
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Table_Field::asString(void)
{
	CSG_Table	*pTable;

	if( (pTable = Get_Table()) != NULL )
	{
		if( m_Value >= 0 && m_Value < pTable->Get_Field_Count() )
		{
			return( pTable->Get_Field_Name(m_Value) );
		}

		return( LNG("[VAL] [not set]") );
	}

	return( LNG("[VAL] [no fields]") );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Set_Value(int Value)
{
	bool		bChanged;
	CSG_Table		*pTable;

	bChanged	= false;

	if( (pTable = Get_Table()) != NULL )
	{
		if( !m_pOwner->is_Optional() )
		{
			if( Value < 0 )
			{
				Value	= 0;
			}
			else if( Value >= pTable->Get_Field_Count() )
			{
				Value	= pTable->Get_Field_Count() - 1;
			}
		}
		else if( Value < 0 || Value >= pTable->Get_Field_Count() )
		{
			Value	= -1;
		}
	}
	else
	{
		Value	= -1;
	}

	if( m_Value != Value )
	{
		m_Value		= Value;
		bChanged	= true;
	}

	return( bChanged );
}

//---------------------------------------------------------
CSG_Table * CSG_Parameter_Table_Field::Get_Table(void)
{
	CSG_Table		*pTable;
	CSG_Parameter	*pParent;

	pTable		= NULL;

	if( (pParent = m_pOwner->Get_Parent()) != NULL )
	{
		switch( m_pOwner->Get_Parent()->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Table:
			pTable	= pParent->asTable();
			break;

		case PARAMETER_TYPE_Shapes:
			pTable	= pParent->asShapes() ? &pParent->asShapes()->Get_Table() : NULL;
			break;

		case PARAMETER_TYPE_TIN:
			pTable	= pParent->asTIN()    ? &pParent->asTIN()   ->Get_Table() : NULL;
			break;
		}
	}

	return( pTable && pTable->Get_Field_Count() > 0 ? pTable : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						DataObject						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object::CSG_Parameter_Data_Object(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pDataObject	= NULL;
}

CSG_Parameter_Data_Object::~CSG_Parameter_Data_Object(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::is_Valid(void)
{
	return(	m_pOwner->is_Optional() || (m_pDataObject && m_pDataObject->is_Valid()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::Set_Value(void *Value)
{
	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Data_Object::asString(void)
{
	if( m_pDataObject == DATAOBJECT_NOTSET )
	{
		m_String	= m_pOwner->is_Output() && !m_pOwner->is_Optional()
					? LNG("[VAL] [create]")
					: LNG("[VAL] [not set]");
	}
	else if( m_pDataObject == DATAOBJECT_CREATE )
	{
		m_String	= LNG("[VAL] [create]");
	}
	else
	{
		m_String	= m_pDataObject->Get_Name();
	}

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Data_Object::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pDataObject	= ((CSG_Parameter_Data_Object *)pSource)->m_pDataObject;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		if(	m_pDataObject == DATAOBJECT_CREATE )
		{
			Stream.Printf(SG_T("%s\n"), ENTRY_DATAOBJECT_CREATE);
		}
		else if( m_pDataObject != DATAOBJECT_NOTSET && m_pDataObject->Get_File_Name() )
		{
			Stream.Printf(SG_T("%s\n"), m_pDataObject->Get_File_Name());
		}
		else
		{
			Stream.Printf(SG_T("\n"));
		}
	}
	else
	{
		CSG_String	sLine;

		if( Stream.Read_Line(sLine) )
		{
			if( !sLine.Cmp(ENTRY_DATAOBJECT_CREATE) )
			{
				Set_Value(DATAOBJECT_CREATE);
			}
			else
			{
				Set_Value(SG_UI_DataObject_Find(sLine, -1));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					DataObject_Output					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object_Output::CSG_Parameter_Data_Object_Output(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= DATAOBJECT_TYPE_Undefined;
}

CSG_Parameter_Data_Object_Output::~CSG_Parameter_Data_Object_Output(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_Value(void *Value)
{
	CSG_Data_Object	*pDataObject	= (CSG_Data_Object *)Value;

	if( m_pDataObject != pDataObject && (!pDataObject || (pDataObject && pDataObject->Get_ObjectType() == m_Type)) )
	{
		m_pDataObject	= pDataObject;

		SG_UI_DataObject_Add(m_pDataObject, false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_DataObject_Type(TSG_Data_Object_Type Type)
{
	if( m_Type == DATAOBJECT_TYPE_Undefined )	// m_Type should not be changed after set once!!!...
	{
		switch( Type )
		{
		default:
			break;

		case DATAOBJECT_TYPE_Grid:
		case DATAOBJECT_TYPE_Table:
		case DATAOBJECT_TYPE_Shapes:
		case DATAOBJECT_TYPE_TIN:
			m_Type	= Type;
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid::CSG_Parameter_Grid(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= GRID_TYPE_Undefined;
}

CSG_Parameter_Grid::~CSG_Parameter_Grid(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Grid::Set_Value(void *Value)
{
	CSG_Grid_System	*pSystem;

	if( Value == DATAOBJECT_CREATE && !m_pOwner->is_Optional() )
	{
		Value	= DATAOBJECT_NOTSET;
	}

	if( m_pDataObject != Value )
	{
		pSystem	= Get_System();

		if(	Value == DATAOBJECT_NOTSET || Value == DATAOBJECT_CREATE
		||	pSystem == NULL || pSystem->is_Equal(((CSG_Grid *)Value)->Get_System()) )
		{
			m_pDataObject	= (CSG_Data_Object *)Value;

			return( true );
		}
		else if( !m_pOwner->Get_Owner()->is_Managed() && pSystem != NULL )
		{
			pSystem->Assign(((CSG_Grid *)Value)->Get_System());
			m_pDataObject	= (CSG_Data_Object *)Value;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid::Get_System(void)
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Grid::Set_Preferred_Type(TSG_Grid_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Grid::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Grid *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						Table							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table::CSG_Parameter_Table(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

CSG_Parameter_Table::~CSG_Parameter_Table(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Table::Set_Value(void *Value)
{
	int			i;
	CSG_Parameters	*pParameters;

	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;

		pParameters		= m_pOwner->Get_Owner();

		for(i=0; i<pParameters->Get_Count(); i++)
		{
			if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
			&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
			{
				pParameters->Get_Parameter(i)->Set_Value(0);
			}				
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Shapes							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Shapes::CSG_Parameter_Shapes(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

CSG_Parameter_Shapes::~CSG_Parameter_Shapes(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_Shapes::Set_Value(void *Value)
{
	int			i;
	CSG_Parameters	*pParameters;

	if(	Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE
	&&	m_Type != SHAPE_TYPE_Undefined && m_Type != ((CSG_Shapes *)Value)->Get_Type() )
	{
		return( false );
	}

	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;

		pParameters		= m_pOwner->Get_Owner();

		for(i=0; i<pParameters->Get_Count(); i++)
		{
			if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
			&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
			{
				pParameters->Get_Parameter(i)->Set_Value(0);
			}				
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Shapes *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_TIN::CSG_Parameter_TIN(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

CSG_Parameter_TIN::~CSG_Parameter_TIN(void)
{}

//---------------------------------------------------------
bool CSG_Parameter_TIN::Set_Value(void *Value)
{
	int			i;
	CSG_Parameters	*pParameters;

	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;

		pParameters		= m_pOwner->Get_Owner();

		for(i=0; i<pParameters->Get_Count(); i++)
		{
			if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
			&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
			{
				pParameters->Get_Parameter(i)->Set_Value(0);
			}				
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						List							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_List::CSG_Parameter_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Objects	= NULL;
	m_nObjects	= 0;
}

CSG_Parameter_List::~CSG_Parameter_List(void)
{
	Del_Items();
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_List::asString(void)
{
	if( Get_Count() > 0 )
	{
		m_String.Printf(SG_T("%d %s ("), Get_Count(), Get_Count() == 1 ? LNG("[VAL] object") : LNG("[VAL] objects"));

		for(int i=0; i<Get_Count(); i++)
		{
			if( i > 0 )
			{
				m_String.Append(SG_T(", "));
			}

			m_String.Append(asDataObject(i)->Get_Name());
		}

		m_String.Append(SG_T("))"));
	}
	else
	{
		m_String.Printf(LNG("[VAL] No objects"));
	}

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject )
	{
		m_Objects	= (CSG_Data_Object **)SG_Realloc(m_Objects, (m_nObjects + 1) * sizeof(CSG_Data_Object *));
		m_Objects[m_nObjects++]	= pObject;
	}
}

//---------------------------------------------------------
int CSG_Parameter_List::Del_Item(int iObject)
{
	int		i;

	if( iObject >= 0 && iObject < m_nObjects )
	{
		m_nObjects--;

		for(i=iObject; i<m_nObjects; i++)
		{
			m_Objects[i]	= m_Objects[i + 1];
		}

		m_Objects	= (CSG_Data_Object **)SG_Realloc(m_Objects, m_nObjects * sizeof(CSG_Data_Object *));
	}

	return( m_nObjects );
}

int CSG_Parameter_List::Del_Item(CSG_Data_Object *pObject)
{
	int		i;

	for(i=0; i<m_nObjects; i++)
	{
		if( pObject == m_Objects[i] )
		{
			return( Del_Item(i) );
		}
	}

	return( 0 );
}

//---------------------------------------------------------
void CSG_Parameter_List::Del_Items(void)
{
	if( m_nObjects > 0 )
	{
		SG_Free(m_Objects);
		m_Objects	= NULL;
		m_nObjects	= 0;
	}
}

//---------------------------------------------------------
void CSG_Parameter_List::On_Assign(CSG_Parameter_Data *pSource)
{
	Del_Items();

	for(int i=0; i<((CSG_Parameter_List *)pSource)->Get_Count(); i++)
	{
		Add_Item(((CSG_Parameter_List *)pSource)->asDataObject(i));
	}
}

//---------------------------------------------------------
bool CSG_Parameter_List::On_Serialize(CSG_File &Stream, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( asDataObject(i)->Get_File_Name() )
			{
				Stream.Printf(SG_T("%s\n"), asDataObject(i)->Get_File_Name());
			}
		}

		Stream.Printf(SG_T("%s\n"), ENTRY_DATAOBJECTLIST_END);
	}
	else
	{
		CSG_String	sLine;
		CSG_Data_Object	*pObject;

		while( Stream.Read_Line(sLine) && sLine.Cmp(ENTRY_DATAOBJECTLIST_END) )
		{
			if( (pObject = SG_UI_DataObject_Find(sLine, -1)) != NULL )
			{
				Add_Item(pObject);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Grid_List::CSG_Parameter_Grid_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{
}

CSG_Parameter_Grid_List::~CSG_Parameter_Grid_List(void)
{}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid_List::Get_System(void)
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						Table_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Table_List::CSG_Parameter_Table_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}

CSG_Parameter_Table_List::~CSG_Parameter_Table_List(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Shapes_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Shapes_List::CSG_Parameter_Shapes_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

CSG_Parameter_Shapes_List::~CSG_Parameter_Shapes_List(void)
{}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_List::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Shapes_List *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_TIN_List::CSG_Parameter_TIN_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}

CSG_Parameter_TIN_List::~CSG_Parameter_TIN_List(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Parameters::CSG_Parameter_Parameters(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pParameters	= new CSG_Parameters(pOwner->Get_Owner()->Get_Owner(), pOwner->Get_Name(), pOwner->Get_Description());
}

CSG_Parameter_Parameters::~CSG_Parameter_Parameters(void)
{
	delete(m_pParameters);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Parameters::asString(void)
{
	m_String.Printf(SG_T("%d %s"), m_pParameters->Get_Count(), LNG("parameters"));

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Parameters::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pParameters->Assign(((CSG_Parameter_Parameters *)pSource)->m_pParameters);
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::On_Serialize(CSG_File &Stream, bool bSave)
{
	return( m_pParameters->Serialize(Stream, bSave) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
