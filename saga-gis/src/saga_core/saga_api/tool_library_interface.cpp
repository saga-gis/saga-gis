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
#include "saga_api.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library_Interface::CSG_Tool_Library_Interface(void)
{
	m_Fnc_Create_Tool	= NULL;
}

//---------------------------------------------------------
CSG_Tool_Library_Interface::~CSG_Tool_Library_Interface(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Library_Interface::Create(const CSG_String &Version, const CSG_String &TLB_Path, TSG_PFNC_TLB_Get_Info Fnc_Info, TSG_PFNC_TLB_Create_Tool Fnc_Create_Tool)
{
	Destroy();

	//-----------------------------------------------------
	m_Info.Set_Count(TLB_INFO_Count);

	for(int i=0; i<TLB_INFO_User; i++)
	{
		m_Info[i]	= Fnc_Info(i);
	}

	CSG_String	Library		= SG_File_Get_Name(TLB_Path, false);

	#if !defined(_SAGA_MSW)
	if( Library.Find("lib") == 0 )
	{
		Library	= Library.Right(Library.Length() - 3);
	}
	#endif

	m_Info[TLB_INFO_Library     ]	= Library;
	m_Info[TLB_INFO_SAGA_Version]	= Version;
	m_Info[TLB_INFO_File        ]	= SG_File_Get_Path_Absolute(TLB_Path);

	if( Version.Cmp(SAGA_VERSION) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("SAGA API %s: %s=%s, %s=%s", _TL("version mismatch"), _TL("current version"), SAGA_VERSION,
			Library.c_str(), Version.c_str()
		));

		return( false );
	}

	//-----------------------------------------------------
	m_Fnc_Create_Tool	= Fnc_Create_Tool;

	CSG_Tool	*pTool;

	for(int ID=0; (pTool = m_Fnc_Create_Tool(ID)) != NULL; ID++)
	{
		if( pTool != TLB_INTERFACE_SKIP_TOOL )
		{
			pTool->m_ID.Printf("%d", ID);
			pTool->m_Library      = m_Info[TLB_INFO_Library  ];
			pTool->m_Library_Menu = m_Info[TLB_INFO_Menu_Path];
			pTool->m_File_Name    = m_Info[TLB_INFO_File     ];

			pTool->m_bHasGUI = SG_UI_Get_Window_Main() != NULL;

			m_Tools.Add(pTool);
		}
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Interface::Destroy(void)
{
	Delete_Tools();

	for(size_t i=0; i<m_Tools.Get_Size(); i++)
	{
		delete((CSG_Tool *)m_Tools[i]);
	}

	m_Tools.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_String & CSG_Tool_Library_Interface::Get_Info(int ID)
{
	return( m_Info[ID] );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Tool_Library_Interface::Get_Count(void)
{
	return( (int)m_Tools.Get_Size() );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Interface::Get_Tool(int i)
{
	if( i >= 0 && i < Get_Count() )
	{
		return( (CSG_Tool *)m_Tools[i] );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Interface::Create_Tool(int ID, bool bWithGUI)
{
	CSG_Tool	*pTool	= m_Fnc_Create_Tool ? m_Fnc_Create_Tool(ID) : NULL;

	if( pTool && pTool != TLB_INTERFACE_SKIP_TOOL )
	{
		pTool->m_ID.Printf("%d", ID);
		pTool->m_Library      = m_Info[TLB_INFO_Library  ];
		pTool->m_Library_Menu = m_Info[TLB_INFO_Menu_Path];
		pTool->m_File_Name    = m_Info[TLB_INFO_File     ];

		pTool->m_bHasGUI = bWithGUI && SG_UI_Get_Window_Main() != NULL;

		m_xTools.Add(pTool);

		return( pTool );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Interface::Delete_Tool(CSG_Tool *pTool)
{
	for(size_t i=0; i<m_xTools.Get_Size(); i++)
	{
		if( pTool == m_xTools.Get(i) && m_xTools.Del(i) )
		{
			delete(pTool);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Interface::Delete_Tools(void)
{
	for(size_t i=0; i<m_xTools.Get_Size(); i++)
	{
		delete((CSG_Tool *)m_xTools[i]);
	}

	m_xTools.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
