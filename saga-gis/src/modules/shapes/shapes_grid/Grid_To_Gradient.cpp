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
	Set_Name		(_TL("Gradient Vectors from Grid(s)"));

	Set_Author		(SG_T("O.Conrad (c) 2006"));

	Set_Description	(_TW(
		"Create lines indicating the gradient. "
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID_A"		, _TL("Grid A"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRID_B"		, _TL("Grid B"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "VECTORS"		, _TL("Gradient Vectors"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Input"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("surface (A)"),
			_TL("slope (A) and aspect (B)"),
			_TL("directional components (A = x, B = y)")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "STEP"		, _TL("Step"),
		_TL(""),
		PARAMETER_TYPE_Int	, 1.0, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "SIZE_MIN"	, _TL("Minimum Size (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.1, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "SIZE_MAX"	, _TL("Maximum Size (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Choice(
		NULL	, "STYLE"		, _TL("Style"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("simple line"),
			_TL("arrow"),
			_TL("arrow (centered to cell)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Gradient::On_Execute(void)
{
	int			x, y, Step, Method;
	double		sMin, sRange, dStep, ex, ey, d;
	TSG_Point	p;
	CSG_Grid	*pGrid_A, *pGrid_B, EX, EY, D;
	CSG_Shapes	*pVectors;

	//-----------------------------------------------------
	pGrid_A		= Parameters("GRID_A")		->asGrid();
	pGrid_B		= Parameters("GRID_B")		->asGrid();

	pVectors	= Parameters("VECTORS")		->asShapes();
	Method		= Parameters("METHOD")		->asInt();
	Step		= Parameters("STEP")		->asInt();
	m_Style		= Parameters("STYLE")		->asInt();
	sMin		= Parameters("SIZE_MIN")	->asDouble() * Get_Cellsize();
	sRange		= Parameters("SIZE_MAX")	->asDouble() * Get_Cellsize() - sMin;
	dStep		= Step * Get_Cellsize();

	//-----------------------------------------------------
	if( Method != 0 && pGrid_B == NULL )
	{
		Error_Set(_TL("second input grid (B) needed to perform desired operation"));

		return( false );
	}

	//-----------------------------------------------------
	EX.Create(*Get_System());
	EY.Create(*Get_System());
	D .Create(*Get_System());
	D .Assign_NoData();

	switch( Method )
	{
	//-----------------------------------------------------
	case 0:	// surface (A)
		pVectors->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s]"), pGrid_A->Get_Name(), _TL("Gradient")));

		for(y=0; y<Get_NY() && Set_Progress(y); y+=Step)
		{
			for(x=0; x<Get_NX(); x+=Step)
			{
				if( pGrid_A->Get_Gradient(x, y, d, ey) )
				{
					EX.Set_Value(x, y, sin(ey));
					EY.Set_Value(x, y, cos(ey));
					D .Set_Value(x, y, tan(d));
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// slope (A) and aspect (B)
		pVectors->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s|%s]"), _TL("Gradient"), pGrid_A->Get_Name(), pGrid_B->Get_Name()));

		for(y=0; y<Get_NY() && Set_Progress(y); y+=Step)
		{
			for(x=0; x<Get_NX(); x+=Step)
			{
				if( !pGrid_B->is_NoData(x, y) && !pGrid_B->is_NoData(x, y) )
				{
					EX.Set_Value(x, y, sin(pGrid_B->asDouble(x, y)));
					EY.Set_Value(x, y, cos(pGrid_B->asDouble(x, y)));
					D .Set_Value(x, y, tan(pGrid_A->asDouble(x, y)));
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// directional components (A = x, B = y)
		pVectors->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s|%s]"), _TL("Gradient"), pGrid_A->Get_Name(), pGrid_B->Get_Name()));

		for(y=0; y<Get_NY() && Set_Progress(y); y+=Step)
		{
			for(x=0; x<Get_NX(); x+=Step)
			{
				if( !pGrid_B->is_NoData(x, y) && !pGrid_B->is_NoData(x, y) && (d = SG_Get_Length(pGrid_A->asDouble(x, y), pGrid_B->asDouble(x, y))) > 0.0 )
				{
					EX.Set_Value(x, y, pGrid_A->asDouble(x, y) / d);
					EY.Set_Value(x, y, pGrid_B->asDouble(x, y) / d);
					D .Set_Value(x, y, d);
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	pVectors->Add_Field("EX"	, SG_DATATYPE_Double);
	pVectors->Add_Field("EY"	, SG_DATATYPE_Double);
	pVectors->Add_Field("LEN"	, SG_DATATYPE_Double);
	pVectors->Add_Field("DIR"	, SG_DATATYPE_Double);

	if( D.Get_ZRange() > 0.0 )
	{
		sRange	= sRange / D.Get_ZRange();
	}

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y+=Step, p.y+=dStep)
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x+=Step, p.x+=dStep)
		{
			if( !D.is_NoData(x, y) )
			{
				CSG_Shape	*pVector	= pVectors->Add_Shape();

				ex	= EX.asDouble(x, y);
				ey	= EY.asDouble(x, y);
				d	= D .asDouble(x, y);

				pVector->Set_Value(0, ex);
				pVector->Set_Value(1, ey);
				pVector->Set_Value(2, d);
				pVector->Set_Value(3, atan2(ex, ey) * M_RAD_TO_DEG);

				if( (d = sMin + sRange * (d - D.Get_ZMin())) > 0.0 )
				{
					Set_Vector(pVector, p, d * ex, d * ey);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_TO_VECTOR(I, X, Y)	(pVector->Add_Point(Point.x + ((X) * dy + (Y) * dx), Point.y + ((Y) * dy - (X) * dx), I))

//---------------------------------------------------------
inline void CGrid_To_Gradient::Set_Vector(CSG_Shape *pVector, const TSG_Point &Point, double dx, double dy)
{
	switch( m_Style )
	{
	case 0:
		ADD_TO_VECTOR(0,  0.00,  0.00);
		ADD_TO_VECTOR(0,  0.00,  1.00);
		break;

	case 1:
		ADD_TO_VECTOR(0,  0.00,  0.00);
		ADD_TO_VECTOR(0,  0.00,  1.00);

		ADD_TO_VECTOR(1,  0.20,  0.75);
		ADD_TO_VECTOR(1,  0.00,  1.00);
		ADD_TO_VECTOR(1, -0.20,  0.75);
		break;

	case 2:
		ADD_TO_VECTOR(0,  0.00, -0.50);
		ADD_TO_VECTOR(0,  0.00,  0.50);

		ADD_TO_VECTOR(1,  0.20,  0.25);
		ADD_TO_VECTOR(1,  0.00,  0.50);
		ADD_TO_VECTOR(1, -0.20,  0.25);
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
