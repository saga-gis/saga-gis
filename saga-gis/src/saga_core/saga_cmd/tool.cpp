
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                Command Line Interface                 //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        tool.cpp                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/datetime.h>

#include "callback.h"

#include "tool.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCMD_Tool::CCMD_Tool(void)
{
	m_pTool = NULL;
}

CCMD_Tool::CCMD_Tool(CSG_Tool *pTool)
{
	Create(pTool);
}

//---------------------------------------------------------
CCMD_Tool::~CCMD_Tool(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::Create(CSG_Tool *pTool)
{
	Destroy();

	if( (m_pTool = pTool) != NULL )
	{
		wxCmdLineParser Parser; Parser.SetSwitchChars("-");

		_Set_Parser(m_pTool->Get_Parameters(), Parser);

		for(int i=0; i<m_pTool->Get_Parameters_Count(); i++)
		{
			_Set_Parser(m_pTool->Get_Parameters(i), Parser);
		}

		wxString Usage = wxString::Format("\nUsage: saga_cmd %s %s %s",
			m_pTool->Get_Library().c_str(),
			m_pTool->Get_ID     ().c_str(),
			Parser.GetUsageString().AfterFirst(' ').AfterFirst(' ')
		);

		m_Usage.Create(&Usage);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CCMD_Tool::Destroy(void)
{
	m_pTool = NULL;

	m_Arguments.Destroy();

	m_Usage.Clear();
}

//---------------------------------------------------------
void CCMD_Tool::Usage(void)
{
	if( !m_Usage.is_Empty() )
	{
		CMD_Print(m_Usage);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::Execute(int argc, char *argv[])
{
	if( !m_pTool || !_Parse(argc, argv) )
	{
		Usage();

		return( false );
	}

	//-----------------------------------------------------
	bool bResult = _Get_Parameters(m_pTool->Get_Parameters(), true);

	for(int i=0; bResult && i<m_pTool->Get_Parameters_Count(); i++)
	{
		bResult = _Get_Parameters(m_pTool->Get_Parameters(i), true);
	}

	if( !bResult )
	{
		Usage();

		return( false );
	}

	//-----------------------------------------------------
	CMD_Set_Tool(this);

	if( m_pTool->On_Before_Execution() )
	{
		bResult = m_pTool->Execute(true);

		m_pTool->On_After_Execution();
	}

	CMD_Set_Tool(NULL);

	//-----------------------------------------------------
	if( bResult )
	{
		_Save_Output(m_pTool->Get_Parameters());

		for(int i=0; i<m_pTool->Get_Parameters_Count(); i++)
		{
			_Save_Output(m_pTool->Get_Parameters(i));
		}

		SG_Get_Data_Manager().Delete();	// remove temporary data to save memory resources
	}
	else
	{
		CMD_Print_Error(_TL("executing tool"), m_pTool->Get_Name());
	}

	SG_UI_ProgressAndMsg_Reset(); SG_UI_Process_Set_Okay();

	return( bResult && _has_Unused() == false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CCMD_Tool::_Get_ID(CSG_Parameter *pParameter)
{
	CSG_String ID(pParameter->Get_Parameters()->Get_Identifier());

	if( ID.Length() > 0 )
	{
		ID += "_";
	}

	ID += pParameter->Get_Identifier();

	ID.Replace(".", "_");

	return( ID );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::_Parse(int argc, char *argv[])
{
	if( argc < 1 )
	{
		return( false );
	}
	
	m_Arguments.Destroy();

	m_Arguments.Add_Field("KEY", SG_DATATYPE_String);
	m_Arguments.Add_Field("VAL", SG_DATATYPE_String);
	m_Arguments.Add_Field("USE", SG_DATATYPE_Int   );

	//-----------------------------------------------------
	for(int i=0; i<argc; i++)
	{
		if( argv[i][0] == '-' )
		{
			CSG_String Value, Key = argv[i] + 1;

			if( Key.Find('=') > 0 )
			{
				Value = Key.AfterFirst ('=');
				Key   = Key.BeforeFirst('=');
			}
			else if( ++i < argc )
			{
				Value = argv[i];
			}

			if( !Key.is_Empty() )
			{
				if( !Value.is_Empty() && Value[0] == '\"' )
				{
					Value = Value.AfterFirst('\"').BeforeLast('\"');
				}
		
				CSG_Table_Record *pArgument = m_Arguments.Add_Record();

				pArgument->Set_Value(0, Key  );
				pArgument->Set_Value(1, Value);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::_has_Unused(void)
{
	CSG_String Unused;

	for(int i=0; i<m_Arguments.Get_Count(); i++)
	{
		if( m_Arguments[i].asInt(2) == 0 ) // argument has not been requested/used!
		{
			if( !Unused.is_Empty() )
			{
				Unused += ", ";
			}

			Unused += m_Arguments[i].asString(0);
		}
	}

	if( !Unused.is_Empty() )
	{
		CMD_Print(CSG_String::Format("%s: %s!\n[%s]\n\n", _TL("Error"), _TL("The tool does not know some of the provided arguments!"), Unused.c_str()));

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_Record * CCMD_Tool::_Found(const CSG_String &Name)
{
	CSG_Table_Record *pArgument = m_Arguments.Find_Record(0, Name);

	if( pArgument )
	{
		pArgument->Set_Value(2, 1); // mark argument as have been requested/used!
	}

	return( pArgument );
}

//---------------------------------------------------------
bool CCMD_Tool::_Found(const CSG_String &Name, CSG_String &Value)
{
	CSG_Table_Record *pArgument = _Found(Name);

	if( pArgument )
	{
		Value = pArgument->asString(1);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CCMD_Tool::_Found(const CSG_String &Name, int &Value)
{
	CSG_String Argument;
	
	return( _Found(Name, Argument) && Argument.asInt(Value) );
}

//---------------------------------------------------------
bool CCMD_Tool::_Found(const CSG_String &Name, double &Value)
{
	CSG_String Argument;
	
	return( _Found(Name, Argument) && Argument.asDouble(Value) );
}

//---------------------------------------------------------
bool CCMD_Tool::_Found(const CSG_String &Name, CSG_DateTime &Value)
{
	CSG_String Argument;
	
	return( _Found(Name, Argument) && Value.Parse_Date(Argument) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::_Set_Parser(CSG_Parameters *pParameters, wxCmdLineParser &Parser)
{
	if( !pParameters )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter *pParameter = pParameters->Get_Parameter(i);

		//-------------------------------------------------
		if( pParameter->is_DataObject() )	// reset data object parameters, avoids problems when tool is called more than once without un-/reloading
		{
			pParameter->Set_Value(DATAOBJECT_NOTSET);
		}
		else if( pParameter->is_DataObject_List() )
		{
			pParameter->asList()->Del_Items();
		}

		//-------------------------------------------------
		if( pParameter->do_UseInCMD() == false )
		{
			continue;
		}

		wxString Description = pParameter->Get_Description(
			PARAMETER_DESCRIPTION_NAME|PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, SG_T("\n\t")
		).c_str();

		Description.Replace("\xb", "");	// unicode problem: quick'n'dirty bug fix, to be replaced

		wxString ID(_Get_ID(pParameter).c_str());

		if( pParameter->is_Input() || pParameter->is_Output() )
		{
			Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR|wxCMD_LINE_PARAM_OPTIONAL);
		}

		//-------------------------------------------------
		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			switch( pParameter->Get_Type() )
			{
			case PARAMETER_TYPE_Parameters  :
				_Set_Parser(pParameter->asParameters(), Parser);
				break;

			case PARAMETER_TYPE_Bool        :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Int         :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Data_Type   :
			case PARAMETER_TYPE_Choice      :
			case PARAMETER_TYPE_Choices     :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Date        :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_DATE  , wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Range       :
				Parser.AddOption(ID + "_MIN", wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				Parser.AddOption(ID + "_MAX", wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Color       :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Colors      :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_FixedTable  :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Grid_System :
				if( pParameter->Get_Children_Count() == 0 )
				{
					Parser.AddOption(ID + "_D"   , wxEmptyString, _TL("Cell Size"                          ), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_X"   , wxEmptyString, _TL("Lower Left Center Cell X-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_Y"   , wxEmptyString, _TL("Lower Left Center Cell Y-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_NX"  , wxEmptyString, _TL("Number of Columns"                  ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_NY"  , wxEmptyString, _TL("Number of Rows"                     ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_FILE", wxEmptyString, _TL("Grid File"                          ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				}
				break;

			default:
				break;
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::_Get_Parameters(CSG_Parameters *pParameters, bool bInitialize)
{
	if( !pParameters )
	{
		return( false );
	}

	if( bInitialize )
	{
		pParameters->Restore_Defaults();
	}

	//-----------------------------------------------------
	return( _Get_Options(pParameters, bInitialize)	// options might 'disable' some input, so check all options before the input is checked!
		&&  _Get_Input  (pParameters)
		&&  _Get_Options(pParameters, false)		// setting input might have reset some options, so check options one more time!
	);
}

//---------------------------------------------------------
bool CCMD_Tool::_Get_Options(CSG_Parameters *pParameters, bool bInitialize)
{
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter *pParameter = pParameters->Get_Parameter(i); CSG_String ID(_Get_ID(pParameter));

		if( pParameter->do_UseInCMD() == false )
		{
			// ignore here
		}

		else if( pParameter->is_Input() )
		{
			// nop now, loading options first
		}

		else if( pParameter->is_Output() )
		{
			CSG_String FileName; // only create optional output if a valid output file name is given!

			if( pParameter->is_DataObject() && pParameter->is_Optional() && !pParameter->asDataObject() && _Found(ID, FileName) && FileName.Length() > 0 )
			{
				pParameter->Set_Value(DATAOBJECT_CREATE);
			}
		}

		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			int valInt; double valFloat; CSG_String valString; CSG_DateTime valDate;

			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Parameters:
				_Get_Parameters(pParameter->asParameters(), bInitialize);

				pParameter->has_Changed();
				break;

			case PARAMETER_TYPE_Bool:
				if( _Found(ID, valString) )
				{
					if(      (valString.asInt(valInt) && valInt == 1) || !valString.CmpNoCase("true" ) )
					{
						pParameter->Set_Value(1);
					}
					else if( (valString.asInt(valInt) && valInt == 0) || !valString.CmpNoCase("false") )
					{
						pParameter->Set_Value(0);
					}
					else
					{
						CMD_Print_Error(pParameter->Get_Name(), _TL("invalid boolean value ('0', '1', 'false', 'true')"));

						return( false );
					}
				}
				break;

			case PARAMETER_TYPE_Int:
				if( _Found(ID, valInt) )
				{
					pParameter->Set_Value(valInt);
				}
				break;

			case PARAMETER_TYPE_Data_Type:
			case PARAMETER_TYPE_Choice:
				if( _Found(ID, valString) )
				{
					if( valString.asInt(valInt) )
					{
						pParameter->Set_Value(valInt);
					}
					else
					{
						pParameter->Set_Value(valString);
					}
				}
				break;

			case PARAMETER_TYPE_Choices:
				if( _Found(ID, valString) )
				{
					pParameter->Set_Value(valString);
				}
				break;

			case PARAMETER_TYPE_Double:
			case PARAMETER_TYPE_Degree:
				if( _Found(ID, valFloat) )
				{
					pParameter->Set_Value(valFloat);
				}
				break;

			case PARAMETER_TYPE_Date:
				if( _Found(ID, valDate) )
				{
					pParameter->Set_Value(valDate.Get_JDN());
				}
				break;

			case PARAMETER_TYPE_Range:
				if( _Found(ID + "_MIN", valFloat) )
				{
					pParameter->asRange()->Set_Min(valFloat);

					pParameter->has_Changed();
				}

				if( _Found(ID + "_MAX", valFloat) )
				{
					pParameter->asRange()->Set_Max(valFloat);

					pParameter->has_Changed();
				}
				break;

			case PARAMETER_TYPE_Color:
				if( _Found(ID, valString) )
				{
					pParameter->Set_Value(CSG_String(valString));
				}
				break;

			case PARAMETER_TYPE_Colors:
				if( _Found(ID, valString) )
				{
					if( SG_File_Exists(valString) )
					{
						pParameter->asColors()->Load(valString);
					}
					else if( valString.asInt(valInt) )
					{
						pParameter->Set_Value(valInt);
					}
				}
				break;

			case PARAMETER_TYPE_String:
				if( _Found(ID, valString) )
				{
					pParameter->Set_Value(valString);
				}
				break;

			case PARAMETER_TYPE_Text:
				if( _Found(ID, valString) )
				{
					CSG_File Stream;

					if( Stream.Open(valString) )
					{
						Stream.Read(valString, Stream.Length());

						pParameter->Set_Value(valString);
					}
					else
					{
						pParameter->Set_Value(valString);
					}
				}
				break;

			case PARAMETER_TYPE_FilePath:
				if( _Found(ID, valString) )
				{
					if( pParameter->asFilePath()->is_Multiple() )
					{
						valString.Prepend("\"");
						valString.Replace(";", "\" \"");
						valString.Append ("\"");
					}

					pParameter->Set_Value(valString);
				}
				break;

			case PARAMETER_TYPE_FixedTable:
				if( _Found(ID,valString) )
				{
					CSG_Table Table(valString);

					pParameter->asTable()->Assign_Values(&Table);

					pParameter->has_Changed();
				}
				break;

			case PARAMETER_TYPE_Grid_System:
				if( pParameter->Get_Children_Count() == 0 )
				{
					CSG_Grid_File_Info Info; double x, y, d; int nx, ny;

					if( _Found(ID + "_FILE", valString) && Info.Create(valString) )
					{
						pParameter->asGrid_System()->Create(Info.m_System);
					}
					else if( _Found(ID + "_NX", nx)
						&&   _Found(ID + "_NY", ny)
						&&   _Found(ID + "_X" , x )
						&&   _Found(ID + "_Y" , y )
						&&   _Found(ID + "_D" , d ) )
					{
						pParameter->asGrid_System()->Create(d, x, y, nx, ny);
					}
					else
					{
						pParameter->asGrid_System()->Create(0., 0., 0., 0, 0);
					}
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	m_pTool->Update_Parameter_States();

	return( true );
}

//---------------------------------------------------------
bool CCMD_Tool::_Get_Input(CSG_Parameters *pParameters)
{
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter *pParameter = pParameters->Get_Parameter(i);

		if( pParameter->is_Input() )
		{
			if( !_Load_Input(pParameter) )
			{
				CMD_Print_Error(pParameter->Get_Name());

				return( false );
			}
		}

		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Table_Field: { CSG_String valString; int valInt;
				if( _Found(_Get_ID(pParameter), valString) )
				{
					if( valString.asInt(valInt) )
					{
						pParameter->Set_Value(valInt);
					}
					else
					{
						pParameter->Set_Value(valString);
					}
				}
				break; }

			case PARAMETER_TYPE_Table_Fields: { CSG_String valString;
				if( _Found(_Get_ID(pParameter), valString) )
				{
					pParameter->Set_Value(valString);
				}
				break; }
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
bool CCMD_Tool::_Load_Input(CSG_Parameter *pParameter)
{
	if(	!pParameter->is_Input() || !pParameter->is_Enabled() )
	{
		return( true );
	}

	CSG_String FileName;

	if( !_Found(_Get_ID(pParameter), FileName) )
	{
		if( !pParameter->is_Optional() )
		{
			CSG_String ID(_Get_ID(pParameter));

			CMD_Print_Error(CSG_String::Format("%s: %s", ID.c_str(), _TL("Provide a value for non-optional argument!")));

			return( false );
		}

		return( true );
	}

	if( pParameter->is_DataObject() )
	{
		if( !SG_Get_Data_Manager().Find(FileName) && !SG_Get_Data_Manager().Add(FileName) && !pParameter->is_Optional() )
		{
			CMD_Print_Error(_TL("input file"), FileName);

			return( false );
		}

		return( pParameter->Set_Value(SG_Get_Data_Manager().Find(FileName, false)) );
	}

	else if( pParameter->is_DataObject_List() )
	{
		pParameter->asList()->Del_Items();

		CSG_String FileNames(FileName);

		do
		{
			FileName  = FileNames.BeforeFirst(';'); FileName.Trim_Both();
			FileNames = FileNames.AfterFirst (';');

			if( !SG_Get_Data_Manager().Find(FileName) )
			{
				SG_Get_Data_Manager().Add(FileName);
			}

			pParameter->asList()->Add_Item(SG_Get_Data_Manager().Find(FileName, false));
		}
		while( FileNames.Length() > 0 );

		pParameter->has_Changed();

		if( pParameter->Get_Parent() && pParameter->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
		{
			pParameter->Get_Parent()->has_Changed();
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::_Save_Output(CSG_Parameters *pParameters)
{
	for(int j=0; j<pParameters->Get_Count(); j++)
	{
		CSG_Parameter *pParameter = pParameters->Get_Parameter(j); CSG_String FileName;

		//-------------------------------------------------
		if( pParameter->is_Input() )
		{
			if( pParameter->is_DataObject() )
			{
				CSG_Data_Object *pObject = pParameter->asDataObject();

				if( pObject && pObject->is_Modified() && SG_File_Exists(pObject->Get_File_Name()) )
				{
					_Save_Output(pObject, pObject->Get_File_Name());
				}
			}

			else if( pParameter->is_DataObject_List() )
			{
				for(int i=0; i<pParameter->asList()->Get_Item_Count(); i++)
				{
					CSG_Data_Object *pObject = pParameter->asList()->Get_Item(i);

					if( pObject->is_Modified() && SG_File_Exists(pObject->Get_File_Name()) )
					{
						_Save_Output(pObject, pObject->Get_File_Name());
					}
				}
			}
		}

		//-------------------------------------------------
		else if( pParameter->is_Output() && _Found(_Get_ID(pParameter), FileName) && FileName.Length() > 0 )
		{
			if( pParameter->is_DataObject() )
			{
				if( pParameter->asDataObject() )
				{
					_Save_Output(pParameter->asDataObject(), FileName);
				}
			}

			else if( pParameter->is_DataObject_List() )
			{
				CSG_Strings FileNames = SG_String_Tokenize(FileName, ";", SG_TOKEN_STRTOK);	// do not return empty tokens

				if( FileNames.Get_Count() > 0 )
				{
					int nFileNames = pParameter->asList()->Get_Item_Count() <= FileNames.Get_Count() ? FileNames.Get_Count() : FileNames.Get_Count() - 1;

					for(int i=0; i<pParameter->asList()->Get_Item_Count(); i++)
					{
						if( i < nFileNames )
						{
							_Save_Output(pParameter->asList()->Get_Item(i), FileNames[i]);
						}
						else
						{
							CSG_String fPath = SG_File_Get_Path     (FileNames[nFileNames]);
							CSG_String fName = SG_File_Get_Name     (FileNames[nFileNames], false);
							CSG_String fExt  = SG_File_Get_Extension(FileNames[nFileNames]);

							if( fName.is_Empty() || !fName.Cmp("*") )
							{
								fName = pParameter->asList()->Get_Item(i)->Get_Name();
							}
							else
							{
								fName += CSG_String::Format("%0*d", SG_Get_Digit_Count(pParameter->asList()->Get_Item_Count()), 1 + i - nFileNames);
							}

							_Save_Output(pParameter->asList()->Get_Item(i), SG_File_Make_Path(fPath, fName, fExt));
						}
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CCMD_Tool::_Save_Output(CSG_Data_Object *pObject, const CSG_String &FileName)
{
	pObject->Set_Name(SG_File_Get_Name(FileName, false));

	return( pObject->Save(FileName) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
