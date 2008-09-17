
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
#include "PROJ4_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPROJ4_Grid::CPROJ4_Grid(int Interface, bool bInputList)
	: CPROJ4_Base(Interface, bInputList)
{
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Set_Name		(CSG_String::Format(SG_T("%s (%s, %s)"),
		_TL("Proj.4"),
		Interface == PROJ4_INTERFACE_DIALOG ? _TL("Dialog") : _TL("Command Line Arguments"),
		m_bInputList ? _TL("List of Grids") : _TL("Grid")
	));

	Set_Author		(SG_T("O. Conrad (c) 2004-8"));

	Set_Description	(_TW(
		"Coordinate Transformation for Grids.\n"
		"Based on the PROJ.4 Cartographic Projections library originally written by Gerald Evenden "
		"and later continued by the United States Department of the Interior, Geological Survey (USGS).\n"
		"<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4 Homepage</a>\n"
	));


	//-----------------------------------------------------
	if( m_bInputList )
	{
		Parameters.Add_Grid_List(
			Parameters("SOURCE_NODE"),
			"SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT, false
		);

		Parameters.Add_Grid_List(
			NULL,
			"TARGET"		, _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);
	}
	else
	{
		Parameters.Add_Grid(
			Parameters("SOURCE_NODE"),
			"SOURCE"		, _TL("Source"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Grid_Output(
			NULL,
			"TARGET"		, _TL("Target"),
			_TL("")
		);

		Parameters.Add_Shapes_Output(
			NULL,
			"SHAPES"		, _TL("Shapes"),
			_TL("")
		);
	}


	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "OUT_X"		, _TL("X Coordinates"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUT_Y"		, _TL("Y Coordinates"),
		_TL("")
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		Parameters("TARGET_NODE"),
		"CREATE_XY"		, _TL("Create X/Y Grids"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Choice(
		Parameters("TARGET_NODE"),
		"TARGET_TYPE"	, _TL("Target"),
		_TL(""),
		CSG_String::Format(m_bInputList ? SG_T("%s|%s|%s|") : SG_T("%s|%s|%s|%s|%s|"),
			_TL("user defined"),
			_TL("automatic fit"),
			_TL("grid system"),
			_TL("grid"),
			_TL("shapes")
		), 0
	);

	Parameters.Add_Choice(
		Parameters("TARGET_NODE")	, "INTERPOLATION"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neigbhor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);


	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_AUTOFIT"	, _TL("Automatic fit")	, _TL(""));

	pParameters->Add_Value(
		NULL, "GRIDSIZE"	, _TL("Grid Size")	, _TL(""), PARAMETER_TYPE_Double, 10000.0, 0.0, true
	);

	pParameters->Add_Choice(
		NULL, "AUTOEXTMODE"	, _TL("Fit Size")	, _TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Extent only (fast)"),
			_TL("Check each point|")
		), 0
	);


	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_USER"		, _TL("User defined")		, _TL(""));

	pParameters->Add_Value(
		NULL, "XMIN"		, _TL("Left")		, _TL(""), PARAMETER_TYPE_Double
	);
	pParameters->Add_Value(
		NULL, "XMAX"		, _TL("Right")		, _TL(""), PARAMETER_TYPE_Double
	);
	pParameters->Add_Value(
		NULL, "YMIN"		, _TL("Bottom")		, _TL(""), PARAMETER_TYPE_Double
	);
	pParameters->Add_Value(
		NULL, "YMAX"		, _TL("Top")		, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "SIZE"		, _TL("Grid Size")	, _TL(""), PARAMETER_TYPE_Double, 10000.0, 0.0, true
	);

	pParameters->Add_Info_Value(
		NULL, "NX"			, _TL("Columns")	, _TL(""), PARAMETER_TYPE_Int
	);
	pParameters->Add_Info_Value(
		NULL, "NY"			, _TL("Rows")		, _TL(""), PARAMETER_TYPE_Int
	);


	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_SYSTEM"	, _TL("Choose Grid Project"), _TL(""));

	pParameters->Add_Grid_System(
		NULL, "SYSTEM"		, _TL("System")		, _TL("")
	);


	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_GRID"		, _TL("Choose Grid")		, _TL(""));

	pParameters->Add_Grid(
		NULL, "GRID"		, _TL("Grid")		, _TL(""), PARAMETER_INPUT	, false
	);


	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_SHAPES"	, _TL("Choose Shapes")		, _TL(""));

	pParameters->Add_Shapes(
		NULL, "SHAPES"		,_TL("Shapes")		, _TL(""), PARAMETER_OUTPUT	, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::On_Execute_Conversion(void)
{
	CSG_Grid_System	System;

	m_Interpolation	= Parameters("INTERPOLATION")->asInt();

	//-----------------------------------------------------
	if( m_bInputList )
	{
		CSG_Parameter_Grid_List	*pSources	= Parameters("SOURCE")->asGridList();
		CSG_Parameter_Grid_List	*pTargets	= Parameters("TARGET")->asGridList();

		if( pSources->Get_Count() > 0 && Get_Target_System(pSources->asGrid(0)->Get_System(), System) )
		{
			return( Set_Grids(System, pSources, pTargets) );
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Grid	*pSource, *pTarget;

		pSource		= Parameters("SOURCE")->asGrid();

		switch( Parameters("TARGET_TYPE")->asInt() )
		{
		default:	// create new grid...
			if( Get_Target_System(pSource->Get_System(), System) )
			{
				Parameters("TARGET")->Set_Value(pTarget	= SG_Create_Grid(System, m_Interpolation == 0 ? pSource->Get_Type() : GRID_TYPE_Float));

				return( Set_Grid(pSource, pTarget) );
			}
			break;

		case 3:		// select existing grid...
			if( Dlg_Parameters("GET_GRID") )
			{
				Parameters("TARGET")->Set_Value(pTarget	= Get_Parameters("GET_GRID")->Get_Parameter("GRID")->asGrid());

				return( Set_Grid(pSource, pTarget) );
			}
			break;

		case 4:		// create grid points as shapes...
			if( Dlg_Parameters("GET_SHAPES") )
			{
				CSG_Shapes	*pShapes;

				Parameters("SHAPES")->Set_Value(pShapes	= Get_Parameters("GET_SHAPES")->Get_Parameter("SHAPES")->asShapes());

				return( Set_Shapes(pSource, pShapes) );
			}
			break;
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Grids(const CSG_Grid_System &System, CSG_Parameter_Grid_List *pSources, CSG_Parameter_Grid_List *pTargets)
{
	if( pSources && pSources->Get_Count() > 0 && pTargets && System.is_Valid() && Set_Inverse() )
	{
		int			x, y, i;
		double		z;
		TSG_Point	Pt_Source, Pt_Target;
		CSG_Grid	*pX, *pY;

		Init_XY(System, &pX, &pY);

		pTargets->Del_Items();

		for(i=0; i<pSources->Get_Count(); i++)
		{
			pTargets->Add_Item(SG_Create_Grid(System, pSources->asGrid(i)->Get_Type()));

			Init_Target(pSources->asGrid(i), pTargets->asGrid(i));
		}

		//-------------------------------------------------
		for(y=0, Pt_Target.y=System.Get_YMin(); y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++, Pt_Target.y+=System.Get_Cellsize())
		{
			for(x=0, Pt_Target.x=System.Get_XMin(); x<System.Get_NX(); x++, Pt_Target.x+=System.Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( Get_Converted(Pt_Source) )
				{
					if( pX )	pX->Set_Value(x, y, Pt_Source.x);
					if( pY )	pY->Set_Value(x, y, Pt_Source.y);

					for(i=0; i<pSources->Get_Count(); i++)
					{
						if( pSources->asGrid(i)->Get_Value(Pt_Source, z, m_Interpolation) )
						{
							pTargets->asGrid(i)->Set_Value(x, y, z);
						}
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Set_Grid(CSG_Grid *pSource, CSG_Grid *pTarget)
{
	if( pSource && pTarget && Set_Inverse() )
	{
		int			x, y;
		double		z;
		TSG_Point	Pt_Source, Pt_Target;
		CSG_Grid	*pX, *pY;

		Init_XY(pTarget->Get_System(), &pX, &pY);

		Init_Target(pSource, pTarget);

		//-------------------------------------------------
		for(y=0, Pt_Target.y=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, Pt_Target.y+=pTarget->Get_Cellsize())
		{
			for(x=0, Pt_Target.x=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, Pt_Target.x+=pTarget->Get_Cellsize())
			{
				Pt_Source	= Pt_Target;

				if( Get_Converted(Pt_Source) )
				{
					if( pX )	pX->Set_Value(x, y, Pt_Source.x);
					if( pY )	pY->Set_Value(x, y, Pt_Source.y);

					if( pSource->Get_Value(Pt_Source, z, m_Interpolation) )
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
bool CPROJ4_Grid::Set_Shapes(CSG_Grid *pSource, CSG_Shapes *pTarget)
{
	int			x, y;
	TSG_Point	Pt_Source, Pt_Target;
	CSG_Shape	*pShape;

	if( pSource && pTarget )
	{
		pTarget->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pSource->Get_Name(), Get_Proj_Name().c_str()));
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
						pShape	= pTarget->Add_Shape();
						pShape->Add_Point(Pt_Target);
						pShape->Set_Value(0, pSource->asDouble(x, y));
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
bool CPROJ4_Grid::Init_XY(const CSG_Grid_System &System, CSG_Grid **ppX, CSG_Grid **ppY)
{
	if( Parameters("CREATE_XY")->asBool() )
	{
		Parameters("OUT_X")->Set_Value(*ppX	= SG_Create_Grid(System, GRID_TYPE_Float));
		(*ppX)->Assign_NoData();
		(*ppX)->Set_Name(_TL("X-Coordinate"));

		Parameters("OUT_Y")->Set_Value(*ppY	= SG_Create_Grid(System, GRID_TYPE_Float));
		(*ppY)->Assign_NoData();
		(*ppY)->Set_Name(_TL("Y-Coordinate"));

		return( true );
	}

	*ppX	= *ppY	= NULL;

	return( false );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Init_Target(CSG_Grid *pSource, CSG_Grid *pTarget)
{
	if( pSource && pTarget )
	{
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_hiValue());
		pTarget->Set_ZFactor(pSource->Get_ZFactor());
		pTarget->Set_Name	(CSG_String::Format(SG_T("%s [%s]"), pSource->Get_Name(), Get_Proj_Name().c_str()));
		pTarget->Set_Unit	(pSource->Get_Unit());
		pTarget->Assign_NoData();

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
inline bool CPROJ4_Grid::Get_MinMax(TSG_Rect &r, TSG_Point p)
{
	if( Get_Converted(p) )
	{
		if( r.xMin > r.xMax )
		{
			r.xMin	= r.xMax	= p.x;
		}
		else if( r.xMin > p.x )
		{
			r.xMin	= p.x;
		}
		else if( r.xMax < p.x )
		{
			r.xMax	= p.x;
		}

		if( r.yMin > r.yMax )
		{
			r.yMin	= r.yMax	= p.y;
		}
		else if( r.yMin > p.y )
		{
			r.yMin	= p.y;
		}
		else if( r.yMax < p.y )
		{
			r.yMax	= p.y;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Get_Target_System(const CSG_Grid_System &Source, CSG_Grid_System &Target)
{
	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// create new user defined grid...
		return( Get_Target_Userdef(Source, Target) );

	case 1:	// create new with chosen cell size and fitted extent...
		return( Get_Target_Autofit(Source, Target) );

	case 2:	// select grid system...
		if( Dlg_Parameters("GET_SYSTEM") )
		{
			Target	= *Get_Parameters("GET_SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System();

			return( true );
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Get_Target_Userdef(const CSG_Grid_System &Source, CSG_Grid_System &Target)
{
	int			x, y;
	TSG_Point	p;
	TSG_Rect	r;

	r.xMin	= r.yMin	= 1.0;	r.xMax	= r.yMax	= 0.0;

	//-------------------------------------------------
	for(y=0, p.y=Source.Get_YMin(); y<Source.Get_NY(); y++, p.y+=Source.Get_Cellsize())
	{
		p.x	= Source.Get_XMin();	Get_MinMax(r, p);
		p.x	= Source.Get_XMax();	Get_MinMax(r, p);
	}

	for(x=0, p.x=Source.Get_XMin(); x<Source.Get_NX(); x++, p.x+=Source.Get_Cellsize())
	{
		p.y	= Source.Get_YMin();	Get_MinMax(r, p);
		p.y	= Source.Get_YMax();	Get_MinMax(r, p);
	}

	//-------------------------------------------------
	if( r.xMin < r.xMax && r.yMin < r.yMax )
	{
		CSG_Parameters	*pParameters	= Get_Parameters("GET_USER");
		double			Cellsize		= (r.xMax - r.xMin) / 100.0;

		pParameters->Get_Parameter("XMIN")	->Set_Value(r.xMin);
		pParameters->Get_Parameter("XMAX")	->Set_Value(r.xMax);
		pParameters->Get_Parameter("YMIN")	->Set_Value(r.yMin);
		pParameters->Get_Parameter("YMAX")	->Set_Value(r.yMax);
		pParameters->Get_Parameter("SIZE")	->Set_Value(Cellsize);
		pParameters->Get_Parameter("NX")	->Set_Value(1 + (int)((r.xMax - r.xMin) / Cellsize));
		pParameters->Get_Parameter("NY")	->Set_Value(1 + (int)((r.yMax - r.yMin) / Cellsize));

		if( Dlg_Parameters("GET_USER") )
		{
			Target.Assign(
				pParameters->Get_Parameter("SIZE")	->asDouble(),
				pParameters->Get_Parameter("XMIN")	->asDouble(),
				pParameters->Get_Parameter("YMIN")	->asDouble(),
				pParameters->Get_Parameter("NX")	->asInt(),
				pParameters->Get_Parameter("NY")	->asInt()
			);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CPROJ4_Grid::Get_Target_Autofit(const CSG_Grid_System &Source, CSG_Grid_System &Target)
{
	int			x, y;
	TSG_Point	p;
	TSG_Rect	r;

	double	Cellsize	= Get_Parameters("GET_AUTOFIT")->Get_Parameter("GRIDSIZE")		->asDouble();
	int		AutoExtMode	= Get_Parameters("GET_AUTOFIT")->Get_Parameter("AUTOEXTMODE")	->asInt();

	r.xMin	= r.yMin	= 1.0;	r.xMax	= r.yMax	= 0.0;

	//---------------------------------------------
	switch( AutoExtMode )
	{
	case 0:	default:
		for(y=0, p.y=Source.Get_YMin(); y<Source.Get_NY(); y++, p.y+=Source.Get_Cellsize())
		{
			p.x	= Source.Get_XMin();	Get_MinMax(r, p);
			p.x	= Source.Get_XMax();	Get_MinMax(r, p);
		}

		for(x=0, p.x=Source.Get_XMin(); x<Source.Get_NX(); x++, p.x+=Source.Get_Cellsize())
		{
			p.y	= Source.Get_YMin();	Get_MinMax(r, p);
			p.y	= Source.Get_YMax();	Get_MinMax(r, p);
		}

		break;

	//---------------------------------------------
	case 1:
		for(y=0, p.y=Source.Get_YMin(); y<Source.Get_NY() && Set_Progress(y, Source.Get_NY()); y++, p.y+=Source.Get_Cellsize())
		{
			for(x=0, p.x=Source.Get_XMin(); x<Source.Get_NX(); x++, p.x+=Source.Get_Cellsize())
			{
				Get_MinMax(r, p);
			}
		}

		break;
	}

	//---------------------------------------------
	if( is_Progress() && r.xMin < r.xMax && r.yMin < r.yMax )
	{
		Target.Assign(
			Cellsize,
			r.xMin, r.yMin,
			1 + (int)((r.xMax - r.xMin) / Cellsize),
			1 + (int)((r.yMax - r.yMin) / Cellsize)
		);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>

//---------------------------------------------------------
int CPROJ4_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	double	xMin, xMax, yMin, yMax, Cellsize;

	if( !SG_STR_CMP(pParameters->Get_Identifier(), SG_T("GET_USER")) )
	{
		xMin		= pParameters->Get_Parameter("XMIN")->asDouble();
		xMax		= pParameters->Get_Parameter("XMAX")->asDouble();
		yMin		= pParameters->Get_Parameter("YMIN")->asDouble();
		yMax		= pParameters->Get_Parameter("YMAX")->asDouble();
		Cellsize	= pParameters->Get_Parameter("SIZE")->asDouble();

		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SIZE")) )
		{
			pParameters->Get_Parameter("XMAX")->Set_Value((xMax = xMin + ((int)((xMax - xMin) / Cellsize)) * Cellsize));
			pParameters->Get_Parameter("YMAX")->Set_Value((yMax = yMin + ((int)((yMax - yMin) / Cellsize)) * Cellsize));
		}
		else 
		{
			if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("XMIN")) )
			{
				if( xMin >= xMax )
				{
					xMin	= xMax - pParameters->Get_Parameter("NX")->asInt() * Cellsize;
					pParameter->Set_Value(xMin);
				}

				pParameters->Get_Parameter("XMAX")->Set_Value(xMin + ((int)((xMax - xMin) / Cellsize)) * Cellsize);
			}
			else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("XMAX")) )
			{
				if( xMin >= xMax )
				{
					xMax	= xMin + pParameters->Get_Parameter("NX")->asInt() * Cellsize;
					pParameter->Set_Value(xMax);
				}

				pParameters->Get_Parameter("XMIN")->Set_Value(xMax - ((int)((xMax - xMin) / Cellsize)) * Cellsize);
			}
			else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("YMIN")) )
			{
				if( yMin >= yMax )
				{
					yMin	= yMax - pParameters->Get_Parameter("NY")->asInt() * Cellsize;
					pParameter->Set_Value(yMin);
				}

				pParameters->Get_Parameter("YMAX")->Set_Value(yMin + ((int)((yMax - yMin) / Cellsize)) * Cellsize);
			}
			else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("YMAX")) )
			{
				if( yMin >= yMax )
				{
					yMax	= yMin + pParameters->Get_Parameter("NY")->asInt() * Cellsize;
					pParameter->Set_Value(yMax);
				}

				pParameters->Get_Parameter("YMIN")->Set_Value(yMax - ((int)((yMax - yMin) / Cellsize)) * Cellsize);
			}
		}

		pParameters->Get_Parameter("NX")->Set_Value(1 + (int)((xMax - xMin) / Cellsize));
		pParameters->Get_Parameter("NY")->Set_Value(1 + (int)((yMax - yMin) / Cellsize));

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
