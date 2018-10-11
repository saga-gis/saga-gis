/**********************************************************
 * Version $Id: tool_py_updater.cpp 2332 2014-11-07 14:12:16Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  tool_py_updater.cpp                  //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                     Olaf Conrad                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute for Geography                //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool_py_updater.h"

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTool_Py_Updater::CTool_Py_Updater(void)
{
	//-----------------------------------------------------
	Set_Name		("Python Tools Interface Updater");

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(
		""
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "INPUT"		, "Input",
		"",
		SG_T("Python Scripts (*.py)|*.py|All Files|*.*")
	);

	Parameters.Add_FilePath(
		NULL	, "OUTPUT"		, "Output",
		"",
		SG_T("Python Scripts (*.py)|*.py|All Files|*.*"), NULL, true
	);

	Parameters.Add_Table(
		NULL	, "TOOLS"		, "Tools",
		"",
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Py_Updater::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	&Tools	= *Parameters("TOOLS")->asTable();

	Tools.Destroy();
	Tools.Set_Name("Tools");

	Tools.Add_Field("LIBRARY", SG_DATATYPE_String);
	Tools.Add_Field("ID"     , SG_DATATYPE_String);
	Tools.Add_Field("NAME"   , SG_DATATYPE_String);
	Tools.Add_Field("GONE"   , SG_DATATYPE_String);
	Tools.Add_Field("OLDNAME", SG_DATATYPE_String);
	Tools.Add_Field("ADDED"  , SG_DATATYPE_String);

	//-----------------------------------------------------
	CSG_String	sTools;
	CSG_File	Stream;

	if( Stream.Open(Parameters("INPUT")->asString(), SG_FILE_R, false) )
	{
		int	Section	= 0;

		CSG_String	sLine;

		while( Stream.Read_Line(sLine) )
		{
			if( Section == 0 )	// header
			{
				sTools	+= sLine + "\n";

				if( !sLine.Cmp("tools = [") )	// begin of tools section
				{
					Section++;
				}
			}
			else if( !sLine.Cmp("]") )	// end of tools section
			{
				Section++;
			}
			else if( Section == 1 )	// tools
			{
				sLine	= sLine.AfterFirst('[').BeforeLast(']');

				CSG_String	Library	= sLine.BeforeFirst(',').AfterFirst('\'').BeforeLast('\'');	sLine	= sLine.AfterFirst(',');
				CSG_String	ID		= sLine.BeforeFirst(','); ID.Trim(false), ID.Trim(true);
				CSG_String	Name	= sLine.AfterFirst (',').AfterFirst('\'').BeforeLast('\'');

				if( !Library.is_Empty() && !ID.is_Empty() && !Name.is_Empty() )
				{
					CSG_Table_Record	*pRecord	= Tools.Add_Record();

					Name.Replace("\\'", "'");

					pRecord->Set_Value(0, Library);
					pRecord->Set_Value(1, ID );
					pRecord->Set_Value(2, Name);
					pRecord->Set_Value(3, "yes");
				}

			//	CSG_String_Tokenizer	t(sLine, ",");	// tokenizer fails when name string contains a comma (',') !!!
			//	if( t.Get_Tokens_Count() == 3 )
			//	{
			//		CSG_Table_Record	*pRecord	= Tools.Add_Record();
			//		pRecord->Set_Value(0, t.Get_Next_Token());
			//		pRecord->Set_Value(1, sLine);
			//		pRecord->Set_Value(2, t.Get_Next_Token().AfterFirst('\'').BeforeLast('\''));
			//	}
			}
		}

		Stream.Close();
	}

	//-----------------------------------------------------
	int	nChanged	= 0;
	int	nAdded		= 0;

	for(int iLibrary=0; iLibrary<SG_Get_Tool_Library_Manager().Get_Count() && Set_Progress(iLibrary, SG_Get_Tool_Library_Manager().Get_Count()); iLibrary++)
	{
		CSG_Tool_Library	*pLibrary	= SG_Get_Tool_Library_Manager().Get_Library(iLibrary);

		if( pLibrary->Get_Library_Name().Cmp("dev_tools"   )	// generally exclude some special libraries (and tool chains ?!)
		&&  pLibrary->Get_Category().Cmp("Garden")
		&&  pLibrary->Get_Type() != TOOL_CHAINS )
		{
			{
				for(int i=0; i<Tools.Get_Count(); i++)
				{
					if( !pLibrary->Get_Library_Name().Cmp(Tools[i].asString(0)) )
					{
						Tools.Select(i, true);
					}
				}
			}

			for(int iTool=0; iTool<pLibrary->Get_Count(); iTool++)
			{
				CSG_Tool	*pTool	= pLibrary->Get_Tool(iTool);

				if( !pTool->is_Interactive() )
				{
					bool	bAdd	= true;

					for(int i=0; bAdd && i<Tools.Get_Selection_Count(); i++)
					{
						CSG_Table_Record	*pRecord	= Tools.Get_Selection(i);

						if( !pTool->Get_ID().Cmp(pRecord->asString(1)) )
						{
							bAdd	= false;

							pRecord->Set_Value(3, "");

							if( pTool->Get_Name().Cmp(pRecord->asString(2)) )
							{
								pRecord->Set_Value(4, pRecord->asString(2));	// old name
								pRecord->Set_Value(2, pTool->Get_Name());

								nChanged++;
							}
						}
					}

					if( bAdd )
					{
						CSG_Table_Record	*pRecord	= Tools.Add_Record();

						pRecord->Set_Value(0, pLibrary->Get_Library_Name());
						pRecord->Set_Value(1, pTool->Get_ID());
						pRecord->Set_Value(2, pTool->Get_Name());
						pRecord->Set_Value(5, "yes");

						nAdded++;
					}
				}
			}

			Tools.Select();
		}
	}

	if( nChanged == 0 && nAdded == 0 )
	{
		Message_Dlg("no changes detected");

		return( true );
	}

	Message_Add(CSG_String::Format("\n%d name changes\n%d tools added\n", nChanged, nAdded));

	//-----------------------------------------------------
	if( sTools.is_Empty() )
	{
		sTools	+= "tools = [\n";
	}

	for(int iTool=0; iTool<Tools.Get_Count(); iTool++)
	{
		if( iTool > 0 )
		{
			sTools	+= ",\n";
		}

		CSG_String	Name	= Tools[iTool].asString(2);

		Name.Replace("'", "\\'");

		sTools	+= CSG_String::Format("['%s', %s, '%s']", Tools[iTool].asString(0), Tools[iTool].asString(1), Name.c_str());
	}

	sTools	+= "\n]\n";

//	this->Message_Add(sTools, false);

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("OUTPUT")->asString(), SG_FILE_W, true) )
	{
		Message_Dlg("could not open file with write access");

		return( false );
	}

	return( Stream.Write(sTools) > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
