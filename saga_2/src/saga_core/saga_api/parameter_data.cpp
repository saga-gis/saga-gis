
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
#define ENTRY_TEXT_END				"[TEXT_ENTRY_END]"
#define ENTRY_DATAOBJECT_CREATE		"[ENTRY_DATAOBJECT_CREATE]"
#define ENTRY_DATAOBJECTLIST_END	"[ENTRY_DATAOBJECTLIST_END]"


///////////////////////////////////////////////////////////
//														 //
//						Base Class						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Data::CParameter_Data(CParameter *pOwner, long Constraint)
{
	m_pOwner		= pOwner;
	m_Constraint	= Constraint;
}

//---------------------------------------------------------
CParameter_Data::~CParameter_Data(void)
{
}

//---------------------------------------------------------
const char * CParameter_Data::Get_Type_Name(void)
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
	case PARAMETER_TYPE_TIN:				return( LNG("[PRM] T.I.N.") );

	case PARAMETER_TYPE_Grid_List:			return( LNG("[PRM] Grid list") );
	case PARAMETER_TYPE_Table_List:			return( LNG("[PRM] Table list") );
	case PARAMETER_TYPE_Shapes_List:		return( LNG("[PRM] Shapes list") );
	case PARAMETER_TYPE_TIN_List:			return( LNG("[PRM] T.I.N. list") );

	case PARAMETER_TYPE_Parameters:			return( LNG("[PRM] Parameters") );
	}
}

//---------------------------------------------------------
bool CParameter_Data::Set_Value(int Value)
{
	return( false );
}

bool CParameter_Data::Set_Value(double Value)
{
	return( false );
}

bool CParameter_Data::Set_Value(void *Value)
{
	return( false );
}

//---------------------------------------------------------
int CParameter_Data::asInt(void)
{
	return( 0 );
}

double CParameter_Data::asDouble(void)
{
	return( 0.0 );
}

void * CParameter_Data::asPointer(void)
{
	return( NULL );
}

const char * CParameter_Data::asString(void)
{
	return( NULL );
}

//---------------------------------------------------------
bool CParameter_Data::Assign(CParameter_Data *pSource)
{
	if( pSource && Get_Type() == pSource->Get_Type() )
	{
		On_Assign(pSource);

		return( true );
	}

	return( false );
}

void CParameter_Data::On_Assign(CParameter_Data *pSource)
{}

//---------------------------------------------------------
bool CParameter_Data::Serialize(FILE *Stream, bool bSave)
{
	return( Stream ? On_Serialize(Stream, bSave) : false );
}

bool CParameter_Data::On_Serialize(FILE *Stream, bool bSave)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Node							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Node::CParameter_Node(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{}

CParameter_Node::~CParameter_Node(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Bool							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Bool::CParameter_Bool(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	m_Value		= false;
}

CParameter_Bool::~CParameter_Bool(void)
{}

//---------------------------------------------------------
bool CParameter_Bool::Set_Value(int Value)
{
	bool	bValue = Value != 0 ? true : false;

	if( m_Value != bValue )
	{
		m_Value		= bValue;

		return( true );
	}

	return( false );
}

bool CParameter_Bool::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

//---------------------------------------------------------
const char * CParameter_Bool::asString(void)
{
	return( m_Value ? LNG("[VAL] yes") : LNG("[VAL] no") );
}

//---------------------------------------------------------
void CParameter_Bool::On_Assign(CParameter_Data *pSource)
{
	m_Value		= ((CParameter_Bool *)pSource)->m_Value;
}

//---------------------------------------------------------
bool CParameter_Bool::On_Serialize(FILE *Stream, bool bSave)
{
    int		i;
    
	if( bSave )
	{
	    i		= m_Value;
		fprintf(Stream, "%d\n",  i);
	}
	else
	{
		fscanf (Stream, "%d", &i);
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
CParameter_Value::CParameter_Value(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	m_Minimum	= 0.0;
	m_bMinimum	= false;

	m_Maximum	= 0.0;
	m_bMaximum	= false;
}

CParameter_Value::~CParameter_Value(void)
{}

//---------------------------------------------------------
bool CParameter_Value::Set_Range(double Minimum, double Maximum)
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
void CParameter_Value::Set_Minimum(double Minimum, bool bOn)
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

void CParameter_Value::Set_Maximum(double Maximum, bool bOn)
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
void CParameter_Value::On_Assign(CParameter_Data *pSource)
{
	m_Minimum	= ((CParameter_Value *)pSource)->m_Minimum;
	m_bMinimum	= ((CParameter_Value *)pSource)->m_bMinimum;

	m_Maximum	= ((CParameter_Value *)pSource)->m_Maximum;
	m_bMaximum	= ((CParameter_Value *)pSource)->m_bMaximum;
}


///////////////////////////////////////////////////////////
//														 //
//						Int								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Int::CParameter_Int(CParameter *pOwner, long Constraint)
	: CParameter_Value(pOwner, Constraint)
{
	m_Value		= 0;
}

CParameter_Int::~CParameter_Int(void)
{}

//---------------------------------------------------------
bool CParameter_Int::Set_Value(int Value)
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

bool CParameter_Int::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

bool CParameter_Int::Set_Value(void *Value)
{
	int		val;

	if( Value )
	{
		m_String.Printf((char *)Value);

		if( m_String.asInt(val) )
		{
			return( Set_Value(val) );
		}
	}

	return( false );
}

//---------------------------------------------------------
const char * CParameter_Int::asString(void)
{
	m_String.Printf("%d", m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CParameter_Int::On_Assign(CParameter_Data *pSource)
{
	CParameter_Value::On_Assign(pSource);

	Set_Value(((CParameter_Value *)pSource)->asInt());
}

//---------------------------------------------------------
bool CParameter_Int::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		fprintf(Stream, "%d\n", m_Value);
	}
	else
	{
		fscanf (Stream, "%d", &m_Value);
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
CParameter_Double::CParameter_Double(CParameter *pOwner, long Constraint)
	: CParameter_Value(pOwner, Constraint)
{
	m_Value		= 0.0;
}

CParameter_Double::~CParameter_Double(void)
{}

//---------------------------------------------------------
bool CParameter_Double::Set_Value(int Value)
{
	return( Set_Value((double)Value) );
}

bool CParameter_Double::Set_Value(double Value)
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

bool CParameter_Double::Set_Value(void *Value)
{
	double	val;

	if( Value )
	{
		m_String.Printf((char *)Value);

		if( m_String.asDouble(val) )
		{
			return( Set_Value(val) );
		}
	}

	return( false );
}

//---------------------------------------------------------
const char * CParameter_Double::asString(void)
{
	m_String.Printf("%lf", m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CParameter_Double::On_Assign(CParameter_Data *pSource)
{
	CParameter_Value::On_Assign(pSource);

	Set_Value(((CParameter_Value *)pSource)->asDouble());
}

//---------------------------------------------------------
bool CParameter_Double::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		fprintf(Stream, "%lf\n", m_Value);
	}
	else
	{
		fscanf (Stream, "%lf", &m_Value);
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
CParameter_Degree::CParameter_Degree(CParameter *pOwner, long Constraint)
	: CParameter_Double(pOwner, Constraint)
{}

CParameter_Degree::~CParameter_Degree(void)
{}

//---------------------------------------------------------
bool CParameter_Degree::Set_Value(void *Value)
{
	return( CParameter_Double::Set_Value(SG_Degree_To_Double((const char *)Value)) );
}

//---------------------------------------------------------
const char * CParameter_Degree::asString(void)
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
CParameter_Range::CParameter_Range(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	pRange	= new CParameters;

	if( (m_Constraint & PARAMETER_INFORMATION) != 0 )
	{
		pLo		= pRange->Add_Info_Value(m_pOwner, "MIN", "Minimum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		pHi		= pRange->Add_Info_Value(m_pOwner, "MAX", "Maximum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
	else
	{
		pLo		= pRange->Add_Value		(m_pOwner, "MIN", "Minimum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		pHi		= pRange->Add_Value		(m_pOwner, "MAX", "Maximum", m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
}

CParameter_Range::~CParameter_Range(void)
{
	delete(pRange);
}

//---------------------------------------------------------
const char * CParameter_Range::asString(void)
{
	m_String.Printf("[%lf] - [%lf]",
		Get_LoParm()->asDouble(),
		Get_HiParm()->asDouble()
	);

	return( m_String );
}

//---------------------------------------------------------
bool CParameter_Range::Set_Range(double loVal, double hiVal)
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
bool CParameter_Range::Set_LoVal(double newValue)
{
	return( pLo->Set_Value(newValue) );
}

double CParameter_Range::Get_LoVal(void)
{
	return( pLo->asDouble() );
}

//---------------------------------------------------------
bool CParameter_Range::Set_HiVal(double newValue)
{
	return( pHi->Set_Value(newValue) );
}

double CParameter_Range::Get_HiVal(void)
{
	return( pHi->asDouble() );
}

//---------------------------------------------------------
void CParameter_Range::On_Assign(CParameter_Data *pSource)
{
	pLo->Assign(((CParameter_Range *)pSource)->pLo);
	pHi->Assign(((CParameter_Range *)pSource)->pHi);
}

//---------------------------------------------------------
bool CParameter_Range::On_Serialize(FILE *Stream, bool bSave)
{
	double	loVal, hiVal;

	if( bSave )
	{
		fprintf(Stream, "%lf %lf\n", Get_LoVal(), Get_HiVal());

		return( true );
	}
	else
	{
		fscanf (Stream, "%lf %lf", &loVal, &hiVal);

		return( Set_Range(loVal, hiVal) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Choice::CParameter_Choice(CParameter *pOwner, long Constraint)
	: CParameter_Int(pOwner, Constraint)
{
	Items		= NULL;
	nItems		= 0;
}

CParameter_Choice::~CParameter_Choice(void)
{
	Del_Items();
}

//---------------------------------------------------------
const char * CParameter_Choice::asString(void)
{
	if( m_Value >= 0 && m_Value < nItems )
	{
		return( Items[m_Value]->c_str() );
	}

	return( LNG("[VAL] [no choice available]") );
}

//---------------------------------------------------------
void CParameter_Choice::Del_Items(void)
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
void CParameter_Choice::Set_Items(const char *String)
{
	const char	*s;
	int			n;
	CSG_String	sItem;

	Del_Items();

	//-----------------------------------------------------
	s	= String;

	while( s && strlen(s) > 0 )
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
const char * CParameter_Choice::Get_Item(int Index)
{
	if( Index >= 0 && Index < nItems )
	{
		return( Items[Index]->c_str() );
	}

	return( NULL );
}

//---------------------------------------------------------
void CParameter_Choice::On_Assign(CParameter_Data *pSource)
{
	int		i;

	Del_Items();

	if( ((CParameter_Choice *)pSource)->nItems > 0 )
	{
		nItems		= ((CParameter_Choice *)pSource)->nItems;
		Items		= (CSG_String **)SG_Malloc(nItems * sizeof(CSG_String *));

		for(i=0; i<nItems; i++)
		{
			Items[i]	= new CSG_String(((CParameter_Choice *)pSource)->Items[i]->c_str());
		}
	}

	CParameter_Int::On_Assign(pSource);
}


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_String::CParameter_String(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	bPassword	= false;
}

CParameter_String::~CParameter_String(void)
{}

//---------------------------------------------------------
bool CParameter_String::is_Valid(void)
{
	return( m_String.Length() > 0 );
}

//---------------------------------------------------------
void CParameter_String::Set_Password(bool bOn)
{
	bPassword	= bOn;
}

//---------------------------------------------------------
bool CParameter_String::Set_Value(void *Value)
{
	if( Value )
	{
		if( m_String.Cmp((char *)Value) )
		{
			m_String.Printf((char *)Value);

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
const char * CParameter_String::asString(void)
{
	return( m_String );
}

//---------------------------------------------------------
void CParameter_String::On_Assign(CParameter_Data *pSource)
{
	m_String.Printf(((CParameter_String *)pSource)->m_String.c_str());
}

//---------------------------------------------------------
bool CParameter_String::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		if( is_Valid() )
		{
			fprintf(Stream, "%s\n", m_String.c_str());
		}
		else
		{
			fprintf(Stream, "\n");
		}
	}
	else
	{
		return( SG_Read_Line(Stream, m_String) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Text::CParameter_Text(CParameter *pOwner, long Constraint)
	: CParameter_String(pOwner, Constraint)
{}

CParameter_Text::~CParameter_Text(void)
{}

//---------------------------------------------------------
bool CParameter_Text::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		if( is_Valid() )
		{
			fprintf(Stream, "%s\n", m_String.c_str());
		}

		fprintf(Stream, "%s\n", ENTRY_TEXT_END);
	}
	else
	{
		CSG_String	sLine;

		m_String.Clear();

		while( SG_Read_Line(Stream, sLine) && sLine.Cmp(ENTRY_TEXT_END) )
		{
			if( m_String.Length() > 0 )
			{
				m_String.Append("\n");
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
CParameter_FilePath::CParameter_FilePath(CParameter *pOwner, long Constraint)
	: CParameter_String(pOwner, Constraint)
{
	m_Filter		= LNG("[FIL] All Files|*.*");

	m_bSave			= false;
	m_bMultiple		= false;
	m_bDirectory	= false;
}

CParameter_FilePath::~CParameter_FilePath(void)
{
}

//---------------------------------------------------------
void CParameter_FilePath::Set_Filter(const char *Filter)
{
	if( Filter )
	{
		m_Filter	= Filter;
	}
	else
	{
		m_Filter.Printf(LNG("[FIL] All Files|*.*"));
	}
}

const char *  CParameter_FilePath::Get_Filter(void)
{
	return( m_Filter.c_str() );
}

//---------------------------------------------------------
void CParameter_FilePath::Set_Flag_Save(bool bFlag)
{
	m_bSave			= bFlag;
}

void CParameter_FilePath::Set_Flag_Multiple(bool bFlag)
{
	m_bMultiple		= bFlag;
}

void CParameter_FilePath::Set_Flag_Directory(bool bFlag)
{
	m_bDirectory	= bFlag;
}

//---------------------------------------------------------
bool CParameter_FilePath::Get_FilePaths(CSG_Strings &FilePaths)
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
void CParameter_FilePath::On_Assign(CParameter_Data *pSource)
{
	CParameter_String::On_Assign(pSource);

	Set_Filter(((CParameter_FilePath *)pSource)->m_Filter.c_str());

	m_bSave			= ((CParameter_FilePath *)pSource)->m_bSave;
	m_bMultiple		= ((CParameter_FilePath *)pSource)->m_bMultiple;
	m_bDirectory	= ((CParameter_FilePath *)pSource)->m_bDirectory;
}


///////////////////////////////////////////////////////////
//														 //
//						Font							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/font.h>

//---------------------------------------------------------
CParameter_Font::CParameter_Font(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	m_pFont		= new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
	m_Color		= 0;
}

CParameter_Font::~CParameter_Font(void)
{
	delete(m_pFont);
}

//---------------------------------------------------------
bool CParameter_Font::Set_Value(int Value)
{
	if( m_Color != Value )
	{
		m_Color		= Value;

		return( true );
	}

	return( false );
}

bool CParameter_Font::Set_Value(void *Value)
{
	if( Value )
	{
		*m_pFont	= *((class wxFont *)Value);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const char * CParameter_Font::asString(void)
{
	m_String.Printf("%s, %dpt", m_pFont->GetFaceName().c_str(), m_pFont->GetPointSize());

	return( m_String );
}

//---------------------------------------------------------
void CParameter_Font::On_Assign(CParameter_Data *pSource)
{
	Set_Value(pSource->asPointer());
	Set_Value(pSource->asInt());
}

//---------------------------------------------------------
bool CParameter_Font::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
//		GetNativeFontInfoDesc
//		fwrite(&logFont	, 1, sizeof(API_LOGFONT), Stream);

		fwrite(&m_Color	, 1, sizeof(m_Color)		, Stream);

		fprintf(Stream, "\n");
	}
	else
	{
//		SetNativeFontInfo
//		fread(&logFont	, 1, sizeof(API_LOGFONT), Stream);

		fread(&m_Color	, 1, sizeof(m_Color)		, Stream);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Color							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Color::CParameter_Color(CParameter *pOwner, long Constraint)
	: CParameter_Int(pOwner, Constraint)
{}

CParameter_Color::~CParameter_Color(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Colors::CParameter_Colors(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{}

CParameter_Colors::~CParameter_Colors(void)
{}

//---------------------------------------------------------
const char * CParameter_Colors::asString(void)
{
	m_String.Printf("%d %s", m_Colors.Get_Count(), LNG("colors"));

	return( m_String );
}

//---------------------------------------------------------
void CParameter_Colors::On_Assign(CParameter_Data *pSource)
{
	m_Colors.Assign(&((CParameter_Colors *)pSource)->m_Colors);
}

//---------------------------------------------------------
bool CParameter_Colors::On_Serialize(FILE *Stream, bool bSave)
{
	return( m_Colors.Serialize(Stream, bSave, false) );
}


///////////////////////////////////////////////////////////
//														 //
//						FixedTable						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_FixedTable::CParameter_FixedTable(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{}

CParameter_FixedTable::~CParameter_FixedTable(void)
{}

//---------------------------------------------------------
const char * CParameter_FixedTable::asString(void)
{
	m_String.Printf(m_Table.Get_Name());

	return( m_String );
}

//---------------------------------------------------------
void CParameter_FixedTable::On_Assign(CParameter_Data *pSource)
{
	m_Table.Assign(&((CParameter_FixedTable *)pSource)->m_Table);
}

//---------------------------------------------------------
bool CParameter_FixedTable::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		return( m_Table.Serialize(Stream, bSave) );
	}
	else
	{
		CTable	t;

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
CParameter_Grid_System::CParameter_Grid_System(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{}

CParameter_Grid_System::~CParameter_Grid_System(void)
{}

//---------------------------------------------------------
bool CParameter_Grid_System::Set_Value(void *Value)
{
	int						i, j;
	CGrid					*pGrid;
	CGrid_System			Invalid;
	CParameters				*pParameters;
	CParameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	if( Value == NULL )
	{
		Value	= &Invalid;
	}

	//-----------------------------------------------------
	if( !m_System.is_Equal(*((CGrid_System *)Value)) )
	{
		m_System.Assign(*((CGrid_System *)Value));

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
					pGrids	= (CParameter_Grid_List *)pParameters->Get_Parameter(i)->Get_Data();

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
const char * CParameter_Grid_System::asString(void)
{
	return( m_System.Get_Name() );
}

//---------------------------------------------------------
void CParameter_Grid_System::On_Assign(CParameter_Data *pSource)
{
	m_System	= ((CParameter_Grid_System *)pSource)->m_System;
}

//---------------------------------------------------------
bool CParameter_Grid_System::On_Serialize(FILE *Stream, bool bSave)
{
	double		Cellsize;
	TSG_Rect	Extent;

	if( bSave )
	{
		Cellsize	= m_System.Get_Cellsize();
		Extent		= m_System.Get_Extent().m_rect;

		fwrite(&Cellsize, 1, sizeof(Cellsize), Stream);
		fwrite(&Extent  , 1, sizeof(Extent)  , Stream);
		fprintf(Stream, "\n");
	}
	else
	{
		fread (&Cellsize, 1, sizeof(Cellsize), Stream);
		fread (&Extent  , 1, sizeof(Extent)  , Stream);

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
CParameter_Table_Field::CParameter_Table_Field(CParameter *pOwner, long Constraint)
	: CParameter_Int(pOwner, Constraint)
{
}

CParameter_Table_Field::~CParameter_Table_Field(void)
{
}

//---------------------------------------------------------
const char * CParameter_Table_Field::asString(void)
{
	CTable	*pTable;

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
bool CParameter_Table_Field::Set_Value(int Value)
{
	bool		bChanged;
	CTable		*pTable;

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
CTable * CParameter_Table_Field::Get_Table(void)
{
	CTable		*pTable;
	CParameter	*pParent;

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
CParameter_DataObject::CParameter_DataObject(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	m_pDataObject	= NULL;
}

CParameter_DataObject::~CParameter_DataObject(void)
{}

//---------------------------------------------------------
bool CParameter_DataObject::is_Valid(void)
{
	return(	m_pOwner->is_Optional() || (m_pDataObject && m_pDataObject->is_Valid()) );
}

//---------------------------------------------------------
bool CParameter_DataObject::Set_Value(void *Value)
{
	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CDataObject *)Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const char * CParameter_DataObject::asString(void)
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
void CParameter_DataObject::On_Assign(CParameter_Data *pSource)
{
	m_pDataObject	= ((CParameter_DataObject *)pSource)->m_pDataObject;
}

//---------------------------------------------------------
bool CParameter_DataObject::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		if(	m_pDataObject == DATAOBJECT_CREATE )
		{
			fprintf(Stream, "%s\n", ENTRY_DATAOBJECT_CREATE);
		}
		else if( m_pDataObject != DATAOBJECT_NOTSET && m_pDataObject->Get_File_Path() )
		{
			fprintf(Stream, "%s\n", m_pDataObject->Get_File_Path());
		}
		else
		{
			fprintf(Stream, "\n");
		}
	}
	else
	{
		CSG_String	sLine;

		if( SG_Read_Line(Stream, sLine) )
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
CParameter_DataObject_Output::CParameter_DataObject_Output(CParameter *pOwner, long Constraint)
	: CParameter_DataObject(pOwner, Constraint)
{
	m_Type	= DATAOBJECT_TYPE_Undefined;
}

CParameter_DataObject_Output::~CParameter_DataObject_Output(void)
{}

//---------------------------------------------------------
bool CParameter_DataObject_Output::Set_Value(void *Value)
{
	CDataObject	*pDataObject	= (CDataObject *)Value;

	if( m_pDataObject != pDataObject && (!pDataObject || (pDataObject && pDataObject->Get_ObjectType() == m_Type)) )
	{
		m_pDataObject	= pDataObject;

		SG_UI_DataObject_Add(m_pDataObject, false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameter_DataObject_Output::Set_DataObject_Type(TDataObject_Type Type)
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
CParameter_Grid::CParameter_Grid(CParameter *pOwner, long Constraint)
	: CParameter_DataObject(pOwner, Constraint)
{
	m_Type	= GRID_TYPE_Undefined;
}

CParameter_Grid::~CParameter_Grid(void)
{}

//---------------------------------------------------------
bool CParameter_Grid::Set_Value(void *Value)
{
	CGrid_System	*pSystem;

	if( Value == DATAOBJECT_CREATE && !m_pOwner->is_Optional() )
	{
		Value	= DATAOBJECT_NOTSET;
	}

	if( m_pDataObject != Value )
	{
		pSystem	= Get_System();

		if(	Value == DATAOBJECT_NOTSET || Value == DATAOBJECT_CREATE
		||	pSystem == NULL || pSystem->is_Equal(((CGrid *)Value)->Get_System()) )
		{
			m_pDataObject	= (CDataObject *)Value;

			return( true );
		}
		else if( !m_pOwner->Get_Owner()->is_Managed() && pSystem != NULL )
		{
			pSystem->Assign(((CGrid *)Value)->Get_System());
			m_pDataObject	= (CDataObject *)Value;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CGrid_System * CParameter_Grid::Get_System(void)
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
void CParameter_Grid::Set_Preferred_Type(TGrid_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CParameter_Grid::On_Assign(CParameter_Data *pSource)
{
	CParameter_DataObject::On_Assign(pSource);

	m_Type	= ((CParameter_Grid *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						Table							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Table::CParameter_Table(CParameter *pOwner, long Constraint)
	: CParameter_DataObject(pOwner, Constraint)
{}

CParameter_Table::~CParameter_Table(void)
{}

//---------------------------------------------------------
bool CParameter_Table::Set_Value(void *Value)
{
	int			i;
	CParameters	*pParameters;

	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CDataObject *)Value;

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
CParameter_Shapes::CParameter_Shapes(CParameter *pOwner, long Constraint)
	: CParameter_DataObject(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

CParameter_Shapes::~CParameter_Shapes(void)
{}

//---------------------------------------------------------
bool CParameter_Shapes::Set_Value(void *Value)
{
	int			i;
	CParameters	*pParameters;

	if(	Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE
	&&	m_Type != SHAPE_TYPE_Undefined && m_Type != ((CShapes *)Value)->Get_Type() )
	{
		return( false );
	}

	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CDataObject *)Value;

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
void CParameter_Shapes::Set_Shape_Type(TShape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CParameter_Shapes::On_Assign(CParameter_Data *pSource)
{
	CParameter_DataObject::On_Assign(pSource);

	m_Type	= ((CParameter_Shapes *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_TIN::CParameter_TIN(CParameter *pOwner, long Constraint)
	: CParameter_DataObject(pOwner, Constraint)
{}

CParameter_TIN::~CParameter_TIN(void)
{}

//---------------------------------------------------------
bool CParameter_TIN::Set_Value(void *Value)
{
	int			i;
	CParameters	*pParameters;

	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CDataObject *)Value;

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
CParameter_List::CParameter_List(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	m_Objects	= NULL;
	m_nObjects	= 0;
}

CParameter_List::~CParameter_List(void)
{
	Del_Items();
}

//---------------------------------------------------------
const char * CParameter_List::asString(void)
{
	if( Get_Count() > 0 )
	{
		m_String.Printf("%d %s (", Get_Count(), Get_Count() == 1 ? LNG("[VAL] object") : LNG("[VAL] objects"));

		for(int i=0; i<Get_Count(); i++)
		{
			if( i > 0 )
			{
				m_String.Append(", ");
			}

			m_String.Append(asDataObject(i)->Get_Name());
		}

		m_String.Append(")");
	}
	else
	{
		m_String.Printf(LNG("[VAL] No objects"));
	}

	return( m_String );
}

//---------------------------------------------------------
void CParameter_List::Add_Item(CDataObject *pObject)
{
	if( pObject )
	{
		m_Objects	= (CDataObject **)SG_Realloc(m_Objects, (m_nObjects + 1) * sizeof(CDataObject *));
		m_Objects[m_nObjects++]	= pObject;
	}
}

//---------------------------------------------------------
int CParameter_List::Del_Item(int iObject)
{
	int		i;

	if( iObject >= 0 && iObject < m_nObjects )
	{
		m_nObjects--;

		for(i=iObject; i<m_nObjects; i++)
		{
			m_Objects[i]	= m_Objects[i + 1];
		}

		m_Objects	= (CDataObject **)SG_Realloc(m_Objects, m_nObjects * sizeof(CDataObject *));
	}

	return( m_nObjects );
}

int CParameter_List::Del_Item(CDataObject *pObject)
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
void CParameter_List::Del_Items(void)
{
	if( m_nObjects > 0 )
	{
		SG_Free(m_Objects);
		m_Objects	= NULL;
		m_nObjects	= 0;
	}
}

//---------------------------------------------------------
void CParameter_List::On_Assign(CParameter_Data *pSource)
{
	Del_Items();

	for(int i=0; i<((CParameter_List *)pSource)->Get_Count(); i++)
	{
		Add_Item(((CParameter_List *)pSource)->asDataObject(i));
	}
}

//---------------------------------------------------------
bool CParameter_List::On_Serialize(FILE *Stream, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( asDataObject(i)->Get_File_Path() )
			{
				fprintf(Stream, "%s\n", asDataObject(i)->Get_File_Path());
			}
		}

		fprintf(Stream, "%s\n", ENTRY_DATAOBJECTLIST_END);
	}
	else
	{
		CSG_String	sLine;
		CDataObject	*pObject;

		while( SG_Read_Line(Stream, sLine) && sLine.Cmp(ENTRY_DATAOBJECTLIST_END) )
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
CParameter_Grid_List::CParameter_Grid_List(CParameter *pOwner, long Constraint)
	: CParameter_List(pOwner, Constraint)
{
}

CParameter_Grid_List::~CParameter_Grid_List(void)
{}

//---------------------------------------------------------
CGrid_System * CParameter_Grid_List::Get_System(void)
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
CParameter_Table_List::CParameter_Table_List(CParameter *pOwner, long Constraint)
	: CParameter_List(pOwner, Constraint)
{}

CParameter_Table_List::~CParameter_Table_List(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Shapes_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CParameter_Shapes_List::CParameter_Shapes_List(CParameter *pOwner, long Constraint)
	: CParameter_List(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

CParameter_Shapes_List::~CParameter_Shapes_List(void)
{}

//---------------------------------------------------------
void CParameter_Shapes_List::Set_Shape_Type(TShape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CParameter_Shapes_List::On_Assign(CParameter_Data *pSource)
{
	CParameter_List::On_Assign(pSource);

	m_Type	= ((CParameter_Shapes_List *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CParameter_TIN_List::CParameter_TIN_List(CParameter *pOwner, long Constraint)
	: CParameter_List(pOwner, Constraint)
{}

CParameter_TIN_List::~CParameter_TIN_List(void)
{}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameter_Parameters::CParameter_Parameters(CParameter *pOwner, long Constraint)
	: CParameter_Data(pOwner, Constraint)
{
	m_pParameters	= new CParameters(pOwner->Get_Owner()->Get_Owner(), pOwner->Get_Name(), pOwner->Get_Description());
}

CParameter_Parameters::~CParameter_Parameters(void)
{
	delete(m_pParameters);
}

//---------------------------------------------------------
const char * CParameter_Parameters::asString(void)
{
	m_String.Printf("%d %s", m_pParameters->Get_Count(), LNG("parameters"));

	return( m_String );
}

//---------------------------------------------------------
void CParameter_Parameters::On_Assign(CParameter_Data *pSource)
{
	m_pParameters->Assign(((CParameter_Parameters *)pSource)->m_pParameters);
}

//---------------------------------------------------------
bool CParameter_Parameters::On_Serialize(FILE *Stream, bool bSave)
{
	return( m_pParameters->Serialize(Stream, bSave) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
