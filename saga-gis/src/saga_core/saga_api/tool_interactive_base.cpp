
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
//              tool_interactive_base.cpp                //
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
#include "tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_MESSAGE_EXECUTION(Text, Style)	{ SG_UI_Msg_Add(Text, true, Style); if( m_pTool->has_GUI() ) { SG_UI_Msg_Add_Execution(Text, true, Style); } }

#define ADD_MESSAGE_TIME(Start)	{ CSG_TimeSpan Time = CSG_DateTime::Now() - Start; CSG_String s;\
	if( Time.Get_Hours       () >= 1 ) { s = Time.Format("%Hh %Mm %Ss"); } else\
	if( Time.Get_Minutes     () >= 1 ) { s = Time.Format(    "%Mm %Ss"); } else\
	if( Time.Get_Seconds     () >= 1 ) { s = Time.Format(        "%Ss"); } else\
	if( Time.Get_Milliseconds() >= 1 ) { s = Time.Format("%l ") + _TL("milliseconds"); } else { s = _TL("less than 1 millisecond"); }\
	SG_UI_Msg_Add_Execution(CSG_String::Format("\n[%s] %s %s", m_pTool->Get_Name().c_str(), _TL("finished in"), s.c_str()), false);\
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Interactive_Base::CSG_Tool_Interactive_Base(void)
{
	m_pTool		= NULL;

	m_Keys		= 0;
	m_Drag_Mode	= TOOL_INTERACTIVE_DRAG_BOX;

	m_Point     .Assign(0., 0.);
	m_Point_Last.Assign(0., 0.);
}

//---------------------------------------------------------
CSG_Tool_Interactive_Base::~CSG_Tool_Interactive_Base(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Interactive_Base::Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode, int Keys)
{
	if( !m_pTool || m_pTool->m_bExecutes )
	{
		return( false );
	}

	m_pTool->m_bError_Ignore = false;
	m_pTool->m_bExecutes     = true;
	m_Point_Last             = m_Point;
	m_Point                  = ptWorld;
	m_Keys                   = Keys;

	CSG_DateTime Started(CSG_DateTime::Now());
	bool bResult = On_Execute_Position(m_Point, Mode);
	if( bResult ) ADD_MESSAGE_TIME(Started);

	m_Keys                   = 0;
	m_pTool->m_bExecutes     = false;

	m_pTool->_Synchronize_DataObjects();

	SG_UI_Process_Set_Okay(); SG_UI_Process_Set_Ready();

	return( bResult );
}

bool CSG_Tool_Interactive_Base::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Interactive_Base::Execute_Keyboard(int Character, int Keys)
{
	if( !m_pTool || m_pTool->m_bExecutes )
	{
		return( false );
	}

	m_pTool->m_bError_Ignore = false;
	m_pTool->m_bExecutes     = true;
	m_Keys                   = Keys;

	CSG_DateTime Started(CSG_DateTime::Now());
	bool bResult = On_Execute_Keyboard(Character);
	if( bResult ) ADD_MESSAGE_TIME(Started);

	m_Keys                   = 0;
	m_pTool->m_bExecutes     = false;

	m_pTool->_Synchronize_DataObjects();

	SG_UI_Process_Set_Okay(); SG_UI_Process_Set_Ready();

	return( bResult );
}

bool CSG_Tool_Interactive_Base::On_Execute_Keyboard(int Character)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Interactive_Base::Execute_Finish(void)
{
	if( !m_pTool || m_pTool->m_bExecutes )
	{
		return( false );
	}

	m_pTool->m_bError_Ignore = false;
	m_pTool->m_bExecutes     = true;

	CSG_DateTime Started(CSG_DateTime::Now());
	bool bResult = On_Execute_Finish();
	if( bResult ) ADD_MESSAGE_TIME(Started);

	m_pTool->m_bExecutes     = false;

	m_pTool->_Synchronize_DataObjects();

	SG_UI_Process_Set_Okay(); SG_UI_Process_Set_Ready();

	ADD_MESSAGE_EXECUTION(CSG_String::Format("[%s] %s", m_pTool->Get_Name().c_str(), bResult
		? _TL("Interactive tool execution has been stopped")
		: _TL("Interactive tool execution failed")),
		bResult ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE
	);

	return( bResult );
}

bool CSG_Tool_Interactive_Base::On_Execute_Finish(void)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool_Interactive_Base::Set_Drag_Mode(int Drag_Mode)
{
	m_Drag_Mode	= Drag_Mode;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
