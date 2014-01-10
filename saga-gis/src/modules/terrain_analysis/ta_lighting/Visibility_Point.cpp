/**********************************************************
 * Version $Id: Visibility_Point.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Visibility_Point.cpp                 //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Visibility_Point.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVisibility_Point::CVisibility_Point(void)
{
	Set_Name(_TL("Visibility (single point)"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description(
		_TL("")
	);

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "VISIBILITY"	, _TL("Visibility"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEIGHT"		, _TL("Height"),
		_TL("Height of the light source above ground."),
		PARAMETER_TYPE_Double, 100.0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Unit"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Visibility"),
			_TL("Shade"),
			_TL("Distance"),
			_TL("Size")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "MULTIPLE_OBS"	, _TL("Multiple Observer"),
		_TL("Allow multiple observer positions."),
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
CVisibility_Point::~CVisibility_Point(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVisibility_Point::On_Execute(void)
{
	m_pDTM			= Parameters("ELEVATION")	->asGrid();
	m_pVisibility	= Parameters("VISIBILITY")	->asGrid();
	m_Height		= Parameters("HEIGHT")		->asDouble();
	m_Method		= Parameters("METHOD")		->asInt();
	m_bMultiple		= Parameters("MULTIPLE_OBS")->asBool();
	
	if( m_bMultiple )
		Initialize(m_pVisibility, m_Method);

	return( true );
}


//---------------------------------------------------------
bool CVisibility_Point::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	int		x_Pos, y_Pos;

	double	z_Pos;


	//-----------------------------------------------------
	if(	Mode != MODULE_INTERACTIVE_LDOWN
	||	!m_pDTM->is_InGrid_byPos(Get_xPosition(), Get_yPosition()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	x_Pos	= Get_xGrid();
	y_Pos	= Get_yGrid();;
	z_Pos	= m_pDTM->asDouble(x_Pos, y_Pos) + m_Height;

	if( !m_bMultiple )
		Initialize(m_pVisibility, m_Method);


	Set_Visibility(m_pDTM, m_pVisibility, x_Pos, y_Pos, z_Pos, m_Height, m_Method);


	//-----------------------------------------------------
	Finalize(m_pVisibility, m_Method);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
