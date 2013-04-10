/**********************************************************
 * Version $Id$
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
CSG_Module_Library_Interface::CSG_Module_Library_Interface(void)
{
	m_nModules	= 0;
	m_Modules	= NULL;
}

//---------------------------------------------------------
CSG_Module_Library_Interface::~CSG_Module_Library_Interface(void)
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
void CSG_Module_Library_Interface::Set_Info(int ID, const CSG_String &Info)
{
	m_Info[ID]	= SG_Translate(Info);
}

//---------------------------------------------------------
const CSG_String & CSG_Module_Library_Interface::Get_Info(int ID)
{
	return( m_Info[ID] );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Module_Library_Interface::Get_Count(void)
{
	return( m_nModules );
}

//---------------------------------------------------------
bool CSG_Module_Library_Interface::Add_Module(CSG_Module *pModule, int ID)
{
	if( pModule )
	{
		if( pModule == MLB_INTERFACE_SKIP_MODULE )
		{
			pModule	= NULL;
		}
		else
		{
			pModule->m_ID	= ID;
		}

		m_Modules				= (CSG_Module **)SG_Realloc(m_Modules, (m_nModules + 1) * sizeof(CSG_Module *));
		m_Modules[m_nModules++]	= pModule;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Module * CSG_Module_Library_Interface::Get_Module(int iModule)
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
void CSG_Module_Library_Interface::Set_File_Name(const CSG_String &File_Name)
{
	m_File_Name	= File_Name;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
