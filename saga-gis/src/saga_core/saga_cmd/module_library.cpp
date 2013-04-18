/**********************************************************
 * Version $Id$
 *********************************************************/
 
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
//                  module_library.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCMD_Module::CCMD_Module(void)
{
	m_pModule	= NULL;
}

CCMD_Module::CCMD_Module(CSG_Module *pModule)
{
	m_pModule	= NULL;

	Create(pModule);
}

//---------------------------------------------------------
CCMD_Module::~CCMD_Module(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Module::Create(CSG_Module *pModule)
{
	Destroy();

	m_CMD.SetSwitchChars(SG_T("-"));

	if( (m_pModule = pModule) != NULL )
	{
		_Set_Parameters(m_pModule->Get_Parameters(), false);

		for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
		{
			_Set_Parameters(m_pModule->Get_Parameters(i), true);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CCMD_Module::Destroy(void)
{
	m_pModule	= NULL;

	m_CMD.Reset();
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Module::Execute(int argc, char *argv[])
{
	if( !m_pModule )
	{
		return( false );
	}

	if( argc <= 1 )
	{
		m_CMD.Usage();

		return( false );
	}

	//-----------------------------------------------------
	// m_CMD.SetCmdLine(argc, argv);
	//
	// We can't do it this way (passing argv as char**) because then we use an
	// overload of the method which (re-)sets the locale from the current
	// enviromment; in order to prevent this, we use wxString overload

	wxString	sCmdLine;

	for(int i=1; i<argc; i++)
	{
		wxString	sTmp = argv[i];

		sCmdLine += wxString::Format(SG_T("\"%s\" "), sTmp.c_str());
	}

	m_CMD.SetCmdLine(sCmdLine);

	//-----------------------------------------------------
	bool	bResult	= _Get_Parameters(m_pModule->Get_Parameters(), true);
		
	for(int i=0; bResult && i<m_pModule->Get_Parameters_Count(); i++)
	{
		_Get_Parameters(m_pModule->Get_Parameters(i), false);
	}

	if( !bResult )
	{
		CMD_Print("");

		m_CMD.Usage();
	}

	//-----------------------------------------------------
	CMD_Set_Module(this);

	if( bResult && m_pModule->On_Before_Execution() )
	{
		bResult	= m_pModule->Execute();

		m_pModule->On_After_Execution();
	}

	CMD_Set_Module(NULL);

	//-----------------------------------------------------
	if( bResult )
	{
		_Save_Output();
	}
	else
	{
		CMD_Print_Error(_TL("executing module"), m_pModule->Get_Name());
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Module::Get_Parameters(CSG_Parameters *pParameters)
{
	return( _Get_Parameters(pParameters, true) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CCMD_Module::_Get_ID(CSG_Parameter *pParameter, const wxString &Modifier)
{
	wxString	ID(pParameter->Get_Owner()->Get_Identifier().c_str());

	if( ID.Length() > 0 )
	{
		ID	+= SG_T("_");
	}

	ID	+= pParameter->Get_Identifier();

	if( Modifier.Length() > 0 )
	{
		ID	+= SG_T("_") + Modifier;
	}

	return( ID );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Module::_Set_Parameters(CSG_Parameters *pParameters, bool bExtra)
{
	if( !pParameters )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		wxString	Description	= pParameter ->Get_Description(
			PARAMETER_DESCRIPTION_NAME|PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, SG_T("\n\t")
		).c_str();

		Description.Replace(wxT("\xb2"), wxT("2"));	// unicode problem: quick'n'dirty bug fix, to be replaced

		if( pParameter->is_Input() || pParameter->is_Output() )
		{
			m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR
			| (pParameter->is_Optional() || pParameter->is_Output() || bExtra
			  ? wxCMD_LINE_PARAM_OPTIONAL : wxCMD_LINE_OPTION_MANDATORY
			));
		}

		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Parameters:
				_Set_Parameters(pParameter->asParameters(), true);
				break;

			case PARAMETER_TYPE_Bool:
				m_CMD.AddSwitch(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Int:
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_Table_Field:
			case PARAMETER_TYPE_Table_Fields:
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Double:
			case PARAMETER_TYPE_Degree:
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Range:
				m_CMD.AddOption(_Get_ID(pParameter, wxT("MIN")), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				m_CMD.AddOption(_Get_ID(pParameter, wxT("MAX")), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_String:
			case PARAMETER_TYPE_Text:
			case PARAMETER_TYPE_FilePath:
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_FixedTable:
				m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Grid_System:
				if( pParameter->Get_Children_Count() == 0 )
				{
					m_CMD.AddOption(_Get_ID(pParameter, wxT("NX")), wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter, wxT("NY")), wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter, wxT( "X")), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter, wxT( "Y")), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					m_CMD.AddOption(_Get_ID(pParameter, wxT( "D")), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				}
				break;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CCMD_Module::_Get_Parameters(CSG_Parameters *pParameters, bool bCreateDataObjects)
{
	if( !pParameters )
	{
		return( false );
	}

	if( m_CMD.Parse(false) != 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		if( pParameter->is_Input() )
		{
			if( !_Load_Input(pParameters->Get_Parameter(i)) )
			{
				CMD_Print_Error(pParameters->Get_Parameter(i)->Get_Name());

				return( false );
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
				_Get_Parameters(pParameter->asParameters(), bCreateDataObjects);
				break;

			case PARAMETER_TYPE_Bool:
				pParameter->Set_Value(m_CMD.Found(_Get_ID(pParameter)) ? 1 : 0);
				break;

			case PARAMETER_TYPE_Int:
				if( m_CMD.Found(_Get_ID(pParameter), &l) )
				{
					pParameter->Set_Value((int)l);
				}
				break;

			case PARAMETER_TYPE_Choice:
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

			case PARAMETER_TYPE_Double:
			case PARAMETER_TYPE_Degree:
				if( m_CMD.Found(_Get_ID(pParameter), &s) && s.ToDouble(&d) )
				{
					pParameter->Set_Value(d);
				}
				break;

			case PARAMETER_TYPE_Range:
				if( m_CMD.Found(_Get_ID(pParameter, wxT("MIN")), &s) && s.ToDouble(&d) )
				{
					pParameter->asRange()->Set_LoVal(d);
				}

				if( m_CMD.Found(_Get_ID(pParameter, wxT("MAX")), &s) && s.ToDouble(&d) )
				{
					pParameter->asRange()->Set_HiVal(d);
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
						s.Prepend(wxT("\""));
						s.Replace(wxT(";"), wxT("\" \""));
						s.Append (wxT("\""));
					}

					pParameter->Set_Value(CSG_String(&s));
				}
				break;

			case PARAMETER_TYPE_FixedTable:
				if( m_CMD.Found(_Get_ID(pParameter), &s) )
				{
					CSG_Table	Table(&s);
					pParameter->asTable()->Assign_Values(&Table);
				}
				break;

			case PARAMETER_TYPE_Grid_System:
				if( pParameter->Get_Children_Count() == 0 )
				{
					long	nx, ny;
					double	d, x, y;

					if(	!m_CMD.Found(_Get_ID(pParameter, wxT("NX")), &nx)
					||	!m_CMD.Found(_Get_ID(pParameter, wxT("NY")), &ny)
					||	!m_CMD.Found(_Get_ID(pParameter, wxT( "X")), &s) || !s.ToDouble(&x)
					||	!m_CMD.Found(_Get_ID(pParameter, wxT( "Y")), &s) || !s.ToDouble(&y)
					||	!m_CMD.Found(_Get_ID(pParameter, wxT( "D")), &s) || !s.ToDouble(&d) )
					{
						pParameter->asGrid_System()->Assign(-1, 0.0, 0.0, 0, 0);
					}
					else
					{
						pParameter->asGrid_System()->Assign(d, x, y, (int)nx, (int)ny);
					}
				}
				break;
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
bool CCMD_Module::_Load_Input(CSG_Parameter *pParameter)
{
	wxString	FileName;

	if(	!pParameter->is_Input() )
	{
		return( true );
	}

	if( !m_CMD.Found(_Get_ID(pParameter), &FileName) )
	{
		return( pParameter->is_Optional() );
	}

	if( pParameter->is_DataObject() )
	{
		if( !SG_Get_Data_Manager().Add(&FileName) && !pParameter->is_Optional() )
		{
			CMD_Print_Error(_TL("input file"), &FileName);

			return( false );
		}

		return( pParameter->Set_Value(SG_Get_Data_Manager().Find(&FileName)) );
	}

	else if( pParameter->is_DataObject_List() )
	{
		pParameter->asList()->Del_Items();

		wxString	FileNames(FileName);

		do
		{
			FileName	= FileNames.BeforeFirst	(';');
			FileNames	= FileNames.AfterFirst	(';');

			if( SG_Get_Data_Manager().Add(&FileName) )
			{
				pParameter->asList()->Add_Item(SG_Get_Data_Manager().Find(&FileName));
			}
		}
		while( FileNames.Length() > 0 );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Module::_Save_Output(void)
{
	if( m_pModule )
	{
		_Save_Output(m_pModule->Get_Parameters());

		for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
		{
			_Save_Output(m_pModule->Get_Parameters(i));
		}

		SG_Get_Data_Manager().Delete_Unsaved();	// remove temporary data to save memory resources

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CCMD_Module::_Save_Output(CSG_Parameters *pParameters)
{
	for(int j=0; j<pParameters->Get_Count(); j++)
	{
		wxString		FileName;

		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(j);

		//-------------------------------------------------
		if( pParameter->is_Input() )
		{
			if( pParameter->is_DataObject() )
			{
				CSG_Data_Object	*pObject	= pParameter->asDataObject();

				if( pObject && pObject->is_Modified() && SG_File_Exists(pObject->Get_File_Name()) )
				{
					pObject->Save(pObject->Get_File_Name());
				}
			}

			else if( pParameter->is_DataObject_List() )
			{
				for(int i=0; i<pParameter->asList()->Get_Count(); i++)
				{
					CSG_Data_Object	*pObject	= pParameter->asList()->asDataObject(i);

					if( pObject->is_Modified() && SG_File_Exists(pObject->Get_File_Name()) )
					{
						pObject->Save(pObject->Get_File_Name());
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
					pParameter->asDataObject()->Save(&FileName);
				}
			}

			else if( pParameter->is_DataObject_List() )
			{
				CSG_Strings	FileNames;

				while( FileName.Length() > 0 )
				{
					CSG_String	s(&FileName.BeforeFirst(';'));

					if( s.Length() > 0 )
					{
						FileNames	+= s;
						FileName	 = FileName.AfterFirst(';');
					}
					else
					{
						FileNames	+= &FileName;
						FileName	.Clear();
					}
				}

				int	nFileNames	= pParameter->asList()->Get_Count() <= FileNames.Get_Count() ? FileNames.Get_Count() : FileNames.Get_Count() - 1;

				for(int i=0; i<pParameter->asList()->Get_Count(); i++)
				{
					if( i < nFileNames )
					{
						pParameter->asList()->asDataObject(i)->Save(FileNames[i]);
					}
					else
					{
						pParameter->asList()->asDataObject(i)->Save(CSG_String::Format(SG_T("%s_%0*d"),
							FileNames[FileNames.Get_Count() - 1].c_str(),
							SG_Get_Digit_Count(pParameter->asList()->Get_Count()),
							1 + i - nFileNames
						));
					}
				}
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
