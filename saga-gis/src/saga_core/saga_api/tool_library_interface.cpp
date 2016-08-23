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
//              tool_library_interface.cpp               //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library_Interface::CSG_Tool_Library_Interface(void)
{
	m_nTools	= 0;
	m_Tools	= NULL;
}

//---------------------------------------------------------
CSG_Tool_Library_Interface::~CSG_Tool_Library_Interface(void)
{
	if( m_Tools && m_nTools > 0 )
	{
		for(int i=0; i<m_nTools; i++)
		{
			if( m_Tools[i] )
			{
				delete(m_Tools[i]);
			}
		}

		SG_Free(m_Tools);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool_Library_Interface::Set_Info(int ID, const CSG_String &Info)
{
	if( ID <= TLB_INFO_User )
	{
		m_Info[ID]	= SG_Translate(Info);
	}
}

//---------------------------------------------------------
const CSG_String & CSG_Tool_Library_Interface::Get_Info(int ID)
{
	return( m_Info[ID] );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Tool_Library_Interface::Get_Count(void)
{
	return( m_nTools );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Interface::Add_Tool(CSG_Tool *pTool, int ID)
{
	if( pTool == NULL )
	{
		return( false );
	}

	if( pTool != TLB_INTERFACE_SKIP_TOOL )
	{
		pTool->m_ID.Printf(SG_T("%d"), ID);
		pTool->m_Library		= Get_Info(TLB_INFO_Library);
		pTool->m_Library_Menu	= Get_Info(TLB_INFO_Menu_Path);
		pTool->m_File_Name	= Get_Info(TLB_INFO_File);
		m_Tools				= (CSG_Tool **)SG_Realloc(m_Tools, (m_nTools + 1) * sizeof(CSG_Tool *));
		m_Tools[m_nTools++]	= pTool;
	}

	return( true );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Interface::Get_Tool(int iTool)
{
	if( iTool >= 0 && iTool < m_nTools )
	{
		return( m_Tools[iTool] );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool_Library_Interface::Set_File_Name(const CSG_String &File_Name)
{
	m_Info[TLB_INFO_File]	= SG_File_Get_Path_Absolute(File_Name);

	CSG_String	Library		= SG_File_Get_Name(File_Name, false);

#if !defined(_SAGA_MSW)
	if( Library.Find("lib") == 0 )
	{
		Library	= Library.Right(Library.Length() - 3);
	}
#endif

	m_Info[TLB_INFO_Library]	= Library;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
