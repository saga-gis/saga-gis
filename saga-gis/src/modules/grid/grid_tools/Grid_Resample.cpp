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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Resample.cpp                   //
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
#include "Grid_Resample.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Resample::CGrid_Resample(void)
{
	CSG_Parameter	*pNode;
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Set_Name		(_TL("Resampling"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Resampling of grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "INPUT_ADD"	, _TL("Additional Grids"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_List(
		NULL	, "OUTPUT_ADD"	, _TL("Additional Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("SCALE_UP"	, _TL("Up-Scaling")				, _TL(""));

	pNode	= pParameters->Add_Choice(
		NULL	, "METHOD"		, _TL("Interpolation Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"),
			_TL("Mean Value"),
			_TL("Mean Value (cell area weighted)"),
			_TL("Minimum Value"),
			_TL("Maximum Value"),
			_TL("Majority")
		), 6
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("SCALE_DOWN"	, _TL("Down-Scaling")			, _TL(""));

	pNode	= pParameters->Add_Choice(
		NULL	, "METHOD"		, _TL("Interpolation Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(SG_UI_Get_Window_Main() ? &Parameters : Add_Parameters("TARGET", _TL("Target System"), _TL("")));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Resample::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "INPUT") && pParameter->asGrid() )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asGrid()->Get_Extent());
	}

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGrid_Resample::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Resample::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pInput	= Parameters("INPUT")->asGrid();

	//-----------------------------------------------------
	m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), pInput->Get_Extent());	Dlg_Parameters("TARGET");	// if called from saga_cmd

	CSG_Grid	*pOutput	= m_Grid_Target.Get_Grid(Parameters("KEEP_TYPE")->asBool() ? pInput->Get_Type() : SG_DATATYPE_Undefined);

	if( !pOutput || !pInput->is_Intersecting(pOutput->Get_Extent()) )
	{
		return( false );
	}

	//-------------------------------------------------
	TSG_Grid_Interpolation	Interpolation;

	if( pInput->Get_Cellsize() < pOutput->Get_Cellsize() )	// Up-Scaling...
	{
		if( !Dlg_Parameters("SCALE_UP") )
		{
			return( false );
		}

		switch( Get_Parameters("SCALE_UP")->Get_Parameter("METHOD")->asInt() )
		{
		case  0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
		case  1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
		case  2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
		case  3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
		case  4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
		case  5:	Interpolation	= GRID_INTERPOLATION_Mean_Nodes;		break;
		case  6:	Interpolation	= GRID_INTERPOLATION_Mean_Cells;		break;
		case  7:	Interpolation	= GRID_INTERPOLATION_Minimum;			break;
		case  8:	Interpolation	= GRID_INTERPOLATION_Maximum;			break;
		case  9:	Interpolation	= GRID_INTERPOLATION_Majority;			break;
		}
	}
	else	// Down-Scaling...
	{
		if( !Dlg_Parameters("SCALE_DOWN") )
		{
			return( false );
		}

		switch( Get_Parameters("SCALE_DOWN")->Get_Parameter("METHOD")->asInt() )
		{
		case  0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
		case  1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
		case  2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
		case  3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
		case  4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
		}
	}

	//-------------------------------------------------
	pOutput->Assign(pInput, Interpolation);
	pOutput->Set_Name(pInput->Get_Name());

	//-------------------------------------------------
	CSG_Grid_System	System(pOutput->Get_System());

	CSG_Parameter_Grid_List	*pInputs	= Parameters("INPUT_ADD" )->asGridList();
	CSG_Parameter_Grid_List	*pOutputs	= Parameters("OUTPUT_ADD")->asGridList();

	pOutputs->Del_Items();

	for(int i=0; i<pInputs->Get_Count() && Process_Get_Okay(); i++)
	{
		pInput	= pInputs->asGrid(i);

		pOutput	= SG_Create_Grid(pOutput->Get_System(),
			Parameters("KEEP_TYPE")->asBool() ? pInput->Get_Type() : SG_DATATYPE_Undefined
		);

		pOutput->Assign(pInput, Interpolation);
		pOutput->Set_Name(pInput->Get_Name());

		pOutputs->Add_Item(pOutput);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
