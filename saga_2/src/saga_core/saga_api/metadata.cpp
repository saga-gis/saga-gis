
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

#include "metadata.h"


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
CSG_MetaData::CSG_MetaData(CSG_MetaData *pParent)
{
	_On_Construction();

	m_pParent	= pParent;
}

//---------------------------------------------------------
void CSG_MetaData::_On_Construction(void)
{
	m_pParent	= NULL;
	m_pChildren	= NULL;
	m_nChildren	= 0;
	m_nBuffer	= 0;
}

//---------------------------------------------------------
CSG_MetaData::~CSG_MetaData(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_MetaData::Destroy(void)
{
	if( m_pChildren )
	{
		for(int i=0; i<m_nChildren; i++)
		{
			delete(m_pChildren[i]);
		}

		SG_Free(m_pChildren);

		m_pChildren	= NULL;
		m_nChildren	= 0;
		m_nBuffer	= 0;
	}

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
#define GET_GROW_SIZE(n)	(n < 64 ? 1 : (n < 1024 ? 32 : 256))

//---------------------------------------------------------
CSG_MetaData * CSG_MetaData::Add_Child(void)
{
	if( (m_nChildren + 1) >= m_nBuffer )
	{
		CSG_MetaData	**pChildren	= (CSG_MetaData **)SG_Realloc(m_pChildren, (m_nBuffer + GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_MetaData *));

		if( pChildren )
		{
			m_pChildren	= pChildren;
			m_nBuffer	+= GET_GROW_SIZE(m_nBuffer);
		}
		else
		{
			return( false );
		}
	}

	return( m_pChildren[m_nChildren++]	= new CSG_MetaData(this) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name, double Content)
{
	return( Add_Child(Name, CSG_String::Format(SG_T("%f"), Content)) );
}

CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name, int Content)
{
	return( Add_Child(Name, CSG_String::Format(SG_T("%d"), Content)) );
}

//---------------------------------------------------------
CSG_MetaData * CSG_MetaData::Add_Child(const CSG_String &Name, const SG_Char *Content)
{
	CSG_MetaData	*pChild	= Add_Child();

	if( pChild )
	{
		pChild->m_Name		= Name;
		pChild->m_Content	= Content ? Content : SG_T("");
	}

	return( pChild );
}

//---------------------------------------------------------
bool CSG_MetaData::Del_Child(int Index)
{
	if( Index >= 0 && Index < m_nChildren )
	{
		delete(m_pChildren[Index]);

		m_nChildren--;

		if( (m_nChildren - 1) < m_nBuffer - GET_GROW_SIZE(m_nBuffer) )
		{
			CSG_MetaData	**pChildren	= (CSG_MetaData **)SG_Realloc(m_pChildren, (m_nBuffer - GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_MetaData *));

			if( pChildren )
			{
				m_pChildren	= pChildren;
				m_nBuffer	-= GET_GROW_SIZE(m_nBuffer);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_MetaData::_Get_Child(const CSG_String &Name) const
{
	for(int i=0; i<m_nChildren; i++)
	{
		if( Name.CmpNoCase(m_pChildren[i]->Get_Name()) == 0 )
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
bool CSG_MetaData::Add_Property(const CSG_String &Name, const CSG_String &Value)
{
	if( _Get_Property(Name) < 0 )
	{
		m_Prop_Names	.Add(Name);
		m_Prop_Values	.Add(Value);

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
bool CSG_MetaData::Assign(const CSG_MetaData &MetaData, bool bAppend)
{
	int		i;

	if( !bAppend )
	{
		Destroy();

		Set_Name	(MetaData.Get_Name   ());
		Set_Content	(MetaData.Get_Content());

		//-------------------------------------------------
		for(i=0; i<MetaData.Get_Property_Count(); i++)
		{
			Add_Property(MetaData.Get_Property_Name(i), MetaData.Get_Property(i));
		}
	}

	//-----------------------------------------------------
	for(i=0; i<MetaData.Get_Children_Count(); i++)
	{
		Add_Child()->Assign(*MetaData.Get_Child(i), false);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	_XML_To_SpecChars(const SG_Char *String)
{
	CSG_String	s;

	if( String && String[0] )
	{
		s	= String;

		s.Replace(SG_T("&auml") , SG_T("ä"));
		s.Replace(SG_T("&ouml") , SG_T("ö"));
		s.Replace(SG_T("&uuml") , SG_T("ü"));
		s.Replace(SG_T("&Auml") , SG_T("Ä"));
		s.Replace(SG_T("&Ouml") , SG_T("Ö"));
		s.Replace(SG_T("&Uuml") , SG_T("Ü"));
		s.Replace(SG_T("&szlig"), SG_T("ß"));
	}

	return( s );
}

//---------------------------------------------------------
CSG_String	_SpecChars_To_XML(const SG_Char *String)
{
	CSG_String	s;

	if( String && String[0] )
	{
		s	= String;

		s.Replace(SG_T("ä"), SG_T("&auml"));
		s.Replace(SG_T("ö"), SG_T("&ouml"));
		s.Replace(SG_T("ü"), SG_T("&uuml"));
		s.Replace(SG_T("Ä"), SG_T("&Auml"));
		s.Replace(SG_T("Ö"), SG_T("&Ouml"));
		s.Replace(SG_T("Ü"), SG_T("&Uuml"));
		s.Replace(SG_T("ß"), SG_T("&szlig"));
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Load(const CSG_String &File, const SG_Char *Extension)
{
	return( Load(CSG_File(SG_File_Make_Path(NULL, File, Extension), SG_FILE_R, false)) );
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
	Set_Name	(_XML_To_SpecChars(pNode->GetName       ()).c_str());
	Set_Content	(_XML_To_SpecChars(pNode->GetNodeContent()).c_str());

	//-----------------------------------------------------
	wxXmlProperty	*pProperty	= pNode->GetProperties();

	while( pProperty )
	{
		Add_Property(_XML_To_SpecChars(pProperty->GetName()).c_str(), _XML_To_SpecChars(pProperty->GetValue()).c_str());

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
	return( Save(CSG_File(SG_File_Make_Path(NULL, File, Extension), SG_FILE_W, false)) );
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

	pNode->SetName	 (Get_Name().Length() ? _SpecChars_To_XML(Get_Name()).c_str() : SG_T("NODE"));
	pNode->SetContent(_SpecChars_To_XML(Get_Content()).c_str());

	if( Get_Content().Length() > 0 )
	{
		wxXmlNode	*pChild	= new wxXmlNode(pNode, wxXML_TEXT_NODE, SG_T("TEXT"));// _SpecChars_To_XML(Get_Name()).c_str());

		pChild->SetContent(_SpecChars_To_XML(Get_Content()).c_str());
	}

	//-----------------------------------------------------
	for(i=0; i<Get_Property_Count(); i++)
	{
		pNode->AddProperty(_SpecChars_To_XML(Get_Property_Name(i)).c_str(), _SpecChars_To_XML(Get_Property(i)).c_str());
	}

	//-----------------------------------------------------
	for(i=Get_Children_Count()-1; i>=0; i--)
	{
		if( Get_Child(i)->Get_Content().Length() > 0 || Get_Child(i)->Get_Children_Count() > 0 )
		{
			wxXmlNode	*pChild	= new wxXmlNode(pNode, wxXML_ELEMENT_NODE, _SpecChars_To_XML(Get_Child(i)->Get_Name()).c_str());

			Get_Child(i)->_Save(pChild);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
