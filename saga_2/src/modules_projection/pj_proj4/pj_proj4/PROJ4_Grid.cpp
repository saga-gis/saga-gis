
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    PROJ4_Grid.cpp                     //
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
#include <string.h>

#include "PROJ4_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPROJ4_Grid::CPROJ4_Grid(void)
{
	CParameters	*pParameters;

	//-----------------------------------------------------
	Set_Name	(_TL("Proj4 (Grid)"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Coordinate Transformation for Grids.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://remotesensing.org/proj/\">Homepage</a>\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "OUT_GRID"	, _TL("Grid"),
		""
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUT_X"		, _TL("X Coordinates"),
		""
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUT_Y"		, _TL("Y Coordinates"),
		""
	);

	Parameters.Add_Shapes_Output(
		NULL	, "OUT_SHAPES"	, _TL("Shapes"),
		""
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		Parameters("SOURCE_NODE"),
		"SOURCE"		, _TL("Source"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		Parameters("TARGET_NODE"),
		"CREATE_XY"		, _TL("Create X/Y Grids"),
		"",
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Choice(
		Parameters("TARGET_NODE"),
		"TARGET_TYPE"	, _TL("Target"),
		"",

		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("user defined"),
			_TL("automatic fit"),
			_TL("grid system"),
			_TL("grid"),
			_TL("shapes")
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
		_TL(
		"Extent only (fast)|"
		"Check each point|"), 0
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
		NULL, "YMAX"		, _TL("Top")			, "", PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "SIZE"		, _TL("Grid Size")	, "", PARAMETER_TYPE_Double, 10000.0, 0.0, true
	);

	pParameters->Add_Info_Value(
		NULL, "NX"			, _TL("Columns")		, "", PARAMETER_TYPE_Int
	);
	pParameters->Add_Info_Value(
		NULL, "NY"			, _TL("Rows")		, "", PARAMETER_TYPE_Int
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_SYSTEM"		, _TL("Choose Grid")			, "");

	pParameters->Add_Grid_System(
		NULL, "SYSTEM"		, _TL("System")		, ""
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_GRID"		, _TL("Choose Grid")			, "");

	pParameters->Add_Grid(
		NULL, "GRID"		, _TL("Grid")		, "", PARAMETER_INPUT	, false
	);


	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GET_SHAPES"		, _TL("Choose Shapes")		, "");

	pParameters->Add_Shapes(
		NULL, "SHAPES"		,_TL("Shapes")		, "", PARAMETER_OUTPUT	, SHAPE_TYPE_Point
	);
}

//---------------------------------------------------------
CPROJ4_Grid::~CPROJ4_Grid(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::On_Execute_Conversion(void)
{
	int		Interpol;
	CGrid	*pSource, *pGrid;
	CShapes	*pShapes;

	//-----------------------------------------------------
	pSource		= Parameters("SOURCE")->asGrid();

	pGrid		= NULL;
	pShapes		= NULL;

	Interpol	= Parameters("INTERPOLATION")->asInt();

	//-----------------------------------------------------
	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// create new user defined grid...
		pGrid	= Get_Target_Userdef(pSource, Interpol == 0);
		break;

	case 1:	// create new with chosen grid size and fitted extent...
		if( Dlg_Extra_Parameters("GET_AUTOFIT") )
		{
			pGrid	= Get_Target_Autofit(
						pSource,
						Get_Extra_Parameters("GET_AUTOFIT")->Get_Parameter("GRIDSIZE")		->asDouble(),
						Get_Extra_Parameters("GET_AUTOFIT")->Get_Parameter("AUTOEXTMODE")	->asInt(),
						Interpol == 0
					);
		}
		break;

	case 2:	// select grid system...
		if( Dlg_Extra_Parameters("GET_SYSTEM") )
		{
			pGrid	= SG_Create_Grid(
						*Get_Extra_Parameters("GET_SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System()
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

		Parameters("OUT_SHAPES")	->Set_Value(pShapes);
	}


	//-----------------------------------------------------
	if( pGrid )
	{
		Set_Grid	(pSource, pGrid, Interpol);

		Parameters("OUT_GRID")		->Set_Value(pGrid);
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
int CPROJ4_Grid::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
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
inline void CPROJ4_Grid::Get_MinMax(double &xMin, double &xMax, double &yMin, double &yMax, TSG_Point Point)
{
	if( Get_Converted(Point) )
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

//---------------------------------------------------------
CGrid * CPROJ4_Grid::Get_Target_Userdef(CGrid *pSource, bool bNearest)
{
	int			x, y;
	double		xMin, yMin, xMax, yMax, size;
	TSG_Point	Pt_Source;
	CGrid		*pTarget;
	CParameters	*pParameters;

	pTarget	= NULL;

	if( pSource )
	{
		//-------------------------------------------------
		xMin	= yMin	= 1.0;
		xMax	= yMax	= 0.0;

		for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY(); y++, Pt_Source.y+=pSource->Get_Cellsize())
		{
			Pt_Source.x	= pSource->Get_XMin();
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

			Pt_Source.x	= pSource->Get_XMax();
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
		}

		for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
		{
			Pt_Source.y	= pSource->Get_YMin();
			Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

			Pt_Source.y	= pSource->Get_YMax();
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
					bNearest ? pSource->Get_Type() : GRID_TYPE_Float,
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
CGrid * CPROJ4_Grid::Get_Target_Autofit(CGrid *pSource, double Grid_Size, int AutoExtMode, bool bNearest)
{
	int			x, y;
	double		xMin, yMin, xMax, yMax;
	TSG_Point	Pt_Source;
	CGrid		*pTarget;

	pTarget	= NULL;

	if( pSource )
	{
		xMin	= yMin	= 1.0;
		xMax	= yMax	= 0.0;

		//---------------------------------------------
		switch( AutoExtMode )
		{
		case 0:	default:
			for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY(); y++, Pt_Source.y+=pSource->Get_Cellsize())
			{
				Pt_Source.x	= pSource->Get_XMin();
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

				Pt_Source.x	= pSource->Get_XMax();
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
			}

			for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
			{
				Pt_Source.y	= pSource->Get_YMin();
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);

				Pt_Source.y	= pSource->Get_YMax();
				Get_MinMax(xMin, xMax, yMin, yMax, Pt_Source);
			}

			break;

		//---------------------------------------------
		case 1:
			for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=pSource->Get_Cellsize())
			{
				for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
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
				bNearest ? pSource->Get_Type() : GRID_TYPE_Float,
				1 + (int)((xMax - xMin) / Grid_Size),
				1 + (int)((yMax - yMin) / Grid_Size),
				Grid_Size,
				xMin, yMin
			);
		}
	}

	return( pTarget );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Grid(CGrid *pSource, CGrid *pTarget, int Interpol)
{
	int			x, y;
	double		z;
	TSG_Point	Pt_Source, Pt_Target;
	CGrid		*pX, *pY;

	if( pSource && pTarget && Set_Transformation_Inverse() )
	{
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_ZFactor(pSource->Get_ZFactor());
		pTarget->Set_Name	(pSource->Get_Name());
		pTarget->Set_Unit	(pSource->Get_Unit());

		pTarget->Assign_NoData();

		if( Parameters("CREATE_XY")->asBool() )
		{
			pX	= SG_Create_Grid(pTarget->Get_System(), GRID_TYPE_Float);
			pX->Assign_NoData();
			pX->Set_Name(_TL("X-Coordinate"));
			Parameters("OUT_X")->Set_Value(pX);

			pY	= SG_Create_Grid(pTarget->Get_System(), GRID_TYPE_Float);
			pY->Assign_NoData();
			pY->Set_Name(_TL("Y-Coordinate"));
			Parameters("OUT_Y")->Set_Value(pY);
		}
		else
		{
			pX	= pY	= NULL;
		}

		//-------------------------------------------------
		for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
		{
			for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( Get_Converted(Pt_Source) )
				{
					if( pSource->Get_Value(Pt_Source, z, Interpol) )
					{
						pTarget->Set_Value(x, y, z);
					}

					if( pX && pY )
					{
						pX->Set_Value(x, y, Pt_Source.x);
						pY->Set_Value(x, y, Pt_Source.y);
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Shapes(CGrid *pSource, CShapes *pTarget)
{
	int			x, y;
	TSG_Point	Pt_Source, Pt_Target;
	CShape		*pShape;

	if( pSource && pTarget )
	{
		pTarget->Create(SHAPE_TYPE_Point, pSource->Get_Name());
		pTarget->Get_Table().Add_Field("Z", TABLE_FIELDTYPE_Double);

		for(y=0, Pt_Source.y=pSource->Get_YMin(); y<pSource->Get_NY() && Set_Progress(y, pSource->Get_NY()); y++, Pt_Source.y+=pSource->Get_Cellsize())
		{
			for(x=0, Pt_Source.x=pSource->Get_XMin(); x<pSource->Get_NX(); x++, Pt_Source.x+=pSource->Get_Cellsize())
			{
				if( !pSource->is_NoData(x, y) )
				{
					Pt_Target	= Pt_Source;

					if( Get_Converted(Pt_Target) )
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
