
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
#include <wx/cmdline.h>

#include <saga_api/saga_api.h>

#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CModule_Library::CModule_Library(void)
{
	m_nModules	= 0;
	m_Modules	= NULL;
	m_pSelected	= NULL;
	m_pCMD		= NULL;
}

//---------------------------------------------------------
CModule_Library::~CModule_Library(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule_Library::Create(const char *FileName, const char *FilePath)
{
	PFNC_MLB_Initialize			MLB_Initialize;
	PFNC_MLB_Get_Interface		MLB_Get_Interface;

	CModule_Library_Interface	*pInterface;
	CModule						*pModule;

	//-----------------------------------------------------
	Destroy();

	m_FileName	= SG_File_Make_Path(FilePath, FileName, NULL).c_str();

	m_Library.Load(m_FileName);

	//-----------------------------------------------------
	if( !m_Library.IsLoaded() )
	{
		Print_Error(LNG("[ERR] Library could not be loaded"), FileName);
	}
	else
	{
		MLB_Initialize		= (PFNC_MLB_Initialize)		m_Library.GetSymbol(SYMBOL_MLB_Initialize);
		MLB_Get_Interface	= (PFNC_MLB_Get_Interface)	m_Library.GetSymbol(SYMBOL_MLB_Get_Interface);

		if(	!MLB_Get_Interface	|| !(pInterface = MLB_Get_Interface())
		||	!MLB_Initialize		|| !MLB_Initialize(m_FileName) )
		{
			Print_Error(LNG("[ERR] Invalid library"), FileName);
		}
		else
		{
			while( (pModule = pInterface->Get_Module(m_nModules)) != NULL )
			{
				m_Modules	= (CModule **)SG_Realloc(m_Modules, (m_nModules + 1) * sizeof(CModule *));
				m_Modules[m_nModules++]	= pModule;
			}

			if( m_nModules > 0 )
			{
				return( true );
			}

			Print_Error(LNG("[ERR] Library does not contain any modules"), FileName);
		}
	}

	return( false );
}

//---------------------------------------------------------
void CModule_Library::Destroy(void)
{
	if( m_nModules > 0 )
	{
		SG_Free(m_Modules);
		m_nModules	= 0;
		m_Modules	= NULL;
		m_pSelected	= NULL;
	}

	if( m_pCMD )
	{
		delete(m_pCMD);
		m_pCMD		= NULL;
	}

	if( m_Library.IsLoaded() )
	{
		m_Library.Unload();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CModule * CModule_Library::Select(const char *ModuleName)
{
	int			i;
	CParameters	*pParameters;
	wxString	Description;

	//-----------------------------------------------------
	if( m_pCMD )
	{
		delete(m_pCMD);
		m_pCMD	= NULL;
	}

	for(i=0, m_pSelected=NULL; i<m_nModules && !m_pSelected; i++)
	{
		if( !strcmp(ModuleName, Get_Module(i)->Get_Name()) )
		{
			m_pSelected	= Get_Module(i);
		}
	}

	if( !m_pSelected )
	{
		long		l;
		wxString	s(ModuleName);

		if( s.ToLong(&l) )
		{
			m_pSelected	= Get_Module((int)l);
		}
	}

	//-----------------------------------------------------
	if( m_pSelected )
	{
		m_pCMD	= new wxCmdLineParser;

		_Set_CMD(m_pCMD, m_pSelected->Get_Parameters());

		for(i=0; i<m_pSelected->Get_Extra_Parameters_Count(); i++)
		{
			pParameters	= m_pSelected->Get_Extra_Parameters(i);

			if( pParameters->Get_Count() > 0 )
			{
				Description.Printf("[%s] %s", LNG("parameters"), pParameters->Get_Description());

				m_pCMD->AddOption(pParameters->Get_Identifier(), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
			}
		}
	}

	return( m_pSelected );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule_Library::Execute(int argc, char *argv[])
{
	bool	bResult	= false;

	if( _Get_CMD(m_pCMD, m_pSelected->Get_Parameters(), argc, argv) )
	{
		bResult	= m_pSelected->Execute();

		_Destroy_DataObjects(bResult);
	}

	if( !bResult )
	{
		Print_Error(LNG("executing module"), m_pSelected->Get_Name());
	}

	return( bResult );
}

//---------------------------------------------------------
bool CModule_Library::Get_Parameters(CParameters *pParameters)
{
	wxString		s;
	wxCmdLineParser	CMD;

	if( !pParameters )
	{
		Print_Error(LNG("extra parameters"), "null");
	}
	else if( pParameters->Get_Count() <= 0 )
	{
		return( true );
	}
	else if( !m_pCMD || !m_pCMD->Found(pParameters->Get_Identifier(), &s) )
	{
		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			if( !pParameters->Get_Parameter(i)->is_Option() && !pParameters->Get_Parameter(i)->is_Optional() )
			{
				Print_Error(LNG("extra parameters needed"), pParameters->Get_Identifier());

				return( false );
			}
		}

		return( true );
	}
	else
	{
		_Set_CMD(&CMD, pParameters);

		if( _Get_CMD(&CMD, pParameters, s) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule_Library::_Set_CMD(wxCmdLineParser *pCMD, CParameters *pParameters)
{
	CParameter	*pParameter;
	wxString	Description;

	//-----------------------------------------------------
	if( pCMD && pParameters )
	{
		pCMD->SetSwitchChars("-");

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			pParameter	= pParameters->Get_Parameter(i);
			Description	= pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, "\n\t").c_str();

			if( pParameter->is_Input() || pParameter->is_Output() )
			{
				pCMD->AddOption(
					pParameter->Get_Identifier(), wxEmptyString, Description,
					wxCMD_LINE_VAL_STRING,
					wxCMD_LINE_NEEDS_SEPARATOR | (pParameter->is_Optional() ? wxCMD_LINE_PARAM_OPTIONAL : wxCMD_LINE_OPTION_MANDATORY)
				);
			}
			else if( pParameter->is_Option() && !pParameter->is_Information() )
			{
				switch( pParameter->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Bool:
					pCMD->AddSwitch(pParameter->Get_Identifier(), wxEmptyString, Description, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_Int:
				case PARAMETER_TYPE_Choice:
				case PARAMETER_TYPE_Table_Field:
					pCMD->AddOption(pParameter->Get_Identifier(), wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_Double:
				case PARAMETER_TYPE_Degree:
					pCMD->AddOption(pParameter->Get_Identifier(), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_Range:
					pCMD->AddOption(wxString::Format("%s_MIN", pParameter->Get_Identifier()), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					pCMD->AddOption(wxString::Format("%s_MAX", pParameter->Get_Identifier()), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_String:
				case PARAMETER_TYPE_Text:
				case PARAMETER_TYPE_FilePath:
					pCMD->AddOption(pParameter->Get_Identifier(), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					break;

				case PARAMETER_TYPE_FixedTable:
					pCMD->AddOption(pParameter->Get_Identifier(), wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------
bool CModule_Library::_Get_CMD(wxCmdLineParser *pCMD, CParameters *pParameters, wxString cmdLine)
{
	if( pCMD && pParameters )
	{
		pCMD->SetCmdLine(cmdLine);

		return( _Get_CMD(pCMD, pParameters) );
	}

	return( false );
}

bool CModule_Library::_Get_CMD(wxCmdLineParser *pCMD, CParameters *pParameters, int argc, char *argv[])
{
	if( pCMD && pParameters )
	{
		pCMD->SetCmdLine(argc, argv);

		return( _Get_CMD(pCMD, pParameters) );
	}

	return( false );
}

bool CModule_Library::_Get_CMD(wxCmdLineParser *pCMD, CParameters *pParameters)
{
	int			i;
	long		l;
	double		d;
	CParameter	*pParameter;
	wxString	s;

	//-----------------------------------------------------
	if( pCMD && pParameters && !pCMD->Parse() && _Create_DataObjects(pCMD, pParameters) )
	{
		for(i=0; i<pParameters->Get_Count(); i++)
		{
			pParameter	= pParameters->Get_Parameter(i);

			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Bool:
				pParameter->Set_Value(pCMD->Found(pParameter->Get_Identifier()) ? 1 : 0);
				break;

			case PARAMETER_TYPE_Int:
			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_Table_Field:
				if( pCMD->Found(pParameter->Get_Identifier(), &l) )
				{
					pParameter->Set_Value((int)l);
				}
				break;

			case PARAMETER_TYPE_Double:
			case PARAMETER_TYPE_Degree:
				if( pCMD->Found(pParameter->Get_Identifier(), &s) && s.ToDouble(&d) )
				{
					pParameter->Set_Value(d);
				}
				break;

			case PARAMETER_TYPE_Range:
				if( pCMD->Found(wxString::Format("%s_MIN", pParameter->Get_Identifier()), &s) && s.ToDouble(&d) )
				{
					pParameter->asRange()->Set_LoVal(d);
				}

				if( pCMD->Found(wxString::Format("%s_MAX", pParameter->Get_Identifier()), &s) && s.ToDouble(&d) )
				{
					pParameter->asRange()->Set_HiVal(d);
				}
				break;

			case PARAMETER_TYPE_String:
			case PARAMETER_TYPE_Text:
			case PARAMETER_TYPE_FilePath:
				if( pCMD->Found(pParameter->Get_Identifier(), &s) )
				{
					pParameter->Set_Value(s.c_str());
				}
				break;

			case PARAMETER_TYPE_FixedTable:
				if( pCMD->Found(pParameter->Get_Identifier(), &s) )
				{
					CTable	Table(s.c_str());
					pParameter->asTable()->Assign_Values(&Table);
				}
				break;
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule_Library::_Create_DataObjects(wxCmdLineParser *pCMD, CParameters *pParameters)
{
	int			j;
	CDataObject	*pObject;
	CParameter	*pParameter;
	wxString	s;

	if( pParameters && pCMD )
	{
		for(j=0; j<pParameters->Get_Count(); j++)
		{
			pParameter	= pParameters->Get_Parameter(j);

			if(	pParameter->is_DataObject() || pParameter->is_DataObject_List() )
			{
				if( pCMD->Found(pParameter->Get_Identifier(), &s) )
				{
					if( pParameter->is_Input() )
					{
						if( pParameter->is_DataObject() )
						{
							switch( pParameter->Get_Type() )
							{
							default:
								pObject	= NULL;
								break;

							case PARAMETER_TYPE_Grid:
								pObject	= new CGrid  (s.c_str());
								if( pObject && pObject->is_Valid() )
								{
									pParameter->Get_Parent()->asGrid_System()->Assign(((CGrid *)pObject)->Get_System());
									pParameter->Set_Value(pObject);
								}
								break;

							case PARAMETER_TYPE_TIN:
								pParameter->Set_Value(pObject = new CTIN   (s.c_str()));
								break;

							case PARAMETER_TYPE_Shapes:
								pParameter->Set_Value(pObject = new CShapes(s.c_str()));
								break;

							case PARAMETER_TYPE_Table:
								pParameter->Set_Value(pObject = new CTable (s.c_str()));
								break;
							}

							if( !pObject || !pObject->is_Valid() )
							{
								Print_Error(LNG("input file"), s);

								return( false );
							}
						}
						else if( pParameter->is_DataObject_List() )
						{
							if( !_Create_DataObject_List(pParameter, s) && !pParameter->is_Optional() )
							{
								Print_Error(LNG("empty input list"), pParameter->Get_Identifier());

								return( false );
							}
						}
					}
					else if( pParameter->is_Output() )
					{
						switch( pParameter->Get_Type() )
						{
						default:
							pObject	= NULL;
							break;

						case PARAMETER_TYPE_Grid:
							pParameter->Set_Value(pObject = new CGrid  (*pParameter->Get_Parent()->asGrid_System(), GRID_TYPE_Float));
							break;

						case PARAMETER_TYPE_TIN:
							pParameter->Set_Value(pObject = new CTIN   ());
							break;

						case PARAMETER_TYPE_Shapes:
							pParameter->Set_Value(pObject = new CShapes());
							break;

						case PARAMETER_TYPE_Table:
							pParameter->Set_Value(pObject = new CTable ());
							break;
						}
					}
				}
				else if( !pParameter->is_Optional() )
				{
					return( false );
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CModule_Library::_Create_DataObject_List(CParameter *pParameter, wxString sList)
{
	CDataObject		*pObject;
	wxString		s;

	if( pParameter && pParameter->is_DataObject_List() )
	{
		do
		{
			s		= sList.BeforeFirst	(';');
			sList	= sList.AfterFirst	(';');

			switch( pParameter->Get_Type() )
			{
			default:							pObject	= NULL;						break;
			case PARAMETER_TYPE_Grid_List:		pObject	= new CGrid  (s.c_str());	break;
			case PARAMETER_TYPE_TIN_List:		pObject	= new CTIN   (s.c_str());	break;
			case PARAMETER_TYPE_Shapes_List:	pObject	= new CShapes(s.c_str());	break;
			case PARAMETER_TYPE_Table_List:		pObject	= new CTable (s.c_str());	break;
			}

			if( pObject && pObject->is_Valid() )
			{
				if( pParameter->Get_Type() == PARAMETER_TYPE_Grid_List && pParameter->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
				{
					pParameter->Get_Parent()->asGrid_System()->Assign(((CGrid *)pObject)->Get_System());
				}

				pParameter->asList()->Add_Item(pObject);
			}
			else if( pObject )
			{
				delete(pObject);

				Print_Error(LNG("input file"), s);
			}
		}
		while( sList.Length() > 0 );

		return( pParameter->asList()->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool CModule_Library::Add_DataObject(CDataObject *pObject)
{
	// leaves unsaved data and a memory leak, if <pObject> is not kept in parameters list either !!!
	// to be done...

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule_Library::_Destroy_DataObjects(bool bSave)
{
	if( m_pSelected && m_pCMD )
	{
		_Destroy_DataObjects(bSave, m_pCMD, m_pSelected->Get_Parameters());

		for(int i=0; i<m_pSelected->Get_Extra_Parameters_Count(); i++)
		{
			// to be done...
			_Destroy_DataObjects(bSave, m_pCMD, m_pSelected->Get_Extra_Parameters(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CModule_Library::_Destroy_DataObjects(bool bSave, wxCmdLineParser *pCMD, CParameters *pParameters)
{
	int			i, j;
	CParameter	*pParameter;
	wxString	s;

	if( pParameters && pCMD )
	{
		for(j=0; j<pParameters->Get_Count(); j++)
		{
			pParameter	= pParameters->Get_Parameter(j);

			if( bSave && pParameter->is_Output() && pCMD->Found(pParameter->Get_Identifier(), &s) )
			{
				if( pParameter->is_DataObject() )
				{
					if( pParameter->asDataObject() )
					{
						pParameter->asDataObject()->Save(s);
					}
				}
				else if( pParameter->is_DataObject_List() )
				{
					for(i=0; i<pParameter->asList()->Get_Count(); i++)
					{
						pParameter->asList()->asDataObject(i)->Save(wxString::Format("%02d_%s", i, s.c_str()));
					}
				}
			}

			if( pParameter->is_DataObject() )
			{
				if( pParameter->asDataObject() )
				{
					delete(pParameter->asDataObject());
					pParameter->Set_Value(DATAOBJECT_NOTSET);
				}
			}
			else if( pParameter->is_DataObject_List() )
			{
				for(i=pParameter->asList()->Get_Count()-1; i>=0; i--)
				{
					delete(pParameter->asList()->asDataObject(i));
				}

				pParameter->asList()->Del_Items();
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
