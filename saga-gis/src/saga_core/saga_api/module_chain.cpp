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
{}

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(const CSG_String &File)
{
	Create(File);
}

//---------------------------------------------------------
CSG_Module_Chain::~CSG_Module_Chain(void)
{
	_Destroy();
}

//---------------------------------------------------------
void CSG_Module_Chain::_Destroy(void)
{
	Parameters.Del_Parameters();

	m_Chain.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_CONTENT(md, id, def)	(md(id) ? md(id)->Get_Content() : CSG_String(def))

//---------------------------------------------------------
bool CSG_Module_Chain::Create(const CSG_String &File)
{
	_Destroy();

	if( !m_Chain.Load(File) || m_Chain.Get_Name().CmpNoCase("toolchain")
	||  !m_Chain.Get_Child("version"   ) || !m_Chain.Get_Child("version")->Cmp_Content(TOOL_CHAIN_VERSION)
	||  !m_Chain.Get_Child("identifier") )
	{
		_Destroy();

		return( false );
	}

	m_File_Name		= File;

	m_ID			= GET_CONTENT(m_Chain, "identifier" , "");
	m_Library		= GET_CONTENT(m_Chain, "group"      , "tool_chains");
	m_Library_Name	= GET_CONTENT(m_Chain, "group_name" , _TL("Tool Chains"));
	Set_Name         (GET_CONTENT(m_Chain, "name"       , _TL("Unnamed")));
	Set_Description  (GET_CONTENT(m_Chain, "description", _TL("no description")));

	if( m_Chain("parameters") )
	{
		for(int i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
		{
			CSG_MetaData	*pParameter	= m_Chain["parameters"](i);

			CSG_Parameter	*pParent	= Parameters(pParameter->Get_Property("parent"));

			CSG_String	ID		= pParameter->Get_Property("tool");	ID	= ID + "::" + pParameter->Get_Property("id");
			CSG_String	Name	= pParameter->Get_Property("name");
			CSG_String	Value	= pParameter->Get_Content();

			int		Constraint	= pParameter->Get_Name().CmpNoCase("input") ? PARAMETER_OUTPUT : PARAMETER_INPUT;

			switch( SG_Parameter_Type_Get_Type(pParameter->Get_Property("type")) )
			{
			case PARAMETER_TYPE_PointCloud  :	Parameters.Add_PointCloud  (pParent, ID, Name, "", Constraint);	break;
			case PARAMETER_TYPE_Grid        :	Parameters.Add_Grid        (pParent, ID, Name, "", Constraint);	break;
			case PARAMETER_TYPE_Table       :	Parameters.Add_Table       (pParent, ID, Name, "", Constraint);	break;
			case PARAMETER_TYPE_Shapes      :	Parameters.Add_Shapes      (pParent, ID, Name, "", Constraint);	break;
			case PARAMETER_TYPE_TIN         :	Parameters.Add_TIN         (pParent, ID, Name, "", Constraint);	break;

			case PARAMETER_TYPE_Bool        :	Parameters.Add_Value       (pParent, ID, Name, "", PARAMETER_TYPE_Bool  ,!Value.CmpNoCase("TRUE"));	break;
			case PARAMETER_TYPE_Int         :	Parameters.Add_Value       (pParent, ID, Name, "", PARAMETER_TYPE_Int   , Value.asInt   ());	break;
			case PARAMETER_TYPE_Double      :	Parameters.Add_Value       (pParent, ID, Name, "", PARAMETER_TYPE_Double, Value.asDouble());	break;
			case PARAMETER_TYPE_Degree      :	Parameters.Add_Value       (pParent, ID, Name, "", PARAMETER_TYPE_Degree, Value.asDouble());	break;

			case PARAMETER_TYPE_String      :	Parameters.Add_String      (pParent, ID, Name, "", Value, false);	break;
			case PARAMETER_TYPE_Text        :	Parameters.Add_String      (pParent, ID, Name, "", Value,  true);	break;

			case PARAMETER_TYPE_FilePath    :	Parameters.Add_FilePath    (pParent, ID, Name, "", Value);	break;

			case PARAMETER_TYPE_Choice      :	Parameters.Add_Choice      (pParent, ID, Name, "", pParameter->Get_Property("choices"))->Set_Value(Value);	break;
			case PARAMETER_TYPE_Range       :	break;

			case PARAMETER_TYPE_Table_Field :	Parameters.Add_Table_Field (pParent, ID, Name, "", !Value.CmpNoCase("TRUE"));	break;
			case PARAMETER_TYPE_Table_Fields:	Parameters.Add_Table_Fields(pParent, ID, Name, "");	break;

			case PARAMETER_TYPE_Grid_System :	Parameters.Add_Grid_System (pParent, ID, Name, "");	break;

			default: break;
			}
		}
	}

	return( is_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::On_Execute(void)
{
	for(int i=m_Chain["tools"].Get_Children_Count()-1; i>=0; i--)
	{
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
bool CSG_Module_Chain::Save_History_to_Model(const CSG_MetaData &History, const CSG_String &File)
{
	if( !History.Get_Property("version") || !History("MODULE") || !History("MODULE")->Get_Child("OUTPUT") )
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

	Chain.Add_Child("parameters");
	Chain.Add_Child("tools"     );

	_Save_History_Add_Tool(History["MODULE"], *Chain("parameters"), *Chain("tools"), true);

	return( Chain.Save(File) );
}

//---------------------------------------------------------
bool CSG_Module_Chain::_Save_History_Add_Tool(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tools, bool bAddOutput)
{
	if( !History("OUTPUT") )
	{
		return( false );
	}

	CSG_MetaData	*pParameter, &Tool	= *Tools.Add_Child("tool");

	CSG_String		Tool_ID(CSG_String::Format(SG_T("tool_%02d"), Tools.Get_Children_Count()));

	Tool.Add_Property("id"     , Tool_ID);
	Tool.Add_Property("library", History.Get_Property("library"));
	Tool.Add_Property("module" , History.Get_Property("id"     ));
	Tool.Add_Property("name"   , History.Get_Property("name"   ));

	if( bAddOutput )
	{
		_Save_History_Add_Data(History["OUTPUT"], Parms, Tool, false);
	}

	pParameter	= Tool.Add_Child("output", CSG_String::Format(SG_T("%s::%s"), Tool_ID.c_str(), History["OUTPUT"].Get_Property("id")));
	pParameter->Add_Property("id"   , History["OUTPUT"].Get_Property("id"   ));
	pParameter->Add_Property("parms", History["OUTPUT"].Get_Property("parms"));

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
				break;

			case PARAMETER_TYPE_FixedTable:
			case PARAMETER_TYPE_Parameters:
			default:
				break;
			}
		}
		else if( !pChild->Get_Name().Cmp("INPUT") )
		{
			_Save_History_Add_Input(*pChild, Parms, Tool);
		}
		else if( !pChild->Get_Name().Cmp("INPUT_LIST") )
		{
			for(int j=0; j<pChild->Get_Children_Count(); j++)
			{
				_Save_History_Add_Input(*pChild->Get_Child(j), Parms, Tool);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Module_Chain::_Save_History_Add_Input(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tool)
{
	CSG_MetaData	*pParameter	= Tool.Add_Child("input");

	pParameter->Add_Property("id"   , History.Get_Property("id"   ));
	pParameter->Add_Property("parms", History.Get_Property("parms"));

	if( !History("MODULE") || !History["MODULE"]("OUTPUT") )
	{
		pParameter->Fmt_Content(SG_T("%s::%s"), Tool.Get_Property("id"), History.Get_Property("id"));

		return( _Save_History_Add_Data(History, Parms, Tool, true) );
	}

	pParameter->Fmt_Content(SG_T("tool_%02d::%s"), Tool.Get_Parent()->Get_Children_Count(), History["MODULE"]["OUTPUT"].Get_Property("id"));

	return( _Save_History_Add_Tool(History["MODULE"], Parms, *Tool.Get_Parent()) );
}

//---------------------------------------------------------
bool CSG_Module_Chain::_Save_History_Add_Data(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tool, bool bInput)
{
	CSG_MetaData	*pParameter	= Parms.Add_Child(bInput ? "input" : "output", CSG_String::Format(SG_T("%s::%s"), Tool.Get_Property("id"), History.Get_Property("id")));

	pParameter->Add_Property("tool", Tool   .Get_Property("id"  ));
	pParameter->Add_Property("id"  , History.Get_Property("id"  ));
	pParameter->Add_Property("type", History.Get_Property("type"));
	pParameter->Add_Property("name", History.Get_Property("name"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
