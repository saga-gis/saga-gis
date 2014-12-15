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
#define GET_XML_CONTENT(md, id, def)	(md(id) ? md(id)->Get_Content() : CSG_String(def))


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

//---------------------------------------------------------
void CSG_Module_Chain::Set_Library_Menu(const CSG_String &Menu)
{
	m_Library_Menu	= Menu;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Create(const CSG_String &File)
{
	Reset();

	//-----------------------------------------------------
	if( !m_Chain.Load(File) || !m_Chain.Cmp_Name("toolchain")
	||  SG_Compare_Version(m_Chain.Get_Property("saga-version"), "2.1.3") < 0
	||  !m_Chain("identifier") || !m_Chain("parameters") )
	{
		Reset();

		return( false );
	}

	//-----------------------------------------------------
	m_File_Name		= File;

	m_ID			= GET_XML_CONTENT(m_Chain, "identifier" , "");
	m_Library		= GET_XML_CONTENT(m_Chain, "group"      , "toolchains");
	m_Menu			= GET_XML_CONTENT(m_Chain, "menu"       , "");
	Set_Name         (GET_XML_CONTENT(m_Chain, "name"       , _TL("Not Named")));
	Set_Author       (GET_XML_CONTENT(m_Chain, "author"     , _TL("unknown")));
	Set_Description  (GET_XML_CONTENT(m_Chain, "description", _TL("no description")));

	if( !m_Menu.is_Empty() && (m_Menu.Length() < 2 || m_Menu[1] != ':') )
	{
		if( m_Chain["menu"].Cmp_Property("absolute", "true", true) || m_Chain["menu"].Cmp_Property("absolute", "1") )
			m_Menu.Prepend("A:");	// absolute path
		else
			m_Menu.Prepend("R:");	// relative path
	}

	//-----------------------------------------------------
	for(int i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		int			Constraint	= 0;
		CSG_String	Value, ID	= Parameter.Get_Property("varname");

		if( Parameter.Cmp_Name("input") )
		{
			Constraint	= PARAMETER_INPUT;
		}
		else if( Parameter.Cmp_Name("output") )
		{
			Constraint	= PARAMETER_OUTPUT;
		}
		else if( Parameter.Cmp_Name("option") )
		{
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
		case PARAMETER_TYPE_Grid           : Parameter.Cmp_Property("target", "none") 
			                               ? Parameters.Add_Grid_Output    (   NULL, ID, Name, Desc)
			                               : Parameters.Add_Grid           (pParent, ID, Name, Desc, Constraint);	break;
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
bool CSG_Module_Chain::Data_Add(const CSG_String &ID, CSG_Parameter *pData)
{
	if( !pData )
	{
		return( false );
	}

	switch( pData->Get_Type() )
	{
	case PARAMETER_TYPE_PointCloud     : m_Data.Add_PointCloud     (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_Grid           : m_Data.Add_Grid           (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_Table          : m_Data.Add_Table          (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_Shapes         : m_Data.Add_Shapes         (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_TIN            : m_Data.Add_TIN            (NULL, ID, "", "", 0)->Assign(pData);	break;

	case PARAMETER_TYPE_PointCloud_List: m_Data.Add_PointCloud_List(NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_Grid_List      : m_Data.Add_Grid_List      (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_Table_List     : m_Data.Add_Table_List     (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_Shapes_List    : m_Data.Add_Shapes_List    (NULL, ID, "", "", 0)->Assign(pData);	break;
	case PARAMETER_TYPE_TIN_List       : m_Data.Add_TIN_List       (NULL, ID, "", "", 0)->Assign(pData);	break;

	default: return( false );
	}

	if( pData->is_DataObject() )
	{
		m_Data_Manager.Add(pData->asDataObject());
	}
	else if( pData->is_DataObject_List() )
	{
		for(int i=0; i<pData->asList()->Get_Count(); i++)
		{
			m_Data_Manager.Add(pData->asList()->asDataObject(i));
		}
	}

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
		Data_Add(Parameters(i)->Get_Identifier(), Parameters(i));
	}

	return( m_Data.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Data_Finalize(void)
{
	int		i;

	//-----------------------------------------------------
	for(i=0; i<Parameters.Get_Count(); i++)	// detach non temporary data before freeing the local data manager !!!
	{
		if( Parameters(i)->is_DataObject() )
		{
			if( Parameters(i)->Get_Type() == PARAMETER_TYPE_DataObject_Output && m_Data(Parameters(i)->Get_Identifier()) )
			{
				Parameters(i)->Set_Value(m_Data(Parameters(i)->Get_Identifier())->asDataObject());
			}

			m_Data_Manager.Delete(Parameters(i)->asDataObject(), true);
		}
		else if( Parameters(i)->is_DataObject_List() )
		{
			for(int j=0; j<Parameters(i)->asList()->Get_Count(); j++)
			{
				m_Data_Manager.Delete(Parameters(i)->asList()->asDataObject(j), true);
			}
		}
	}

	m_Data_Manager.Delete_All();

	m_Data.Destroy();

	//-----------------------------------------------------
	for(i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_Parameter	*pParameter	= Parameters(Parameter.Get_Property("varname"));

			if( pParameter && pParameter->is_DataObject() && pParameter->asDataObject() )
			{
				if( Parameter("output_name") && !Parameter["output_name"].Get_Content().is_Empty() )
				{
					pParameter->asDataObject()->Set_Name(Parameter["output_name"].Get_Content());
				}

				if( Parameter("colours") )
				{
					DataObject_Set_Colors(pParameter->asDataObject(), 11,
						Parameter["colours"].Get_Content().asInt(),
						Parameter["colours"].Cmp_Property("revert", "true", true) || Parameter["colours"].Cmp_Property("revert", "1")
					);
				}
			}
		}
	}

	//-----------------------------------------------------
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
	Message_Add(CSG_String::Format("\n%s: %s", _TL("Run Tool"), pModule->Get_Name().c_str()), false);

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

	int		i;

	CSG_String	Tool_ID	= Tool.Get_Property("id");

	//-----------------------------------------------------
	for(i=0; i<Tool.Get_Children_Count(); i++)	// set data objects first
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
			if( Parameter.Cmp_Property("varname", "1") || Parameter.Cmp_Property("varname", "true", true) )
			{	// does option want a value from tool chain parameters and do these provide one ?
				pParameter->Assign(Parameters(Parameter.Get_Content()));
			}
			else
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

			if( pParameter->is_DataObject() || pParameter->is_DataObject_List() )
			{
				if( pParameter->Get_Type() == pData->Get_Type() )
				{
					if( !pParameter->Assign(pData) )
					{
						return( false );
					}
				}
				else if( pParameter->is_DataObject_List() && pData->is_DataObject() )
				{
					pParameter->asList()->Add_Item(pData->asDataObject());
				}

				pParameter->has_Changed();
			}
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
	for(i=0; i<Tool.Get_Children_Count(); i++)	// now set options
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
			if( Parameter.Cmp_Property("varname", "1") || Parameter.Cmp_Property("varname", "true", true) )
			{	// does option want a value from tool chain parameters and do these provide one ?
				pParameter->Assign(Parameters(Parameter.Get_Content()));
			}
			else
			{
				pParameter->Set_Value(Parameter.Get_Content());
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
	for(i=0; i<Tool.Get_Children_Count(); i++)	// add all data objects declared as output to variable list
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_String	ID	= Parameter.Get_Property("id");

			CSG_Parameter	*pParameter	= pModule->Get_Parameters(Parameter.Get_Property("parms"))
				? pModule->Get_Parameters(Parameter.Get_Property("parms"))->Get_Parameter(ID)
				: pModule->Get_Parameters()->Get_Parameter(ID);

			if( !pParameter || !Data_Add(Parameter.Get_Content(), pParameter) )
			{
				return( false );
			}
		}
	}

	//-----------------------------------------------------
	for(i=-1; i<pModule->Get_Parameters_Count(); i++)	// save memory: free all data objects that have not been added to variable list
	{
		CSG_Parameters	*pParameters	= i < 0 ? pModule->Get_Parameters() : pModule->Get_Parameters(i);

		for(int j=0; j<pParameters->Get_Count(); j++)
		{
			CSG_Parameter	*pParameter	= pParameters->Get_Parameter(j);

			if( pParameter->is_Output() )
			{
				if( pParameter->is_DataObject() )
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
CSG_Module_Chains::CSG_Module_Chains(const CSG_String &Library_Name, const CSG_String &Path)
{
	m_Library_Name	= Library_Name;

	if( m_Library_Name.is_Empty() )
	{
		m_Library_Name	= "toolchains";
		m_Name			= _TL("Tool Chains");
		m_Description	= _TL("Unsorted tool chains");
		m_Menu			= _TL("Tool Chains");
	}
	else
	{
		CSG_MetaData	XML(SG_File_Make_Path(Path, Library_Name, SG_T("xml")));

		if( !XML.Cmp_Name("toolchains") )
		{
			XML.Destroy();
		}

		m_Name			= GET_XML_CONTENT(XML, "name"       , m_Library_Name);
		m_Description	= GET_XML_CONTENT(XML, "description", _TL("no description"));
		m_Menu			= GET_XML_CONTENT(XML, "menu"       , _TL("Tool Chains"));
	}

	//-----------------------------------------------------
	m_nModules	= 0;
	m_pModules	= NULL;
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
	case MLB_INFO_Category   :	return( _TL("Tool Chains") );
	}

	return( "" );
}

//---------------------------------------------------------
bool CSG_Module_Chains::Add_Module(CSG_Module_Chain *pModule)
{
	m_pModules	= (CSG_Module_Chain **)SG_Realloc(m_pModules, (m_nModules + 1) * sizeof(CSG_Module_Chain *));
	m_pModules[m_nModules++]	= pModule;

	pModule->Set_Library_Menu(Get_Info(MLB_INFO_Menu_Path));

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
	if( SG_Compare_Version(History.Get_Property("saga-version"), "2.1.3") < 0
	||  !History("MODULE") || !History("MODULE")->Get_Child("OUTPUT") )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	Chain;

	Chain.Set_Name    ("toolchain"  );
	Chain.Add_Property("saga-version", SAGA_VERSION);

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
	if( !History("OUTPUT") || !History["OUTPUT"].Get_Property("id") )
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

	CSG_String	VarName	= CSG_String::Format("%s__%s", Tool_ID.c_str(), Output.Get_Property("id"));

	pParameter	= Tool.Add_Child("output", VarName);
	pParameter->Add_Property("parms", Output.Get_Property("parms"));
	pParameter->Add_Property("id"   , Output.Get_Property("id"   ));

	if( bAddOutput )
	{
		pParameter	= Parms.Add_Child("output");

		pParameter->Add_Property("varname"    , VarName);
		pParameter->Add_Property("type"       , Output.Get_Property("type"));
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
				pParameter->Add_Property("parms", pChild->Get_Property("parms"));
				pParameter->Add_Property("id"   , pChild->Get_Property("id"   ));
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

	pInput->Add_Property("parms", History.Get_Property("parms"));
	pInput->Add_Property("id"   , History.Get_Property("id"   ));

	if( History("MODULE") && History["MODULE"]("OUTPUT") && History["MODULE"]["OUTPUT"].Get_Property("id") )
	{
		pInput->Fmt_Content("tool_%02d__%s", Tool.Get_Parent()->Get_Children_Count() + 1, History["MODULE"]["OUTPUT"].Get_Property("id"));

		return( _Save_History_Add_Tool(History["MODULE"], Parms, *Tool.Get_Parent()) );
	}

	CSG_String	VarName	= CSG_String::Format("%s__%s", Tool.Get_Property("id"), History.Get_Property("id"));

	pInput->Set_Content(VarName);

	CSG_MetaData	*pParameter	= Parms.Ins_Child("input", 0);

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
