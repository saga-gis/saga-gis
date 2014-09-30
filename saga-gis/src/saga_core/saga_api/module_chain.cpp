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
#define TOOL_CHAIN_VERSION	"1.0.0"


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
	Reset();
}

//---------------------------------------------------------
void CSG_Module_Chain::Reset(void)
{
	Parameters.Del_Parameters();

	m_Chain.Destroy();

	m_Menu.Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_CONTENT(md, id, def)	(md(id) ? md(id)->Get_Content() : CSG_String(def))

//---------------------------------------------------------
bool CSG_Module_Chain::Create(const CSG_String &File)
{
	Reset();

	//-----------------------------------------------------
	if( !m_Chain.Load(File) || !m_Chain.Cmp_Name("toolchain") || !m_Chain.Cmp_Property("version", TOOL_CHAIN_VERSION)
	||  !m_Chain("identifier") || !m_Chain("parameters") )
	{
		Reset();

		return( false );
	}

	//-----------------------------------------------------
	m_File_Name		= File;

	m_ID			= GET_CONTENT(m_Chain, "identifier" , "");
	m_Library		= GET_CONTENT(m_Chain, "group"      , "toolchains");
//	m_Library_Name	= GET_CONTENT(m_Chain, "group_name" , _TL("Tool Chains"));
	m_Menu			= GET_CONTENT(m_Chain, "menu"       , "");
	Set_Name         (GET_CONTENT(m_Chain, "name"       , _TL("Unnamed")));
	Set_Description  (GET_CONTENT(m_Chain, "description", _TL("no description")));

	//-----------------------------------------------------
	for(int i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		int			Constraint;
		CSG_String	ID, Value;

		if( Parameter.Cmp_Name("input") )
		{
			ID			= Parameter.Get_Property("varname");
			Constraint	= PARAMETER_INPUT;
		}
		else if( Parameter.Cmp_Name("output") )
		{
			ID			= Parameter.Get_Property("varname");
			Constraint	= PARAMETER_OUTPUT;
		}
		else if( Parameter.Cmp_Name("option") )
		{
			ID			= Parameter.Get_Property("tool"); if( !ID.is_Empty() ) ID = ID + "::"; ID = ID + Parameter.Get_Property("id");
			Constraint	= 0;
			Value		= Parameter.Get_Content("value");
		}

		if( ID.is_Empty() || Parameters(ID) )
		{
			continue;
		}

		CSG_String	Name	= Parameter.Get_Content("name"       );
		CSG_String	Desc	= Parameter.Get_Content("description");

		CSG_Parameter	*pParent	= Parameters(Parameter.Get_Property("parent"));

		//-------------------------------------------------
		switch( SG_Parameter_Type_Get_Type(Parameter.Get_Property("type")) )
		{
		case PARAMETER_TYPE_PointCloud     : Parameters.Add_PointCloud     (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Grid           : Parameters.Add_Grid           (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Table          : Parameters.Add_Table          (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Shapes         : Parameters.Add_Shapes         (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_TIN            : Parameters.Add_TIN            (pParent, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_PointCloud_List: Parameters.Add_PointCloud_List(pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Grid_List      : Parameters.Add_Grid_List      (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Table_List     : Parameters.Add_Table_List     (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Shapes_List    : Parameters.Add_Shapes_List    (pParent, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_TIN_List       : Parameters.Add_TIN_List       (pParent, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Bool           : Parameters.Add_Value          (pParent, ID, Name, Desc, PARAMETER_TYPE_Bool  ,!Value.CmpNoCase("TRUE"));	break;
		case PARAMETER_TYPE_Int            : Parameters.Add_Value          (pParent, ID, Name, Desc, PARAMETER_TYPE_Int   , Value.asInt   ());	break;
		case PARAMETER_TYPE_Double         : Parameters.Add_Value          (pParent, ID, Name, Desc, PARAMETER_TYPE_Double, Value.asDouble());	break;
		case PARAMETER_TYPE_Degree         : Parameters.Add_Value          (pParent, ID, Name, Desc, PARAMETER_TYPE_Degree, Value.asDouble());	break;

		case PARAMETER_TYPE_String         : Parameters.Add_String         (pParent, ID, Name, Desc, Value, false);	break;
		case PARAMETER_TYPE_Text           : Parameters.Add_String         (pParent, ID, Name, Desc, Value,  true);	break;

		case PARAMETER_TYPE_FilePath       : Parameters.Add_FilePath       (pParent, ID, Name, Desc, Value);	break;

		case PARAMETER_TYPE_Choice         : Parameters.Add_Choice         (pParent, ID, Name, Desc, Parameter.Get_Content("choices"))->Set_Value(Value);	break;
		case PARAMETER_TYPE_Range          : Parameters.Add_Range          (pParent, ID, Name, Desc, Value.BeforeFirst(';').asDouble(), Value.AfterFirst (';').asDouble());	break;

		case PARAMETER_TYPE_Table_Field    : Parameters.Add_Table_Field    (pParent, ID, Name, Desc, !Value.CmpNoCase("TRUE"));	break;
		case PARAMETER_TYPE_Table_Fields   : Parameters.Add_Table_Fields   (pParent, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Grid_System    : Parameters.Add_Grid_System    (pParent, ID, Name, Desc);	break;

		default: break;
		}
	}

	//-----------------------------------------------------
	return( is_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::On_Execute(void)
{
	bool	bResult	= true;

	if( Data_Initialize() )
	{
		for(int i=0; i<m_Chain["tools"].Get_Children_Count() && bResult; i++)
		{
			bResult	= Tool_Run(m_Chain["tools"][i]);
		}
	}

	Data_Finalize();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Data_Add(const CSG_String &ID, TSG_Parameter_Type Type, CSG_Data_Object *pData)
{
	if( !pData )
	{
		return( false );
	}

	switch( Type )
	{
	case PARAMETER_TYPE_PointCloud:	m_Data.Add_PointCloud(NULL, ID, "", "", 0)->Set_Value(pData);	break;
	case PARAMETER_TYPE_Grid      :	m_Data.Add_Grid      (NULL, ID, "", "", 0)->Set_Value(pData);	break;
	case PARAMETER_TYPE_Table     :	m_Data.Add_Table     (NULL, ID, "", "", 0)->Set_Value(pData);	break;
	case PARAMETER_TYPE_Shapes    :	m_Data.Add_Shapes    (NULL, ID, "", "", 0)->Set_Value(pData);	break;
	case PARAMETER_TYPE_TIN       :	m_Data.Add_TIN       (NULL, ID, "", "", 0)->Set_Value(pData);	break;

	default: return( false );
	}

	m_Data_Manager.Add(pData);

	return( true );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Data_Exists(CSG_Data_Object *pData)
{
	for(int i=0; i<m_Data.Get_Count(); i++)
	{
		if( m_Data(i)->is_DataObject() )
		{
			if( pData == m_Data(i)->asDataObject() )
			{
				return( true );
			}
		}
		else if( m_Data(i)->is_DataObject_List() )
		{
			for(int j=0; j<m_Data(i)->asList()->Get_Count(); j++)
			{
				if( pData == m_Data(i)->asList()->asDataObject(j) )
				{
					return( true );
				}
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Data_Initialize(void)
{
	m_Data.Set_Manager(NULL);

	for(int i=0; i<Parameters.Get_Count(); i++)
	{
		Data_Add(
			Parameters(i)->Get_Identifier(),
			Parameters(i)->Get_Type      (),
			Parameters(i)->asDataObject  ()
		);
	}

	return( m_Data.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Data_Finalize(void)
{
	for(int i=0; i<Parameters.Get_Count(); i++)
	{
		if( Parameters(i)->is_DataObject() )
		{
			m_Data_Manager.Delete(Parameters(i)->asDataObject(), true);	// detach non temporary data !!!
		}
	}

	m_Data_Manager.Delete_All();

	m_Data.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Tool_Run(const CSG_MetaData &Tool)
{
	//-----------------------------------------------------
	if( !Tool.Cmp_Name("tool") || !Tool.Get_Property("library") || !Tool.Get_Property("module") )
	{
		Error_Set(_TL("invalid tool definition"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Module(Tool.Get_Property("module"));

	CSG_Module	*pModule;

	if(	!(pModule = SG_Get_Module_Library_Manager().Get_Module(Tool.Get_Property("library"), Module        ))
	&&  !(pModule = SG_Get_Module_Library_Manager().Get_Module(Tool.Get_Property("library"), Module.asInt())) )
	{
		Error_Fmt("%s [%s].[%s]", _TL("could not find tool"),  Tool.Get_Property("library"), Module.c_str());

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(pModule->Get_Name());

	pModule->Settings_Push(&m_Data_Manager);

	bool	bResult	= false;

	if( !Tool_Initialize(Tool, pModule) )
	{
		Error_Fmt("%s [%s].[%s]", _TL("tool initialization failed"), pModule->Get_Library().c_str(), pModule->Get_Name().c_str());
	}
	else if( !(bResult = pModule->Execute()) )
	{
		Error_Fmt("%s [%s].[%s]", _TL("tool execution failed"     ), pModule->Get_Library().c_str(), pModule->Get_Name().c_str());
	}

	Tool_Finalize(Tool, pModule);

	pModule->Settings_Pop();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Tool_Initialize(const CSG_MetaData &Tool, CSG_Module *pModule)
{
	//-----------------------------------------------------
	pModule->Get_Parameters()->Set_Callback();	// ???!!!

	CSG_String	Tool_ID	= Tool.Get_Property("id");

	for(int i=0; i<Tool.Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		CSG_String	ID	= Parameter.Get_Property("id");

		CSG_Parameter	*pParameter	= pModule->Get_Parameters(Parameter.Get_Property("parms"))
			? pModule->Get_Parameters(Parameter.Get_Property("parms"))->Get_Parameter(ID)
			: pModule->Get_Parameters()->Get_Parameter(ID);

		if( !pParameter )
		{
			return( false );
		}
		else if( Parameter.Cmp_Name("option") )
		{
			if( !pParameter->Assign(Parameters(Tool_ID + "::" + pParameter->Get_Identifier())) )	// set from tool chain parameters
			{
				pParameter->Set_Value(Parameter.Get_Content());
			}
		}
		else if( Parameter.Cmp_Name("input") )
		{
			CSG_Parameter	*pData	= m_Data(Parameter.Get_Content());

			if( !pData )	// each input for this tool should be available now !!!
			{
				return( false );
			}

			if( pParameter->is_DataObject() )
			{
				if( !pParameter->Assign(pData) )
				{
					return( false );
				}
			}
			else if( pParameter->is_DataObject_List() )
			{
				if( !pParameter->asList()->Add_Item(pData->asDataObject()) )
				{
					return( false );
				}
			}

			pParameter->has_Changed();
		}
		else if( Parameter.Cmp_Name("output") )
		{
			if( !pParameter->Assign(m_Data(Parameter.Get_Content())) )
			{
				pParameter->Set_Value(DATAOBJECT_CREATE);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Tool_Finalize(const CSG_MetaData &Tool, CSG_Module *pModule)
{
	int		i;

	//-----------------------------------------------------
	for(i=0; i<Tool.Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_String	ID	= Parameter.Get_Property("id");

			CSG_Parameter	*pParameter	= pModule->Get_Parameters(Parameter.Get_Property("parms"))
				? pModule->Get_Parameters(Parameter.Get_Property("parms"))->Get_Parameter(ID)
				: pModule->Get_Parameters()->Get_Parameter(ID);

			if( !pParameter || !Data_Add(Parameter.Get_Content(), pParameter->Get_Type(), pParameter->asDataObject()) )
			{
				return( false );
			}
		}
	}

	//-----------------------------------------------------
	for(i=-1; i<pModule->Get_Parameters_Count(); i++)	// free all data objects that have not been added to variable list
	{
		CSG_Parameters	*pParameters	= i < 0 ? pModule->Get_Parameters() : pModule->Get_Parameters(i);

		for(int j=0; j<pParameters->Get_Count(); j++)
		{
			CSG_Parameter	*pParameter	= pParameters->Get_Parameter(j);

			if( pParameter->is_Output() )
			{
				if( !Data_Exists(pParameter->asDataObject()) )
				{
					m_Data_Manager.Delete(pParameter->asDataObject());
				}
			}
			else if( pParameter->is_DataObject_List() )
			{
				for(int k=0; k<m_Data.Get_Count(); k++)
				{
					if( !Data_Exists(pParameter->asList()->asDataObject(k)) )
					{
						m_Data_Manager.Delete(pParameter->asList()->asDataObject(k));
					}
				}
			}
		}
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

	Chain.Set_Name    ("toolchain"  );
	Chain.Add_Property("version"    , TOOL_CHAIN_VERSION);

	Chain.Add_Child   ("group"      , "toolchains");
	Chain.Add_Child   ("identifier" , SG_File_Get_Name(File, false));
	Chain.Add_Child   ("name"       , SG_File_Get_Name(File, false));
	Chain.Add_Child   ("description", _TL("created from history"));

	Chain.Add_Child   ("parameters" );
	Chain.Add_Child   ("tools"      );

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

	//-----------------------------------------------------
	CSG_MetaData	*pParameter, &Tool	= *Tools.Ins_Child("tool", 0);

	CSG_String		Tool_ID(CSG_String::Format("tool_%02d", Tools.Get_Children_Count()));

	Tool.Add_Property("id"     , Tool_ID);
	Tool.Add_Property("library", History.Get_Property("library"));
	Tool.Add_Property("module" , History.Get_Property("id"     ));
	Tool.Add_Property("name"   , History.Get_Property("name"   ));

	//-----------------------------------------------------
	const CSG_MetaData	&Output	= History["OUTPUT"];

	CSG_String	VarName	= CSG_String::Format("%s__%s", Tool_ID, Output.Get_Property("id"));

	pParameter	= Tool.Add_Child("output", VarName);
	pParameter->Add_Property("id"   , Output.Get_Property("id"   ));
	pParameter->Add_Property("parms", Output.Get_Property("parms"));

	if( bAddOutput )
	{
		pParameter	= Parms.Add_Child("output");

		pParameter->Add_Property("varname"    , VarName);
		pParameter->Add_Property("type"       , Output.Get_Property("type"));
	//	pParameter->Add_Property("tool"       , Tool_ID);
	//	pParameter->Add_Property("id"         , Output.Get_Property("id"  ));
		pParameter->Add_Child   ("name"       , Output.Get_Property("name"));
	//	pParameter->Add_Child   ("description", Output.Get_Property("name"));
	}

	//-----------------------------------------------------
	for(int i=0; i<History.Get_Children_Count(); i++)	// Options and Input
	{
		CSG_MetaData	*pChild	= History.Get_Child(i);

		if( !pChild->Get_Name().Cmp("OPTION") )
		{
			pParameter	= NULL;

			switch( SG_Parameter_Type_Get_Type(pChild->Get_Property("type")) )
			{
			case PARAMETER_TYPE_Bool        :
			case PARAMETER_TYPE_Int         :
			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      :
			case PARAMETER_TYPE_Range       :
			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
				pParameter	= Tool.Add_Child("option", pChild->Get_Content());
				break;

			case PARAMETER_TYPE_Choice      :
				pParameter	= Tool.Add_Child("option", pChild->Get_Property("index"));
				break;

			case PARAMETER_TYPE_Grid_System :
				if( pChild->Get_Children_Count() == 0 )
				{
					pParameter	= Tool.Add_Child("option", pChild->Get_Content());
				}
				break;

			default: break;
			}

			if( pParameter )
			{
				pParameter->Add_Property("id"   , pChild->Get_Property("id"   ));
				pParameter->Add_Property("parms", pChild->Get_Property("parms"));
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
	CSG_MetaData	*pInput	= Tool.Add_Child("input");

	pInput->Add_Property("id"   , History.Get_Property("id"   ));
	pInput->Add_Property("parms", History.Get_Property("parms"));

	if( History("MODULE") && History["MODULE"]("OUTPUT") )
	{
		pInput->Fmt_Content(SG_T("tool_%02d::%s"), Tool.Get_Parent()->Get_Children_Count() + 1, History["MODULE"]["OUTPUT"].Get_Property("id"));

		return( _Save_History_Add_Tool(History["MODULE"], Parms, *Tool.Get_Parent()) );
	}

	CSG_String	VarName	= CSG_String::Format("%s__%s", Tool.Get_Property("id"), History.Get_Property("id"));

	pInput->Set_Content(VarName);

	CSG_MetaData	*pParameter	= Parms.Add_Child("input");

	pParameter->Add_Property("varname"    , VarName);
	pParameter->Add_Property("type"       , History.Get_Property("type"));
	pParameter->Add_Child   ("name"       , History.Get_Property("name"));
//	pParameter->Add_Child   ("description", History.Get_Property("name"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
