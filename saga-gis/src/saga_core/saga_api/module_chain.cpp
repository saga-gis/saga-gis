/**********************************************************
 * Version $Id: module_chain.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   module_chain.cpp                    //
//                                                       //
//          Copyright (C) 2014 by Olaf Conrad            //
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
#include "module_chain.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(void)
{
	Create();
}

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(const CSG_String &File)
{
	Create(File);
}

//---------------------------------------------------------
CSG_Module_Chain::~CSG_Module_Chain(void)
{
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Create(void)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Create(const CSG_String &File)
{
	if( !m_Chain.Load(File)
	||  !m_Chain.Get_Child("VERSION") || !m_Chain.Get_Child("VERSION")->Cmp_Content("SAGA_TOOL_CHAIN_v1.0")
	||  !m_Chain.Get_Child("GROUP")
	||  !m_Chain.Get_Child("IDENTIFIER")
	||  !m_Chain.Get_Child("NAME")	)
	{
		m_Chain.Destroy();

		return( false );
	}

	return( is_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::On_Execute(void)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Save_History_to_Model(const CSG_MetaData &History, const CSG_String &File)
{
	CSG_MetaData	Chain;

	CSG_MetaData	&Parameter	= *Chain.Add_Child("PARAMETERS");
	CSG_MetaData	&Tools		= *Chain.Add_Child("TOOLS");
	CSG_MetaData	&Data		= *Chain.Add_Child("DATA");

	Chain.Add_Child("VERSION"   , "SAGA_TOOL_CHAIN_v1.0");
	Chain.Add_Child("GROUP"     , "tool_chains")->Add_Property("NAME", "Tool Chains");
	Chain.Add_Child("IDENTIFIER", SG_File_Get_Name(File, false));
	Chain.Add_Child("NAME"      , SG_File_Get_Name(File, false));
	Chain.Add_Child("DESCRIPTON", _TL("Created from history."));


	return( Chain.Save(File, SG_T("xml")) );
//	return( Chain.Save(File, SG_T("smdl")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module_Chains::CSG_Module_Chains(const CSG_String &Library_Name, const CSG_String &Name, const CSG_String &Description, const CSG_String &Menu)
{
	m_Library_Name	= Library_Name;
	m_Name			= Name;
	m_Description	= Description;
	m_Menu			= Menu;

	m_nModules		= 0;
	m_pModules		= NULL;
}

//---------------------------------------------------------
CSG_Module_Chains::~CSG_Module_Chains(void)
{
	for(int i=0; i<m_nModules; i++)
	{
		delete(m_pModules[i]);
	}

	SG_FREE_SAFE(m_pModules);

	m_nModules	= 0;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Module_Chains::Get_Info(int Type) const
{
	switch( Type )
	{
	case MLB_INFO_Name       :	return( m_Name );
	case MLB_INFO_Description:	return( m_Description );
	case MLB_INFO_Menu_Path  :	return( m_Menu );
	}

	return( "" );
}

//---------------------------------------------------------
bool CSG_Module_Chains::Add_Module(CSG_Module_Chain *pModule)
{
	m_pModules	= (CSG_Module_Chain **)SG_Realloc(m_pModules, (m_nModules + 1) * sizeof(CSG_Module_Chain *));
	m_pModules[m_nModules++]	= pModule;

	return( true );
}

//---------------------------------------------------------
CSG_Module * CSG_Module_Chains::Get_Module(int Index, TSG_Module_Type Type) const
{
	CSG_Module	*pModule	= Index >= 0 && Index < m_nModules ? m_pModules[Index] : NULL;

	return(	pModule && (Type == MODULE_TYPE_Base || Type == pModule->Get_Type()) ? pModule : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
