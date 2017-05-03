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
#include "Grid_To_Gradient.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_To_Gradient::CGrid_To_Gradient(int Method)
{
	m_Method	= Method;

	Set_Author		(SG_T("O.Conrad (c) 2006"));

	//-----------------------------------------------------
	switch( m_Method )
	{
	case 0:	// surface
		Set_Name		(_TL("Gradient Vectors from Surface"));

		Set_Description	(_TW(
			"Create lines indicating the gradient. "
		));

		Parameters.Add_Grid(
			NULL	, "SURFACE"		, _TL("Surface"),
			_TL(""),
			PARAMETER_INPUT
		);

		break;

	case 1:	// direction and length
		Set_Name		(_TL("Gradient Vectors from Direction and Length"));

		Set_Description	(_TW(
			"Create lines indicating the gradient. "
		));

		Parameters.Add_Grid(
			""	, "DIR"			, _TL("Direction"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Grid(
			""	, "LEN"			, _TL("Length"),
			_TL(""),
			PARAMETER_INPUT
		);

		break;

	case 2:	// directional components
		Set_Name		(_TL("Gradient Vectors from Directional Components"));

		Set_Description	(_TW(
			"Create lines indicating the gradient. "
		));

		Parameters.Add_Grid(
			""	, "X"			, _TL("X Component"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Grid(
			""	, "Y"			, _TL("Y Component"),
			_TL(""),
			PARAMETER_INPUT
		);

		break;
	}


	//-----------------------------------------------------
	Parameters.Add_Shapes(
		""	, "VECTORS"		, _TL("Gradient Vectors"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		""	, "STEP"		, _TL("Step"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Range(
		""	, "SIZE"		, _TL("Size Range"),
		_TL("size range as percentage of step"),
		25.0, 100.0, 0.0, true
	);

	Parameters.Add_Choice(
		""	, "AGGR"		, _TL("Aggregation"),
		_TL("how to request values if step size is more than one cell"),
		CSG_String::Format("%s|%s|",
			_TL("nearest neighbour"),
			_TL("mean value")
		), 1
	);

	Parameters.Add_Choice(
		""	, "STYLE"		, _TL("Style"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("simple line"),
			_TL("arrow"),
			_TL("arrow (centered to cell)")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Gradient::On_Execute(void)
{
	int						x, y, Step;
	double					sMin, sRange, ex, ey, d;
	TSG_Point				p;
	TSG_Grid_Resampling	Interpolation;
	CSG_Grid_System			System;
	CSG_Grid				EX, EY, D;
	CSG_Shapes				*pVectors;

	//-----------------------------------------------------
	pVectors		= Parameters("VECTORS")	->asShapes();
	Step			= Parameters("STEP")	->asInt();
	m_Style			= Parameters("STYLE")	->asInt();
	sMin			= Parameters("SIZE")	->asRange()->Get_LoVal() * Step * Get_Cellsize() / 100.0;
	sRange			= Parameters("SIZE")	->asRange()->Get_HiVal() * Step * Get_Cellsize() / 100.0 - sMin;
	Interpolation	= Parameters("AGGR")	->asInt() == 0 ? GRID_RESAMPLING_NearestNeighbour : GRID_RESAMPLING_Mean_Cells;

	//-----------------------------------------------------
	if( Step > Get_NX() || Step > Get_NY() )
	{
		Error_Set(_TL("step size should not exceed number of cells in x or y direction"));

		return( false );
	}

	//-----------------------------------------------------
//	System.Assign(Step * Get_Cellsize(), Get_XMin(), Get_YMin(), Get_NX() / Step, Get_NY() / Step);
	System.Assign(Step * Get_Cellsize(), Get_XMin(), Get_YMin(), Get_System()->Get_XMax(), Get_System()->Get_YMax());

	EX.Create(System);
	EY.Create(System);
	D .Create(System);
	D .Assign_NoData();

	switch( m_Method )
	{
	//-----------------------------------------------------
	case 0:	// surface
		{
			CSG_Grid	Surface(System), *pSurface	= Parameters("SURFACE")->asGrid();	Surface.Assign(pSurface, Interpolation);
			
			pVectors->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s]"), pSurface->Get_Name(), _TL("Gradient")));

			for(y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
			{
				for(x=0; x<System.Get_NX(); x++)
				{
					if( Surface.Get_Gradient(x, y, d, ey) )
					{
						EX.Set_Value(x, y, sin(ey));
						EY.Set_Value(x, y, cos(ey));
						D .Set_Value(x, y, tan(d));
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// direction and length
		{
			CSG_Grid	*pDir	= Parameters("DIR")->asGrid(), _X(*Get_System());
			CSG_Grid	*pLen	= Parameters("LEN")->asGrid(), _Y(*Get_System());

			for(y=0; y<Get_NY() && Set_Progress(y, Get_NY()); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					if( !pDir->is_NoData(x, y) && !pLen->is_NoData(x, y) )
					{
						_X.Set_Value(x, y, pLen->asDouble(x, y) * sin(pDir->asDouble(x, y)));
						_Y.Set_Value(x, y, pLen->asDouble(x, y) * cos(pDir->asDouble(x, y)));
					}
					else
					{
						_X.Set_NoData(x, y);
						_Y.Set_NoData(x, y);
					}
				}
			}

			CSG_Grid	X(System);	X.Assign(&_X, Interpolation);
			CSG_Grid	Y(System);	Y.Assign(&_Y, Interpolation);

			pVectors->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s|%s]"), _TL("Gradient"), pDir->Get_Name(), pLen->Get_Name()));

			for(y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
			{
				for(x=0; x<System.Get_NX(); x++)
				{
					if( !X.is_NoData(x, y) && !Y.is_NoData(x, y) && (d = SG_Get_Length(X.asDouble(x, y), Y.asDouble(x, y))) > 0.0 )
					{
						EX.Set_Value(x, y, X.asDouble(x, y) / d);
						EY.Set_Value(x, y, Y.asDouble(x, y) / d);
						D .Set_Value(x, y, d);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// directional components
		{
			CSG_Grid	X(System), *pX	= Parameters("X")->asGrid();	X.Assign(pX, Interpolation);
			CSG_Grid	Y(System), *pY	= Parameters("Y")->asGrid();	Y.Assign(pY, Interpolation);

			pVectors->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s|%s]"), _TL("Gradient"), pX->Get_Name(), pY->Get_Name()));

			for(y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
			{
				for(x=0; x<System.Get_NX(); x++)
				{
					if( !X.is_NoData(x, y) && !Y.is_NoData(x, y) && (d = SG_Get_Length(X.asDouble(x, y), Y.asDouble(x, y))) > 0.0 )
					{
						EX.Set_Value(x, y, X.asDouble(x, y) / d);
						EY.Set_Value(x, y, Y.asDouble(x, y) / d);
						D .Set_Value(x, y, d);
					}
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

	if( D.Get_Range() > 0.0 )
	{
		sRange	= sRange / D.Get_Range();
	}

	//-----------------------------------------------------
	for(y=0, p.y=System.Get_YMin(); y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++, p.y+=System.Get_Cellsize())
	{
		for(x=0, p.x=System.Get_XMin(); x<System.Get_NX(); x++, p.x+=System.Get_Cellsize())
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

				if( (d = sMin + sRange * (d - D.Get_Min())) > 0.0 )
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
