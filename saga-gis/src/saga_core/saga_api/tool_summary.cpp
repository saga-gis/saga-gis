
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
//                   tool_summary.cpp                    //
//                                                       //
//          Copyright (C) 2018 by Olaf Conrad            //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "saga_api.h"


///////////////////////////////////////////////////////////
//														 //
//		XML tags for mark-up of tool synopsis	 		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_XML_SYSTEM				SG_T("system")
#define SG_XML_SYSTEM_VER			SG_T("version")
#define SG_XML_SYSTEM_MLP			SG_T("library-path")
#define SG_XML_LIBRARY				SG_T("library")
#define SG_XML_LIBRARY_PATH			SG_T("path")
#define SG_XML_LIBRARY_NAME			SG_T("name")
#define SG_XML_LIBRARY_CATEGORY			SG_T("category")
#define SG_XML_TOOL					SG_T("module")
#define SG_XML_TOOL_ATT_NAME		SG_T("name")
#define SG_XML_TOOL_ATT_ID			SG_T("id")
#define SG_XML_TOOL_ATT_VERSION		SG_T("version")
#define SG_XML_TOOL_ATT_AUTHOR		SG_T("author")
#define SG_XML_SPECIFICATION		SG_T("specification")
#define SG_XML_SPEC_ATT_GRID		SG_T("grid")
#define SG_XML_SPEC_ATT_INTERA		SG_T("interactive")
#define SG_XML_MENU					SG_T("menu")
#define SG_XML_DESCRIPTION			SG_T("description")
#define SG_XML_PARAM				SG_T("parameter")
#define SG_XML_PARAM_ATT_NAME		SG_T("name")
#define SG_XML_PARAM_ATT_CLASS		SG_T("class")
#define SG_XML_PARAM_MANDATORY		SG_T("mandatory")
#define SG_XML_PARAM_TYPE			SG_T("type")
#define SG_XML_PARAM_IDENTIFIER		SG_T("identifier")
#define SG_XML_PARAM_PARENT			SG_T("parent")
#define SG_XML_PARAM_LIST			SG_T("list")
#define SG_XML_PARAM_ITEM			SG_T("item")
#define SG_XML_PARAM_TABLE			SG_T("table")
#define SG_XML_PARAM_FIELD			SG_T("field")
#define SG_XML_PARAM_FIELD_ATT_NAME	SG_T("name")
#define SG_XML_PARAM_FIELD_ATT_TYPE	SG_T("type")
#define SG_XML_PARAM_MIN			SG_T("min")
#define SG_XML_PARAM_MAX			SG_T("max")
#define SG_XML_PARAM_DEFAULT		SG_T("default")

//---------------------------------------------------------
#define SG_GET_XML_TAGGED_STR(value, tag)	CSG_String::Format("<%s>%s</%s>", tag, value, tag)
#define SG_GET_XML_TAGGED_INT(value, tag)	CSG_String::Format("<%s>%d</%s>", tag, value, tag)
#define SG_GET_XML_TAGGED_FLT(value, tag)	CSG_String::Format("<%s>%f</%s>", tag, value, tag)


///////////////////////////////////////////////////////////
//														 //
//						Tool							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void SG_XML_Add_Parameter(CSG_MetaData *pParent, CSG_Parameter *pParameter, CSG_String ID = "")
{
	if( !ID.is_Empty() )	ID	+= "_";	ID	+= pParameter->Get_Identifier();

	CSG_MetaData	*pItem	= pParent->Add_Child(SG_XML_PARAM);

	pItem->Add_Property(SG_XML_PARAM_ATT_NAME , pParameter->Get_Name());
	pItem->Add_Property(SG_XML_PARAM_ATT_CLASS,	pParameter->is_Input() ? "input" : pParameter->is_Output() ? "output" : "option");

	pItem->Add_Child(SG_XML_PARAM_IDENTIFIER, ID);

	if( pParameter->Get_Type() == PARAMETER_TYPE_DataObject_Output )
	{
	    pItem->Add_Child(SG_XML_PARAM_TYPE , CSG_String::Format("%s %s",
			pParameter->Get_Type_Name().Make_Lower().c_str(),
			SG_Get_DataObject_Name(pParameter->Get_DataObject_Type()).Make_Lower().c_str()
		));
    }
    else
    {
	    pItem->Add_Child(SG_XML_PARAM_TYPE , pParameter->Get_Type_Name().Make_Lower());
    }

	pItem->Add_Child(SG_XML_PARAM_MANDATORY, pParameter->is_Optional() ? SG_T("false") : SG_T("true"));
	pItem->Add_Child(SG_XML_DESCRIPTION, pParameter->Get_Description());

	switch( pParameter->Get_Type() )
	{
	//-----------------------------------------------------
	case PARAMETER_TYPE_Bool:
		pItem->Add_Child(SG_XML_PARAM_DEFAULT, pParameter->asBool());
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Int:
	case PARAMETER_TYPE_Double:
	case PARAMETER_TYPE_Degree:
		if( ((CSG_Parameter_Value *)pParameter->Get_Data())->has_Minimum() ) pItem->Add_Child(SG_XML_PARAM_MIN, ((CSG_Parameter_Value *)pParameter->Get_Data())->Get_Minimum());
		if( ((CSG_Parameter_Value *)pParameter->Get_Data())->has_Maximum() ) pItem->Add_Child(SG_XML_PARAM_MAX, ((CSG_Parameter_Value *)pParameter->Get_Data())->Get_Maximum());
		if( !pParameter->Get_Data()->Get_Default().is_Empty() )              pItem->Add_Child(SG_XML_PARAM_DEFAULT, pParameter->Get_Data()->Get_Default());
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Date:
		break;

	case PARAMETER_TYPE_Range:
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Choice:
		{
			CSG_MetaData	*pChild	= pItem->Add_Child(SG_XML_PARAM_LIST);

			for(int i=0; i<pParameter->asChoice()->Get_Count(); i++)
			{
				pChild->Add_Child(SG_XML_PARAM_ITEM, pParameter->asChoice()->Get_Item(i));
			}

			if( !pParameter->asChoice()->Get_Default().is_Empty() )
			{
				pItem->Add_Child(SG_XML_PARAM_DEFAULT, pParameter->asChoice()->Get_Default());
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Choices:
		{
			CSG_MetaData	*pChild	= pItem->Add_Child(SG_XML_PARAM_LIST);

			for(int i=0; i<pParameter->asChoices()->Get_Item_Count(); i++)
			{
				pChild->Add_Child(SG_XML_PARAM_ITEM, pParameter->asChoices()->Get_Item(i));
			}

			if( !pParameter->asChoices()->Get_Default().is_Empty() )
			{
				pItem->Add_Child(SG_XML_PARAM_DEFAULT, pParameter->asChoices()->Get_Default());
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_String:
	case PARAMETER_TYPE_Text:
	case PARAMETER_TYPE_FilePath:
		break;

	case PARAMETER_TYPE_Font:
		break;

	case PARAMETER_TYPE_Color:
	case PARAMETER_TYPE_Colors:
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_FixedTable:
		{
			CSG_MetaData	*pChild	= pItem->Add_Child(SG_XML_PARAM_TABLE);

			for(int i=0; i<pParameter->asTable()->Get_Field_Count(); i++)
			{
				CSG_MetaData	*pField	= pChild->Add_Child(SG_XML_PARAM_FIELD);

				pField->Add_Property(SG_XML_PARAM_FIELD_ATT_NAME, pParameter->asTable()->Get_Field_Name(i));
				pField->Add_Property(SG_XML_PARAM_FIELD_ATT_TYPE, SG_Data_Type_Get_Name(pParameter->asTable()->Get_Field_Type(i)));
			}
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Grid_System:
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Table_Field:
	case PARAMETER_TYPE_Table_Fields:
		if( pParameter->Get_Parent() )
		{
			pItem->Add_Child(SG_XML_PARAM_PARENT, pParameter->Get_Parent()->Get_Identifier());
		}
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Parameters:
		{
			for(int i=0; i<pParameter->asParameters()->Get_Count(); i++)
			{
				SG_XML_Add_Parameter(pItem, pParameter->asParameters()->Get_Parameter(i), ID);
			}
		}

	//-----------------------------------------------------
	default:
		break;
	}
}

//---------------------------------------------------------
CSG_String CSG_Tool::Get_Summary(bool bParameters, const CSG_String &Menu, const CSG_String &Description, int Format)
{
	#define SUMMARY_DO_ADD	(SG_UI_Get_Window_Main() ? Parameters(i)->do_UseInGUI() : Parameters(i)->do_UseInCMD())

	int			i;
	CSG_String	s;

	//-----------------------------------------------------
	switch( Format )
	{
	case SG_SUMMARY_FMT_XML:
		{
			CSG_MetaData	m;

			m.Set_Name    (SG_XML_TOOL);
			m.Add_Property(SG_XML_TOOL_ATT_NAME   , Get_Name       ());
			m.Add_Property(SG_XML_TOOL_ATT_ID     , Get_ID         ());
			m.Add_Property(SG_XML_TOOL_ATT_AUTHOR , Get_Author     ());
			m.Add_Property(SG_XML_TOOL_ATT_VERSION, Get_Version    ());
			m.Add_Child   (SG_XML_DESCRIPTION     , Get_Description());
			m.Add_Child   (SG_XML_MENU            , Get_MenuPath   ());
			m.Add_Child   (SG_XML_SPEC_ATT_GRID   , is_Grid        () ? "true" : "false");
			m.Add_Child   (SG_XML_SPEC_ATT_INTERA , is_Interactive () ? "true" : "false");

		//	CSG_MetaData	*pChild	= m.Add_Child(SG_XML_SPECIFICATION);
		//	pChild->Add_Property(SG_XML_SPEC_ATT_GRID  , is_Grid        () ? "true" : "false");
		//	pChild->Add_Property(SG_XML_SPEC_ATT_INTERA, is_Interactive () ? "true" : "false");

			if( bParameters )
			{
				for(i=0; i<Parameters.Get_Count(); i++)
				{
					if( SUMMARY_DO_ADD && Parameters(i)->is_Input() )
 					{
						SG_XML_Add_Parameter(&m, Parameters(i));
					}
				}

				for(i=0; i<Parameters.Get_Count(); i++)
				{
					if( SUMMARY_DO_ADD && Parameters(i)->is_Output() )
 					{
						SG_XML_Add_Parameter(&m, Parameters(i));
					}
				}

				for(i=0; i<Parameters.Get_Count(); i++)
				{
					if( SUMMARY_DO_ADD && Parameters(i)->is_Option()
					&&  Parameters(i)->Get_Type() != PARAMETER_TYPE_Node
					&&  Parameters(i)->Get_Type() != PARAMETER_TYPE_Grid_System )
 					{
						SG_XML_Add_Parameter(&m, Parameters(i));
					}
				}
			}

			s	= m.asText(1);
		}
		break;

	//-----------------------------------------------------
	case SG_SUMMARY_FMT_HTML:
		{
			#define SUMMARY_ADD_STR(label, value)	CSG_String::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%s</td></tr>", label, value)
			#define SUMMARY_ADD_INT(label, value)	CSG_String::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%d</td></tr>", label, value)

			s	+= CSG_String::Format("<h4>%s</h4><table border=\"0\">", _TL("Tool"));

			s	+= SUMMARY_ADD_STR(_TL("Name"   ), Get_Name   ().c_str());
			s	+= SUMMARY_ADD_STR(_TL("Author" ), Get_Author ().c_str());
			s	+= SUMMARY_ADD_STR(_TL("Version"), Get_Version().c_str());
			s	+= SUMMARY_ADD_STR(_TL("Library"), Get_Library().c_str());
			s	+= SUMMARY_ADD_STR(_TL("ID"     ), Get_ID     ().c_str());

			if( is_Interactive() && is_Grid() )
			{
				s	+= SUMMARY_ADD_STR(_TL("Specification"), CSG_String::Format("%s, %s", _TL("grid"), _TL("interactive")).c_str() );
			}
			else if( is_Interactive() )
			{
				s	+= SUMMARY_ADD_STR(_TL("Specification"), _TL("interactive"));
			}
			else if( is_Grid() )
			{
				s	+= SUMMARY_ADD_STR(_TL("Specification"), _TL("grid"));
			}

			if( Get_Type() == TOOL_TYPE_Chain )
			{
				s	+= SUMMARY_ADD_STR(_TL("File"), Get_File_Name().c_str() );
			}

			if( Menu.Length() > 0 )
			{
				CSG_String	sMenu(Menu);

				sMenu.Replace("|", " <b>></b> ");

				s	+= SUMMARY_ADD_STR(_TL("Menu"  ), sMenu.c_str());
			}

			s	+= "</table>";

			#undef SUMMARY_ADD_STR
			#undef SUMMARY_ADD_INT

			//---------------------------------------------
			s	+= CSG_String::Format("<hr><h4>%s</h4>", _TL("Description"));

			s	+= !Description.is_Empty() ? Description : Get_Description();

			//---------------------------------------------
			if( Description.is_Empty() && Get_References().Get_Count() > 0 )
			{
				s	+= CSG_String::Format("<hr><h4>%s</h4><ul>", _TL("References"));

				for(i=0; i<Get_References().Get_Count(); i++)
				{
					s	+= "<li>" + Get_References()[i] + "</li>";
				}

				s	+= "</ul>";
			}

			//---------------------------------------------
			if( bParameters )
			{
				bool	bFirst, bOptionals	= false;

				s	+= CSG_String::Format("<hr><h4>%s</h4>", _TL("Parameters"));
				s	+= CSG_String::Format("<table border=\"1\" width=\"100%%\" valign=\"top\" cellpadding=\"5\" rules=\"all\"><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",
						_TL("Name"), _TL("Type"), _TL("Identifier"), _TL("Description"), _TL("Constraints")
					);

				//-----------------------------------------
				for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
				{
					CSG_Parameter	*pParameter	= Parameters(i);

					if( SUMMARY_DO_ADD && pParameter->is_Input() )
					{
						if( bFirst )
						{
							bFirst	= false;
							s	+= CSG_String::Format("<tr><th colspan=\"5\">%s</th></tr>", _TL("Input"));
						}

						s	+= CSG_String::Format("<tr><td>%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
							pParameter->Get_Name(),
							pParameter->is_Optional() ? SG_T(" (*)") : SG_T(" "),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
							pParameter->Get_Identifier(),
							pParameter->Get_Description(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
						);
					}
				}

				//-----------------------------------------
				for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
				{
					CSG_Parameter	*pParameter	= Parameters(i);

					if( SUMMARY_DO_ADD && pParameter->is_Output() )
					{
						if( bFirst )
						{
							bFirst	= false;
							s	+= CSG_String::Format("<tr><th colspan=\"5\">%s</th></tr>", _TL("Output"));
						}

						s	+= CSG_String::Format("<tr><td>%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
							pParameter->Get_Name(),
							pParameter->is_Optional() ? SG_T(" (*)") : SG_T(""),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
							pParameter->Get_Identifier(),
							pParameter->Get_Description(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
						);
					}
				}

				//-----------------------------------------
				for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
				{
					CSG_Parameter	*pParameter	= Parameters(i);

					if( SUMMARY_DO_ADD && pParameter->is_Option() && pParameter->Get_Type() != PARAMETER_TYPE_Grid_System )
					{
						if( bFirst )
						{
							bFirst	= false;
							s	+= CSG_String::Format("<tr><th colspan=\"5\">%s</th></tr>", _TL("Options"));
						}

						s	+= CSG_String::Format("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
							pParameter->Get_Name(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
							pParameter->Get_Identifier(),
							pParameter->Get_Description(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
						);
					}
					else if( pParameter->is_Optional() )
					{
						bOptionals	= true;
					}
				}

				//-----------------------------------------
				s	+= "</table>";

				if( bOptionals )
				{
					s	+= CSG_String::Format("(*) <i>%s</i>", _TL("optional"));
				}
			}
		}
		break;

	//-----------------------------------------------------
	case SG_SUMMARY_FMT_FLAT: default:
		{
			#define SUMMARY_ADD_STR(label, value)	CSG_String::Format("%s:\t%s\n", label, value)
			#define SUMMARY_ADD_INT(label, value)	CSG_String::Format("%s:\t%d\n", label, value)

			s	+= SUMMARY_ADD_STR(_TL("Name"   ), Get_Name   ().c_str());
			s	+= SUMMARY_ADD_STR(_TL("ID"     ), Get_ID     ().c_str());
			s	+= SUMMARY_ADD_STR(_TL("Author" ), Get_Author ().c_str());
			s	+= SUMMARY_ADD_STR(_TL("Version"), Get_Version().c_str());

			if( is_Interactive() && is_Grid() )
			{
				s	+= SUMMARY_ADD_STR(_TL("Specification"), CSG_String::Format("%s, %s", _TL("grid"), _TL("interactive")).c_str() );
			}
			else if( is_Interactive() )
			{
				s	+= SUMMARY_ADD_STR(_TL("Specification"), _TL("interactive"));
			}
			else if( is_Grid() )
			{
				s	+= SUMMARY_ADD_STR(_TL("Specification"), _TL("grid"));
			}

			if( Get_Type() == TOOL_TYPE_Chain )
			{
				s	+= SUMMARY_ADD_STR(_TL("File"), Get_File_Name().c_str() );
			}

			if( Menu.Length() > 0 )
			{
				CSG_String	sMenu(Menu);

				sMenu.Replace("|", " <b>></b> ");

				s	+= SUMMARY_ADD_STR(_TL("Menu"  ), sMenu.c_str());
			}

			#undef SUMMARY_ADD_STR
			#undef SUMMARY_ADD_INT

			//---------------------------------------------
			s	+= "\n____________________________\n";

			s	+= CSG_String::Format("%s:\n", _TL("Description"));

			s	+= !Description.is_Empty() ? Description : Get_Description();

			//---------------------------------------------
			if( Description.is_Empty() && Get_References().Get_Count() > 0 )
			{
				s	+= "\n____________________________\n";

				s	+= CSG_String::Format("\n%s:\n", _TL("References"));

				for(i=0; i<Get_References().Get_Count(); i++)
				{
					s	+= " - " + Get_References()[i] + "\n";
				}
			}

			//---------------------------------------------
			if( bParameters )
			{
				bool	bFirst, bOptionals	= false;

				s	+= "\n";

				//-----------------------------------------
				for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
				{
					CSG_Parameter	*pParameter	= Parameters(i);

					if( SUMMARY_DO_ADD && pParameter->is_Input() )
					{
						if( bFirst )
						{
							bFirst	= false;
							s	+= "____________________________\n";
							s	+= CSG_String::Format("%s:\n", _TL("Input"));
						}

						s	+= CSG_String::Format("_\n%s\n%s\n%s\n%s\n%s\n",
							pParameter->Get_Name(),
							pParameter->Get_Identifier(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
							pParameter->Get_Description(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
						);
					}
				}

				//-----------------------------------------
				for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
				{
					CSG_Parameter	*pParameter	= Parameters(i);

					if( SUMMARY_DO_ADD && pParameter->is_Output() )
					{
						if( bFirst )
						{
							bFirst	= false;
							s	+= "____________________________\n";
							s	+= CSG_String::Format("%s:\n", _TL("Output"));
						}

						s	+= CSG_String::Format("_\n%s\n%s\n%s\n%s\n%s\n",
							pParameter->Get_Name(),
							pParameter->Get_Identifier(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
							pParameter->Get_Description(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
						);
					}
				}

				//-----------------------------------------
				for(i=0, bFirst=true; i<Parameters.Get_Count(); i++)
				{
					CSG_Parameter	*pParameter	= Parameters(i);

					if( SUMMARY_DO_ADD && pParameter->is_Option() && pParameter->Get_Type() != PARAMETER_TYPE_Grid_System )
					{
						if( bFirst )
						{
							bFirst	= false;
							s	+= "____________________________\n";
							s	+= CSG_String::Format("%s:\n", _TL("Options"));
						}

						s	+= CSG_String::Format("_\n%s\n%s\n%s\n%s\n%s\n",
							pParameter->Get_Name(),
							pParameter->Get_Identifier(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE).c_str(),
							pParameter->Get_Description(),
							pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES).c_str()
						);
					}
					else if( pParameter->is_Optional() )
					{
						bOptionals	= true;
					}
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//					Tool Library						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Library::Get_Summary(int Format, bool bWithGUINeeded) const
{
	bool	bToolChains	= Get_File_Name().is_Empty();

	int			i;
	CSG_String	s;

	switch( Format )
	{
	//-----------------------------------------------------
	case SG_SUMMARY_FMT_XML:

		s	+= "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n";
		s	+= CSG_String::Format("<%s>\n"         , SG_XML_LIBRARY);
		s	+= CSG_String::Format("\t<%s>%s</%s>\n", SG_XML_LIBRARY_PATH, Get_File_Name().c_str(), SG_XML_LIBRARY_PATH);
		s	+= CSG_String::Format("\t<%s>%s</%s>\n", SG_XML_LIBRARY_NAME, Get_Info(TLB_INFO_Name).c_str(), SG_XML_LIBRARY_NAME);
		s	+= CSG_String::Format("\t<%s>%s</%s>\n", SG_XML_LIBRARY_CATEGORY, Get_Info(TLB_INFO_Category).c_str(), SG_XML_LIBRARY_CATEGORY);

		for(i=0; i<Get_Count(); i++)
		{
			if( Get_Tool(i) && (bWithGUINeeded || !Get_Tool(i)->needs_GUI()) )
			{
				s	+= CSG_String::Format("\t<%s %s=\"%s\" %s=\"%s\"/>\n", SG_XML_TOOL,
					SG_XML_TOOL_ATT_ID  , Get_Tool(i)->Get_ID  ().c_str(),
					SG_XML_TOOL_ATT_NAME, Get_Tool(i)->Get_Name().c_str()
				);
			}
		}

		s	+= CSG_String::Format("</%s>\n", SG_XML_LIBRARY);

		break;

	//-----------------------------------------------------
	case SG_SUMMARY_FMT_HTML: default:

		#define SUMMARY_ADD_STR(label, value)	CSG_String::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%s</td></tr>", label, value)
		#define SUMMARY_ADD_INT(label, value)	CSG_String::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%d</td></tr>", label, value)

		s	+= CSG_String::Format("<h4>%s</h4>", _TL("Tool Library"));

		s	+= "<table border=\"0\">";

		s	+= SUMMARY_ADD_STR(_TL("Name"   ), Get_Info(TLB_INFO_Name   ).c_str());
		s	+= SUMMARY_ADD_STR(_TL("Author" ), Get_Info(TLB_INFO_Author ).c_str());
		s	+= SUMMARY_ADD_STR(_TL("Version"), Get_Info(TLB_INFO_Version).c_str());
		s	+= SUMMARY_ADD_STR(_TL("ID"     ), Get_Library_Name        ().c_str());
		s	+= SUMMARY_ADD_STR(_TL("File"   ), Get_File_Name           ().c_str());

		s	+= "</table>";

		//-------------------------------------------------
		s	+= CSG_String::Format("<hr><h4>%s</h4>", _TL("Description"));

		s	+= Get_Info(TLB_INFO_Description);

		//-------------------------------------------------
		s	+= CSG_String::Format("<hr><h4>%s</h4>", _TL("Tools"));

		s	+= "<table border=\"0\">";

		s	+= CSG_String::Format("<tr align=\"left\"><th>%s</th><th>%s</th></tr>", _TL("ID"), _TL("Name"));

		for(i=0; i<Get_Count(); i++)
		{
			if( Get_Tool(i) && (bWithGUINeeded || !Get_Tool(i)->needs_GUI()) )
			{
				s	+= SUMMARY_ADD_STR(Get_Tool(i)->Get_ID().c_str(), Get_Tool(i)->Get_Name().c_str());
			}
		}

		s	+= "</table>";

		s.Replace("\n", "<br>");

		#undef SUMMARY_ADD_STR
		#undef SUMMARY_ADD_INT

		break;

	//-----------------------------------------------------
	case SG_SUMMARY_FMT_FLAT:

		if( !bToolChains )
		{
			s	+= CSG_String::Format("\n%s:\t", _TL("Library" )) + Get_Info(TLB_INFO_Name    );
			s	+= CSG_String::Format("\n%s:\t", _TL("Category")) + Get_Info(TLB_INFO_Category);
			s	+= CSG_String::Format("\n%s:\t", _TL("File"    )) + Get_File_Name();
		}
		else
		{
			s	+= CSG_String::Format("\n%s:\t", _TL("Tool Chains")) + Get_Info(TLB_INFO_Name);
		}

		s	+= CSG_String::Format("\n%s:\n", _TL("Description")) + Get_Info(TLB_INFO_Description);

		s	+= CSG_String::Format("\n\n%s:\n", _TL("Tools"));

		for(i=0; i<Get_Count(); i++)
		{
			if( Get_Tool(i) && (bWithGUINeeded || !Get_Tool(i)->needs_GUI()) )
			{
				s	+= " [" + Get_Tool(i)->Get_ID() + "]\t" + Get_Tool(i)->Get_Name() + "\n";
			}
		}

		break;
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Store tool and tool library description to HTML files.
//---------------------------------------------------------
bool CSG_Tool_Library::Get_Summary(const CSG_String &Path)	const
{
	CSG_File	f;

	if( f.Open(SG_File_Make_Path(Path, Get_Library_Name(), "html"), SG_FILE_W) )
	{
		f.Write(Get_Summary());
	}

	for(int j=0; j<Get_Count(); j++)
	{
		if( Get_Tool(j) && f.Open(SG_File_Make_Path(Path, Get_Library_Name() + "_" + Get_Tool(j)->Get_ID(), "html"), SG_FILE_W) )
		{
			f.Write(Get_Tool(j)->Get_Summary());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//				Tool Library Manager					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Library_Manager::Get_Summary(int Format)	const
{
	//-----------------------------------------------------
	int			i, nTools;

	CSG_Table	Libraries;

	Libraries.Add_Field("LIB"  , SG_DATATYPE_String);
	Libraries.Add_Field("TOOLS", SG_DATATYPE_Int   );
	Libraries.Add_Field("NAME" , SG_DATATYPE_String);
	Libraries.Add_Field("PATH" , SG_DATATYPE_String);
	Libraries.Add_Field("CHAIN", SG_DATATYPE_Int   );

	for(i=0, nTools=0; i<Get_Count(); i++)
	{
		if( Get_Library(i)->Get_Count() > 0 )
		{
			nTools	+= Get_Library(i)->Get_Count();

			Libraries.Add_Record();

			Libraries[i].Set_Value(0, Get_Library(i)->Get_Library_Name());
			Libraries[i].Set_Value(1, Get_Library(i)->Get_Count());
			Libraries[i].Set_Value(2, Get_Library(i)->Get_Name());
			Libraries[i].Set_Value(3, SG_File_Get_Path(Get_Library(i)->Get_File_Name()));
			Libraries[i].Set_Value(4, Get_Library(i)->Get_File_Name().is_Empty() ? 1 : 0);
		}
	}

	Libraries.Set_Index(4, TABLE_INDEX_Ascending, 0, TABLE_INDEX_Ascending);

	//-----------------------------------------------------
	CSG_String	s;

	switch( Format )
	{
	//-----------------------------------------------------
	case SG_SUMMARY_FMT_XML:

		s	+= "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n";
		s	+= CSG_String::Format("<%s>\n", SG_XML_SYSTEM);
		s	+= CSG_String::Format("<%s>%s</%s>\n", SG_XML_SYSTEM_VER, SAGA_VERSION, SG_XML_SYSTEM_VER);

		for(int i=0; i<Libraries.Get_Count(); i++)
		{
			s	+= CSG_String::Format("\t<%s %s=\"%s\"/>\n", SG_XML_LIBRARY, SG_XML_LIBRARY_NAME, Libraries[i].asString(0));
		}

		s	+= CSG_String::Format("</%s>\n", SG_XML_SYSTEM);

		break;

	//-----------------------------------------------------
	case SG_SUMMARY_FMT_HTML: default:

		#define SUMMARY_ADD_INT(label, value)	CSG_String::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%d</td></tr>", label, value)

		s	+= CSG_String::Format("<h4>%s</h4>", _TL("Tool Libraries"));

		s	+= "<table border=\"0\">";

		s	+= SUMMARY_ADD_INT(_TL("Libraries"), Libraries.Get_Count());
		s	+= SUMMARY_ADD_INT(_TL("Tools"    ), nTools);

		s	+= "</table>";

		s	+= CSG_String::Format("<hr><h4>%s</h4><table border=\"1\">", _TL("Libraries"));

		s	+= CSG_String::Format("<tr align=\"left\"><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>",
				_TL("Library" ),
				_TL("Tools"   ),
				_TL("Name"    ),
				_TL("Location")
			);

		for(i=0; i<Libraries.Get_Count(); i++)
		{
			s	+= CSG_String::Format("<tr><td>%s</td><td>%d</td><td>%s</td><td>%s</td></tr>",
					Libraries[i].asString(0),
					Libraries[i].asInt   (1),
					Libraries[i].asString(2),
					Libraries[i].asString(3)
				);
		}

		s	+= "</table>";

		#undef SUMMARY_ADD_INT

		break;

	//-----------------------------------------------------
	case SG_SUMMARY_FMT_FLAT:

		s	+= CSG_String::Format("\n%d %s (%d %s):\n", Libraries.Get_Count(), _TL("loaded tool libraries"), nTools, _TL("tools"));

		for(i=0; i<Libraries.Get_Count(); i++)
		{
			if( Libraries[i].asInt(4) == 0 )
				s	+= CSG_String::Format(" - %s\n"  , Libraries[i].asString(0));
			else
				s	+= CSG_String::Format(" - %s *\n", Libraries[i].asString(0));
		}

		s	+= CSG_String::Format("\n\n*) %s\n", _TL("tool chain libraries"));

		break;
	}

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Store tool and tool library description to HTML files.
//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Get_Summary(const CSG_String &Path)	const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library	*pLibrary	= Get_Library(i);

		CSG_String	Directory	= SG_File_Make_Path(Path, pLibrary->Get_Library_Name());

		if( SG_Dir_Create(Directory) )
		{
			pLibrary->Get_Summary(Directory);
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
