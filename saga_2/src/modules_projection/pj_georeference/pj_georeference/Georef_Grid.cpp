
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Grid_Georeference                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Georef_Grid.cpp                    //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

// Widely rearranged by O.Conrad April 2006 !!!

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Georef_Grid.h"

#include <string.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Grid::CGeoref_Grid(void)
{
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------

	Set_Name		(_TL("Georeferencing - Grids"));

	Set_Author		(_TL("(c) 2004 Ringeler, (c) 2006 O.Conrad"));

	Set_Description	(_TL(
		"Georeferencing of grids. Either choose the attribute fields (x/y) "
		"with the projected coordinates for the reference points (origin) or supply a "
		"additional points layer with correspondend points in the target projection. "
		"\n"
		"This library uses the Minpack routines for solving the nonlinear equations and "
		"nonlinear least squares problem. You find minpack and more information "
		"about minpack at:\n"
		"  <a target=\"_blank\" href=\"http://www.netlib.org/minpack\">"
		"  http://www.netlib.org/minpack</a>\n"
		"\n"
		"or download the C source codes:\n"
		"  <a target=\"_blank\" href=\"http://www.netlib.org/minpack/cminpack.tar\">"
		"  http://www.netlib.org/minpack/cminpack.tar</a>\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		""
	);

	Parameters.Add_Shapes_Output(
		NULL	, "SHAPES"	, _TL("Shapes"),
		""
	);


	CSG_Parameter	*pSource	= Parameters.Add_Shapes(
		NULL	, "REF_SOURCE"	, _TL("Reference Points (Origin)"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "REF_TARGET"	, _TL("Reference Points (Projection)"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field(
		pSource	, "XFIELD"		, _TL("x Position"),
		""
	);

	Parameters.Add_Table_Field(
		pSource	, "YFIELD"		, _TL("y Position"),
		""
	);


	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"),
		"SOURCE"		, _TL("Source"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		Parameters("TARGET_NODE"),
		"TARGET_TYPE"	, _TL("Target"),
		"",

		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("User defined"),
			_TL("Automatic fit"),
			_TL("Grid Project"),
			_TL("Grid"),
			_TL("Shapes")
		), 0
	);

	Parameters.Add_Choice(
		Parameters("TARGET_NODE")	, "INTERPOLATION"	, _TL("Grid Interpolation"),
		"",

		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neigbhor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_AUTOFIT"	, _TL("Automatic fit")	, "");

	pParameters->Add_Value(
		NULL, "GRIDSIZE"	, _TL("Grid Size")	, "", PARAMETER_TYPE_Double, 10000.0, 0.0, true
	);

	pParameters->Add_Choice(
		NULL, "AUTOEXTMODE"	, _TL("Fit Size")	, "",

		CSG_String::Format("%s|%s|",
			_TL("Extent only (fast)"),
			_TL("Check each point")
		), 0
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_USER"		, _TL("User defined")		, "");

	pParameters->Add_Value(
		NULL, "XMIN"		, _TL("Left")		, "", PARAMETER_TYPE_Double
	);
	pParameters->Add_Value(
		NULL, "XMAX"		, _TL("Right")		, "", PARAMETER_TYPE_Double
	);
	pParameters->Add_Value(
		NULL, "YMIN"		, _TL("Bottom")		, "", PARAMETER_TYPE_Double
	);
	pParameters->Add_Value(
		NULL, "YMAX"		, _TL("Top")		, "", PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "SIZE"		, _TL("Grid Size")	, "", PARAMETER_TYPE_Double, 10000.0, 0.0, true
	);

	pParameters->Add_Info_Value(
		NULL, "NX"			, _TL("Columns")	, "", PARAMETER_TYPE_Int
	);
	pParameters->Add_Info_Value(
		NULL, "NY"			, _TL("Rows")		, "", PARAMETER_TYPE_Int
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_GRID"		, _TL("Choose Grid")			, "");

	pParameters->Add_Grid(
		NULL, "GRID"		, _TL("Grid")		, "", PARAMETER_INPUT	, false
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_SHAPES"		, _TL("Choose Shapes")		, "");

	pParameters->Add_Shapes(
		NULL, "SHAPES"		, _TL("Shapes")		, "", PARAMETER_OUTPUT	, SHAPE_TYPE_Point
	);
}

//---------------------------------------------------------
CGeoref_Grid::~CGeoref_Grid(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoref_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	double	xMin, xMax, yMin, yMax, size;

	if( !strcmp(pParameters->Get_Identifier(), "GET_USER") )
	{
		xMin	= pParameters->Get_Parameter("XMIN")->asDouble();
		xMax	= pParameters->Get_Parameter("XMAX")->asDouble();
		yMin	= pParameters->Get_Parameter("YMIN")->asDouble();
		yMax	= pParameters->Get_Parameter("YMAX")->asDouble();
		size	= pParameters->Get_Parameter("SIZE")->asDouble();

		if( !strcmp(pParameter->Get_Identifier(), "SIZE") )
		{
			pParameters->Get_Parameter("XMAX")->Set_Value((xMax = xMin + ((int)((xMax - xMin) / size)) * size));
			pParameters->Get_Parameter("YMAX")->Set_Value((yMax = yMin + ((int)((yMax - yMin) / size)) * size));
		}
		else 
		{
			if( !strcmp(pParameter->Get_Identifier(), "XMIN") )
			{
				if( xMin >= xMax )
				{
					xMin	= xMax - pParameters->Get_Parameter("NX")->asInt() * size;
					pParameter->Set_Value(xMin);
				}

				pParameters->Get_Parameter("XMAX")->Set_Value(xMin + ((int)((xMax - xMin) / size)) * size);
			}
			else if( !strcmp(pParameter->Get_Identifier(), "XMAX") )
			{
				if( xMin >= xMax )
				{
					xMax	= xMin + pParameters->Get_Parameter("NX")->asInt() * size;
					pParameter->Set_Value(xMax);
				}

				pParameters->Get_Parameter("XMIN")->Set_Value(xMax - ((int)((xMax - xMin) / size)) * size);
			}
			else if( !strcmp(pParameter->Get_Identifier(), "YMIN") )
			{
				if( yMin >= yMax )
				{
					yMin	= yMax - pParameters->Get_Parameter("NY")->asInt() * size;
					pParameter->Set_Value(yMin);
				}

				pParameters->Get_Parameter("YMAX")->Set_Value(yMin + ((int)((yMax - yMin) / size)) * size);
			}
			else if( !strcmp(pParameter->Get_Identifier(), "YMAX") )
			{
				if( yMin >= yMax )
				{
					yMax	= yMin + pParameters->Get_Parameter("NY")->asInt() * size;
					pParameter->Set_Value(yMax);
				}

				pParameters->Get_Parameter("YMIN")->Set_Value(yMax - ((int)((yMax - yMin) / size)) * size);
			}
		}

		pParameters->Get_Parameter("NX")->Set_Value(1 + (int)((xMax - xMin) / size));
		pParameters->Get_Parameter("NY")->Set_Value(1 + (int)((yMax - yMin) / size));

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
bool CGeoref_Grid::On_Execute(void)
{
	int		xField, yField;
	CSG_Shapes	*pShapes_A, *pShapes_B;

	//-----------------------------------------------------
	pShapes_A	= Parameters("REF_SOURCE")	->asShapes();
	pShapes_B	= Parameters("REF_TARGET")	->asShapes();
	xField		= Parameters("XFIELD")		->asInt();
	yField		= Parameters("YFIELD")		->asInt();

	//-----------------------------------------------------
	if( ( pShapes_B && m_Engine.Set_Engine(pShapes_A, pShapes_B))
	||	(!pShapes_B && m_Engine.Set_Engine(pShapes_A, xField, yField))	)
	{
		Message_Add(m_Engine.Get_Message());

		return( Get_Conversion() );
	}

	Error_Set(m_Engine.Get_Message());

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Get_Conversion(void)
{
	int			Interpolation;
	TSG_Grid_Type	Type;
	CSG_Grid		*pSource, *pGrid;
	CSG_Shapes		*pShapes;

	//-----------------------------------------------------
	pSource			= Parameters("SOURCE")->asGrid();

	pGrid			= NULL;
	pShapes			= NULL;

	Interpolation	= Parameters("INTERPOLATION")->asInt();
	Type			= Interpolation == 0 ? pSource->Get_Type() : GRID_TYPE_Float;

	//-----------------------------------------------------
	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// create new user defined grid...
		pGrid	= Get_Target_Userdef(pSource, Type);
		break;

	case 1:	// create new with chosen grid size and fitted extent...
		if( Dlg_Extra_Parameters("GET_AUTOFIT") )
		{
			pGrid	= Get_Target_Autofit(
						pSource,
						Get_Extra_Parameters("GET_AUTOFIT")->Get_Parameter("GRIDSIZE")		->asDouble(),
						Get_Extra_Parameters("GET_AUTOFIT")->Get_Parameter("AUTOEXTMODE")	->asInt(),
						Type
					);
		}
		break;

	case 2:	// select grid project...
		if( Dlg_Extra_Parameters("GET_GRID") )
		{
			pGrid	= SG_Create_Grid(
						Get_Extra_Parameters("GET_GRID")->Get_Parameter("GRID")->asGrid()
					);
		}
		break;

	case 3:	// select grid...
		if( Dlg_Extra_Parameters("GET_GRID") )
		{
			pGrid	= Get_Extra_Parameters("GET_GRID")->Get_Parameter("GRID")->asGrid();
		}
		break;

	case 4:	// shapes...
		if( Dlg_Extra_Parameters("GET_SHAPES") )
		{
			pShapes	= Get_Extra_Parameters("GET_SHAPES")->Get_Parameter("SHAPES")->asShapes();
		}
		break;

	default:
		return( false );
	}

	//-----------------------------------------------------
	if( pShapes )
	{
		Set_Shapes	(pSource, pShapes);

		Parameters("SHAPES")	->Set_Value(pShapes);
	}


	//-----------------------------------------------------
	if( pGrid )
	{
		Set_Grid	(pSource, pGrid, Interpolation);

		Parameters("GRID")		->Set_Value(pGrid);
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
CSG_Grid * CGeoref_Grid::Get_Target_Userdef(CSG_Grid *pSource, TSG_Grid_Type Type)
{
	int			x, y;
	double		xMin, yMin, xMax, yMax, size;
	TSG_Point	Pt_Source;
	CSG_Grid		*pTarget;
	CSG_Parameters	*pParameters;

	pTarget	= NULL;

	if( pSource )
	{
		//-------------------------------------------------
		xMin	= yMin	= 1.0;
		xMax	= yMax	= 0.0;

		for(y=0, Pt_Source.y=0; y<pSource->Get_NY(); y++, Pt_Source.y+=1)
		{
			Pt_Source.x	= 0;
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

			Pt_Source.x	= pSource->Get_NX();
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
		}

		for(x=0, Pt_Source.x=0; x<pSource->Get_NX(); x++, Pt_Source.x+=1)
		{
			Pt_Source.y	= 0;
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

			Pt_Source.y	= pSource->Get_NY();
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
		}

		//-------------------------------------------------
		if( xMin < xMax && yMin < yMax )
		{
			pParameters	= Get_Extra_Parameters("GET_USER");

			pParameters->Get_Parameter("XMIN")->Set_Value(xMin);
			pParameters->Get_Parameter("XMAX")->Set_Value(xMax);
			pParameters->Get_Parameter("YMIN")->Set_Value(yMin);
			pParameters->Get_Parameter("YMAX")->Set_Value(yMax);
			size	= (xMax - xMin) / 100.0;
			pParameters->Get_Parameter("SIZE")->Set_Value(size);
			pParameters->Get_Parameter("NX")->Set_Value(1 + (int)((xMax - xMin) / size));
			pParameters->Get_Parameter("NY")->Set_Value(1 + (int)((yMax - yMin) / size));

			if( Dlg_Extra_Parameters("GET_USER") )
			{
				size	= pParameters->Get_Parameter("SIZE")->asDouble();

				pTarget	= SG_Create_Grid(
					Type,
					pParameters->Get_Parameter("NX")->asInt(),
					pParameters->Get_Parameter("NY")->asInt(),
					size,
					pParameters->Get_Parameter("XMIN")->asDouble(),
					pParameters->Get_Parameter("YMIN")->asDouble()
				);
			}
		}
	}

	return( pTarget );
}

//---------------------------------------------------------
CSG_Grid * CGeoref_Grid::Get_Target_Autofit(CSG_Grid *pSource, double Grid_Size, int AutoExtMode, TSG_Grid_Type Type)
{
	int			x, y;
	double		xMin, yMin, xMax, yMax;
	TSG_Point	Pt_Source;
	CSG_Grid		*pTarget;

	pTarget	= NULL;

	if( pSource )
	{
		xMin	= yMin	= 1.0;
		xMax	= yMax	= 0.0;

		//---------------------------------------------
		switch( AutoExtMode )
		{
		case 0:	default:
			for(y=0, Pt_Source.y=0; y<pSource->Get_NY(); y++, Pt_Source.y+=1)
			{
				Pt_Source.x	= 0;
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

				Pt_Source.x	= pSource->Get_NX();
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
			}

			for(x=0, Pt_Source.x=0; x<pSource->Get_NX(); x++, Pt_Source.x+=1)
			{
				Pt_Source.y	= 0;
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

				Pt_Source.y	= pSource->Get_NY();
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
			}

			break;

		//---------------------------------------------
		case 1:
			for(y=0, Pt_Source.y=0; y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=1)
			{
				for(x=0, Pt_Source.x=0; x<pSource->Get_NX(); x++, Pt_Source.x+=1)
				{
					if( !pSource->is_NoData(x, y) )
					{
						Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
					}
				}
			}

			break;
		}

		//---------------------------------------------
		if( is_Progress() && xMin < xMax && yMin < yMax )
		{
			pTarget	= SG_Create_Grid(
				Type,
				1 + (int)((xMax - xMin) / Grid_Size),
				1 + (int)((yMax - yMin) / Grid_Size),
				Grid_Size,
				xMin, yMin
			);
		}
	}

	return( pTarget );
}

//---------------------------------------------------------
inline void CGeoref_Grid::Get_MinMax(double &xMin, double &xMax, double &yMin, double &yMax, TSG_Point Point)
{
	if( m_Engine.Get_Converted(Point) )
	{
		if( xMin > xMax )
		{
			xMin	= xMax	= Point.x;
		}
		else if( xMin > Point.x )
		{
			xMin	= Point.x;
		}
		else if( xMax < Point.x )
		{
			xMax	= Point.x;
		}

		if( yMin > yMax )
		{
			yMin	= yMax	= Point.y;
		}
		else if( yMin > Point.y )
		{
			yMin	= Point.y;
		}
		else if( yMax < Point.y )
		{
			yMax	= Point.y;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Set_Grid(CSG_Grid *pSource, CSG_Grid *pTarget, int Interpolation)
{
	int			x, y;
	double		z;
	TSG_Point	Pt_Source, Pt_Target;

	if( pSource && pTarget )
	{
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_ZFactor(pSource->Get_ZFactor());
		pTarget->Set_Name	(pSource->Get_Name());
		pTarget->Set_Unit	(pSource->Get_Unit());

		pTarget->Assign_NoData();

		//-------------------------------------------------
		for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
		{
			for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( m_Engine.Get_Converted(Pt_Source, true) )
				{
					z	= pSource->Get_Value(
							pSource->Get_XMin() + Pt_Source.x * pSource->Get_Cellsize(),
							pSource->Get_YMin() + Pt_Source.y * pSource->Get_Cellsize(),
							Interpolation
						);

					if( !pSource->is_NoData_Value(z) )
					{
						pTarget->Set_Value(x, y, z);
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGeoref_Grid::Set_Shapes(CSG_Grid *pSource, CSG_Shapes *pTarget)
{
	int			x, y;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Shape		*pShape;

	if( pSource && pTarget )
	{
		pTarget->Create(SHAPE_TYPE_Point, pSource->Get_Name());
		pTarget->Get_Table().Add_Field("Z", TABLE_FIELDTYPE_Double);

		for(y=0, Pt_Source.y=0; y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=1)
		{
			for(x=0, Pt_Source.x=0; x<pSource->Get_NX(); x++, Pt_Source.x+=1)
			{
				if( !pSource->is_NoData(x, y) )
				{
					Pt_Target	= Pt_Source;

					if( m_Engine.Get_Converted(Pt_Target) )
					{
						pShape		= pTarget->Add_Shape();
						pShape->Add_Point(Pt_Target);
						pShape->Get_Record()->Set_Value(0, pSource->asDouble(x, y));
					}
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
