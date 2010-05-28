
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
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"),
			_TL("Mean Value"),
			_TL("Mean Value (cell area weighted)"),
			_TL("Minimum Value"),
			_TL("Maximum Value")
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
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Resample::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Resample::On_Execute(void)
{
	bool					bResult;
	TSG_Grid_Interpolation	Interpolation;
	CSG_Grid				*pInput, *pOutput;
	CSG_Parameters			*pParameters;

	//-----------------------------------------------------
	bResult	= false;

	pInput	= Parameters("INPUT")->asGrid();

	//-----------------------------------------------------
	pOutput	= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(pInput->Get_Extent()) && Dlg_Parameters("USER") )
		{
			pOutput	= m_Grid_Target.Get_User(Parameters("KEEP_TYPE")->asBool() ? pInput->Get_Type() : SG_DATATYPE_Undefined);
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			pOutput	= m_Grid_Target.Get_Grid(Parameters("KEEP_TYPE")->asBool() ? pInput->Get_Type() : SG_DATATYPE_Undefined);
		}
		break;
	}

	//-----------------------------------------------------
	if( !pOutput || !pInput->is_Intersecting(pOutput->Get_Extent()) )
	{
		return( false );
	}

	pParameters	= NULL;

	//-------------------------------------------------
	// Up-Scaling...

	if( pInput->Get_Cellsize() < pOutput->Get_Cellsize() )
	{
		if( Dlg_Parameters("SCALE_UP") )
		{
			switch( Get_Parameters("SCALE_UP")->Get_Parameter("METHOD")->asInt() )
			{
			case 0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
			case 1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
			case 2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
			case 3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
			case 4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
			case 5:	Interpolation	= GRID_INTERPOLATION_Mean_Nodes;		break;
			case 6:	Interpolation	= GRID_INTERPOLATION_Mean_Cells;		break;
			case 7:	Interpolation	= GRID_INTERPOLATION_Minimum;			break;
			case 8:	Interpolation	= GRID_INTERPOLATION_Maximum;			break;
			}

			pParameters	= Get_Parameters("SCALE_UP");
		}
	}

	//-------------------------------------------------
	// Down-Scaling...

	else
	{
		if( Dlg_Parameters("SCALE_DOWN") )
		{
			switch( Get_Parameters("SCALE_DOWN")->Get_Parameter("METHOD")->asInt() )
			{
			case 0:	Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;
			case 1:	Interpolation	= GRID_INTERPOLATION_Bilinear;			break;
			case 2:	Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;
			case 3:	Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;
			case 4:	Interpolation	= GRID_INTERPOLATION_BSpline;			break;
			}

			pParameters	= Get_Parameters("SCALE_DOWN");
		}
	}

	//-------------------------------------------------
	if( pParameters )
	{
		pOutput->Set_Name(pInput->Get_Name());

		pOutput->Assign(pInput, Interpolation);

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
