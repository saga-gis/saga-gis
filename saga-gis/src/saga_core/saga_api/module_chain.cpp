/**********************************************************
 * Version $Id: module_chain.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   module_chain.cpp                    //
//                                                       //
//          Copyright (C) 2014 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#include "module_chain.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(void)
{
	Create();
}

//---------------------------------------------------------
CSG_Module_Chain::CSG_Module_Chain(const CSG_String &File)
{
	Create(File);
}

//---------------------------------------------------------
CSG_Module_Chain::~CSG_Module_Chain(void)
{
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Create(void)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Module_Chain::Create(const CSG_String &File)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::On_Execute(void)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Chain::Save_History_to_Model(const CSG_MetaData &History, const CSG_String &File)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module_Chains::CSG_Module_Chains(void)
{
}

//---------------------------------------------------------
CSG_Module_Chains::~CSG_Module_Chains(void)
{
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Module_Chains::Get_Info(int Type) const
{
	switch( Type )
	{
	case MLB_INFO_Name       :	return( _TL("Models") );
	case MLB_INFO_Author     :	return( _TL("") );
	case MLB_INFO_Description:	return( _TL("Models") );
	case MLB_INFO_Version    :	return( _TL("") );
	case MLB_INFO_Menu_Path  :	return( _TL("Models") );
	}

	return( "" );
}

//---------------------------------------------------------
bool CSG_Module_Chains::Add_Module(CSG_Module_Chain *pModule)
{
	return( false );
}

//---------------------------------------------------------
CSG_String CSG_Module_Chains::Get_Menu(int i)	const
{
	return( "" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
