
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
//              module_interactive_base.cpp              //
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
CModule_Interactive_Base::CModule_Interactive_Base(void)
{
	m_pModule			= NULL;

	m_Keys				= 0;
	m_Drag_Mode			= MODULE_INTERACTIVE_DRAG_BOX;

	m_Point.m_point.x	= m_Point_Last.m_point.x	= 0.0;
	m_Point.m_point.y	= m_Point_Last.m_point.y	= 0.0;
}

//---------------------------------------------------------
CModule_Interactive_Base::~CModule_Interactive_Base(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CModule_Interactive_Base::Execute_Position(CSG_Point ptWorld, TModule_Interactive_Mode Mode, int Keys)
{
	bool	bResult	= false;

	if( m_pModule && !m_pModule->m_bExecutes )
	{
		SG_Callback_Process_Set_Okay();

		m_pModule->m_bExecutes		= true;
		m_pModule->m_bError_Ignore	= false;

		m_Point_Last				= m_Point;
		m_Point						= ptWorld;

		m_Keys						= Keys;

		bResult						= On_Execute_Position(m_Point, Mode);

		m_Keys						= 0;

		m_pModule->m_bExecutes		= false;
	}

	return( bResult );
}

bool CModule_Interactive_Base::On_Execute_Position(CSG_Point ptWorld, TModule_Interactive_Mode Mode)
{
	return( false );
}

//---------------------------------------------------------
bool CModule_Interactive_Base::Execute_Keyboard(int Character, int Keys)
{
	bool	bResult	= false;

	if( m_pModule && !m_pModule->m_bExecutes )
	{
		SG_Callback_Process_Set_Okay();

		m_pModule->m_bExecutes		= true;
		m_pModule->m_bError_Ignore	= false;

		m_Keys						= Keys;

		bResult						= On_Execute_Keyboard(Character);

		m_Keys						= 0;

		m_pModule->m_bExecutes		= false;
	}

	return( bResult );
}

bool CModule_Interactive_Base::On_Execute_Keyboard(int Character)
{
	return( false );
}

//---------------------------------------------------------
bool CModule_Interactive_Base::Execute_Finish(void)
{
	bool	bResult	= false;

	if( m_pModule && !m_pModule->m_bExecutes )
	{
		SG_Callback_Process_Set_Okay();

		m_pModule->m_bExecutes		= true;
		m_pModule->m_bError_Ignore	= false;

		bResult						= On_Execute_Finish();

		m_pModule->m_bExecutes		= false;
	}

	return( bResult );
}

bool CModule_Interactive_Base::On_Execute_Finish(void)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CModule_Interactive_Base::Set_Drag_Mode(int Drag_Mode)
{
	m_Drag_Mode	= Drag_Mode;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
