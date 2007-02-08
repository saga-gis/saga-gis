
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                   Grid_Georeference                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Collect_Points.cpp                  //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

// Completely rearranged by O.Conrad April 2006 !!!

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Collect_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCollect_Points::CCollect_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create Reference Points"));

	Set_Author		(_TL("(c) 2004 Ringeler, (c) 2006 O.Conrad"));

	Set_Description	(_TW(
		"Digitize reference points for georeferencing grids, images and shapes. "
		"Click with the mouse on known locations in the map window "
		"and add the reference coordinates. "
		"After choosing 4 or more points, stop the interactive module execution "
		"by unchecking it in the in the modules menu."
	));


	Parameters.Add_Shapes(
		NULL, "REF_SOURCE"	, _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL, "REF_TARGET"	, _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("REFERENCE", _TL("Point Position"), _TL(""));

	pParameters->Add_Value(
		NULL, "X"			, _TL("x Position"),
		_TL(""),
		PARAMETER_TYPE_Int
	);

	pParameters->Add_Value(
		NULL, "Y"			, _TL("y Position"),
		_TL(""),
		PARAMETER_TYPE_Int
	);
}


//---------------------------------------------------------
CCollect_Points::~CCollect_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCollect_Points::On_Execute(void)
{
	m_pSource	= Parameters("REF_SOURCE")	->asShapes();
	m_pSource	->Create(SHAPE_TYPE_Point, _TL("Reference Points (Origin)"));
	m_pSource	->Get_Table().Add_Field("X", TABLE_FIELDTYPE_Double);
	m_pSource	->Get_Table().Add_Field("Y", TABLE_FIELDTYPE_Double);

	if( (m_pTarget = Parameters("REF_TARGET")->asShapes()) != NULL )
	{
		m_pTarget	->Create(SHAPE_TYPE_Point, _TL("Reference Points (Projection)"));
		m_pTarget	->Get_Table().Add_Field("X", TABLE_FIELDTYPE_Double);
		m_pTarget	->Get_Table().Add_Field("Y", TABLE_FIELDTYPE_Double);
	}

	return( true );
}

//---------------------------------------------------------
bool CCollect_Points::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	if( Mode == MODULE_INTERACTIVE_LUP && Dlg_Parameters("REFERENCE") )
	{
		double	xSource, ySource, xTarget, yTarget;
		CSG_Shape	*pShape;

		xSource	= ptWorld.Get_X();
		ySource	= ptWorld.Get_Y();

		xTarget	= Get_Parameters("REFERENCE")->Get_Parameter("X")->asDouble();
		yTarget	= Get_Parameters("REFERENCE")->Get_Parameter("Y")->asDouble();

		pShape	= m_pSource->Add_Shape();
		pShape	->Add_Point(xSource, ySource);
		pShape	->Get_Record()->Set_Value(0, xTarget);
		pShape	->Get_Record()->Set_Value(1, yTarget);

		DataObject_Update(m_pSource);

		if( m_pTarget )
		{
			pShape	= m_pTarget->Add_Shape();
			pShape	->Add_Point(xTarget, yTarget);
			pShape	->Get_Record()->Set_Value(0, xSource);
			pShape	->Get_Record()->Set_Value(1, ySource);

			DataObject_Update(m_pTarget);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
