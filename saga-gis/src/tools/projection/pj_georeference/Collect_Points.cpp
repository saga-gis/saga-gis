
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "Collect_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCollect_Points::CCollect_Points(void)
{
	Set_Name		(_TL("Create Reference Points"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Digitize reference points for georeferencing grids, images and shapes. "
		"Click with the mouse on known locations in the map window "
		"and add the reference coordinates. "
		"After choosing 4 or more points, stop the interactive module execution "
		"by unchecking it in the in the tools menu."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "REF_SOURCE", _TL("Reference Points (Origin)"    ), _TL(""), PARAMETER_OUTPUT         , SHAPE_TYPE_Point);
	Parameters.Add_Shapes("", "REF_TARGET", _TL("Reference Points (Projection)"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point);

	Parameters.Add_Choice("", "METHOD"    , _TL("Method"                       ), _TL(""), GEOREF_METHODS_CHOICE, 0);
	Parameters.Add_Int   ("", "ORDER"     , _TL("Polynomial Order"             ), _TL(""), 3, 1, true);
	Parameters.Add_Bool  ("", "REFRESH"   , _TL("Clear Reference Points"       ), _TL(""), false);

	//-----------------------------------------------------
	m_Reference.Create(_TL("Point Position"), _TL(""));

	m_Reference.Add_Double("", "X", _TL("x Position"), _TL(""));
	m_Reference.Add_Double("", "Y", _TL("y Position"), _TL(""));
}


///////////////////////////////////////////////////////////
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
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("ORDER", pParameter->asInt() == GEOREF_Polynomial);	// only show for polynomial, user defined order
	}

	if( pParameter->Cmp_Identifier("REF_SOURCE") )
	{
		pParameters->Get_Parameter("REFRESH")->Set_Enabled(is_Compatible(pParameter->asShapes()));
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCollect_Points::On_Execute(void)
{
	m_Engine.Destroy();

	m_pPoints = Parameters("REF_SOURCE")->asShapes();

	m_Reference.Restore_Defaults();

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
		for(sLong i=0; i<m_pPoints->Get_Count(); i++)
		{
			CSG_Shape *pPoint = m_pPoints->Get_Shape(i);

			m_Engine.Add_Reference(pPoint->Get_Point(), CSG_Point(
				pPoint->asDouble(2),
				pPoint->asDouble(3)
			));
		}

		int Method = Parameters("METHOD")->asInt();
		int Order  = Parameters("ORDER" )->asInt();

		m_Engine.Evaluate(Method, Order);
	}

	return( true );
}

//---------------------------------------------------------
bool CCollect_Points::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode == TOOL_INTERACTIVE_LUP )
	{
		TSG_Point ptTarget;

		if( m_Engine.Get_Converted(ptTarget = ptWorld) )
		{
			m_Reference("X")->Set_Value(ptTarget.x);
			m_Reference("Y")->Set_Value(ptTarget.y);
		}

		if( Dlg_Parameters(m_Reference) )
		{
			int	Method = Parameters("METHOD")->asInt();
			int	Order  = Parameters("ORDER" )->asInt();

			CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

			pPoint->Add_Point(ptWorld);
			pPoint->Set_Value(0, ptWorld.x);
			pPoint->Set_Value(1, ptWorld.y);
			pPoint->Set_Value(2, ptTarget.x = m_Reference("X")->asDouble());
			pPoint->Set_Value(3, ptTarget.y = m_Reference("Y")->asDouble());

			if( m_Engine.Add_Reference(ptWorld, ptTarget) && m_Engine.Evaluate(Method, Order) && m_Engine.Get_Reference_Count() == m_pPoints->Get_Count() )
			{
				for(sLong i=0; i<m_pPoints->Get_Count(); i++)
				{
					m_pPoints->Get_Shape(i)->Set_Value(4, m_Engine.Get_Reference_Residual(i));
				}
			}

			DataObject_Update(m_pPoints);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CCollect_Points::On_Execute_Finish(void)
{
	CSG_Shapes *pTarget = Parameters("REF_TARGET")->asShapes();

	if( pTarget != NULL )
	{
		pTarget->Create(SHAPE_TYPE_Point, _TL("Reference Points (Projection)"));

		pTarget->Add_Field("X_SRC", SG_DATATYPE_Double);
		pTarget->Add_Field("Y_SRC", SG_DATATYPE_Double);
		pTarget->Add_Field("X_MAP", SG_DATATYPE_Double);
		pTarget->Add_Field("Y_MAP", SG_DATATYPE_Double);
		pTarget->Add_Field("RESID", SG_DATATYPE_Double);

		for(sLong iPoint=0; iPoint<m_pPoints->Get_Count(); iPoint++)
		{
			CSG_Shape *pPoint = pTarget->Add_Shape(m_pPoints->Get_Shape(iPoint), SHAPE_COPY_ATTR);

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
