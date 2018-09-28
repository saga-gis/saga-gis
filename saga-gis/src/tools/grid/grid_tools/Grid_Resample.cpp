/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
	Parameters.Add_Grid_List("",
		"INPUT"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"OUTPUT"	, _TL("Resampled Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Bool("",
		"KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SCALE_UP"	, _TL("Upscaling Method"),
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
	Parameters.Add_Choice("",
		"SCALE_DOWN", _TL("Downscaling Method"),
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Resample::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("PARAMETERS_GRID_SYSTEM") && pParameter->asGrid_System() )
	{
		m_Grid_Target.Set_User_Defined(pParameters, *pParameter->asGrid_System());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Resample::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( SG_UI_Get_Window_Main() )
	{
		double	Scaling	= 0.0;

		if( pParameters->Get_Parameter("INPUT")->asGridList()->Get_Grid_Count() > 0 )
		{
			double	Input	= pParameters->Get_Parameter("INPUT")->asGridList()->Get_Grid(0)->Get_System().Get_Cellsize();

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

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Resample::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pInput	= Parameters("INPUT")->asGridList();

	if( pInput->Get_Grid_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System	System	= m_Grid_Target.Get_System();

	if( Get_System().Get_Extent().Intersects(System.Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("clip extent does not match extent of input grids"));

		return( false );
	}

	//-------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	if( Get_Cellsize() < System.Get_Cellsize() )	// Up-Scaling...
	{
		switch( Parameters("SCALE_UP")->asInt() )
		{
		default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
		case  1:	Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
		case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
		case  3:	Resampling	= GRID_RESAMPLING_BSpline         ;	break;
		case  4:	Resampling	= GRID_RESAMPLING_Mean_Nodes      ;	break;
		case  5:	Resampling	= GRID_RESAMPLING_Mean_Cells      ;	break;
		case  6:	Resampling	= GRID_RESAMPLING_Minimum         ;	break;
		case  7:	Resampling	= GRID_RESAMPLING_Maximum         ;	break;
		case  8:	Resampling	= GRID_RESAMPLING_Majority        ;	break;
		}
	}
	else	// Down-Scaling...
	{
		switch( Parameters("SCALE_DOWN")->asInt() )
		{
		default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
		case  1:	Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
		case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
		case  3:	Resampling	= GRID_RESAMPLING_BSpline         ;	break;
		}
	}

	//-------------------------------------------------
	bool	bKeepType	= Parameters("KEEP_TYPE")->asBool();

	Parameters("OUTPUT")->asGridList()->Del_Items();

	for(int i=0; i<pInput->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		CSG_Data_Object	*pResampled, *pObject = pInput->Get_Item(i);

		switch( pObject->Get_ObjectType() )
		{
		default:
			{
				CSG_Grid	*pGrid	= (CSG_Grid  *)pObject;

				pResampled	= SG_Create_Grid (System,
					bKeepType ? pGrid->Get_Type() : SG_DATATYPE_Undefined
				);

				((CSG_Grid  *)pResampled)->Assign(pGrid, Resampling);
			}
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			{
				CSG_Grids	*pGrids	= (CSG_Grids *)pObject;

				pResampled	= SG_Create_Grids(System, pGrids->Get_Attributes(), pGrids->Get_Z_Attribute(),
					bKeepType ? pGrids->Get_Type() : SG_DATATYPE_Undefined, true
				);

				((CSG_Grids *)pResampled)->Assign(pGrids, Resampling);
			}
			break;
		}

		pResampled->Set_Name(pObject->Get_Name());
		pResampled->Set_Description(pObject->Get_Description());
		pResampled->Get_MetaData().Assign(pObject->Get_MetaData());

		Parameters("OUTPUT")->asGridList()->Add_Item(pResampled);

		DataObject_Add(pResampled);
		DataObject_Set_Parameters(pResampled, pObject);
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
