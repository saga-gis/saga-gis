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
	//-----------------------------------------------------
	Set_Name		(_TL("Resampling"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Resampling of grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "INPUT"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "SCALE_UP"	, _TL("Upscaling Method"),
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
	Parameters.Add_Choice(
		NULL	, "SCALE_DOWN"	, _TL("Downscaling Method"),
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
	m_Grid_Target.Create(SG_UI_Get_Window_Main() ? &Parameters : Add_Parameters("TARGET", _TL("Target System"), _TL("")), false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Resample::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "PARAMETERS_GRID_SYSTEM") )
	{
		if( pParameter->asGrid_System() )
		{
			CSG_Grid_System	Input	= *pParameter->asGrid_System();

			m_Grid_Target.Set_User_Defined(pParameters, Input.Get_Extent(), Input.Get_NY(), false, 0);
		}
	}

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGrid_Resample::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( SG_UI_Get_Window_Main() )
	{
		double	Scaling	= 0.0;

		if( pParameters->Get_Parameter("INPUT")->asGridList()->Get_Count() > 0 )
		{
			double	Input	= pParameters->Get_Parameter("INPUT")->asGridList()->asGrid(0)->Get_System().Get_Cellsize();

			if( pParameters->Get_Parameter("DEFINITION")->asInt() == 0 )	// user defined
			{
				Scaling	= Input - pParameters->Get_Parameter("USER_SIZE")->asDouble();
			}
			else if( pParameters->Get_Parameter("SYSTEM")->asGrid_System() && pParameters->Get_Parameter("SYSTEM")->asGrid_System()->Get_Cellsize() > 0.0 )
			{
				Scaling	= Input - pParameters->Get_Parameter("SYSTEM")->asGrid_System()->Get_Cellsize();
			}
		}

		pParameters->Set_Enabled("SCALE_UP"  , Scaling < 0.0);
		pParameters->Set_Enabled("SCALE_DOWN", Scaling > 0.0);
	}

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
	CSG_Parameter_Grid_List	*pInputs	= Parameters("INPUT" )->asGridList();
	CSG_Parameter_Grid_List	*pOutputs	= Parameters("OUTPUT")->asGridList();

	if( pInputs->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System	Input	= pInputs->asGrid(0)->Get_System();

	m_Grid_Target.Cmd_Update(Input.Get_Extent());	// if called from saga_cmd

	CSG_Grid_System	Output	= m_Grid_Target.Get_System();

	if( Input.Get_Extent().Intersects(Output.Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("clip extent does not match extent of input grids"));

		return( false );
	}

	//-------------------------------------------------
	TSG_Grid_Interpolation	Interpolation;

	if( Input.Get_Cellsize() < Output.Get_Cellsize() )	// Up-Scaling...
	{
		switch( Parameters("SCALE_UP")->asInt() )
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
		switch( Parameters("SCALE_DOWN")->asInt() )
		{
		case  0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
		case  1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
		case  2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
		case  3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
		case  4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
		}
	}

	//-------------------------------------------------
	pOutputs->Del_Items();

	for(int i=0; i<pInputs->Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid	*pInput		= pInputs->asGrid(i);

		CSG_Grid	*pOutput	= SG_Create_Grid(Output,
			Parameters("KEEP_TYPE")->asBool() ? pInput->Get_Type() : SG_DATATYPE_Undefined
		);

		pOutput->Assign(pInput, Interpolation);
		pOutput->Set_Name(pInput->Get_Name());

		pOutputs->Add_Item(pOutput);
	}

	//-------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
