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
//                     metadata.cpp                      //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>

#include "metadata.h"
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(void)
{
	_On_Construction();
}

bool CSG_MetaData::Create(void)
{
	return( true );
}

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(const CSG_MetaData &MetaData)
{
	_On_Construction();

	Create(MetaData);
}

bool CSG_MetaData::Create(const CSG_MetaData &MetaData)
{
	return( Assign(MetaData) );
}

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(const CSG_String &File, const SG_Char *Extension)
{
	_On_Construction();

	Create(File, Extension);
}

bool CSG_MetaData::Create(const CSG_String &File, const SG_Char *Extension)
{
	return( Load(File, Extension) );
}

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(CSG_File &Stream)
{
	_On_Construction();

	Create(Stream);
}

bool CSG_MetaData::Create(CSG_File &Stream)
{
	return( Load(Stream) );
}

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(CSG_MetaData *pParent)
{
	_On_Construction();

	m_pParent	= pParent;
}

//---------------------------------------------------------
void CSG_MetaData::_On_Construction(void)
{
	m_pParent	= NULL;

	m_Children.Create(sizeof(CSG_MetaData **), 0, SG_ARRAY_GROWTH_1);
}

//---------------------------------------------------------
CSG_MetaData::~CSG_MetaData(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_MetaData::Destroy(void)
{
	CSG_MetaData	**m_pChildren	= (CSG_MetaData **)m_Children.Get_Array();

	for(int i=0; i<Get_Children_Count(); i++)
	{
		delete(m_pChildren[i]);
	}

	m_Children.Destroy();

//	m_pParent	= NULL;

//	m_Name			.Clear();
//	m_Content		.Clear();

	m_Prop_Names	.Clear();
	m_Prop_Values	.Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData * CSG_MetaData::Add_Child(void)
{
	return( Ins_Child(-1) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name)
{
	return( Ins_Child(Name, -1) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name, const CSG_String &Content)
{
	return( Ins_Child(Name, Content, -1) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name, double Content)
{
	return( Ins_Child(Name, SG_Get_String(Content, -16), -1) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name, int Content)
{
	return( Ins_Child(Name, CSG_String::Format(SG_T("%d"), Content), -1) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_MetaData &MetaData, bool bAddChildren)
{
	return( Ins_Child(MetaData, -1, bAddChildren) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData * CSG_MetaData::Ins_Child(int Position)
{
	if( !m_Children.Inc_Array() )
	{
		return( NULL );
	}

	CSG_MetaData	**pChildren	= (CSG_MetaData **)m_Children.Get_Array();

	if( Position < 0 || Position >= Get_Children_Count() )
	{
		Position	= Get_Children_Count() - 1;
	}

	for(int i=Get_Children_Count()-1; i>Position; i--)
	{
		pChildren[i]	= pChildren[i - 1];
	}

	return( pChildren[Position] = new CSG_MetaData(this) );
}

//---------------------------------------------------------
CSG_MetaData * CSG_MetaData::Ins_Child(const CSG_String &Name, const CSG_String &Content, int Position)
{
	CSG_MetaData	*pChild	= Ins_Child(Position);

	if( pChild )
	{
		pChild->m_Name		= Name;
		pChild->m_Content	= Content;
	}

	return( pChild );
}

CSG_MetaData * CSG_MetaData::Ins_Child(const CSG_String &Name, int Position)
{
	return( Ins_Child(Name, CSG_String(""), Position) );
}

CSG_MetaData * CSG_MetaData::Ins_Child(const CSG_String &Name, double Content, int Position)
{
	return( Ins_Child(Name, SG_Get_String(Content, -16), Position) );
}

CSG_MetaData * CSG_MetaData::Ins_Child(const CSG_String &Name, int Content, int Position)
{
	return( Ins_Child(Name, CSG_String::Format(SG_T("%d"), Content), Position) );
}

CSG_MetaData * CSG_MetaData::Ins_Child(const CSG_MetaData &MetaData, int Position, bool bAddChildren)
{
	CSG_MetaData	*pChild	= Ins_Child(Position);

	if( pChild )
	{
		pChild->Assign(MetaData, bAddChildren);
	}

	return( pChild );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Mov_Child(int from_Index, int to_Index)
{
	if( from_Index < 0 || from_Index >= Get_Children_Count()
	||    to_Index < 0 ||   to_Index >= Get_Children_Count() )
	{
		return( false );
	}

	if( from_Index != to_Index )
	{
		CSG_MetaData	**pChildren	= (CSG_MetaData **)m_Children.Get_Array();
		CSG_MetaData	*pChild	= pChildren[from_Index];

		if( from_Index < to_Index )
		{
			for(int i=from_Index; i<to_Index; i++)
			{
				pChildren[i]	= pChildren[i + 1];
			}
		}
		else // if( from_Index > to_Index )
		{
			for(int i=from_Index; i>to_Index; i--)
			{
				pChildren[i]	= pChildren[i - 1];
			}
		}

		pChildren[to_Index]	= pChild;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Del_Child(int Index)
{
	if( Index >= 0 && Index < Get_Children_Count() )
	{
		CSG_MetaData	**pChildren	= (CSG_MetaData **)m_Children.Get_Array();

		delete(pChildren[Index]);

		for(int i=Index, j=Index+1; j<Get_Children_Count(); i++, j++)
		{
			pChildren[i]	= pChildren[j];
		}

		m_Children.Dec_Array();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_MetaData::Add_Children(const CSG_MetaData &MetaData)
{
	if( &MetaData != this )
	{
		for(int i=0; i<MetaData.Get_Children_Count(); i++)
		{
			Add_Child(MetaData[i], true);
		}
	}

	return( true );
}

//---------------------------------------------------------
/**
  * Deletes children if depth level is reached, i.e. if depth level equals zero
  * all children of the node will be deleted, if it is one only the grandchildren
  * will be affected, and so on.
  * If Name is a valid string, only those children are taken into account that
  * have this name.
*/
//---------------------------------------------------------
bool CSG_MetaData::Del_Children(int Depth, const SG_Char *Name)
{
	if( Depth < 0 )
	{
		// nop
	}
	else if( Name && *Name )
	{
		for(int i=Get_Children_Count()-1; i>=0; i--)
		{
			if( Get_Child(i)->Get_Name().CmpNoCase(Name) )
			{
				Get_Child(i)->Del_Children(Depth, Name);
			}
			else if( Depth > 0 )
			{
				Get_Child(i)->Del_Children(Depth - 1, Name);
			}
			else
			{
				Del_Child(i);
			}
		}
	}
	else if( Depth > 0 )
	{
		for(int i=0; i<Get_Children_Count(); i++)
		{
			Get_Child(i)->Del_Children(Depth - 1, Name);
		}
	}
	else
	{
		for(int i=0; i<Get_Children_Count(); i++)
		{
			delete(Get_Child(i));
		}

		m_Children.Destroy();
	}

	return( true );
}

//---------------------------------------------------------
int CSG_MetaData::_Get_Child(const CSG_String &Name) const
{
	for(int i=0; i<Get_Children_Count(); i++)
	{
		if( Name.CmpNoCase(Get_Child(i)->Get_Name()) == 0 )
		{
			return( i );
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Cmp_Name(const CSG_String &String, bool bNoCase) const
{
	return( bNoCase ? !m_Name.CmpNoCase(String) : !m_Name.Cmp(String) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_MetaData::Fmt_Content(const char *Format, ...)
{
	wxString	s;

	va_list	argptr;

#ifdef _SAGA_LINUX
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_start(argptr, _Format);
	s.PrintfV(_Format, argptr);
#else
	va_start(argptr, Format);
	s.PrintfV(Format, argptr);
#endif

	m_Content	= CSG_String(&s);

	va_end(argptr);
}

//---------------------------------------------------------
void CSG_MetaData::Fmt_Content(const wchar_t *Format, ...)
{
	wxString	s;

	va_list	argptr;

#ifdef _SAGA_LINUX
	wxString	_Format(Format);	_Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_start(argptr, _Format);
	s.PrintfV(_Format, argptr);
#else
	va_start(argptr, Format);
	s.PrintfV(Format, argptr);
#endif

	m_Content	= CSG_String(&s);

	va_end(argptr);
}

//---------------------------------------------------------
bool CSG_MetaData::Cmp_Content(const CSG_String &String, bool bNoCase) const
{
	return( bNoCase ? !m_Content.CmpNoCase(String) : !m_Content.Cmp(String) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Add_Property(const CSG_String &Name, const CSG_String &Value)
{
	if( !Value.is_Empty() &&_Get_Property(Name) < 0 )
	{
		m_Prop_Names .Add(Name );
		m_Prop_Values.Add(Value);

		return( true );
	}

	return( false );
}

bool CSG_MetaData::Add_Property(const CSG_String &Name, double Value)
{
	return( Add_Property(Name, CSG_String::Format(SG_T("%f"), Value)) );
}

bool CSG_MetaData::Add_Property(const CSG_String &Name, int Value)
{
	return( Add_Property(Name, CSG_String::Format(SG_T("%d"), Value)) );
}

//---------------------------------------------------------
bool CSG_MetaData::Set_Property(const CSG_String &Name, const CSG_String &Value, bool bAddIfNotExists)
{
	int		Index;

	if( (Index = _Get_Property(Name)) >= 0 )
	{
		m_Prop_Values[Index]	= Value;

		return( true );
	}
	else if( bAddIfNotExists )
	{
		m_Prop_Names	.Add(Name);
		m_Prop_Values	.Add(Value);

		return( true );
	}

	return( false );
}

bool CSG_MetaData::Set_Property(const CSG_String &Name, double Value, bool bAddIfNotExists)
{
	return( Set_Property(Name, CSG_String::Format(SG_T("%f"), Value, bAddIfNotExists)) );
}

bool CSG_MetaData::Set_Property(const CSG_String &Name, int Value, bool bAddIfNotExists)
{
	return( Set_Property(Name, CSG_String::Format(SG_T("%d"), Value, bAddIfNotExists)) );
}

//---------------------------------------------------------
bool CSG_MetaData::Get_Property(const CSG_String &Name, CSG_String &Value)	const
{
	const SG_Char	*cString	= Get_Property(Name);

	if( cString )
	{
		Value	= cString;

		return( true );
	}

	return( false );
}

bool CSG_MetaData::Get_Property(const CSG_String &Name, double &Value)	const
{
	CSG_String	s;

	return( Get_Property(Name, s) && s.asDouble(Value) );
}

bool CSG_MetaData::Get_Property(const CSG_String &Name, int &Value)	const
{
	CSG_String	s;

	return( Get_Property(Name, s) && s.asInt(Value) );
}

//---------------------------------------------------------
bool CSG_MetaData::Cmp_Property(const CSG_String &Name, const CSG_String &String, bool bNoCase) const
{
	CSG_String	s;

	return( Get_Property(Name, s) && (bNoCase ? !s.CmpNoCase(String) : !s.Cmp(String)) );
}

//---------------------------------------------------------
int CSG_MetaData::_Get_Property(const CSG_String &Name) const
{
	for(int i=0; i<m_Prop_Names.Get_Count(); i++)
	{
		if( Name.CmpNoCase(m_Prop_Names[i]) == 0 )
		{
			return( i );
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_MetaData::asText(int Flags) const
{
	CSG_String	s;

	if( Flags == 0 )
	{
		for(int i=0; i<Get_Children_Count(); i++)
		{
			s	+= Get_Child(i)->Get_Name() + ":\t" + Get_Child(i)->Get_Content() + "\n";
		}
	}
	else
	{
		wxXmlDocument	XML;

		wxXmlNode	*pRoot	= new wxXmlNode(NULL, wxXML_ELEMENT_NODE, Get_Name().c_str());

		XML.SetRoot(pRoot);

		_Save(pRoot);

		wxStringOutputStream	Stream;

		XML.Save(Stream);

		s	= &Stream.GetString();

		if( Flags == 2 )	// remove <xml>
		{
			s	= s.AfterFirst('\n');
		}
	}

	return( s );
}

//---------------------------------------------------------
CSG_Table CSG_MetaData::asTable(int Flags) const
{
	CSG_Table	t;

	t.Add_Field("NAME" , SG_DATATYPE_String);
	t.Add_Field("VALUE", SG_DATATYPE_String);

	for(int i=0; i<Get_Children_Count(); i++)
	{
		CSG_Table_Record	*r	= t.Add_Record();

		r->Set_Value(0, Get_Child(i)->Get_Name());
		r->Set_Value(1, Get_Child(i)->Get_Content());
	}

	return( t );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Assign(const CSG_MetaData &MetaData, bool bAddChildren)
{
	if( &MetaData != this )
	{
		Destroy();

		Set_Name	(MetaData.Get_Name   ());
		Set_Content	(MetaData.Get_Content());

		for(int i=0; i<MetaData.Get_Property_Count(); i++)
		{
			Add_Property(MetaData.Get_Property_Name(i), MetaData.Get_Property(i));
		}

		if( bAddChildren )
		{
			Add_Children(MetaData);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Load(const CSG_String &File, const SG_Char *Extension)
{
	Destroy();

	wxXmlDocument	XML;

	if( SG_File_Exists(SG_File_Make_Path(NULL, File, Extension)) && XML.Load(SG_File_Make_Path(NULL, File, Extension).c_str()) )
	{
		_Load(XML.GetRoot());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_MetaData::Load(CSG_File &File)
{
	Destroy();

	wxXmlDocument	XML;

	wxFFileInputStream	Stream(File.Get_Stream());

	if( Stream.IsOk() && XML.Load(Stream) )
	{
		_Load(XML.GetRoot());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_MetaData::_Load(wxXmlNode *pNode)
{
	m_Name		= pNode->GetName       ().wc_str();
	m_Content	= pNode->GetNodeContent().wc_str();

	//-----------------------------------------------------
	wxXmlAttribute	*pProperty	= pNode->GetAttributes();

	while( pProperty )
	{
		Add_Property(&pProperty->GetName(), &pProperty->GetValue());

		pProperty	= pProperty->GetNext();
	}

	//-----------------------------------------------------
	wxXmlNode	*pChild	= pNode->GetChildren();

	while( pChild )
	{
		if( pChild->GetType() != wxXML_TEXT_NODE )
		{
			Add_Child()->_Load(pChild);
		}

		pChild	= pChild->GetNext();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Save(const CSG_String &File, const SG_Char *Extension) const
{
	wxXmlDocument	XML;

	wxXmlNode	*pRoot	= new wxXmlNode(NULL, wxXML_ELEMENT_NODE, Get_Name().c_str());

	XML.SetRoot(pRoot);

	_Save(pRoot);

	if( XML.Save(SG_File_Make_Path(NULL, File, Extension).c_str()) )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_MetaData::Save(CSG_File &File) const
{
	wxXmlDocument	XML;

	wxXmlNode	*pRoot	= new wxXmlNode(NULL, wxXML_ELEMENT_NODE, Get_Name().c_str());

	XML.SetRoot(pRoot);

	_Save(pRoot);

	wxFFileOutputStream	Stream(File.Get_Stream());

	if( Stream.IsOk() && XML.Save(Stream) )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_MetaData::_Save(wxXmlNode *pNode) const
{
	int		i;

	//-----------------------------------------------------
	pNode->SetName	 (CSG_String(Get_Name().Length() ? Get_Name() : CSG_String("NODE")).c_str());
	pNode->SetContent(Get_Content().c_str());

	if( Get_Content().Length() > 0 || (Get_Property_Count() == 0 && Get_Children_Count() == 0) )
	{
		wxXmlNode	*pChild	= new wxXmlNode(pNode, wxXML_TEXT_NODE, SG_T("TEXT"));

		pChild->SetContent(Get_Content().c_str());
	}

	//-----------------------------------------------------
	for(i=0; i<Get_Property_Count(); i++)
	{
		pNode->AddAttribute(Get_Property_Name(i).c_str(), Get_Property(i));
	}

	//-----------------------------------------------------
	for(i=Get_Children_Count()-1; i>=0; i--)
	{
		Get_Child(i)->_Save(new wxXmlNode(pNode, wxXML_ELEMENT_NODE, Get_Child(i)->Get_Name().c_str()));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Load_WKT(const CSG_String &WKT)
{
	return( false );
}

bool CSG_MetaData::_Load_WKT(const CSG_String &WKT)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_MetaData::Save_WKT(CSG_String &WKT) const
{
	return( false );
}

bool CSG_MetaData::_Save_WKT(CSG_String &WKT) const
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
