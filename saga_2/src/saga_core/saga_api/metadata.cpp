
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

#include "metadata.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData_Node::CSG_MetaData_Node(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_MetaData_Node::CSG_MetaData_Node(CSG_MetaData_Node *pParent)
{
	_On_Construction();

	m_pParent	= pParent;
}

//---------------------------------------------------------
void CSG_MetaData_Node::_On_Construction(void)
{
	m_pParent	= NULL;
	m_pChildren	= NULL;
	m_nChildren	= 0;
	m_nBuffer	= 0;
}

//---------------------------------------------------------
CSG_MetaData_Node::~CSG_MetaData_Node(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_MetaData_Node::Destroy(void)
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

	m_pParent	= NULL;

	m_Name			.Clear();
	m_Content		.Clear();

	m_Prop_Names	.Clear();
	m_Prop_Values	.Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GROW_SIZE(n)	(n < 64 ? 1 : (n < 1024 ? 32 : 256))

//---------------------------------------------------------
CSG_MetaData_Node * CSG_MetaData_Node::Add_Child(void)
{
	if( (m_nChildren + 1) >= m_nBuffer )
	{
		CSG_MetaData_Node	**pChildren	= (CSG_MetaData_Node **)SG_Realloc(m_pChildren, (m_nBuffer + GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_MetaData_Node *));

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

	return( m_pChildren[m_nChildren++]	= new CSG_MetaData_Node(this) );
}

CSG_MetaData_Node * CSG_MetaData_Node::Add_Child(const CSG_String &Name, double Content)
{
	return( Add_Child(Name, CSG_String::Format(SG_T("%f"), Content)) );
}

CSG_MetaData_Node * CSG_MetaData_Node::Add_Child(const CSG_String &Name, int Content)
{
	return( Add_Child(Name, CSG_String::Format(SG_T("%d"), Content)) );
}

//---------------------------------------------------------
CSG_MetaData_Node * CSG_MetaData_Node::Add_Child(const CSG_String &Name, const CSG_String &Content)
{
	CSG_MetaData_Node	*pChild	= Add_Child();

	if( pChild )
	{
		pChild->Set_Name   (Name);
		pChild->Set_Content(Content);
	}

	return( pChild );
}

//---------------------------------------------------------
bool CSG_MetaData_Node::Del_Child(int Index)
{
	if( Index >= 0 && Index < m_nChildren )
	{
		delete(m_pChildren[Index]);

		m_nChildren--;

		if( (m_nChildren - 1) < m_nBuffer - GET_GROW_SIZE(m_nBuffer) )
		{
			CSG_MetaData_Node	**pChildren	= (CSG_MetaData_Node **)SG_Realloc(m_pChildren, (m_nBuffer - GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_MetaData_Node *));

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
int CSG_MetaData_Node::_Get_Child(const CSG_String &Name) const
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
bool CSG_MetaData_Node::Add_Property(const CSG_String &Name, const CSG_String &Value)
{
	if( _Get_Property(Name) < 0 )
	{
		m_Prop_Names	.Add(Name);
		m_Prop_Values	.Add(Value);

		return( true );
	}

	return( false );
}

bool CSG_MetaData_Node::Add_Property(const CSG_String &Name, double Value)
{
	return( Add_Property(Name, CSG_String::Format(SG_T("%f"), Value)) );
}

bool CSG_MetaData_Node::Add_Property(const CSG_String &Name, int Value)
{
	return( Add_Property(Name, CSG_String::Format(SG_T("%d"), Value)) );
}

//---------------------------------------------------------
bool CSG_MetaData_Node::Set_Property(const CSG_String &Name, const CSG_String &Value, bool bAddIfNotExists)
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

bool CSG_MetaData_Node::Set_Property(const CSG_String &Name, double Value, bool bAddIfNotExists)
{
	return( Set_Property(Name, CSG_String::Format(SG_T("%f"), Value, bAddIfNotExists)) );
}

bool CSG_MetaData_Node::Set_Property(const CSG_String &Name, int Value, bool bAddIfNotExists)
{
	return( Set_Property(Name, CSG_String::Format(SG_T("%d"), Value, bAddIfNotExists)) );
}

//---------------------------------------------------------
bool CSG_MetaData_Node::Get_Property(const CSG_String &Name, CSG_String &Value)	const
{
	const SG_Char	*cString	= Get_Property(Name);

	if( cString )
	{
		Value	= cString;

		return( true );
	}

	return( false );
}

bool CSG_MetaData_Node::Get_Property(const CSG_String &Name, double &Value)	const
{
	CSG_String	s;

	return( Get_Property(Name, s) && s.asDouble(Value) );
}

bool CSG_MetaData_Node::Get_Property(const CSG_String &Name, int &Value)	const
{
	CSG_String	s;

	return( Get_Property(Name, s) && s.asInt(Value) );
}

//---------------------------------------------------------
int CSG_MetaData_Node::_Get_Property(const CSG_String &Name) const
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(void)
{
	_On_Construction();

	Create();
}

bool CSG_MetaData::Create(void)
{
	Destroy();

	return( true );
}

//---------------------------------------------------------
CSG_MetaData::CSG_MetaData(const CSG_String &File)
{
	_On_Construction();

	Create(File);
}

bool CSG_MetaData::Create(const CSG_String &File)
{
	return( Load(File) );
}

//---------------------------------------------------------
void CSG_MetaData::_On_Construction(void)
{}

//---------------------------------------------------------
CSG_MetaData::~CSG_MetaData(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_MetaData::Destroy(void)
{
	m_Root.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Load(const CSG_String &File)
{
	Destroy();

	wxXmlDocument	XML;

	if( XML.Load(File.c_str()) )
	{
		_Load(&m_Root, XML.GetRoot());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_MetaData::_Load(CSG_MetaData_Node *pNode, wxXmlNode *pXMLNode)
{
	pNode->Set_Name		(pXMLNode->GetName       ().c_str());
	pNode->Set_Content	(pXMLNode->GetNodeContent().c_str());

	//-----------------------------------------------------
	wxXmlProperty	*pProperty	= pXMLNode->GetProperties();

	while( pProperty )
	{
		pNode->Add_Property(pProperty->GetName().c_str(), pProperty->GetValue().c_str());

		pProperty	= pProperty->GetNext();
	}

	//-----------------------------------------------------
	wxXmlNode	*pChild	= pXMLNode->GetChildren();

	while( pChild )
	{
		_Load(pNode->Add_Child(), pChild);

		pChild	= pChild->GetNext();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_MetaData::Save(const CSG_String &File)
{
	wxXmlDocument	XML;

	wxXmlNode	*pRoot	= new wxXmlNode(NULL, wxXML_ELEMENT_NODE, m_Root.Get_Name().c_str());

	XML.SetRoot(pRoot);

	_Save(&m_Root, pRoot);

	if( XML.Save(File.c_str()) )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_MetaData::_Save(CSG_MetaData_Node *pNode, wxXmlNode *pXMLNode)
{
	int		i;

	pXMLNode->SetName	(pNode->Get_Name   ().c_str());
	pXMLNode->SetContent(pNode->Get_Content().c_str());

	if( pNode->Get_Content().Length() > 0 )
	{
		wxXmlNode	*pChild	= new wxXmlNode(pXMLNode, wxXML_TEXT_NODE, pNode->Get_Name().c_str());

		pChild->SetContent(pNode->Get_Content().c_str());
	}

	//-----------------------------------------------------
	for(i=0; i<pNode->Get_Property_Count(); i++)
	{
		pXMLNode->AddProperty(pNode->Get_Property_Name(i).c_str(), pNode->Get_Property(i));
	}

	//-----------------------------------------------------
	for(i=0; i<pNode->Get_Children_Count(); i++)
	{
		wxXmlNode	*pChild	= new wxXmlNode(pXMLNode, wxXML_ELEMENT_NODE, pNode->Get_Child(i)->Get_Name().c_str());

		_Save(pNode->Get_Child(i), pChild);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
