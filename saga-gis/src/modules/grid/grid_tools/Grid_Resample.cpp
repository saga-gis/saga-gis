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
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"),
			_TL("Mean Value"),
			_TL("Mean Value (cell area weighted)"),
			_TL("Minimum Value"),
			_TL("Maximum Value"),
			_TL("Majority")
		), 5
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "SCALE_DOWN"	, _TL("Downscaling Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");
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
		CSG_Grid_System	*pSystem	= pParameter->asGrid_System();

		if( pSystem && pSystem->is_Valid() )
		{
			bool	bCells	= Parameters("TARGET_USER_FITS")->asInt() == 1;

			m_Grid_Target.Set_User_Defined(pParameters,
				pSystem->Get_XMin(bCells), pSystem->Get_YMin(bCells),
				pSystem->Get_Cellsize(),
				pSystem->Get_NX(), pSystem->Get_NY(), bCells
			);
		}
	}
	else
	{
		m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);
	}

	return( CSG_Module_Grid::On_Parameter_Changed(pParameters, pParameter) );
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

			if( pParameters->Get_Parameter("TARGET_DEFINITION")->asInt() == 0 )	// user defined
			{
				Scaling	= Input - pParameters->Get_Parameter("TARGET_USER_SIZE")->asDouble();
			}
			else if( pParameters->Get_Parameter("TARGET_SYSTEM")->asGrid_System() && pParameters->Get_Parameter("TARGET_SYSTEM")->asGrid_System()->Get_Cellsize() > 0.0 )
			{
				Scaling	= Input - pParameters->Get_Parameter("TARGET_SYSTEM")->asGrid_System()->Get_Cellsize();
			}
		}

		pParameters->Set_Enabled("SCALE_UP"  , Scaling <  0.0);
		pParameters->Set_Enabled("SCALE_DOWN", Scaling >= 0.0);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
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

	CSG_Grid_System	Output	= m_Grid_Target.Get_System();

	if( Input.Get_Extent().Intersects(Output.Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("clip extent does not match extent of input grids"));

		return( false );
	}

	//-------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	if( Input.Get_Cellsize() < Output.Get_Cellsize() )	// Up-Scaling...
	{
		switch( Parameters("SCALE_UP")->asInt() )
		{
		default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
		case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
		case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
		case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
		case  4:	Resampling	= GRID_RESAMPLING_Mean_Nodes;		break;
		case  5:	Resampling	= GRID_RESAMPLING_Mean_Cells;		break;
		case  6:	Resampling	= GRID_RESAMPLING_Minimum;			break;
		case  7:	Resampling	= GRID_RESAMPLING_Maximum;			break;
		case  8:	Resampling	= GRID_RESAMPLING_Majority;			break;
		}
	}
	else	// Down-Scaling...
	{
		switch( Parameters("SCALE_DOWN")->asInt() )
		{
		default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
		case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
		case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
		case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
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

		pOutput->Assign(pInput, Resampling);
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
