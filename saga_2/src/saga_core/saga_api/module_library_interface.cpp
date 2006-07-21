
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
//             module_library_interface.cpp              //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CModule_Library_Interface::CModule_Library_Interface(void)
{
	m_nModules	= 0;
	m_Modules	= NULL;
}

//---------------------------------------------------------
CModule_Library_Interface::~CModule_Library_Interface(void)
{
	if( m_Modules && m_nModules > 0 )
	{
		for(int i=0; i<m_nModules; i++)
		{
			delete(m_Modules[i]);
		}

		SG_Free( m_Modules );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule_Library_Interface::Set_Info(int ID, const char *Info)
{
	m_Info[ID]	= Get_Translation(Info);
}

//---------------------------------------------------------
const char * CModule_Library_Interface::Get_Info(int ID)
{
	return( m_Info[ID] );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CModule_Library_Interface::Get_Count(void)
{
	return( m_nModules );
}

//---------------------------------------------------------
bool CModule_Library_Interface::Add_Module(CModule *pModule)
{
	if( pModule )
	{
		pModule->Set_Managed(true);
		pModule->Set_Translation(m_Translator);

		m_Modules				= (CModule **)SG_Realloc(m_Modules, (m_nModules + 1) * sizeof(CModule *));
		m_Modules[m_nModules++]	= pModule;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CModule * CModule_Library_Interface::Get_Module(int iModule)
{
	if( iModule >= 0 && iModule < m_nModules )
	{
		return( m_Modules[iModule] );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule_Library_Interface::Set_File_Name(const char *File_Name)
{
	m_File_Name	= File_Name;

	m_Translator.Create(m_File_Name);
}

//---------------------------------------------------------
const char * CModule_Library_Interface::Get_File_Name(void)
{
	return( m_File_Name );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CModule_Library_Interface::Get_Translation(const char *Text)
{
	return( m_Translator.Get_Translation(Text) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
