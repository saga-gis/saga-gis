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

	if( (m_pModule = pModule) != NULL )
	{
		m_pModule->Set_Managed(true);

		_Set_CMD(m_pModule->Get_Parameters(), false);

		for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
		{
			_Set_CMD(m_pModule->Get_Parameters(i), true);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CCMD_Module::Destroy(void)
{
	if( m_pModule )
	{
		m_pModule->Set_Managed(false);
	}

	m_pModule	= NULL;

	m_CMD.Reset();

	m_Data_Objects.Clear();
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

	if( argc == 1 )
	{
		m_CMD.Usage();

		return( true );
	}

	//-----------------------------------------------------
	m_Data_Objects.Clear();

	/* m_CMD.SetCmdLine(argc, argv);
	We can't do it this way (passing argv as char**) because then we use an
	overload of the method which (re-)sets the locale from the current
	enviromment; in order to prevent this, we use wxString overload */

	wxString	sCmdLine;

	for(int i=1; i<argc; i++)
	{
		wxString	sTmp = argv[i];
		sCmdLine += wxString::Format(SG_T("\"%s\" "), sTmp.c_str());
	}

	m_CMD.SetCmdLine(sCmdLine);

	bool	bResult	= _Get_CMD(m_pModule->Get_Parameters(), false);
		
	for(int i=0; i<m_pModule->Get_Parameters_Count() && bResult; i++)
	{
		_Get_CMD(m_pModule->Get_Parameters(i), true);
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
	_Destroy_DataObjects(bResult);

	m_Data_Objects.Clear();

	if( !bResult )
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
	return( _Get_CMD(pParameters, false) );
}

//---------------------------------------------------------
bool CCMD_Module::Add_DataObject(CSG_Data_Object *pObject)
{
	m_Data_Objects.Add(pObject);

	return( true );
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
void CCMD_Module::_Set_CMD(CSG_Parameters *pParameters, bool bExtra)
{
	CSG_Parameter	*pParameter;
	wxString		Description;

	//-----------------------------------------------------
	if( pParameters )
	{
		m_CMD.SetSwitchChars(SG_T("-"));

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			pParameter	= pParameters->Get_Parameter(i);
			Description	= pParameter->Get_Description(
				PARAMETER_DESCRIPTION_NAME|PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, SG_T("\n\t")
			).c_str();

			Description.Replace(wxT("\xb2"), wxT("2"));	// unicode problem: quick'n'dirty bug fix, to be replaced

			if( pParameter->is_Input() || pParameter->is_Output() )
			{
				m_CMD.AddOption(
					_Get_ID(pParameter), wxEmptyString, Description,
					wxCMD_LINE_VAL_STRING,
					wxCMD_LINE_NEEDS_SEPARATOR | (pParameter->is_Optional() || bExtra ? wxCMD_LINE_PARAM_OPTIONAL : wxCMD_LINE_OPTION_MANDATORY)
				);
			}
			else if( pParameter->is_Option() && !pParameter->is_Information() )
			{
				switch( pParameter->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Bool:
					m_CMD.AddSwitch(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_Int:
					m_CMD.AddOption(_Get_ID(pParameter), wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_Choice:
				case PARAMETER_TYPE_Table_Field:
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

				case PARAMETER_TYPE_Parameters:
					_Set_CMD(pParameter->asParameters(), true);
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------
bool CCMD_Module::_Get_CMD(CSG_Parameters *pParameters, bool bNoDataObjects)
{
	//-----------------------------------------------------
	if( !pParameters )
	{
		CMD_Print_Error(_TL("Internal system error"));

		return( false );
	}

	if( m_CMD.Parse(false) != 0 || (bNoDataObjects == false && _Create_DataObjects(pParameters) == false) )
	{
		m_CMD.Usage();

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		long			l;
		double			d;
		wxString		s;

		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		if( !pParameter->is_Information() && !pParameter->is_DataObject() )
		{
			switch( pParameter->Get_Type() )
			{
			default:
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

			case PARAMETER_TYPE_Parameters:
				_Get_CMD(pParameter->asParameters(), bNoDataObjects);
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
bool CCMD_Module::_Create_DataObjects(CSG_Parameters *pParameters)
{
	//-----------------------------------------------------
	if( !pParameters )
	{
		CMD_Print_Error(_TL("Internal system error"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bObjects	= false;
	int		nObjects	= 0;

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		wxString		FileName;

		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		if(	pParameter->is_DataObject() || pParameter->is_DataObject_List() )
		{
			bObjects	= true;

			if( m_CMD.Found(_Get_ID(pParameter), &FileName) )
			{
				if( pParameter->is_Input() )
				{
					if( pParameter->is_DataObject() )
					{
						if( !_Create_DataObject(pParameter, FileName) && !pParameter->is_Optional() )
						{
							CMD_Print_Error(_TL("input file"), FileName);

							return( false );
						}

						nObjects++;
					}
					else if( pParameter->is_DataObject_List() )
					{
						if( !_Create_DataObject_List(pParameter, FileName) && !pParameter->is_Optional() )
						{
							CMD_Print_Error(_TL("empty input list"), _Get_ID(pParameter));

							return( false );
						}

						nObjects++;
					}
				}
				else if( pParameter->is_Output() )
				{
					if( !_Create_DataObject(pParameter, FileName) )
					{
						pParameter->Set_Value(DATAOBJECT_CREATE);
					}

					nObjects++;
				}
			}
			else if( !pParameter->is_Optional() )
			{
				return( false );
			}
		}
	}

	return( bObjects == false || nObjects > 0 );
}

//---------------------------------------------------------
bool CCMD_Module::_Create_DataObject(CSG_Parameter *pParameter, const wxString &FileName)
{
	if( !SG_File_Exists(FileName) )
	{
		return( false );
	}

	CSG_Data_Object	*pObject;

	switch( pParameter->Get_Type() )
	{
	default:						pObject	= NULL;											break;
	case PARAMETER_TYPE_TIN:		pObject = new CSG_TIN			(&FileName);	break;
	case PARAMETER_TYPE_PointCloud:	pObject = new CSG_PointCloud	(&FileName);	break;
	case PARAMETER_TYPE_Shapes:		pObject = new CSG_Shapes		(&FileName);	break;
	case PARAMETER_TYPE_Table:		pObject = new CSG_Table			(&FileName);	break;
	case PARAMETER_TYPE_Grid:		pObject	= new CSG_Grid			(&FileName);	break;
	}

	if( pObject )
	{
		if( pObject->is_Valid() && pParameter->Get_Type() == PARAMETER_TYPE_Grid )
		{
			if( !pParameter->Get_Parent()->asGrid_System()->is_Valid() )
			{
				pParameter->Get_Parent()->asGrid_System()->Assign(((CSG_Grid *)pObject)->Get_System());
			}
			else if( !pParameter->Get_Parent()->asGrid_System()->is_Equal(((CSG_Grid *)pObject)->Get_System()) )
			{
				delete(pObject);

				return( false );
			}
		}

		if( pObject->is_Valid() )
		{
			pParameter->Set_Value(pObject);

			return( true );
		}

		delete(pObject);
	}

	return( false );
}

//---------------------------------------------------------
bool CCMD_Module::_Create_DataObject_List(CSG_Parameter *pParameter, wxString FileNames)
{
	CSG_Data_Object		*pObject;
	wxString			FileName;

	if( pParameter && pParameter->is_DataObject_List() )
	{
		do
		{
			FileName	= FileNames.BeforeFirst	(';');
			FileNames	= FileNames.AfterFirst	(';');

			switch( pParameter->Get_Type() )
			{
			default:								pObject	= NULL;								break;
			case PARAMETER_TYPE_Grid_List:			pObject	= new CSG_Grid      (&FileName);	break;
			case PARAMETER_TYPE_TIN_List:			pObject	= new CSG_TIN       (&FileName);	break;
			case PARAMETER_TYPE_PointCloud_List:	pObject	= new CSG_PointCloud(&FileName);	break;
			case PARAMETER_TYPE_Shapes_List:		pObject	= new CSG_Shapes    (&FileName);	break;
			case PARAMETER_TYPE_Table_List:			pObject	= new CSG_Table     (&FileName);	break;
			}

			if( pObject && pObject->is_Valid() )
			{
				if( pParameter->Get_Type() == PARAMETER_TYPE_Grid_List && (pParameter->Get_Parent() && pParameter->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System) )
				{	// grid system dependent grid list: first grid defines the grid system to be used!
					if( pParameter->asList()->Get_Count() == 0 )
					{
						pParameter->Get_Parent()->asGrid_System()->Assign  (((CSG_Grid *)pObject)->Get_System());
					}

					if( pParameter->Get_Parent()->asGrid_System()->is_Equal(((CSG_Grid *)pObject)->Get_System()) )
					{
						pParameter->asList()->Add_Item(pObject);
					}
				}
				else
				{
					pParameter->asList()->Add_Item(pObject);
				}
			}
			else if( pObject )
			{
				delete(pObject);

				CMD_Print_Error(_TL("input file"), FileName);
			}
		}
		while( FileNames.Length() > 0 );

		return( pParameter->asList()->Get_Count() > 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCMD_Module::_Destroy_DataObjects(bool bSave)
{
	if( m_pModule )
	{
		_Destroy_DataObjects(bSave, m_pModule->Get_Parameters());

		for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
		{
			_Destroy_DataObjects(bSave, m_pModule->Get_Parameters(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CCMD_Module::_Destroy_DataObjects(bool bSave, CSG_Parameters *pParameters)
{
	if( !pParameters )
	{
		return( false );
	}

	for(int j=0; j<pParameters->Get_Count(); j++)
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(j);

		wxString	FileName;

		if( !bSave || !pParameter->is_Output() || !m_CMD.Found(_Get_ID(pParameter), &FileName) )
		{
			FileName.Clear();
		}

		//-------------------------------------------------
		if( pParameter->is_DataObject() && pParameter->asDataObject() )
		{
			CSG_Data_Object	*pObject	= pParameter->asDataObject();

			if( pParameter->is_Input() && pObject->is_Modified() )
			{
				pObject->Save(pObject->Get_File_Name());
			}
			else if( FileName.Length() > 0 )
			{
				pObject->Save(&FileName);
			}

			m_Data_Objects.Add(pObject);

			pParameter->Set_Value(DATAOBJECT_NOTSET);
		}

		//-------------------------------------------------
		else if( pParameter->is_DataObject_List() )
		{
			if( pParameter->is_Input() )
			{
				for(int i=0; i<pParameter->asList()->Get_Count(); i++)
				{
					CSG_Data_Object	*pObject	= pParameter->asList()->asDataObject(i);

					if( pObject->is_Modified() )
					{
						pObject->Save(pObject->Get_File_Name());
					}

					m_Data_Objects.Add(pObject);
				}
			}
			else if( FileName.Length() > 0 ) // if( pParameter->is_Output() )
			{
				CSG_Strings	FileNames;

				while( FileName.Length() > 0 )
				{
					CSG_String	s(FileName.BeforeFirst(';').wx_str());

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
					CSG_Data_Object	*pObject	= pParameter->asList()->asDataObject(i);

					if( i < nFileNames )
					{
						pObject->Save(FileNames[i]);
					}
					else
					{
						pObject->Save(CSG_String::Format(SG_T("%s_%0*d"),
							FileNames[FileNames.Get_Count() - 1].c_str(),
							SG_Get_Digit_Count(pParameter->asList()->Get_Count()),
							1 + i - nFileNames
						));
					}

					m_Data_Objects.Add(pObject);
				}
			}

			pParameter->asList()->Del_Items();
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
CCMD_Data_Objects::CCMD_Data_Objects(void)
{
	m_pObjects	= NULL;
	m_nObjects	= 0;
}

//---------------------------------------------------------
CCMD_Data_Objects::~CCMD_Data_Objects(void)
{
	Clear(false);
}

//---------------------------------------------------------
void CCMD_Data_Objects::Clear(bool bDelete)
{
	if( m_pObjects )
	{
		if( bDelete )
		{
			for(int i=0; i<m_nObjects; i++)
			{
				delete(m_pObjects[i]);
			}
		}

		SG_Free(m_pObjects);

		m_pObjects	= NULL;
		m_nObjects	= 0;
	}
}

//---------------------------------------------------------
void CCMD_Data_Objects::Add(class CSG_Data_Object *pObject)
{
	if( pObject != DATAOBJECT_NOTSET && pObject != DATAOBJECT_CREATE )
	{
		for(int i=0; i<m_nObjects; i++)
		{
			if( m_pObjects[i] == pObject )
			{
				return;
			}
		}

		m_pObjects	= (CSG_Data_Object **)SG_Realloc(m_pObjects, (m_nObjects + 1) * sizeof(CSG_Data_Object *));
		m_pObjects[m_nObjects++]	= pObject;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
