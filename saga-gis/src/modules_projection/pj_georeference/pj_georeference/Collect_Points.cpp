/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    pj_georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Collect_Points.cpp                  //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//    e-mail:     oconrad@gwdg.de                        //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Digitize reference points for georeferencing grids, images and shapes. "
		"Click with the mouse on known locations in the map window "
		"and add the reference coordinates. "
		"After choosing 4 or more points, stop the interactive module execution "
		"by unchecking it in the in the modules menu."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "REF_SOURCE"	, _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "REF_TARGET"	, _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "REFRESH"		, _TL("Clear Reference Points"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("REFERENCE", _TL("Point Position"), _TL(""));

	pParameters->Add_Value(
		NULL	, "X"			, _TL("x Position"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL	, "Y"			, _TL("y Position"),
		_TL(""),
		PARAMETER_TYPE_Double
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCollect_Points::is_Compatible(CSG_Shapes *pPoints)
{
	return( pPoints != DATAOBJECT_NOTSET && pPoints != DATAOBJECT_CREATE
		&&  pPoints->Get_Count() > 0 &&  pPoints->Get_Field_Count() >= 5 );
}

//---------------------------------------------------------
int CCollect_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "REF_SOURCE") )
	{
		pParameters->Get_Parameter("REFRESH")->Set_Enabled(is_Compatible(pParameter->asShapes()));
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCollect_Points::On_Execute(void)
{
	m_Engine.Destroy();

	m_pPoints	= Parameters("REF_SOURCE")->asShapes();

	if( !is_Compatible(m_pPoints) || Parameters("REFRESH")->asBool() )
	{
		m_pPoints->Create(SHAPE_TYPE_Point, _TL("Reference Points (Origin)"));

		m_pPoints->Add_Field("X_SRC", SG_DATATYPE_Double);
		m_pPoints->Add_Field("Y_SRC", SG_DATATYPE_Double);
		m_pPoints->Add_Field("X_MAP", SG_DATATYPE_Double);
		m_pPoints->Add_Field("Y_MAP", SG_DATATYPE_Double);
		m_pPoints->Add_Field("RESID", SG_DATATYPE_Double);
	}
	else
	{
		for(int i=0; i<m_pPoints->Get_Count(); i++)
		{
			CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

			m_Engine.Add_Reference(pPoint->Get_Point(0), CSG_Point(
				pPoint->asDouble(2),
				pPoint->asDouble(3)
			));
		}

		m_Engine.Evaluate();
	}

	return( true );
}

//---------------------------------------------------------
bool CCollect_Points::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	if( Mode == MODULE_INTERACTIVE_LUP )
	{
		TSG_Point	ptTarget;

		if( m_Engine.Get_Converted(ptTarget = ptWorld) )
		{
			Get_Parameters("REFERENCE")->Get_Parameter("X")->Set_Value(ptTarget.x);
			Get_Parameters("REFERENCE")->Get_Parameter("Y")->Set_Value(ptTarget.y);
		}

		if( Dlg_Parameters("REFERENCE") )
		{
			CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

			pPoint->Add_Point(ptWorld);
			pPoint->Set_Value(0, ptWorld.Get_X());
			pPoint->Set_Value(1, ptWorld.Get_Y());
			pPoint->Set_Value(2, ptTarget.x = Get_Parameters("REFERENCE")->Get_Parameter("X")->asDouble());
			pPoint->Set_Value(3, ptTarget.y = Get_Parameters("REFERENCE")->Get_Parameter("Y")->asDouble());

			if( m_Engine.Add_Reference(ptWorld, ptTarget) && m_Engine.Evaluate() && m_pPoints->Get_Count() == m_Engine.Get_Reference_Count() )
			{
				for(int i=0; i<m_pPoints->Get_Count(); i++)
				{
					m_pPoints->Get_Shape(i)->Set_Value(4, m_Engine.Get_Reference_Residual(i));
				}
			}

			DataObject_Update(m_pPoints);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CCollect_Points::On_Execute_Finish(void)
{
	CSG_Shapes	*pTarget	= Parameters("REF_TARGET")->asShapes();

	if( pTarget != NULL )
	{
		pTarget->Create(SHAPE_TYPE_Point, _TL("Reference Points (Projection)"));

		pTarget->Add_Field("X_SRC", SG_DATATYPE_Double);
		pTarget->Add_Field("Y_SRC", SG_DATATYPE_Double);
		pTarget->Add_Field("X_MAP", SG_DATATYPE_Double);
		pTarget->Add_Field("Y_MAP", SG_DATATYPE_Double);
		pTarget->Add_Field("RESID", SG_DATATYPE_Double);

		for(int iPoint=0; iPoint<m_pPoints->Get_Count(); iPoint++)
		{
			CSG_Shape	*pPoint	= pTarget->Add_Shape(m_pPoints->Get_Shape(iPoint), SHAPE_COPY_ATTR);

			pPoint->Add_Point(
				pPoint->asDouble(2),
				pPoint->asDouble(3)
			);
		}
	}

	m_Engine.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
