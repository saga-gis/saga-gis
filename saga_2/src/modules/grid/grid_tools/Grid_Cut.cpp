
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Grid_Cut.cpp                      //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include <string.h>

#include "Grid_Cut.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cut::CGrid_Cut(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Cutting"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Create a new grid from interactively selected cut of an input grid.\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "CUT"		, _TL("Cut"),
		_TL("")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("CUT", _TL("Cut"), _TL(""));

	pParameters->Add_Value(
		NULL, "XMIN"		, _TL("Left")		, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "XMAX"		, _TL("Right")		, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "YMIN"		, _TL("Bottom")		, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "YMAX"		, _TL("Top")		, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "NX"			, _TL("Columns")	, _TL(""), PARAMETER_TYPE_Int, 1, 1, true
	);

	pParameters->Add_Value(
		NULL, "NY"			, _TL("Rows")		, _TL(""), PARAMETER_TYPE_Int, 1, 1, true
	);
}

//---------------------------------------------------------
CGrid_Cut::~CGrid_Cut(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Cut::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	int		nx, ny;
	double	xMin, xMax, yMin, yMax, d;

	if( m_pInput && !SG_STR_CMP(pParameters->Get_Identifier(), SG_T("CUT")) )
	{
		xMin	= pParameters->Get_Parameter("XMIN")->asDouble();
		xMax	= pParameters->Get_Parameter("XMAX")->asDouble();
		yMin	= pParameters->Get_Parameter("YMIN")->asDouble();
		yMax	= pParameters->Get_Parameter("YMAX")->asDouble();
		nx		= pParameters->Get_Parameter("NX")	->asInt();
		ny		= pParameters->Get_Parameter("NY")	->asInt();

		if( xMin > xMax )	{	d	= xMin;	xMin	= xMax;	xMax	= d;	}
		if( yMin > yMax )	{	d	= yMin;	yMin	= yMax;	yMax	= d;	}

		d		= m_pInput->Get_Cellsize();

		if     ( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NX")) )
		{
			xMax	= xMin + nx * d;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("XMIN")) )
		{
			nx		= 1    + (int)((xMax - xMin) / d);
			xMin	= xMax - nx * d;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("XMAX")) )
		{
			nx		= 1    + (int)((xMax - xMin) / d);
			xMax	= xMin + nx * d;
		}

		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NY")) )
		{
			yMax	= yMin + ny * d;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("YMIN")) )
		{
			ny		= 1    + (int)((yMax - yMin) / d);
			yMin	= yMax - ny * d;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("YMAX")) )
		{
			ny		= 1    + (int)((yMax - yMin) / d);
			yMax	= yMin + ny * d;
		}

		pParameters->Get_Parameter("XMIN")	->Set_Value(xMin);
		pParameters->Get_Parameter("XMAX")	->Set_Value(xMax);
		pParameters->Get_Parameter("YMIN")	->Set_Value(yMin);
		pParameters->Get_Parameter("YMAX")	->Set_Value(yMax);
		pParameters->Get_Parameter("NX")	->Set_Value(nx);
		pParameters->Get_Parameter("NY")	->Set_Value(ny);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cut::On_Execute(void)
{
	m_bDown		= false;
	m_pInput	= Parameters("GRID")->asGrid();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cut::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	CSG_Rect		r;
	CSG_Grid_System	System;
	CSG_Grid			*pCut;
	CSG_Parameters		*pParameters;

	switch( Mode )
	{
	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LDOWN:
		if( m_bDown == false )
		{
			m_bDown		= true;
			m_ptDown	= Fit_to_Grid(ptWorld);
		}

		return( true );

	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LUP:
		if( m_bDown == true )
		{
			m_bDown		= false;
			ptWorld		= Fit_to_Grid(ptWorld);

			r.Assign(m_ptDown.Get_X(), m_ptDown.Get_Y(), ptWorld.Get_X(), ptWorld.Get_Y());

			pParameters	= Get_Parameters("CUT");

			pParameters->Get_Parameter("XMIN")	->Set_Value(r.Get_XMin());
			pParameters->Get_Parameter("XMAX")	->Set_Value(r.Get_XMax());
			pParameters->Get_Parameter("YMIN")	->Set_Value(r.Get_YMin());
			pParameters->Get_Parameter("YMAX")	->Set_Value(r.Get_YMax());
			pParameters->Get_Parameter("NX")	->Set_Value(1 + (int)(r.Get_XRange() / m_pInput->Get_Cellsize()));
			pParameters->Get_Parameter("NY")	->Set_Value(1 + (int)(r.Get_YRange() / m_pInput->Get_Cellsize()));

			if( Dlg_Parameters("CUT") )
			{
				r.Assign(
					pParameters->Get_Parameter("XMIN")->asDouble(),
					pParameters->Get_Parameter("YMIN")->asDouble(),
					pParameters->Get_Parameter("XMAX")->asDouble(),
					pParameters->Get_Parameter("YMAX")->asDouble()
				);

				if( r.Intersect(m_pInput->Get_Extent()) && System.Assign(m_pInput->Get_Cellsize(), r) )
				{
					pCut	= SG_Create_Grid(System, m_pInput->Get_Type());
					pCut->Assign(m_pInput, GRID_INTERPOLATION_NearestNeighbour);
					pCut->Set_Name(m_pInput->Get_Name());
					Parameters("CUT")->Set_Value(pCut);
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CGrid_Cut::Fit_to_Grid(TSG_Point pt)
{
	pt.x	= m_pInput->Get_XMin() + m_pInput->Get_Cellsize() * (int)(0.5 + (pt.x - m_pInput->Get_XMin()) / m_pInput->Get_Cellsize());
	pt.y	= m_pInput->Get_YMin() + m_pInput->Get_Cellsize() * (int)(0.5 + (pt.y - m_pInput->Get_YMin()) / m_pInput->Get_Cellsize());

	return( pt );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
