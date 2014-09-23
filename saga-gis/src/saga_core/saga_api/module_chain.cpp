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
#include "saga_api.h"

#include "module_chain.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define TOOL_CHAIN_VERSION	"SAGA_TOOL_CHAIN_v1.0.0"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(void)
{
	Destroy();
}

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(const CSG_String &File)
{
	Create(File);
}

//---------------------------------------------------------
CSG_Module_Chain::~CSG_Module_Chain(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_Module_Chain::Destroy(void)
{
	Parameters.Destroy();

	m_Chain.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_CONTENT(md, id, def)	(md.Get_Child(id) ? md.Get_Child(id)->Get_Content() : CSG_String(def))

//---------------------------------------------------------
bool CSG_Module_Chain::Create(const CSG_String &File)
{
	Destroy();

	if( !m_Chain.Load(File) || m_Chain.Get_Name().CmpNoCase("toolchain")
	||  !m_Chain.Get_Child("version"   ) || !m_Chain.Get_Child("version")->Cmp_Content(TOOL_CHAIN_VERSION)
	||  !m_Chain.Get_Child("identifier") )
	{
		Destroy();

		return( false );
	}

	m_ID			= GET_CONTENT(m_Chain, "identifier" , "");
	m_Library		= GET_CONTENT(m_Chain, "group"      , "tool_chains");
	m_Library_Name	= GET_CONTENT(m_Chain, "group_name" , _TL("Tool Chains"));
	Set_Name         (GET_CONTENT(m_Chain, "name"       , _TL("Unnamed")));
	Set_Description  (GET_CONTENT(m_Chain, "description", _TL("")));

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
	if( !History.Get_Property("version")	// new version
	||  !History("MODULE") || !History("MODULE")->Get_Child("OUTPUT") )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	Chain;

	Chain.Set_Name ("toolchain");

	Chain.Add_Child("version"    , TOOL_CHAIN_VERSION);
	Chain.Add_Child("group"      , "tool_chains");
	Chain.Add_Child("group_name" , "Tool Chains");
	Chain.Add_Child("identifier" , SG_File_Get_Name(File, false));
	Chain.Add_Child("name"       , SG_File_Get_Name(File, false));
	Chain.Add_Child("description", _TL("created from history"));

	CSG_MetaData	&Parms	= *Chain.Add_Child("parameters");
	CSG_MetaData	&Tools	= *Chain.Add_Child("tools"     );

	_Save_History_to_Model(History["MODULE"], Tools, Parms, "OUTPUT");

	return( Chain.Save(File, SG_T("xml")) );	//	return( Chain.Save(File, SG_T("smdl")) );
}

//---------------------------------------------------------
bool CSG_Module_Chain::_Save_History_to_Model(const CSG_MetaData &History, CSG_MetaData &Tools, CSG_MetaData &Parms, const CSG_String &Output_ID)
{
	if( !History("OUTPUT") )
	{
		return( false );
	}

	CSG_MetaData	*pParameter, &Tool	= *Tools.Add_Child("tool");

	CSG_String	Tool_ID	= CSG_String::Format(SG_T("tool_%01d"), Tools.Get_Children_Count());

	Tool.Add_Property("id"     , Tool_ID);
	Tool.Add_Property("library", History.Get_Property("library"));
	Tool.Add_Property("module" , History.Get_Property("id"     ));
	Tool.Add_Property("name"   , History.Get_Property("name"   ));

	if( Output_ID.Find('|') < 0 )
	{
		pParameter	= Parms.Add_Child("output", Output_ID);
		pParameter->Add_Property("tool" , Tool_ID);
		pParameter->Add_Property("type" , History["OUTPUT"].Get_Property("type"));
	}

	pParameter	= Tool.Add_Child("output", Output_ID);
	pParameter->Add_Property("id"   , History["OUTPUT"].Get_Property("id"   ));
	pParameter->Add_Property("parms", History["OUTPUT"].Get_Property("parms"));
	pParameter->Add_Property("type" , History["OUTPUT"].Get_Property("type" ));

	for(int i=0; i<History.Get_Children_Count(); i++)	// Options
	{
		CSG_MetaData	*pChild	= History.Get_Child(i);

		if( !pChild->Get_Name().Cmp("OPTION") )
		{
			switch( SG_Parameter_Type_Get_Type(pChild->Get_Property("type")) )
			{
			case PARAMETER_TYPE_Grid_System:
				if( pChild->Get_Children_Count() == 0 )
				{
					break;
				}

			case PARAMETER_TYPE_Bool:
			case PARAMETER_TYPE_Int:
			case PARAMETER_TYPE_Double:
			case PARAMETER_TYPE_Degree:
			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_Range:
			case PARAMETER_TYPE_Table_Field:
			case PARAMETER_TYPE_Table_Fields:
			case PARAMETER_TYPE_String:
			case PARAMETER_TYPE_Text:
			case PARAMETER_TYPE_FilePath:
				pParameter	= Tool.Add_Child("option", pChild->Get_Content());
			pParameter->Add_Property("id"   , pChild->Get_Property("id"   ));
			pParameter->Add_Property("parms", pChild->Get_Property("parms"));
			pParameter->Add_Property("type" , pChild->Get_Property("type" ));
				break;

			case PARAMETER_TYPE_FixedTable:
			case PARAMETER_TYPE_Parameters:
			default:
				break;
			}
		}
		else if( !pChild->Get_Name().Cmp("INPUT") )
		{
			pParameter	= Tool.Add_Child("input", Tool_ID + "|" + pChild->Get_Property("id"));
			pParameter->Add_Property("id"   , pChild->Get_Property("id"   ));
			pParameter->Add_Property("parms", pChild->Get_Property("parms"));
			pParameter->Add_Property("type" , pChild->Get_Property("type" ));

			if( pChild->Get_Child("MODULE") && pChild->Get_Child("MODULE")->Get_Child("OUTPUT") )
			{
				_Save_History_to_Model(*pChild->Get_Child("MODULE"), Tools, Parms, Tool_ID + "|" + pChild->Get_Property("id"));
			}
			else
			{
				pParameter	= Parms.Add_Child("input", Tool_ID + "|" + pChild->Get_Property("id"));
				pParameter->Add_Property("tool" , Tool_ID);
				pParameter->Add_Property("id"   , pChild->Get_Property("id"  ));
				pParameter->Add_Property("type" , pChild->Get_Property("type"));
			}
		}
		else if( !pChild->Get_Name().Cmp("INPUT_LIST") )
		{
			CSG_MetaData	*pList	= pChild;

			for(int j=0; j<pList->Get_Children_Count(); j++)
			{
				pChild		= pList->Get_Child(j);

				pParameter	= Tool.Add_Child("input", Tool_ID + "|" + pChild->Get_Property("id"));
				pParameter->Add_Property("id"   , pChild->Get_Property("id"   ));
				pParameter->Add_Property("parms", pChild->Get_Property("parms"));
				pParameter->Add_Property("type" , pChild->Get_Property("type" ));

				if( pChild->Get_Child("MODULE") && pChild->Get_Child("MODULE")->Get_Child("OUTPUT") )
				{
					_Save_History_to_Model(*pChild->Get_Child("MODULE"), Tools, Parms, Tool_ID + "|" + pChild->Get_Property("id"));
				}
				else
				{
					pParameter	= Parms.Add_Child("input", Tool_ID + "|" + pChild->Get_Property("id"));
					pParameter->Add_Property("tool" , Tool_ID);
					pParameter->Add_Property("id"   , pChild->Get_Property("id"  ));
					pParameter->Add_Property("type" , pChild->Get_Property("type"));
				}
			}
		}
	}

	return( true );
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
