
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
	m_pTool	= NULL;
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
		_Set_Parameters(m_pTool->Get_Parameters());

		for(int i=0; i<m_pTool->Get_Parameters_Count(); i++)
		{
			_Set_Parameters(m_pTool->Get_Parameters(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CCMD_Tool::Destroy(void)
{
	m_pTool	= NULL;

	m_CMD.Reset();
	m_CMD.SetSwitchChars("-");
}

//---------------------------------------------------------
void CCMD_Tool::Usage(void)
{
	if( m_pTool )
	{
		wxString	Usage	= wxString::Format("\nUsage: saga_cmd %s %s %s",
			m_pTool->Get_Library().c_str(),
			m_pTool->Get_ID     ().c_str(),
			m_CMD.GetUsageString().AfterFirst(' ').AfterFirst(' ')
		);

		SG_Printf(&Usage);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::Execute(int argc, char *argv[])
{
	//-----------------------------------------------------
	if( !m_pTool )
	{
		return( false );
	}

	if( argc <= 1 )
	{
		Usage();

		return( false );
	}

	//-----------------------------------------------------
	// m_CMD.SetCmdLine(argc, argv);
	//
	// We can't do it this way (passing argv as char**) because then we use an
	// overload of the method which (re-)sets the locale from the current
	// enviromment; in order to prevent this, we use wxString overload
	{
		wxString	sCmdLine;

		for(int i=1; i<argc; i++)
		{
			sCmdLine	+= wxString(i == 1 ? "\"" : " \"") + argv[i] + "\"";
		}

		m_CMD.SetCmdLine(sCmdLine);
	}

	if( m_CMD.Parse(false) != 0 )
	{
		Usage();

		return( false );
	}

	//-----------------------------------------------------
	int		i;

	bool	bResult	= _Get_Parameters(m_pTool->Get_Parameters(), true);

	for(i=0; bResult && i<m_pTool->Get_Parameters_Count(); i++)
	{
		bResult	= _Get_Parameters(m_pTool->Get_Parameters(i), true);
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
		bResult	= m_pTool->Execute(true);

		m_pTool->On_After_Execution();
	}

	CMD_Set_Tool(NULL);

	//-----------------------------------------------------
	if( bResult )
	{
		_Save_Output(m_pTool->Get_Parameters());

		for(i=0; i<m_pTool->Get_Parameters_Count(); i++)
		{
			_Save_Output(m_pTool->Get_Parameters(i));
		}

		SG_Get_Data_Manager().Delete_Unsaved();	// remove temporary data to save memory resources
	}
	else
	{
		CMD_Print_Error(_TL("executing tool"), m_pTool->Get_Name());
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CCMD_Tool::_Get_ID(CSG_Parameter *pParameter, const wxString &Modifier)
{
	wxString	ID(pParameter->Get_Parameters()->Get_Identifier().c_str());

	if( ID.Length() > 0 )
	{
		ID	+= "_";
	}

	ID	+= pParameter->Get_Identifier();

	if( Modifier.Length() > 0 )
	{
		ID	+= "_" + Modifier;
	}

	return( ID );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Tool::_Set_Parameters(CSG_Parameters *pParameters)
{
	if( !pParameters )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

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

		wxString	Description	= pParameter->Get_Description(
			PARAMETER_DESCRIPTION_NAME|PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, SG_T("\n\t")
		).c_str();

		Description.Replace("\xb", "");	// unicode problem: quick'n'dirty bug fix, to be replaced

		if( pParameter->is_Input() || pParameter->is_Output() )
		{
			m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR|wxCMD_LINE_PARAM_OPTIONAL);
		}

		//-------------------------------------------------
		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			switch( pParameter->Get_Type() )
			{
			case PARAMETER_TYPE_Parameters  :
				_Set_Parameters(pParameter->asParameters());
				break;

			case PARAMETER_TYPE_Bool        :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Int         :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Choice      :
			case PARAMETER_TYPE_Choices     :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Date        :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_DATE  , wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Range       :
				m_CMD.AddOption(_Get_ID(pParameter, "MIN"), wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				m_CMD.AddOption(_Get_ID(pParameter, "MAX"), wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Color       :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Colors      :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_FixedTable  :
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Grid_System :
				if( pParameter->Get_Children_Count() == 0 )
				{
					m_CMD.AddOption(_Get_ID(pParameter,    "D"), wxEmptyString, _TL("Cell Size"                          ), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter,    "X"), wxEmptyString, _TL("Lower Left Center Cell X-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter,    "Y"), wxEmptyString, _TL("Lower Left Center Cell Y-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter,   "NX"), wxEmptyString, _TL("Number of Columns"                  ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter,   "NY"), wxEmptyString, _TL("Number of Rows"                     ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter, "FILE"), wxEmptyString, _TL("Grid File"                          ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
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
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

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
			if( pParameter->is_DataObject() && pParameter->is_Optional() && !pParameter->asDataObject() && m_CMD.Found(_Get_ID(pParameter)) )
			{
				pParameter->Set_Value(DATAOBJECT_CREATE);
			}
		}

		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			long		l;
			double		d;
			wxString	s;

			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Parameters:
				_Get_Parameters(pParameter->asParameters(), bInitialize);

				pParameter->has_Changed();
				break;

			case PARAMETER_TYPE_Bool:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					if(      (s.ToLong(&l) && l == 1) || !s.CmpNoCase("true" ) )
					{
						pParameter->Set_Value(1);
					}
					else if( (s.ToLong(&l) && l == 0) || !s.CmpNoCase("false") )
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
				if( m_CMD.Found(_Get_ID(pParameter), &l) )
				{
					pParameter->Set_Value((int)l);
				}
				break;

			case PARAMETER_TYPE_Choice:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					if( s.ToLong(&l) )
					{
						pParameter->Set_Value((int)l);
					}
					else
					{
						pParameter->Set_Value(CSG_String(&s));
					}
				}
				break;

			case PARAMETER_TYPE_Choices:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					pParameter->Set_Value(CSG_String(&s));
				}
				break;

			case PARAMETER_TYPE_Double:
			case PARAMETER_TYPE_Degree:
				if( m_CMD.Found(_Get_ID(pParameter), &d) )
				{
					pParameter->Set_Value(d);
				}
				break;

			case PARAMETER_TYPE_Date:
				{
					wxDateTime	Date;

					if( m_CMD.Found(_Get_ID(pParameter), &Date) )
					{
						pParameter->Set_Value(Date.GetJDN());
					}
				}
				break;

			case PARAMETER_TYPE_Range:
				if( m_CMD.Found(_Get_ID(pParameter, "MIN"), &d) )
				{
					pParameter->asRange()->Set_Min(d);

					pParameter->has_Changed();
				}

				if( m_CMD.Found(_Get_ID(pParameter, "MAX"), &d) )
				{
					pParameter->asRange()->Set_Max(d);

					pParameter->has_Changed();
				}
				break;

			case PARAMETER_TYPE_Color:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					pParameter->Set_Value(CSG_String(&s));
				}
				break;

			case PARAMETER_TYPE_Colors:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					pParameter->asColors()->Load(CSG_String(&s));
				}
				break;

			case PARAMETER_TYPE_String:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					pParameter->Set_Value(CSG_String(&s));
				}
				break;

			case PARAMETER_TYPE_Text:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					CSG_File	Stream;

					if( Stream.Open(CSG_String(&s)) )
					{
						CSG_String	t;

						Stream.Read(t, Stream.Length());

						pParameter->Set_Value(t.c_str());
					}
					else
					{
						pParameter->Set_Value(CSG_String(&s));
					}
				}
				break;

			case PARAMETER_TYPE_FilePath:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					if( pParameter->asFilePath()->is_Multiple() )
					{
						s.Prepend("\"");
						s.Replace(";", "\" \"");
						s.Append ("\"");
					}

					pParameter->Set_Value(CSG_String(&s));
				}
				break;

			case PARAMETER_TYPE_FixedTable:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					CSG_Table	Table(&s);

					pParameter->asTable()->Assign_Values(&Table);

					pParameter->has_Changed();
				}
				break;

			case PARAMETER_TYPE_Grid_System:
				if( pParameter->Get_Children_Count() == 0 )
				{
					CSG_Grid_File_Info Info; double x, y; long nx, ny;

					if( m_CMD.Found(_Get_ID(pParameter, "FILE"), &s) && Info.Create(&s) )
					{
						pParameter->asGrid_System()->Create(Info.m_System);
					}
					else if( m_CMD.Found(_Get_ID(pParameter, "NX"), &nx)
						&&   m_CMD.Found(_Get_ID(pParameter, "NY"), &ny)
						&&   m_CMD.Found(_Get_ID(pParameter,  "X"), &x )
						&&   m_CMD.Found(_Get_ID(pParameter,  "Y"), &y )
						&&   m_CMD.Found(_Get_ID(pParameter,  "D"), &d ) )
					{
						pParameter->asGrid_System()->Create(d, x, y, (int)nx, (int)ny);
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
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

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
			long		l;
			wxString	s;

			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Table_Field:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					if( s.ToLong(&l) )
					{
						pParameter->Set_Value((int)l);
					}
					else
					{
						pParameter->Set_Value(CSG_String(&s));
					}
				}
				break;

			case PARAMETER_TYPE_Table_Fields:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					pParameter->Set_Value(CSG_String(&s));
				}
				break;
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
	wxString	FileName;

	if(	!pParameter->is_Input() || !pParameter->is_Enabled() )
	{
		return( true );
	}

	if( !m_CMD.Found(_Get_ID(pParameter), &FileName) )
	{
		if( !pParameter->is_Optional() )
		{
			wxString	Msg(wxString::Format(_TL("The value for the option '%s' must be specified."), _Get_ID(pParameter)));

			CMD_Print_Error(&Msg);

			return( false );
		}

		return( true );
	}

	if( pParameter->is_DataObject() )
	{
		if( !SG_Get_Data_Manager().Find(&FileName) && !SG_Get_Data_Manager().Add(&FileName) && !pParameter->is_Optional() )
		{
			CMD_Print_Error(_TL("input file"), &FileName);

			return( false );
		}

		return( pParameter->Set_Value(SG_Get_Data_Manager().Find(&FileName, false)) );
	}

	else if( pParameter->is_DataObject_List() )
	{
		pParameter->asList()->Del_Items();

		wxString	FileNames(FileName);

		do
		{
			FileName	= FileNames.BeforeFirst(';').Trim(false);
			FileNames	= FileNames.AfterFirst (';');

			if( !SG_Get_Data_Manager().Find(&FileName) )
			{
				SG_Get_Data_Manager().Add(&FileName);
			}

			pParameter->asList()->Add_Item(SG_Get_Data_Manager().Find(&FileName, false));
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
		wxString	FileName;

		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(j);

		//-------------------------------------------------
		if( pParameter->is_Input() )
		{
			if( pParameter->is_DataObject() )
			{
				CSG_Data_Object	*pObject	= pParameter->asDataObject();

				if( pObject && pObject->is_Modified() && SG_File_Exists(pObject->Get_File_Name()) )
				{
					_Save_Output(pObject, pObject->Get_File_Name());
				}
			}

			else if( pParameter->is_DataObject_List() )
			{
				for(int i=0; i<pParameter->asList()->Get_Item_Count(); i++)
				{
					CSG_Data_Object	*pObject	= pParameter->asList()->Get_Item(i);

					if( pObject->is_Modified() && SG_File_Exists(pObject->Get_File_Name()) )
					{
						_Save_Output(pObject, pObject->Get_File_Name());
					}
				}
			}
		}

		//-------------------------------------------------
		else if( pParameter->is_Output() &&  m_CMD.Found(_Get_ID(pParameter), &FileName) && FileName.Length() > 0 )
		{
			if( pParameter->is_DataObject() )
			{
				if( pParameter->asDataObject() )
				{
					_Save_Output(pParameter->asDataObject(), &FileName);
				}
			}

			else if( pParameter->is_DataObject_List() )
			{
				CSG_Strings	FileNames	= SG_String_Tokenize(&FileName, ";", SG_TOKEN_STRTOK);	// do not return empty tokens

				if( FileNames.Get_Count() > 0 )
				{
					int	nFileNames	= pParameter->asList()->Get_Item_Count() <= FileNames.Get_Count() ? FileNames.Get_Count() : FileNames.Get_Count() - 1;

					for(int i=0; i<pParameter->asList()->Get_Item_Count(); i++)
					{
						if( i < nFileNames )
						{
							_Save_Output(pParameter->asList()->Get_Item(i), FileNames[i]);
						}
						else
						{
							CSG_String	fPath	= SG_File_Get_Path     (FileNames[nFileNames]);
							CSG_String	fName	= SG_File_Get_Name     (FileNames[nFileNames], false);
							CSG_String	fExt	= SG_File_Get_Extension(FileNames[nFileNames]);

							if( fName.is_Empty() || !fName.Cmp("*") )
							{
								fName	= pParameter->asList()->Get_Item(i)->Get_Name();
							}
							else
							{
								fName	+= CSG_String::Format("%0*d", SG_Get_Digit_Count(pParameter->asList()->Get_Item_Count()), 1 + i - nFileNames);
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
