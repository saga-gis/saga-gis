
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_To_Gradient.cpp                 //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Grid_To_Gradient.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_To_Gradient::CGrid_To_Gradient(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Gradient from Grid"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"Create lines indicating the gradient. "
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "SHAPES"		, _TL("Gradient"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "STEP"		, _TL("Skip"),
		_TL(""),
		PARAMETER_TYPE_Int	, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "SIZE_MIN"	, _TL("Minimum Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "SIZE_MAX"	, _TL("Maximum Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0
	);

	Parameters.Add_Choice(
		NULL, "STYLE"		, _TL("Style"),
		_TL(""),
		CSG_String::Format(SG_T("%s|"),
			_TL("Line"),
			_TL("Arrow")
		), 0
	);
}

//---------------------------------------------------------
CGrid_To_Gradient::~CGrid_To_Gradient(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Gradient::On_Execute(void)
{
	bool	bTangens	= false;
	int		Style, Step, x, y;
	double	sMin, sRange, Min, Max, dStep, Slope, Aspect, xPt, yPt;
	CSG_Grid	*pGrid;
	CSG_Shapes	*pShapes;
	CSG_Shape	*pShape;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")		->asGrid();
	pShapes	= Parameters("SHAPES")		->asShapes();
	Step	= Parameters("STEP")		->asInt();
	Style	= Parameters("STYLE")		->asInt();
	sMin	= Parameters("SIZE_MIN")	->asDouble();
	sRange	= Parameters("SIZE_MAX")	->asDouble() - sMin;
	dStep	= Step * Get_Cellsize();

	pShapes->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("Gradient")));
	pShapes->Add_Field("X"	, SG_DATATYPE_Double);
	pShapes->Add_Field("Y"	, SG_DATATYPE_Double);
	pShapes->Add_Field("S"	, SG_DATATYPE_Double);
	pShapes->Add_Field("A"	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(y=0, Min=0.0, Max=-1.0; y<Get_NY() && Set_Progress(y); y+=Step)
	{
		for(x=0; x<Get_NX(); x+=Step)
		{
			if( pGrid->Get_Gradient(x, y, Slope, Aspect) )
			{
				if( Min > Max )
					Min	= Max	= Slope;
				else if( Min > Slope )
					Min	= Slope;
				else if( Max < Slope )
					Max	= Slope;
			}
		}
	}

	if( Min < Max )
	{
		sRange	= sRange / (Max - Min);
	}

	//-----------------------------------------------------
	for(y=0, yPt=Get_YMin(); y<Get_NY() && Set_Progress(y); y+=Step, yPt+=dStep)
	{
		for(x=0, xPt=Get_XMin(); x<Get_NX(); x+=Step, xPt+=dStep)
		{
			if( pGrid->Get_Gradient(x, y, Slope, Aspect) )
			{
				pShape	= pShapes->Add_Shape();
				pShape->Set_Value(0, xPt);
				pShape->Set_Value(1, yPt);
				pShape->Set_Value(2, Slope);
				pShape->Set_Value(3, Aspect);
				pShape->Add_Point(xPt, yPt);

		//		Slope	= sMin + sRange * (bTangens ? tan(Slope) : Slope / M_PI_090);
				Slope	= sMin + sRange * (Slope - Min);

				pShape->Add_Point(
					xPt + sin(Aspect) * Slope,
					yPt + cos(Aspect) * Slope
				);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
