
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
//                    tool_chain.cpp                     //
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#include "saga_api.h"

#include "tool_chain.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_XML_CONTENT(XML, ID, DEFAULT, TRANSLATE)	(!XML(ID) ? CSG_String(DEFAULT) : !TRANSLATE ? XML[ID].Get_Content() : CSG_String(SG_Translate(XML[ID].Get_Content())))

#define IS_TRUE_STRING(String)           (!String.CmpNoCase("true") || !String.CmpNoCase("1"))
#define IS_TRUE_PROPERTY(Item, Property) (Item.Cmp_Property(Property, "true", true) || Item.Cmp_Property(Property, "1"))

#define Get_List_Count(p)   (p->asGridList() ? p->asGridList()->Get_Grid_Count() : p->asList() ? p->asList()->Get_Item_Count() : 0)
#define Get_List_Item(p, i) (p->asGridList() ? p->asGridList()->Get_Grid     (i) : p->asList() ? p->asList()->Get_Item     (i) : NULL)


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Chain::CSG_Tool_Chain(void)
{
	// nop
}

//---------------------------------------------------------
CSG_Tool_Chain::CSG_Tool_Chain(const CSG_Tool_Chain &Tool, bool bWithGUI, bool bWithCMD)
{
	Create(Tool, bWithGUI, bWithCMD);
}

//---------------------------------------------------------
CSG_Tool_Chain::CSG_Tool_Chain(const CSG_String &File)
{
	Create(File);
}

//---------------------------------------------------------
CSG_Tool_Chain::CSG_Tool_Chain(const CSG_MetaData &Chain)
{
	Create(Chain);
}

//---------------------------------------------------------
CSG_Tool_Chain::~CSG_Tool_Chain(void)
{
	Reset();
}

//---------------------------------------------------------
void CSG_Tool_Chain::Reset(void)
{
	Parameters.Del_Parameters();
	Parameters.Del_References();

	m_Chain.Destroy();

	m_Conditions.Destroy();

	m_Menu.Clear();
}

//---------------------------------------------------------
void CSG_Tool_Chain::Set_Library_Menu(const CSG_String &Menu)
{
	m_Library_Menu	= Menu;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Create(const CSG_Tool_Chain &Tool, bool bWithGUI, bool bWithCMD)
{
	if( !Create(Tool.m_Chain) )
	{
		return( false );
	}

	m_ID           = Tool.m_ID;
	m_Library      = Tool.m_Library;
	m_Library_Menu = Tool.m_Library_Menu;
	m_File_Name	   = Tool.m_File_Name;
	m_bGUI         = bWithGUI && m_bGUI;
	m_bCMD         = bWithCMD && m_bCMD;

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Create(const CSG_String &File)
{
	if( !SG_File_Cmp_Extension(File, "xml") )
	{
		return( false );
	}

	if( File.Right(sizeof(".pyt.xml")).Make_Lower().Find(".pyt.xml") >= 0 )
	{
		return( false );
	}

	CSG_MetaData Chain;

	if( !Chain.Load(File) )
	{
		Error_Fmt("%s: %s", _TL("failed to load or parse xml file"), File.c_str());

		return( false );
	}

	if( Chain.Cmp_Name("toolchains") )	// don't report any error, this xml-file provides info for a category of tool chains
	{
		return( false );
	}

	if( !Chain.Cmp_Name("toolchain") || !Chain("identifier") || !Chain("parameters") )
	{
		Error_Fmt("%s: %s", _TL("xml file is not a valid tool chain"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", File.Cmp(m_File_Name) ? _TL("Loading tool chain") : _TL("Reloading tool chain"), File.c_str()), true);

	if( Create(Chain) )
	{
		m_File_Name	= File;

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	m_File_Name.Clear(); Reset();

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Create(const CSG_MetaData &Chain)
{
	if( !Chain.Cmp_Name("toolchain") || !Chain("identifier") || !Chain("parameters") || !Chain("tools") )
	{
		return( false );
	}

	if( SG_Compare_Version(Chain.Get_Property("saga-version"), "2.1.3") < 0 )
	{
		Error_Fmt("%s %s: %s", _TL("Warning"), _TL("unsupported tool chain version"), Chain.Get_Property("saga-version"));
	}

	//-----------------------------------------------------
	Reset();

	m_Chain			= Chain;
	m_ID			= GET_XML_CONTENT(m_Chain, "identifier" ,     ""               , false) ;
	m_Library		= GET_XML_CONTENT(m_Chain, "group"      ,     "toolchains"     , false) ;
	m_Menu			= GET_XML_CONTENT(m_Chain, "menu"       ,     ""               ,  true) ;
	Set_Name         (GET_XML_CONTENT(m_Chain, "name"       , _TL("Not Named"     ),  true));
	Set_Author       (GET_XML_CONTENT(m_Chain, "author"     , _TL("unknown"       ), false));
	Set_Description  (GET_XML_CONTENT(m_Chain, "description", _TL("no description"),  true));

	if( m_Library.is_Empty() )
	{
		m_Library      = "toolchains";
	}

	Add_References();

	CSG_String	Description	= Get_Description();
	Description.Replace("[[", "<");	// support for xml/html tags
	Description.Replace("]]", ">");
	Set_Description(Description);

	if( !m_Menu.is_Empty() && (m_Menu.Length() < 2 || m_Menu[1] != ':') )
	{
		if( IS_TRUE_PROPERTY(m_Chain["menu"], "absolute") )
			m_Menu.Prepend("A:");	// absolute path
		else
			m_Menu.Prepend("R:");	// relative path
	}

	m_bAddHistory	= IS_TRUE_PROPERTY(m_Chain["tools"], "history");

	//-----------------------------------------------------
	for(int i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		CSG_String	ID	= Parameter.Get_Property("varname");

		if( ID.is_Empty() || Parameters(ID) )
		{
			continue;
		}

		if( Parameter.Get_Property("with_gui") && !IS_TRUE_PROPERTY(Parameter, "with_gui") && has_GUI() == false )
		{
			continue;
		}

		if( Parameter.Get_Property("with_cmd") && !IS_TRUE_PROPERTY(Parameter, "with_cmd") && has_GUI() != false )
		{
			continue;
		}

		//-------------------------------------------------
		int			Constraint	= 0;
		CSG_String	Value;
		bool		bMin = false, bMax = false;
		double		 Min = 0.   ,  Max = 0.;

		if( Parameter.Cmp_Name("input") )
		{
			Constraint	= IS_TRUE_PROPERTY(Parameter, "optional") ? PARAMETER_INPUT_OPTIONAL : PARAMETER_INPUT;
		}
		else if( Parameter.Cmp_Name("output") )
		{
			Constraint	= IS_TRUE_PROPERTY(Parameter, "optional") ? PARAMETER_OUTPUT_OPTIONAL : PARAMETER_OUTPUT;
		}
		else if( Parameter.Cmp_Name("option") && Parameter("value") )
		{
			Value	= Parameter.Get_Content("value");

			bMin	= Parameter["value"].Get_Property("min", Min);
			bMax	= Parameter["value"].Get_Property("max", Max);
		}

		//-------------------------------------------------
		if( Parameter("condition") )
		{
			CSG_MetaData	&Conditions	= *m_Conditions.Add_Child(ID);

			for(int j=0; j<Parameter.Get_Children_Count(); j++)	// there might be more than one condition to be checked
			{
				if( Parameter[j].Cmp_Name("condition") )
				{
					Conditions.Add_Child(Parameter[j]);
				}
			}
		}

		//-------------------------------------------------
		CSG_String	Name	= SG_Translate(Parameter.Get_Content("name"       ));
		CSG_String	Desc	= SG_Translate(Parameter.Get_Content("description"));

		CSG_Parameter	*pParent	= Parameters(Parameter.Get_Property("parent"));
		CSG_String		ParentID(pParent ? pParent->Get_Identifier() : SG_T(""));

		//-------------------------------------------------
		switch( SG_Parameter_Type_Get_Type(Parameter.Get_Property("type")) )
		{
		case PARAMETER_TYPE_Node             : Parameters.Add_Node           (ParentID, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Bool             : Parameters.Add_Bool           (ParentID, ID, Name, Desc, IS_TRUE_STRING(Value));	break;
		case PARAMETER_TYPE_Int              : Parameters.Add_Int            (ParentID, ID, Name, Desc, Value.asInt   (), (int)Min, bMin, (int)Max, bMax);	break;
		case PARAMETER_TYPE_Double           : Parameters.Add_Double         (ParentID, ID, Name, Desc, Value.asDouble(),      Min, bMin,      Max, bMax);	break;
		case PARAMETER_TYPE_Degree           : Parameters.Add_Degree         (ParentID, ID, Name, Desc, Value.asDouble(),      Min, bMin,      Max, bMax);	break;

		case PARAMETER_TYPE_Date             : Parameters.Add_Date           (ParentID, ID, Name, Desc, 0.)->Set_Value(Value);	break;

		case PARAMETER_TYPE_Range            : Parameters.Add_Range          (ParentID, ID, Name, Desc, Value.BeforeFirst(';').asDouble(), Value.AfterFirst(';').asDouble(), Min, bMin, Max, bMax);	break;
		case PARAMETER_TYPE_Data_Type        :
		case PARAMETER_TYPE_Choice           : Parameters.Add_Choice         (ParentID, ID, Name, Desc, Parameter.Get_Content("choices"))->Set_Value(Value);	break;
		case PARAMETER_TYPE_Choices          : Parameters.Add_Choices        (ParentID, ID, Name, Desc, Parameter.Get_Content("choices"))->Set_Value(Value);	break;

		case PARAMETER_TYPE_String           : Parameters.Add_String         (ParentID, ID, Name, Desc, Value, false);	break;
		case PARAMETER_TYPE_Text             : Parameters.Add_String         (ParentID, ID, Name, Desc, Value,  true);	break;

		case PARAMETER_TYPE_FilePath         : Parameters.Add_FilePath       (ParentID, ID, Name, Desc, Parameter.Get_Content("filter"), Value,
												IS_TRUE_PROPERTY(Parameter, "save"     ),
												IS_TRUE_PROPERTY(Parameter, "directory"),
												IS_TRUE_PROPERTY(Parameter, "multiple" )
											); break;

		case PARAMETER_TYPE_Font             : break;
		case PARAMETER_TYPE_Color            : Parameters.Add_Color          (ParentID, ID, Name, Desc, Value.asInt());	break;
		case PARAMETER_TYPE_Colors           : Parameters.Add_Colors         (ParentID, ID, Name, Desc);	break;
		case PARAMETER_TYPE_FixedTable       : Parameters.Add_FixedTable     (ParentID, ID, Name, Desc)->Serialize(*Parameter("option"), false);	break;

		case PARAMETER_TYPE_Grid_System      : Parameters.Add_Grid_System    (ParentID, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Table_Field      : Parameters.Add_Table_Field    (ParentID, ID, Name, Desc, (!Value.CmpNoCase("true") || !Value.CmpNoCase("1")));	break;
		case PARAMETER_TYPE_Table_Fields     : Parameters.Add_Table_Fields   (ParentID, ID, Name, Desc);	break;

		case PARAMETER_TYPE_Grid             : Parameter.Cmp_Property("target", "none") 
			                                 ? Parameters.Add_Grid_Output    (      "", ID, Name, Desc)
			                                 : Parameters.Add_Grid           (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Grids            : Parameter.Cmp_Property("target", "none") 
			                                 ? Parameters.Add_Grids_Output   (      "", ID, Name, Desc)
			                                 : Parameters.Add_Grids          (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Table            : Parameters.Add_Table          (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Shapes           : Parameters.Add_Shapes         (ParentID, ID, Name, Desc, Constraint,
												Parameter.Cmp_Property("feature_type", "point"  ) ? SHAPE_TYPE_Point   :
												Parameter.Cmp_Property("feature_type", "points" ) ? SHAPE_TYPE_Points  :
												Parameter.Cmp_Property("feature_type", "line"   ) ? SHAPE_TYPE_Line    :
												Parameter.Cmp_Property("feature_type", "polygon") ? SHAPE_TYPE_Polygon : SHAPE_TYPE_Undefined
											); break;

		case PARAMETER_TYPE_TIN              : Parameters.Add_TIN            (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_PointCloud       : Parameters.Add_PointCloud     (ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_Grid_List        : Parameters.Add_Grid_List      (ParentID, ID, Name, Desc, Constraint, !IS_TRUE_PROPERTY(Parameter, "no_system"));	break;
		case PARAMETER_TYPE_Grids_List       : Parameters.Add_Grids_List     (ParentID, ID, Name, Desc, Constraint, !IS_TRUE_PROPERTY(Parameter, "no_system"));	break;
		case PARAMETER_TYPE_Table_List       : Parameters.Add_Table_List     (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_Shapes_List      : Parameters.Add_Shapes_List    (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_TIN_List         : Parameters.Add_TIN_List       (ParentID, ID, Name, Desc, Constraint);	break;
		case PARAMETER_TYPE_PointCloud_List  : Parameters.Add_PointCloud_List(ParentID, ID, Name, Desc, Constraint);	break;

		case PARAMETER_TYPE_DataObject_Output: break;
		case PARAMETER_TYPE_Parameters       : break;	// to do ?

		default: break;
		}
	}

	//-----------------------------------------------------
	return( is_Okay() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::do_Sync_Projections(void)	const
{
	return( !m_Chain("CRS_SYNC") || IS_TRUE_STRING(m_Chain["CRS_SYNC"].Get_Content()) );
}

//---------------------------------------------------------
void CSG_Tool_Chain::Add_References(void)
{
	for(int i=0; i<m_Chain.Get_Children_Count(); i++)
	{
		if( !m_Chain[i].Get_Name().CmpNoCase("REFERENCE") )
		{
			CSG_String	Authors, Year, Title, Where, Link, Link_Text, DOI;

			if( m_Chain[i]("AUTHORS"  ) ) Authors   = m_Chain[i].Get_Content("AUTHORS"  );
			if( m_Chain[i]("YEAR"     ) ) Year      = m_Chain[i].Get_Content("YEAR"     );
			if( m_Chain[i]("TITLE"    ) ) Title     = m_Chain[i].Get_Content("TITLE"    );
			if( m_Chain[i]("WHERE"    ) ) Where     = m_Chain[i].Get_Content("WHERE"    );
			if( m_Chain[i]("LINK"     ) ) Link      = m_Chain[i].Get_Content("LINK"     );
			if( m_Chain[i]("LINK_TEXT") ) Link_Text = m_Chain[i].Get_Content("LINK_TEXT");
			if( m_Chain[i]("DOI"      ) ) DOI       = m_Chain[i].Get_Content("DOI"      );

			if( !DOI.is_Empty() )
			{
				Link = "https://doi.org/" + DOI; Link_Text = "doi:" + DOI;
			}

			if( !Authors.is_Empty() && !Year.is_Empty() && !Title.is_Empty() )
			{
				Add_Reference(Authors, Year, Title, Where, Link.c_str(), Link_Text.c_str());
			}
			else if( !Link.is_Empty() )
			{
				Add_Reference(Link, Link_Text.c_str());
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Tool_Chain::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	for(int iParameter=0; iParameter<m_Conditions.Get_Children_Count(); iParameter++)
	{
		const CSG_MetaData	&Conditions	= m_Conditions[iParameter];

		if( (*pParameters)(Conditions.Get_Name()) )
		{
			bool	bEnable	= true;

			for(int iCondition=0; bEnable && iCondition<Conditions.Get_Children_Count(); iCondition++)
			{
				bEnable	= Check_Condition(Conditions[iCondition], pParameters);
			}

			(*pParameters)(Conditions.Get_Name())->Set_Enabled(bEnable);
		}
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::On_Execute(void)
{
	bool bResult = Data_Initialize();

	if( !bResult )
	{
		Error_Set(_TL("no data objects"));
	}

	for(int i=0; bResult && i<m_Chain["tools"].Get_Children_Count(); i++)
	{
		bResult = Tool_Run(m_Chain["tools"][i]);
	}

	Data_Finalize();

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Add(const CSG_String &ID, CSG_Parameter *pData)
{
	if( !pData )
	{
		return( false );
	}

	CSG_Parameter *pParameter = m_Data(ID);	// check if the local data manager already has a parameter with this identifier

	if( pParameter )	// it has, so don't add twice!
	{
		if( pParameter->is_Input() && pParameter->Get_Type() != pData->Get_Type() )	// don't allow to change parameter's data object type!
		{
			Error_Fmt("%s\n[%s] %s <> %s", _TL("Tool chain uses same variable name for different data object types."),
				ID.c_str(), pParameter->Get_Type_Identifier().c_str(), pData->Get_Type_Identifier().c_str()
			);

			return( false );
		}
	}

	//-----------------------------------------------------
	else switch( pData->Get_Type() )
	{
	case PARAMETER_TYPE_PointCloud     : pParameter	= m_Data.Add_PointCloud     ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Grid           : pParameter	= m_Data.Add_Grid           ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Grids          : pParameter	= m_Data.Add_Grids          ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Table          : pParameter	= m_Data.Add_Table          ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Shapes         : pParameter	= m_Data.Add_Shapes         ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_TIN            : pParameter	= m_Data.Add_TIN            ("", ID, "", "", 0       );	break;

	case PARAMETER_TYPE_PointCloud_List: pParameter	= m_Data.Add_PointCloud_List("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Grid_List      : pParameter	= m_Data.Add_Grid_List      ("", ID, "", "", 0, false);	break;
	case PARAMETER_TYPE_Grids_List     : pParameter	= m_Data.Add_Grids_List     ("", ID, "", "", 0, false);	break;
	case PARAMETER_TYPE_Table_List     : pParameter	= m_Data.Add_Table_List     ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_Shapes_List    : pParameter	= m_Data.Add_Shapes_List    ("", ID, "", "", 0       );	break;
	case PARAMETER_TYPE_TIN_List       : pParameter	= m_Data.Add_TIN_List       ("", ID, "", "", 0       );	break;

	case PARAMETER_TYPE_DataObject_Output:
		switch( pData->Get_DataObject_Type() )
		{
		case SG_DATAOBJECT_TYPE_PointCloud: pParameter	= m_Data.Add_PointCloud     ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Grid      : pParameter	= m_Data.Add_Grid           ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Grids     : pParameter	= m_Data.Add_Grids          ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Table     : pParameter	= m_Data.Add_Table          ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_Shapes    : pParameter	= m_Data.Add_Shapes         ("", ID, "", "", 0       );	break;
		case SG_DATAOBJECT_TYPE_TIN       : pParameter	= m_Data.Add_TIN            ("", ID, "", "", 0       );	break;
		default:
			return( true );
		}
		break;

	default:
		return( true );
	}

	//-----------------------------------------------------
	if( pData->is_DataObject() )
	{
		if( pParameter->is_DataObject() )
		{
			pParameter->Set_Value(pData->asDataObject());
		}
		else
		{
			pParameter->asList()->Add_Item(pData->asDataObject());
		}

		m_Data_Manager.Add(pData->asDataObject());
	}
	else if( pData->is_DataObject_List() && pParameter->is_DataObject_List() )
	{
		for(int i=0; i<Get_List_Count(pData); i++)
		{
			pParameter->asList()->Add_Item(Get_List_Item(pData, i));
			m_Data_Manager.Add            (Get_List_Item(pData, i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Add_TempList(const CSG_String &ID, const CSG_String &Type)
{
	if( !m_Data(ID) )
	{
		switch( SG_Parameter_Type_Get_Type(Type) )
		{
		case PARAMETER_TYPE_PointCloud_List: m_Data.Add_PointCloud_List("", ID, "", "", 0       );	break;
		case PARAMETER_TYPE_Grid_List      : m_Data.Add_Grid_List      ("", ID, "", "", 0, false);	break;
		case PARAMETER_TYPE_Grids_List     : m_Data.Add_Grids_List     ("", ID, "", "", 0, false);	break;
		case PARAMETER_TYPE_Table_List     : m_Data.Add_Table_List     ("", ID, "", "", 0       );	break;
		case PARAMETER_TYPE_Shapes_List    : m_Data.Add_Shapes_List    ("", ID, "", "", 0       );	break;
		case PARAMETER_TYPE_TIN_List       : m_Data.Add_TIN_List       ("", ID, "", "", 0       );	break;

		default:
			Error_Fmt("%s: %s [%s|%s]", SG_T("datalist"), _TL("unsupported data list type"), ID.c_str(), Type.c_str());

			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Del_Temp(const CSG_String &ID, bool bData)
{
	CSG_Parameter *pData = m_Data(ID);

	if( pData )
	{
		if( bData )
		{
			if( pData->is_DataObject() )
			{
				m_Data_Manager.Delete(pData->asDataObject());
			}
			else if( pData->is_DataObject_List() )
			{
				for(int i=0; i<Get_List_Count(pData); i++)
				{
					m_Data_Manager.Delete(Get_List_Item(pData, i));
				}
			}
		}

		m_Data.Del_Parameter(ID);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Update(const CSG_String &ID, bool bShow)
{
	CSG_Parameter *pData = m_Data(ID);

	if( pData && pData->asDataObject() && pData->asDataObject()->is_Valid() )
	{
		SG_UI_DataObject_Add(pData->asDataObject(), bShow ? SG_UI_DATAOBJECT_SHOW_MAP : SG_UI_DATAOBJECT_UPDATE);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Exists(CSG_Data_Object *pData)
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
			for(int j=0; j<Get_List_Count(m_Data(i)); j++)
			{
				if( pData == Get_List_Item(m_Data(i), j) )
				{
					return( true );
				}
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Initialize(void)
{
	m_Data.Set_Manager(NULL);

	for(int i=0; i<Parameters.Get_Count(); i++)
	{
		CSG_Parameter &P = Parameters[i];

		if( !(P.is_DataObject() && !P.asDataObject()) )
		{
			if( !Data_Add(P.Get_Identifier(), &P) )
			{
				return( false );
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Data_Finalize(void)
{
	for(int i=0; i<Parameters.Get_Count(); i++)	// detach non temporary data before freeing the local data manager !!!
	{
		CSG_Parameter &P = Parameters[i];

		if( P.is_DataObject() )
		{
			if( m_Data(P.Get_Identifier()) )
			{
				if( P.Get_Type() == PARAMETER_TYPE_DataObject_Output || (P.is_Output() && !P.is_Optional()) )
				{
					P.Set_Value(m_Data(P.Get_Identifier())->asDataObject());
				}
			}

			m_Data_Manager.Delete(P.asDataObject(), true);
		}
		else if( P.is_DataObject_List() )
		{
			if( P.is_Output() && m_Data(P.Get_Identifier()) )	// output lists cannot be up-to-date yet
			{
				CSG_Parameter *pData = m_Data(P.Get_Identifier());

				for(int j=0; j<Get_List_Count(pData); j++)	// csg_parameter::assign() will not work, if parameters data manager is the standard data manager because it checks for existing data sets
				{
					P.asList()->Add_Item(Get_List_Item(pData, j));
				}
			}

			for(int j=0; j<Get_List_Count(Parameters(i)); j++)
			{
				m_Data_Manager.Delete(Get_List_Item(Parameters(i), j), true);
			}
		}
	}

	m_Data_Manager.Delete();

	m_Data.Destroy();

	//-----------------------------------------------------
	for(int i=0; i<m_Chain["parameters"].Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Parameter	= m_Chain["parameters"][i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_Parameter	*pParameter	= Parameters(Parameter.Get_Property("varname"));

			if( pParameter && pParameter->is_DataObject() && pParameter->asDataObject() && pParameter->asDataObject() != DATAOBJECT_CREATE )
			{
				if( Parameter("colours") )
				{
					DataObject_Set_Colors(pParameter->asDataObject(), 11,
						Parameter["colours"].Get_Content().asInt(), IS_TRUE_PROPERTY(Parameter["colours"], "revert")
					);
				}

				if( Parameter("output_name") )
				{
					if( IS_TRUE_PROPERTY(Parameter["output_name"], "input") )
					{
						CSG_Parameter	*pInput	= Parameters(Parameter["output_name"].Get_Content());

						if( pInput && pInput->is_DataObject() && pInput->asDataObject() )
						{
							CSG_String	Suffix;

							if( Parameter["output_name"].Get_Property("suffix", Suffix) && !Suffix.is_Empty() )
							{
								Suffix	= " [" + Suffix + "]";
							}

							pParameter->asDataObject()->Set_Name(pInput->asDataObject()->Get_Name() + Suffix);
						}
					}
					else if( !Parameter["output_name"].Get_Content().is_Empty() )
					{
						pParameter->asDataObject()->Set_Name(Parameter["output_name"].Get_Content());
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Parameter_isCompatible(TSG_Parameter_Type A, TSG_Parameter_Type B)
{
	if( A == B )
	{
		return( true );
	}

	switch( A )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Table:
		return( B == PARAMETER_TYPE_Shapes
			||  B == PARAMETER_TYPE_PointCloud
			||  B == PARAMETER_TYPE_TIN );

	case PARAMETER_TYPE_Shapes:
		return( B == PARAMETER_TYPE_PointCloud );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Message(const CSG_MetaData &Message)
{
	if( Message.Cmp_Name("message") )
	{
		if( IS_TRUE_PROPERTY(Message, "dialog") )
		{
			Message_Dlg(Message.Get_Content());
		}

		Message_Fmt("\n%s", Message.Get_Content().c_str());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Check_Condition(const CSG_MetaData &Condition, CSG_Parameters *pData)
{
	CSG_String Type;

	if( !Condition.Cmp_Name("condition") || !Condition.Get_Property("type", Type) )
	{
		return( true );
	}

	//-----------------------------------------------------
	CSG_String Variable;

	if( !Condition.Get_Property("varname", Variable) && !Condition.Get_Property("variable", Variable) )
	{
		Variable = Condition.Get_Content();
	}

	//-----------------------------------------------------
	if( !Type.CmpNoCase("has_gui"   ) )	// executed from saga_gui ? (tool might offer different parameters if called from saga_cmd, python etc.)
	{
		return( IS_TRUE_STRING(Variable) ? has_GUI() != false : has_GUI() == false );
	}

	//-----------------------------------------------------
	if( !Type.CmpNoCase("exists"    ) )	// data object exists
	{
		CSG_Parameter *pParameter = (*pData)(Variable);

		return( pParameter && ((pParameter->is_DataObject() && pParameter->asDataObject()) || (pParameter->is_DataObject_List() && pParameter->asList()->Get_Item_Count())) );
	}

	if( !Type.CmpNoCase("not_exists") )	// data object does not exist
	{
		return( (*pData)(Variable) == NULL || (*pData)(Variable)->asDataObject() == NULL );
	}

	//-----------------------------------------------------
	CSG_Parameter *pOption = (*pData)(Variable);

	if( pOption == NULL )
	{
		return( true );
	}

	switch( pOption->Get_Type() )
	{
	//-----------------------------------------------------
	case PARAMETER_TYPE_Bool     :
		{
			CSG_String Value;

			if( Condition.Get_Property("value", Value) )
			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	return( (IS_TRUE_STRING(Value) ?  pOption->asBool() : !pOption->asBool()) );	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	return( (IS_TRUE_STRING(Value) ? !pOption->asBool() :  pOption->asBool()) );	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Int      :
	case PARAMETER_TYPE_Color    :
	case PARAMETER_TYPE_Data_Type:
	case PARAMETER_TYPE_Choice   :
		{
			int Value;

			if( Condition.Get_Property("value", Value) )
			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	if( Value != pOption->asInt() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	if( Value == pOption->asInt() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("<") || !Type.CmpNoCase("less"     ) )	{	if( Value >= pOption->asInt() )	{	return( false );	}	}
				else if( !Type.CmpNoCase(">") || !Type.CmpNoCase("greater"  ) )	{	if( Value <= pOption->asInt() )	{	return( false );	}	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Double   :
	case PARAMETER_TYPE_Degree   :
		{
			double Value;

			if( Condition.Get_Property("value", Value) )
			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	if( Value != pOption->asDouble() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	if( Value == pOption->asDouble() )	{	return( false );	}	}
				else if( !Type.CmpNoCase("<") || !Type.CmpNoCase("less"     ) )	{	if( Value >= pOption->asDouble() )	{	return( false );	}	}
				else if( !Type.CmpNoCase(">") || !Type.CmpNoCase("greater"  ) )	{	if( Value <= pOption->asDouble() )	{	return( false );	}	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_String   :
	case PARAMETER_TYPE_Text     :
	case PARAMETER_TYPE_FilePath :
		{
			CSG_String Value;

			Condition.Get_Property("value", Value);	// no 'if', bcos empty string would return false !!

			{
				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	if( Value.Cmp(pOption->asString()) != 0 )	{	return( false );	}	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	if( Value.Cmp(pOption->asString()) == 0 )	{	return( false );	}	}
				else if( !Type.CmpNoCase("<") || !Type.CmpNoCase("less"     ) )	{	if( Value.Cmp(pOption->asString()) >= 0 )	{	return( false );	}	}
				else if( !Type.CmpNoCase(">") || !Type.CmpNoCase("greater"  ) )	{	if( Value.Cmp(pOption->asString()) <= 0 )	{	return( false );	}	}
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Grid_System:
		{
			CSG_String Value;

			if( Condition.Get_Property("value", Value) )
			{
				bool bValid = pOption->asGrid_System()->is_Valid();

				if(      !Type.CmpNoCase("=") || !Type.CmpNoCase("equal"    ) )	{	return( (IS_TRUE_STRING(Value) ?  bValid : !bValid) );	}
				else if( !Type.CmpNoCase("!") || !Type.CmpNoCase("not_equal") )	{	return( (IS_TRUE_STRING(Value) ? !bValid :  bValid) );	}
			}
		}
		break;

	//-----------------------------------------------------
	default:
		// nop
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach(const CSG_MetaData &Commands)
{
	for(int i=0; i<Commands.Get_Children_Count(); i++)	// add internal target lists, if any..
	{
		if( Commands[i].Cmp_Name("output") || Commands[i].Cmp_Name("datalist") )
		{
			Data_Add_TempList(Commands[i].Get_Content(), Commands[i].Get_Property("type"));
		}
	}

	//-----------------------------------------------------
	bool bIgnoreErrors = IS_TRUE_PROPERTY(Commands, "ignore_errors");

	CSG_String VarName;

	if( Commands.Get_Property("varname", VarName) )
	{
		return( ForEach_Iterator(Commands, VarName, bIgnoreErrors) );
	}

	if( Commands.Get_Property("input", VarName) )
	{
		return( ForEach_Object  (Commands, VarName, bIgnoreErrors)
		    ||  ForEach_File    (Commands, VarName, bIgnoreErrors) );
	}

	Error_Set("foreach statement misses iterator or input list definition");

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach_Iterator(const CSG_MetaData &Commands, const CSG_String &VarName, bool bIgnoreErrors)
{
	CSG_Parameter *pIterator = Parameters(VarName);

	if( pIterator )
	{
		Error_Set("foreach statement iterator variable name is already in use");

		return( false );
	}

	//-----------------------------------------------------
	CSG_String s;

	double begin = Commands.Get_Property("begin", s) ? (Parameters(s) ? Parameters[s].asDouble() : s.asDouble()) : 0.;
	double   end = Commands.Get_Property("end"  , s) ? (Parameters(s) ? Parameters[s].asDouble() : s.asDouble()) : 0.;

	if( begin >= end )
	{
		Error_Set("foreach iterator statement with invalid range (define begin < end)");

		return( false );
	}

	double step = 1.;

	if( Commands.Get_Property("steps", s) )
	{
		double steps = Parameters(s) ? Parameters[s].asDouble() : s.asDouble();

		if( steps > 0 )
		{
			step = (end - begin) / steps;
		}
	}
	else if( Commands.Get_Property("step", s) )
	{
		 step = Parameters(s) ? Parameters[s].asDouble() : s.asDouble();
	}

	if( step <= 0. )
	{
		Error_Set("foreach iterator statement with invalid step size (define step > 0 or steps > 0)");

		return( false );
	}

	Message_Fmt("\nfor i = %f to %f step %f (%d steps)", begin, end, step, (int)((end - begin) / step));

	//-----------------------------------------------------
	bool bResult = true;

	pIterator = Parameters.Add_Double("", VarName, "Iterator", "");

	for(double i=begin; bResult && i<=end; i+=step)
	{
		Message_Fmt("\nfor step: %f", i);

		pIterator->Set_Value(i);

		for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
		{
			const CSG_MetaData &Tool = Commands[iTool];

			if( Tool.Cmp_Name("tool") )
			{
				bResult	= Tool_Run(Tool, bIgnoreErrors);

				if( !bResult && bIgnoreErrors )
				{
					bResult = true;
				}
			}
		}
	}

	Parameters.Del_Parameter(VarName);

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach_Object(const CSG_MetaData &Commands, const CSG_String &ListVarName, bool bIgnoreErrors)
{
	CSG_Parameter *pList = m_Data(ListVarName);

	if( !pList )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool bResult = true;

	if( pList->is_DataObject_List() )
	{
		for(int iObject=0; bResult && iObject<Get_List_Count(pList->asList()); iObject++)
		{
			for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
			{
				const CSG_MetaData &Tool = Commands[iTool];

				if( Tool.Cmp_Name("tool") )
				{
					for(int j=0; j<Tool.Get_Children_Count(); j++)
					{
						if( Tool[j].Cmp_Name("input") && Tool[j].Get_Content().Find(ListVarName) == 0 )
						{
							Tool(j)->Set_Content(ListVarName + CSG_String::Format("[%d]", iObject));
						}
					}
				}

				bResult = Tool_Run(Tool, bIgnoreErrors);
			}

			if( !bResult && bIgnoreErrors )
			{
				bResult = true;
			}
		}
	}

	//-----------------------------------------------------
	else if( pList->Get_Type() == PARAMETER_TYPE_Grids )
	{
		for(int iObject=0; bResult && iObject<pList->asGrids()->Get_Grid_Count(); iObject++)
		{
			for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
			{
				const CSG_MetaData &Tool = Commands[iTool];

				if( Tool.Cmp_Name("tool") )
				{
					for(int j=0; j<Tool.Get_Children_Count(); j++)
					{
						if( Tool[j].Cmp_Name("input") && Tool[j].Get_Content().Find(ListVarName) == 0 )
						{
							Tool(j)->Set_Content(ListVarName + CSG_String::Format("[%d]", iObject));
						}
					}
				}

				bResult = Tool_Run(Tool, bIgnoreErrors);
			}

			if( !bResult && bIgnoreErrors )
			{
				bResult = true;
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::ForEach_File(const CSG_MetaData &Commands, const CSG_String &ListVarName, bool bIgnoreErrors)
{
	CSG_Parameter *pList = Parameters(ListVarName);

	if( !pList || pList->Get_Type() != PARAMETER_TYPE_FilePath )
	{
		return( false );
	}

	CSG_Strings Files;

	pList->asFilePath()->Get_FilePaths(Files);

	//-----------------------------------------------------
	bool bResult = true;

	for(int iFile=0; bResult && iFile<Files.Get_Count(); iFile++)
	{
		for(int iTool=0; bResult && iTool<Commands.Get_Children_Count(); iTool++)
		{
			const CSG_MetaData &Tool = Commands[iTool];

			if( Tool.Cmp_Name("tool") )
			{
				CSG_Array_Int Input;

				for(int j=0; j<Tool.Get_Children_Count(); j++)
				{
					if( Tool[j].Cmp_Name("option") && Tool[j].Get_Content().Find(ListVarName) == 0 && IS_TRUE_PROPERTY(Tool[j], "varname") )
					{
						Tool(j)->Set_Content(Files[iFile]);
						Tool(j)->Set_Property("varname", "false");

						Input += j;
					}
				}

				bResult = Tool_Run(Tool, bIgnoreErrors);

				for(size_t i=0; i<Input.Get_uSize(); i++)
				{
					Tool(Input[i])->Set_Content(ListVarName);
					Tool(Input[i])->Set_Property("varname", "true");
				}
			}
			else
			{
				bResult = Tool_Run(Tool, bIgnoreErrors);
			}

			if( !bResult && bIgnoreErrors )
			{
				bResult = true;
			}
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Run(const CSG_MetaData &Tool, bool bShowError)
{
	if( Tool.Cmp_Name("message") )
	{
		Message(Tool);

		return( true );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("comment") )
	{
		return( true );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("datalist") )
	{
		return( Data_Add_TempList(Tool.Get_Content(), Tool.Get_Property("type")) );
	}

	if( Tool.Cmp_Name("delete") )
	{
		return( Data_Del_Temp(Tool.Get_Content(), IS_TRUE_PROPERTY(Tool, "data")) );
	}

	if( Tool.Cmp_Name("update") )
	{
		return( Data_Update(Tool.Get_Content(), IS_TRUE_PROPERTY(Tool, "show")) );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("condition") )
	{
		const CSG_MetaData *pTools = (!Check_Condition(Tool, &m_Data) || !Check_Condition(Tool, &Parameters))
			? Tool("else") : (Tool("if") ? Tool("if") : &Tool);

		for(int i=0; pTools && i<pTools->Get_Children_Count(); i++)
		{
			if( !Tool_Run((*pTools)[i]) )
			{
				return( false );
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	if( Tool.Cmp_Name("foreach") )
	{
		return( ForEach(Tool) );
	}

	//-----------------------------------------------------
	if( !Tool.Cmp_Name("tool") )
	{
		return( true );	// only proceed, if it is tagged as tool...
	}
	
	if( !Tool.Get_Property("library") || !(Tool.Get_Property("tool") || Tool.Get_Property("module")) )
	{
		if( bShowError ) Error_Set(_TL("invalid tool definition"));

		return( false );
	}

	//-----------------------------------------------------
	const SG_Char *Name = Tool.Get_Property("tool") ? Tool.Get_Property("tool") : Tool.Get_Property("module");

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool(Tool.Get_Property("library"), Name,
		IS_TRUE_PROPERTY(Tool, "with_gui")	// this option allows to run a tool in 'gui-mode', e.g. to popup variogram dialogs for kriging interpolation
	);

	if(	!pTool )
	{
		if( bShowError ) Error_Fmt("%s [%s].[%s]", _TL("could not find tool"), Tool.Get_Property("library"), Name);

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(pTool->Get_Name());

	pTool->Settings_Push(&m_Data_Manager);

	bool bResult = false;

	if( !pTool->On_Before_Execution() )
	{
		if( bShowError ) Error_Fmt("%s [%s].[%s]", _TL("before tool execution check failed"), pTool->Get_Library().c_str(), pTool->Get_Name().c_str());
	}
	else if( !Tool_Initialize(Tool, pTool) )
	{
		if( bShowError ) Error_Fmt("%s [%s].[%s]", _TL("tool initialization failed"        ), pTool->Get_Library().c_str(), pTool->Get_Name().c_str());
	}
	else if( !(bResult = pTool->Execute(m_bAddHistory)) )
	{
		Message_Fmt               ("%s [%s].[%s]", _TL("tool execution failed"             ), pTool->Get_Library().c_str(), pTool->Get_Name().c_str());
	}

	if( bResult )
	{
		pTool->On_After_Execution();
	}

	Tool_Finalize(Tool, pTool);

	pTool->Settings_Pop();

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Check_Condition(const CSG_MetaData &Tool)
{
	if( Tool("condition") )
	{
		for(int i=0; i<Tool.Get_Children_Count(); i++)	// there might be more than one condition to be checked
		{
			if( !Check_Condition(Tool[i], &m_Data) )
			{
				return( false );
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Get_Parameter(CSG_String ID, CSG_Parameters *pParameters, CSG_Parameter **ppParameter, CSG_Parameter **ppOwner)
{
	CSG_Parameter	*pParameter	= (*pParameters)(ID.BeforeFirst('.'));

	if( pParameter )
	{
		switch( pParameter->Get_Type() )
		{
		default:
			*ppParameter	= pParameter;

			return( true );

		case PARAMETER_TYPE_Parameters:
			*ppOwner	= pParameter;

			return( Tool_Get_Parameter(ID.AfterFirst('.'), pParameter->asParameters(), ppParameter, ppOwner) );

		case PARAMETER_TYPE_Range:
			if(      !ID.AfterFirst('.').CmpNoCase("min") || !ID.AfterFirst('.').CmpNoCase("minimum") )
			{
				*ppParameter	= pParameter->asRange()->Get_Min_Parameter();
				*ppOwner		= pParameter;
			}
			else if( !ID.AfterFirst('.').CmpNoCase("max") || !ID.AfterFirst('.').CmpNoCase("maximum") )
			{
				*ppParameter	= pParameter->asRange()->Get_Max_Parameter();
				*ppOwner		= pParameter;
			}
			else
			{
				*ppParameter	= pParameter;
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Get_Parameter(const CSG_MetaData &Parameter, CSG_Tool *pTool, CSG_Parameter **ppParameter, CSG_Parameter **ppOwner)
{
	CSG_String	ID(Parameter.Get_Property("parms"));

	if( !ID.is_Empty() )
	{
		ID	+= '.';
	}

	ID	+= Parameter.Get_Property("id");

	//-----------------------------------------------------
	CSG_Parameter	*pOwner; if( !ppOwner ) { ppOwner = &pOwner; } *ppOwner = NULL;

	CSG_Parameters	*pParameters	= pTool->Get_Parameters(ID.BeforeFirst('.'));

	if( pParameters )
	{
		ID	= ID.AfterFirst('.');
	}
	else
	{
		pParameters	= pTool->Get_Parameters();
	}

	return( Tool_Get_Parameter(ID, pParameters, ppParameter, ppOwner) );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Initialize(const CSG_MetaData &Tool, CSG_Tool *pTool)
{
	//-----------------------------------------------------
	for(int i=0; i<Tool.Get_Children_Count(); i++)	// check for invalid parameters...
	{
		const CSG_MetaData &Parameter = Tool[i]; if( Parameter.Cmp_Name("comment") ) { continue; }

		if( Parameter.Cmp_Name("message") )
		{
			Message(Parameter);

			continue;
		}

		CSG_Parameter *pParameter, *pOwner;

		if( !Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner) )
		{
			Error_Fmt("%s: %s", _TL("parameter not found"), Parameter.Get_Property("id"));

			return( false );
		}
	}

	//-----------------------------------------------------
	if( Tool.Get_Property("grid_system") )
	{
		CSG_Parameter *pParameter = Parameters(Tool.Get_Property("grid_system"));

		if( pParameter && pParameter->asGrid_System() )
		{
			if( !pTool->Set_Grid_System(*pParameter->asGrid_System()) )
			{
				Message_Fmt("\n%s: %s\n", _TL("Warning"), _TL("failed to set tool's grid system"));
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<Tool.Get_Children_Count(); i++)	// initialize all options first, some might en-/disable mandatory input data
	{
		const CSG_MetaData &Parameter = Tool[i]; if( !Parameter.Cmp_Name("option") ) { continue; }

		CSG_Parameter *pParameter, *pOwner; Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner);

		if( pParameter->Get_Type() == PARAMETER_TYPE_Grid_System )
		{
			CSG_Parameter *pData = m_Data(Parameter.Get_Content());

			if( pData && pData->asDataObject() && pData->asDataObject()->asGrid() && pData->asDataObject()->asGrid()->Get_System().is_Valid() )
			{
				pParameter->asGrid_System()->Create(pData->asDataObject()->asGrid()->Get_System());

				continue;
			}
		}

		if( IS_TRUE_PROPERTY(Parameter, "varname")
			? pParameter->Set_Value(Parameters(Parameter.Get_Content()))
			: pParameter->Set_Value(           Parameter.Get_Content() ) )
		{
			pParameter->has_Changed(); if( pOwner ) { pOwner->has_Changed(); }
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<Tool.Get_Children_Count(); i++)	// set data input first
	{
		const CSG_MetaData &Parameter = Tool[i]; if( !Parameter.Cmp_Name("input") ) { continue; }

		CSG_Parameter *pParameter, *pOwner; Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner);

		int	Index;

		if( Parameter.Get_Content().Find('[') < 1 || !Parameter.Get_Content().AfterFirst('[').asInt(Index) )
		{
			Index = -1;
		}

		bool bResult = false;

		CSG_Parameter *pData = m_Data(Index < 0 ? Parameter.Get_Content() : Parameter.Get_Content().BeforeFirst('['));

		if( pData && (pParameter->is_DataObject() || pParameter->is_DataObject_List()) )
		{
			if( Index >= 0 && (pData->is_DataObject_List() || pData->asGrids()) )
			{
				CSG_Data_Object	*pObject = pData->is_DataObject_List()
					? pData->asList ()->Get_Item    (Index)
					: pData->asGrids()->Get_Grid_Ptr(Index);

				if( pParameter->is_DataObject() )
				{
					bResult = pParameter->Set_Value(pObject);
				}
				if( pParameter->is_DataObject_List() )
				{
					bResult = pParameter->asList()->Add_Item(pObject);
				}
			}
			else if( pParameter->Get_Type() == pData->Get_Type() )
			{
				if( pParameter->is_DataObject_List() && pParameter->asList()->Get_Item_Count() > 0 )
				{
					bResult = true;

					for(int i=0; bResult && i<pData->asList()->Get_Item_Count(); i++)
					{
						bResult = pParameter->asList()->Add_Item(pData->asList()->Get_Item(i));
					}
				}
				else
				{
					bResult = pParameter->Assign(pData);
				}
			}
			else if( pParameter->is_DataObject_List() && pData->is_DataObject() )
			{
				bResult = pParameter->asList()->Add_Item(pData->asDataObject());
			}
			else if( Parameter_isCompatible(pParameter->Get_Type(), pData->Get_Type()) )
			{
				bResult = pParameter->Set_Value(pData->asDataObject());
			}
		}

		if( bResult )
		{
			pParameter->has_Changed(); if( pOwner ) { pOwner->has_Changed(); }
		}
		else if( pParameter->is_Enabled() )
		{
			Error_Fmt("%s: %s", _TL("set input"), Parameter.Get_Property("id"));

			return( false );
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<Tool.Get_Children_Count(); i++)	// now set all options
	{
		const CSG_MetaData &Parameter = Tool[i]; if( !Parameter.Cmp_Name("option") ) { continue; }

		CSG_Parameter *pParameter, *pOwner; Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner);

		if( IS_TRUE_PROPERTY(Parameter, "varname") )
		{	// does option want a value from tool chain parameters and do these provide one ?
			switch( pParameter->Get_Type() )
			{
			default:
				pParameter->Set_Value(Parameters(Parameter.Get_Content()));
				break;

			case PARAMETER_TYPE_FixedTable:
				if( m_Data(Parameter.Get_Content()) && pParameter->asTable()->Assign_Values(m_Data(Parameter.Get_Content())->asTable()) )
				{
					pParameter->has_Changed();
				}
				break;
			}
		}
		else
		{
			switch( pParameter->Get_Type() )
			{
			case PARAMETER_TYPE_FixedTable:
				if( Parameter("OPTION") )
				{
					pParameter->Serialize(*Parameter("OPTION"), false);
				}
				break;

			default: {
				CSG_String	Value(Parameter.Get_Content());

				if( Value.Find("$(") >= 0 )
				{
					for(int j=0; j<Parameters.Get_Count(); j++)
					{
						CSG_String	Var; Var.Printf("$(%s)", Parameters(j)->Get_Identifier());

						if( Value.Find(Var) >= 0 )
						{
							Value.Replace(Var, Parameters(j)->asString());
						}
					}
				}

				pParameter->Set_Value(Value);

				if( pOwner )
				{
					pOwner->has_Changed();
				}

				break; }
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<Tool.Get_Children_Count(); i++)	// finally set the data output
	{
		const CSG_MetaData &Parameter = Tool[i]; if( !Parameter.Cmp_Name("output") ) { continue; }

		CSG_Parameter *pParameter, *pOwner; Tool_Get_Parameter(Parameter, pTool, &pParameter, &pOwner);

		if( !pParameter->Assign(m_Data(Parameter.Get_Content())) )
		{
			if( pParameter->is_DataObject() )
			{
				pParameter->Set_Value(DATAOBJECT_CREATE);
			}
			else if( pParameter->is_DataObject_List() )
			{
				pParameter->asList()->Del_Items();
			}
			else if( pParameter->asGrids() )
			{
				pParameter->asGrids()->Del_Grids();
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::Tool_Finalize(const CSG_MetaData &Tool, CSG_Tool *pTool)
{
	for(int i=0; i<Tool.Get_Children_Count(); i++)	// add all data objects declared as output to variable list
	{
		const CSG_MetaData	&Parameter	= Tool[i];

		if( Parameter.Cmp_Name("output") )
		{
			CSG_String	ID      (Parameter.Get_Property("id"   ));
			CSG_String	ID_parms(Parameter.Get_Property("parms"));

			CSG_Parameter	*pParameter	= pTool->Get_Parameters(ID_parms)
				? (*pTool->Get_Parameters(ID_parms))(ID)
				: (*pTool->Get_Parameters(        ))(ID);

			if( !pParameter || !Data_Add(Parameter.Get_Content(), pParameter) )
			{
				return( false );
			}
		}
	}

	//-----------------------------------------------------
	for(int i=-1; i<pTool->Get_Parameters_Count(); i++)	// save memory: free all data objects that have not been added to variable list
	{
		CSG_Parameters	*pParameters	= i < 0 ? pTool->Get_Parameters() : pTool->Get_Parameters(i);

		for(int j=0; j<pParameters->Get_Count(); j++)
		{
			CSG_Parameter	*pParameter	= (*pParameters)(j);

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
					for(int k=0; k<Get_List_Count(pParameter); k++)
					{
						if( !Data_Exists(Get_List_Item(pParameter, k)) )
						{
							m_Data_Manager.Delete(Get_List_Item(pParameter, k));
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Chain::Get_Script(CSG_Tool *pTool, bool bHeader, bool bAllParameters)
{
	CSG_MetaData Tool; Tool.Set_Name("tool");

	Tool.Add_Property("library", pTool->Get_Library());
	Tool.Add_Property("tool"   , pTool->Get_ID     ());
	Tool.Add_Property("name"   , pTool->Get_Name   ());

	_Get_Script_Tool(Tool, pTool->Get_Parameters(), bAllParameters, "", bHeader);

	for(int i=0; i<pTool->Get_Parameters_Count(); i++)
	{
		_Get_Script_Tool(Tool, pTool->Get_Parameters(i), bAllParameters, pTool->Get_Parameters(i)->Get_Identifier() + '.', bHeader);
	}

	if( !bHeader )
	{
		return( Tool.asText(1) );
	}

	//-----------------------------------------------------
	CSG_MetaData Parameters;

	_Get_Script_Parameters(Parameters, pTool->Get_Parameters(), "");

	for(int i=0; i<pTool->Get_Parameters_Count(); i++)
	{
		_Get_Script_Parameters(Parameters, pTool->Get_Parameters(i), pTool->Get_Parameters(i)->Get_Identifier() + '.');
	}

	//-----------------------------------------------------
	CSG_MetaData Tools; Tools.Set_Name("toolchain");

	Tools.Add_Property("saga-version", SAGA_VERSION);

	Tools.Add_Child("group"      );
	Tools.Add_Child("identifier" , "define-a-unique-tool-identifier-here");
	Tools.Add_Child("name"       , pTool->Get_Name() + " [Tool Chain]");
	Tools.Add_Child("author"     );
	Tools.Add_Child("description");
	Tools.Add_Child("menu"       , pTool->Get_MenuPath(true))->Add_Property("absolute", "true");
	Tools.Add_Child("crs_sync"   , "true");
	Tools.Add_Child("parameters" )->Add_Children(Parameters);
	Tools.Add_Child("tools"      )->Add_Child(Tool);
	Tools          ("tools"      )->Add_Property("history", "false");

	return( Tools.asText(1) );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::_Get_Script_Tool(CSG_MetaData &Tool, CSG_Parameters *pParameters, bool bAllParameters, const CSG_String &Prefix, bool bVarNames)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter &P = *pParameters->Get_Parameter(iParameter);

		if( !bAllParameters && (!P.is_Enabled(false) || P.is_Information()) )
		{
			continue;
		}

		CSG_MetaData *pChild = NULL;

		switch( P.Get_Type() )
		{
		case PARAMETER_TYPE_Parameters  :
			_Get_Script_Tool(Tool, P.asParameters(), bAllParameters, Prefix + P.Get_Identifier() + '.', true);
			break;

		case PARAMETER_TYPE_Bool        :
			pChild	= Tool.Add_Child("option", P.asBool() ? "true" : "false");
			break;

		case PARAMETER_TYPE_Int         :
		case PARAMETER_TYPE_Double      :
		case PARAMETER_TYPE_Degree      :
		case PARAMETER_TYPE_Date        :
		case PARAMETER_TYPE_Range       :
		case PARAMETER_TYPE_String      :
		case PARAMETER_TYPE_Text        :
		case PARAMETER_TYPE_FilePath    :
		case PARAMETER_TYPE_Choices     :
		case PARAMETER_TYPE_Table_Field :
		case PARAMETER_TYPE_Table_Fields:
			pChild	= Tool.Add_Child("option", P.asString());
			break;

		case PARAMETER_TYPE_Data_Type   :
		case PARAMETER_TYPE_Choice      :
			pChild	= Tool.Add_Child("option", P.asInt());
			break;

		case PARAMETER_TYPE_FixedTable  :
			pChild	= Tool.Add_Child("option");
			P.Serialize(*pChild, true);
			break;

		case PARAMETER_TYPE_Grid_System :
			if( P.Get_Children_Count() == 0 )
			{
				pChild	= Tool.Add_Child("option", P.asString());
			}
			break;

		default:
			if( P.is_Input() )
			{
				pChild 	= Tool.Add_Child("input");
				pChild->Set_Content(P.is_Optional() ? "INPUT_OPTIONAL" : "INPUT");
			}
			else if( P.is_Output() )
			{
				pChild	= Tool.Add_Child("output");
				pChild->Set_Content("OUTPUT");
			}
			break;
		}

		if( pChild )
		{
			pChild->Add_Property("id", Prefix + P.Get_Identifier());

			if( bVarNames )
			{
				if( P.is_Option() )
				{
					pChild->Add_Property("varname", "true");
				}

				pChild->Set_Content(Prefix + P.Get_Identifier());
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::_Get_Script_Parameters(CSG_MetaData &Parameters, CSG_Parameters *pParameters, const CSG_String &Prefix)
{
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter &P = *pParameters->Get_Parameter(i);

		if( P.Get_Type() == PARAMETER_TYPE_Parameters )
		{
			_Get_Script_Parameters(Parameters, P.asParameters(), Prefix + P.Get_Identifier() + '.');

			continue;	// no support for sub-parameter-lists in here
		}

		CSG_MetaData &Parameter = *Parameters.Add_Child(
			P.is_Option() ? "option" :
			P.is_Output() ? "output" : "input"
		);

		Parameter.Add_Property("varname"    , P.Get_Identifier ());
		Parameter.Add_Property("type"       , SG_Parameter_Type_Get_Identifier(P.Get_Type()));
		Parameter.Add_Child   ("name"       , P.Get_Name       ());
		Parameter.Add_Child   ("description", P.Get_Description());

		if( P.Get_Parent() )
		{
			Parameter.Add_Property("parent", P.Get_Parent()->Get_Identifier());
		}

		if( P.Get_Type() == PARAMETER_TYPE_Node
		||  P.Get_Type() == PARAMETER_TYPE_Grid_System )
		{
			continue;	// nothing more to do for these types
		}

		if( P.is_Option() )
		{
			CSG_MetaData &Value = *Parameter.Add_Child("value", P.asString());

			if( P.asValue() )
			{
				if( P.asValue()->has_Minimum() ) Value.Add_Property("min", P.asValue()->Get_Minimum());
				if( P.asValue()->has_Maximum() ) Value.Add_Property("max", P.asValue()->Get_Maximum());
			}

			if( P.asChoice () ) Parameter.Add_Child("choices", P.asChoice ()->Get_Items());
			if( P.asChoices() ) Parameter.Add_Child("choices", P.asChoices()->Get_Items());

			if( P.asFilePath() )
			{
				Parameter.Add_Property("save"     , P.asFilePath()->is_Save     () ? "true" : "false");
				Parameter.Add_Property("directory", P.asFilePath()->is_Directory() ? "true" : "false");
				Parameter.Add_Property("multiple" , P.asFilePath()->is_Multiple () ? "true" : "false");
				Parameter.Add_Child   ("filter"   , P.asFilePath()->Get_Filter());
			}

			if( P.Get_Type() == PARAMETER_TYPE_FixedTable )
			{
				P.Serialize(Parameter, true);
			}

			if( P.Get_Type() == PARAMETER_TYPE_Table_Field )
			{
				Value.Set_Content(P.is_Optional() ? "true" : "false");
			}
		}
		else	// data objects
		{
			if( P.is_Optional() )
			{
				Parameter.Add_Property("optional", "true");
			}

			if( P.Get_Type() == PARAMETER_TYPE_DataObject_Output )
			{
				switch( ((CSG_Parameter_Data_Object_Output *)&P)->Get_DataObject_Type() )
				{
				case SG_DATAOBJECT_TYPE_Grid:
					Parameter.Set_Property("type"  , SG_Parameter_Type_Get_Identifier(PARAMETER_TYPE_Grid));
					Parameter.Add_Property("target", "none");
					break;

				case SG_DATAOBJECT_TYPE_Grids:
					Parameter.Set_Property("type"  , SG_Parameter_Type_Get_Identifier(PARAMETER_TYPE_Grids));
					Parameter.Add_Property("target", "none");
					break;

				default:
					break;
				}
			}

			if( P.Get_Type() == PARAMETER_TYPE_Shapes )
			{
				switch( ((CSG_Parameter_Shapes *)&P)->Get_Shape_Type() )
				{
				case SHAPE_TYPE_Point  : Parameter.Add_Property("feature_type", "point"  ); break;
				case SHAPE_TYPE_Points : Parameter.Add_Property("feature_type", "points" ); break;
				case SHAPE_TYPE_Line   : Parameter.Add_Property("feature_type", "line"   ); break;
				case SHAPE_TYPE_Polygon: Parameter.Add_Property("feature_type", "polygon"); break;
				default:                                                                    break;
				}
			}

			if( P.Get_Type() == PARAMETER_TYPE_Grid_List
			&&  !((CSG_Parameter_Grid_List *)&P)->Get_System() )
			{
				Parameter.Add_Property("no_system", "true");
			}

			if( P.Get_Type() == PARAMETER_TYPE_Grids_List
			&&  !((CSG_Parameter_Grid_List *)&P)->Get_System() )
			{
				Parameter.Add_Property("no_system", "true");
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Chains::CSG_Tool_Chains(const CSG_String &Library_Name, const CSG_String &Path)
{
	m_Library_Name	= Library_Name;

	//-----------------------------------------------------
	if( m_Library_Name.is_Empty() || !m_Library_Name.Cmp("toolchains") )
	{
		m_Library_Name	= "_tool_chains_uncategorized";
		m_Name			= _TL("Uncategorized Tool Chains");
		m_Description	= _TL("Uncategorized Tool Chains");
		m_Menu			= _TL("Uncategorized Tool Chains");
	}

	//-----------------------------------------------------
	else
	{
		CSG_MetaData	XML(SG_File_Make_Path(Path, Library_Name, "xml"));

		if( !XML.Cmp_Name("toolchains") )
		{
			XML.Destroy();
		}

		m_Name			= GET_XML_CONTENT(XML, "name"       , m_Library_Name       , true);
		m_Description	= GET_XML_CONTENT(XML, "description", _TL("no description"), true);
		m_Menu			= GET_XML_CONTENT(XML, "menu"       , _TL("Tool Chains"   ), true);

		m_Description.Replace("[[", "<");	// support for xml/html tags
		m_Description.Replace("]]", ">");

		// add references...
		for(int i=0; i<XML.Get_Children_Count(); i++)
		{
			if( !XML[i].Get_Name().CmpNoCase("REFERENCE") )
			{
				CSG_String	Authors, Year, Title, Where, Link, Link_Text, DOI;

				if( XML[i]("AUTHORS"  ) ) Authors   = XML[i].Get_Content("AUTHORS"  );
				if( XML[i]("YEAR"     ) ) Year      = XML[i].Get_Content("YEAR"     );
				if( XML[i]("TITLE"    ) ) Title     = XML[i].Get_Content("TITLE"    );
				if( XML[i]("WHERE"    ) ) Where     = XML[i].Get_Content("WHERE"    );
				if( XML[i]("LINK"     ) ) Link      = XML[i].Get_Content("LINK"     );
				if( XML[i]("LINK_TEXT") ) Link_Text = XML[i].Get_Content("LINK_TEXT");
				if( XML[i]("DOI"      ) ) DOI       = XML[i].Get_Content("DOI"      );

				if( !DOI.is_Empty() )
				{
					Link = "https://doi.org/" + DOI; Link_Text = "doi:" + DOI;
				}

				if( !Authors.is_Empty() && !Year.is_Empty() && !Title.is_Empty() )
				{
					Add_Reference(Authors, Year, Title, Where, Link.c_str(), Link_Text.c_str());
				}
				else if( !Link.is_Empty() )
				{
					Add_Reference(Link, Link_Text.c_str());
				}
			}
		}
	}
}

//---------------------------------------------------------
CSG_Tool_Chains::~CSG_Tool_Chains(void)
{
	Delete_Tools();

	for(size_t i=0; i<m_Tools.Get_uSize(); i++)
	{
		delete((CSG_Tool_Chain *)m_Tools[i]);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Chains::Get_Info(int Type) const
{
	switch( Type )
	{
	case TLB_INFO_Name       :	return( m_Name             );
	case TLB_INFO_Description:	return( m_Description      );
	case TLB_INFO_Menu_Path  :	return( m_Menu             );
	case TLB_INFO_Category   :	return( _TL("Tool Chains") );
	}

	return( "" );
}

//---------------------------------------------------------
bool CSG_Tool_Chains::Add_Tool(CSG_Tool_Chain *pTool)
{
	m_Tools.Add(pTool);

	pTool->Set_Library_Menu(Get_Info(TLB_INFO_Menu_Path));

	return( true );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Chains::Get_Tool(int Index, TSG_Tool_Type Type) const
{
	CSG_Tool *pTool = Index >= 0 && Index < Get_Count() ? (CSG_Tool_Chain *)m_Tools[Index] : NULL;

	return(	pTool && (Type == TOOL_TYPE_Base || Type == pTool->Get_Type()) ? pTool : NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Chains::Create_Tool(const CSG_String &Name, bool bWithGUI, bool bWithCMD)
{
	CSG_Tool *pTool = CSG_Tool_Library::Get_Tool(Name);

	if( pTool && pTool->Get_Type() == TOOL_TYPE_Chain )
	{
		m_xTools.Add(pTool = new CSG_Tool_Chain(*((CSG_Tool_Chain *)pTool), bWithGUI, bWithCMD));

		return( pTool );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Tool_Chains::Delete_Tool(CSG_Tool *pTool)
{
	if( m_xTools.Del(pTool) || m_Tools.Del(pTool) )
	{
		delete((CSG_Tool_Chain *)pTool);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Chains::Delete_Tools(void)
{
	for(size_t i=0; i<m_xTools.Get_uSize(); i++)
	{
		delete((CSG_Tool_Chain *)m_xTools[i]);
	}

	m_xTools.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Chain::Save_History_to_Model(const CSG_MetaData &History, const CSG_String &File)
{
	if( SG_Compare_Version(History.Get_Property("saga-version"), "2.1.3") < 0 || !(History("TOOL") || History("MODULE")) )
	{
		return( false );
	}

	const CSG_MetaData	&Tool(History("TOOL") ? History["TOOL"] : History["MODULE"]);

	if( !Tool("OUTPUT") )
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

	_Save_History_Add_Tool(Tool, *Chain("parameters"), *Chain("tools"), true);

	for(int i=0; i<Chain["tools"].Get_Children_Count(); i++)
	{
		Chain["tools"](i)->Del_Property("id");
	}

	return( Chain.Save(File) );
}

//---------------------------------------------------------
bool CSG_Tool_Chain::_Save_History_Add_Tool(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tools, bool bAddOutput)
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
	Tool.Add_Property("tool"   , History.Get_Property("id"     ));
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
			case PARAMETER_TYPE_Date        :
			case PARAMETER_TYPE_Range       :
			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
			case PARAMETER_TYPE_Choices     :
				pParameter	= Tool.Add_Child("option", pChild->Get_Content());
				break;

			case PARAMETER_TYPE_Data_Type   :
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
bool CSG_Tool_Chain::_Save_History_Add_Input(const CSG_MetaData &History, CSG_MetaData &Parms, CSG_MetaData &Tool)
{
	CSG_MetaData	*pInput	= Tool.Add_Child("input");

	pInput->Add_Property("parms", History.Get_Property("parms"));
	pInput->Add_Property("id"   , History.Get_Property("id"   ));

	if( History("TOOL") || History("MODULE") )
	{
		const CSG_MetaData	&History_Tool(History("TOOL") ? History["TOOL"] : History["MODULE"]);

		if( History_Tool("OUTPUT") && History_Tool["OUTPUT"].Get_Property("id") )
		{
			pInput->Fmt_Content("tool_%02d__%s", Tool.Get_Parent()->Get_Children_Count() + 1, History_Tool["OUTPUT"].Get_Property("id"));

			return( _Save_History_Add_Tool(History_Tool, Parms, *Tool.Get_Parent()) );
		}
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
