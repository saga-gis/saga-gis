
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Interpolation.cpp                   //
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
#include "Interpolation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation::CInterpolation(void)
{
	CParameter	*pNode_0, *pNode_1;
	CParameters	*pParameters;

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "OUTPUT"		, _TL("Interpolation"),
		""
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		"",
		PARAMETER_INPUT
	);

	pNode_1	= Parameters.Add_Table_Field(
		pNode_0	, "FIELD"		, _TL("Attribute"),
		""
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "TARGET_TYPE"	, _TL("Target Dimensions"),
		"",

		CAPI_String::Format("%s|%s|%s|",
			_TL("User defined"),
			_TL("Grid Project"),
			_TL("Grid")
		), 0
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("USER", _TL("User defined grid"), "");

	pNode_0	= pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode_0	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pNode_1	= pParameters->Add_Range(
		pNode_0	, "X_EXTENT"	, _TL("X-Extent"),
		""
	);

	pNode_1	= pParameters->Add_Range(
		pNode_0	, "Y_EXTENT"	, _TL("Y-Extent"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GRID", _TL("Choose Grid"), "");

	pNode_0	= pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);
}

//---------------------------------------------------------
CInterpolation::~CInterpolation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::On_Initialize_Parameters(void)
{
	return( true );
}

//---------------------------------------------------------
void CInterpolation::On_Finalize_Parameters(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid * CInterpolation::_Get_Target_Grid(CParameters *pParameters, CShapes *pShapes)
{
	int			nx, ny;
	double		Cell_Size, xMin, yMin, xMax, yMax;

	if( pParameters->Get_Parameter("FIT_EXTENT")->asBool() )
	{
		xMin	= pShapes->Get_Extent().m_rect.xMin;
		yMin	= pShapes->Get_Extent().m_rect.yMin;
		xMax	= pShapes->Get_Extent().m_rect.xMax;
		yMax	= pShapes->Get_Extent().m_rect.yMax;
	}
	else
	{
		xMin	= pParameters->Get_Parameter("X_EXTENT")->asRange()->Get_LoVal();
		yMin	= pParameters->Get_Parameter("Y_EXTENT")->asRange()->Get_LoVal();
		xMax	= pParameters->Get_Parameter("X_EXTENT")->asRange()->Get_HiVal();
		yMax	= pParameters->Get_Parameter("Y_EXTENT")->asRange()->Get_HiVal();
	}

	Cell_Size	= pParameters->Get_Parameter("CELL_SIZE")->asDouble();

	nx			= 1 + (int)((xMax - xMin) / Cell_Size);
	ny			= 1 + (int)((yMax - yMin) / Cell_Size);

	return( API_Create_Grid(GRID_TYPE_Float, nx, ny, Cell_Size, xMin, yMin) );
}

//---------------------------------------------------------
CShapes * CInterpolation::_Get_Point_Shapes(CShapes *pShapes)
{
	int		iShape, iPart, iPoint;
	CShape	*pShape, *pPoint;
	CShapes	*pPoints;

	if( pShapes->Get_Type() != SHAPE_TYPE_Point )
	{
		pPoints	= API_Create_Shapes(SHAPE_TYPE_Point, NULL, &pShapes->Get_Table());

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pShape->Get_Record());
					pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));
				}
			}
		}

		return( pPoints );
	}

	return( pShapes );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::On_Execute(void)
{
	bool	bResult	= false;
	CShapes	*pShapes, *pPoints;

	//-----------------------------------------------------
	pShapes		= Parameters("SHAPES")	->asShapes();
	zField		= Parameters("FIELD")	->asInt();

	if( pShapes->Get_Table().Get_Field_Count() > 0 )
	{
		pGrid		= NULL;

		switch( Parameters("TARGET_TYPE")->asInt() )
		{
		case 0:	// User defined...
			if( Dlg_Extra_Parameters("USER") )
			{
				pGrid	= _Get_Target_Grid(Get_Extra_Parameters("USER"), pShapes);
			}
			break;

		case 1:	// Grid Project...
			if( Dlg_Extra_Parameters("GRID") )
			{
				pGrid	= API_Create_Grid(Get_Extra_Parameters("GRID")->Get_Parameter("GRID")->asGrid());
			}
			break;

		case 2:	// Grid...
			if( Dlg_Extra_Parameters("GRID") )
			{
				pGrid	= Get_Extra_Parameters("GRID")->Get_Parameter("GRID")->asGrid();
			}
			break;
		}

		//-------------------------------------------------
		if( pGrid )
		{
			pGrid->Set_Name(CAPI_String::Format("%s (%s)", pShapes->Get_Name(), Get_Name()));
			pGrid->Assign_NoData();
			Parameters("OUTPUT")->Set_Value(pGrid);

			//---------------------------------------------
			if( Use_SearchEngine() )
			{
				if( SearchEngine.Create(pPoints = _Get_Point_Shapes(pShapes)) )
				{
					bResult	= Interpolate();
				
					SearchEngine.Destroy();
				}

				if( pPoints != pShapes )
				{
					delete(pPoints);
				}
			}
			else
			{
				bResult	= Interpolate();
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CInterpolation::Interpolate(void)
{
	int		x, y;

	if( On_Initialize_Parameters() )
	{
		for(y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
		{
			for(x=0; x<pGrid->Get_NX(); x++)
			{
				Get_Grid_Value(x, y);
			}
		}

		On_Finalize_Parameters();

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
