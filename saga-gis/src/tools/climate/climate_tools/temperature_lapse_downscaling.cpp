
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             temperature_lapse_downscaling.cpp         //
//                                                       //
//                 Copyright (C) 2021 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "temperature_lapse_downscaling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTemperature_Lapse_Downscaling::CTemperature_Lapse_Downscaling(void)
{
	Set_Name		(_TL("Lapse Rate Based Temperature Downscaling"));

	Set_Author		("O.Conrad (c) 2021");

	Set_Description	(_TW(
		"The Lapse Rate Based Temperature Downscaling is quite simple, "
		"but might perform well for mountainous regions, where the "
		"altitudinal gradient is the main driver for local temperature "
		"variation. First, a given lapse rate is used to estimate sea "
		"level temperatures from elevation and temperature data at a "
		"coarse resolution. Second, the same lapse rate is used to "
		"estimate the terrain surface temperature using higher resoluted "
		"elevation data and the spline interpolated sea level temperatures "
		"from the previous step. "
		"The lapse rates can be defined as one constant value valid for the "
		"whole area of interest, or as varying value as defined by an "
		"additional input grid. Alternatively a constant lapse rate can be "
		"estimated from the coarse resolution input with a regression analysis. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("", "LORES_GRID_SYSTEM"    , _TL("Coarse Resolution"    ), _TL(""));
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_DEM"  , _TL("Elevation"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_T"    , _TL("Temperature"          ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_LAPSE", _TL("Lapse Rate"           ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_SLT"  , _TL("Sea Level Temperature"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Grid_System("", "HIRES_GRID_SYSTEM"    , _TL("High Resolution"      ), _TL(""));
	Parameters.Add_Grid("HIRES_GRID_SYSTEM", "HIRES_DEM"  , _TL("Elevation"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("HIRES_GRID_SYSTEM", "HIRES_T"    , _TL("Temperature"          ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("",
		"LAPSE_METHOD"	, _TL("Lapse Rate"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("constant lapse rate"),
			_TL("constant lapse rate from regression"),
			_TL("varying lapse rate from grid")
		), 1
	);

	Parameters.Add_Table("",
		"REGRS_SUMMARY"	, _TL("Regression Summary"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("LAPSE_METHOD",
		"REGRS_LAPSE"	, _TL("Regression"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("elevation"),
			_TL("elevation and position"),
			_TL("elevation and position (2nd order polynom)")
		), 2
	);

	Parameters.Add_Bool("LAPSE_METHOD",
		"LIMIT_LAPSE"	, _TL("Limit Minimum Lapse Rate"),
		_TL("If set, lapse rates from regression are limited to a minimum as specified by the constant lapse rate parameter."),
		false
	);

	Parameters.Add_Double("LAPSE_METHOD",
		"CONST_LAPSE"	, _TL("Constant Lapse Rate"),
		_TL("Constant lapse rate in degree of temperature per 100 meter."),
		0.6
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTemperature_Lapse_Downscaling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("LAPSE_METHOD") )
	{
		int Method = pParameter->asInt(); bool bLimit = (*pParameters)("LIMIT_LAPSE")->asBool();

		pParameters->Set_Enabled("CONST_LAPSE"  , Method == 0 || (Method == 1 && bLimit));
		pParameters->Set_Enabled("REGRS_SUMMARY", Method == 1);
		pParameters->Set_Enabled("REGRS_LAPSE"  , Method == 1);
		pParameters->Set_Enabled("LIMIT_LAPSE"  , Method == 1);
		pParameters->Set_Enabled("LORES_LAPSE"  , Method == 2);
	}

	if( pParameter->Cmp_Identifier("LIMIT_LAPSE") )
	{
		int Method = (*pParameters)("LAPSE_METHOD")->asInt(); bool bLimit = pParameter->asBool();

		pParameters->Set_Enabled("CONST_LAPSE", Method == 0 || (Method == 1 && bLimit));
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTemperature_Lapse_Downscaling::On_Execute(void)
{
	CSG_Grid_System	LoRes(*Parameters("LORES_GRID_SYSTEM")->asGrid_System());

	CSG_Grid *pLoRes_Z   = Parameters("LORES_DEM")->asGrid();
	CSG_Grid *pLoRes_T   = Parameters("LORES_T"  )->asGrid();
	CSG_Grid *pLoRes_SLT = Parameters("LORES_SLT")->asGrid(), LoRes_SLT;

	if( !pLoRes_SLT )
	{
		pLoRes_SLT = &LoRes_SLT; pLoRes_SLT->Create(LoRes);
	}

	//-----------------------------------------------------
	double dT_const = 0.006; CSG_Grid *pLoRes_dT = NULL;

	switch( Parameters("LAPSE_METHOD")->asInt() )
	{
	default: // constant lapse rate
		dT_const	= Parameters("CONST_LAPSE")->asDouble() / 100.;
		break;

	case  1: // constant lapse rate from regression
	//	Process_Set_Text("%s...", _TL("Regression"));

		if( !Get_Regression(pLoRes_T, pLoRes_Z, dT_const) )
		{
			return( false );
		}
		break;

	case  2: // varying lapse rate from grid
		pLoRes_dT	= Parameters("LORES_LAPSE")->asGrid();
		break;
	}

	//-----------------------------------------------------
	for(int y=0; y<LoRes.Get_NY() && Set_Progress(y, LoRes.Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<LoRes.Get_NX(); x++)
		{
			if( !pLoRes_Z->is_NoData(x, y) && !pLoRes_T->is_NoData(x, y) && !(pLoRes_dT && pLoRes_dT->is_NoData(x, y)) )
			{
				double	dT	= pLoRes_dT ? pLoRes_dT->asDouble(x, y) : dT_const;

				pLoRes_SLT->Set_Value(x, y, pLoRes_T->asDouble(x, y) + dT * pLoRes_Z->asDouble(x, y));
			}
			else
			{
				pLoRes_SLT->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Grid_System	HiRes(*Parameters("HIRES_GRID_SYSTEM")->asGrid_System());

	CSG_Grid *pHiRes_DEM = Parameters("HIRES_DEM")->asGrid();
	CSG_Grid *pHiRes_T   = Parameters("HIRES_T"  )->asGrid();

	pHiRes_T->Fmt_Name("%s [%s]", pLoRes_T->Get_Name(), _TL("downscaled"));

	for(int y=0; y<HiRes.Get_NY() && Set_Progress(y, HiRes.Get_NY()); y++)
	{
		double	py	= HiRes.Get_YMin() + y * HiRes.Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<HiRes.Get_NX(); x++)
		{
			double	px	= HiRes.Get_XMin() + x * HiRes.Get_Cellsize(), T, dT;

			if( !pHiRes_DEM->is_NoData(x, y) && pLoRes_SLT->Get_Value(px, py, T) && (!pLoRes_dT || pLoRes_dT->Get_Value(px, py, dT)) )
			{
				if( pLoRes_dT )
				{
					dT	/= 100.; // degree per 100m -> degree per meter
				}
				else
				{
					dT	= dT_const;
				}

				pHiRes_T->Set_Value(x, y, T - dT * pHiRes_DEM->asDouble(x, y));
			}
			else
			{
				pHiRes_T->Set_NoData(x, y);
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
bool CTemperature_Lapse_Downscaling::Get_Regression(CSG_Grid *pT, CSG_Grid *pZ, double &dT)
{
	int	Method	= Parameters("REGRS_LAPSE")->asInt();

	CSG_Vector	Sample;

	switch( Method )
	{
	default: Sample.Create(2); break; // T = a + b*Z
	case  1: Sample.Create(4); break; // T = a + b*Z + c*X + d*Y
	case  2: Sample.Create(6); break; // T = a + b*Z + c*X + d*Y + e*X*X + f*Y*Y
	}

	const SG_Char *Vars[] = { SG_T("T"), SG_T("Z"), SG_T("X"), SG_T("Y"), SG_T("X^2"), SG_T("Y^2") };

	CSG_Strings	Names(Sample.Get_N(), Vars);

	//-----------------------------------------------------
	CSG_Matrix	Samples;

	CSG_Grid_System	System(pT->Get_System());

	for(int y=0; y<System.Get_NY(); y++)
	{
		for(int x=0; x<System.Get_NX(); x++)
		{
			if( !pT->is_NoData(x, y) && !pZ->is_NoData(x, y) )
			{
				switch( Method )
				{
				case  2: // T = a + b*Z + c*X + d*Y + e*X*X + f*Y*Y
					Sample[5]	= y*y;
					Sample[4]	= x*x;

				case  1: // T = a + b*Z + c*X + d*Y
					Sample[3]	= y;
					Sample[2]	= x;

				default: // T = a + b*Z
					Sample[1]	= pZ->asDouble(x, y);
					Sample[0]	= pT->asDouble(x, y);
				}

				Samples.Add_Row(Sample);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Regression_Multiple	Regression;

	if( !Regression.Get_Model(Samples, &Names) )
	{
		Error_Set(_TL("Regression failed"));

		return( false );
	}

	Message_Add(Regression.Get_Info(), false);

	dT	= -Regression.Get_RCoeff(0);

	if( Parameters("LIMIT_LAPSE")->asBool() )
	{
		double dT_min = Parameters("CONST_LAPSE")->asDouble() / 100.;

		if( dT < dT_min )
		{
			dT	= dT_min;
		}
	}

	Message_Fmt("\n\n%s: %g", _TL("Constant lapse rate from regression"), dT * 100.);

	CSG_Table *pSummary = Parameters("REGRS_SUMMARY")->asTable();

	if( pSummary )
	{
		pSummary->Destroy();
		pSummary->Fmt_Name("%s (%s: %s)", _TL("Lapse Rate"), _TL("Regression"), pT->Get_Name());
		pSummary->Add_Field(_TL("Parameter"), SG_DATATYPE_String);
		pSummary->Add_Field(_TL("Value"    ), SG_DATATYPE_Double);

		#define Add_Entry(name, value) { CSG_Table_Record &r = *pSummary->Add_Record(); r.Set_Value(0, name); r.Set_Value(1, value); }

		Add_Entry(_TL("Lapse Rate"), dT * 100.          );
		Add_Entry(_TL("R-squared" ), Regression.Get_R2());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTemperature_Lapse_Interpolation::CTemperature_Lapse_Interpolation(void)
{
	Set_Name		(_TL("Lapse Rate Based Temperature Interpolation"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"The Lapse Rate Based Temperature Interpolation is quite simple, "
		"but might perform well for mountainous regions, where the "
		"altitudinal gradient is the main driver for local temperature "
		"variation. First, a given lapse rate is used to estimate sea "
		"level temperatures from elevation and temperature data at a "
		"coarse resolution. Second, the same lapse rate is used to "
		"estimate the terrain surface temperature using higher resoluted "
		"elevation data and the spline interpolated sea level temperatures "
		"from the previous step. "
		"The lapse rates can be defined as one constant value valid for the "
		"whole area of interest, or as varying value as defined by an "
		"additional input grid. Alternatively a constant lapse rate can be "
		"estimated from the coarse resolution input with a regression analysis. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "POINTS"     , _TL("Observations"         ), _TL(""), PARAMETER_INPUT, SHAPE_TYPE_Point);
	Parameters.Add_Grid  ("", "DEM"        , _TL("Elevation"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "TEMPERATURE", _TL("Temperature"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "SLT"        , _TL("Sea Level Temperature"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Table_Field("POINTS", "FIELD_T", _TL("Temperature"), _TL(""), false);
	Parameters.Add_Table_Field("POINTS", "FIELD_Z", _TL("Elevation"  ), _TL(""),  true);

	Parameters.Add_Choice("",
		"INTERPOLATION" , _TL("Interpolation"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Multilevel B-Spline Interpolation"),
			_TL("Inverse Distance Weighted")
		), 0
	);

	Parameters.Add_Double("INTERPOLATION",
		"IDW_POWER"     , _TL("Power"),
		_TL(""),
		2.
	);

	Parameters.Add_Choice("",
		"LAPSE_METHOD"  , _TL("Lapse Rate"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("user defined lapse rate"),
			_TL("lapse rate from regression")
		), 1
	);

	Parameters.Add_Table("",
		"REGRS_SUMMARY" , _TL("Regression Summary"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("LAPSE_METHOD",
		"REGRS_LAPSE"   , _TL("Regression"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("elevation"),
			_TL("elevation and position"),
			_TL("elevation and position (2nd order polynom)")
		), 1
	);

	Parameters.Add_Bool("LAPSE_METHOD",
		"LIMIT_LAPSE"   , _TL("Limit Minimum Lapse Rate"),
		_TL("If set, lapse rates from regression are limited to a minimum as specified by the constant lapse rate parameter."),
		false
	);

	Parameters.Add_Double("LAPSE_METHOD",
		"CONST_LAPSE"   , _TL("Constant Lapse Rate"),
		_TL("Constant lapse rate in degree of temperature per 100 meter."),
		0.6
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTemperature_Lapse_Interpolation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("INTERPOLATION") )
	{
		pParameters->Set_Enabled("IDW_POWER"    , pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("LAPSE_METHOD") )
	{
		int Method = pParameter->asInt(); bool bLimit = (*pParameters)("LIMIT_LAPSE")->asBool();

		pParameters->Set_Enabled("CONST_LAPSE"  , Method == 0 || (Method == 1 && bLimit));
		pParameters->Set_Enabled("REGRS_SUMMARY", Method == 1);
		pParameters->Set_Enabled("REGRS_LAPSE"  , Method == 1);
		pParameters->Set_Enabled("LIMIT_LAPSE"  , Method == 1);
	}

	if( pParameter->Cmp_Identifier("LIMIT_LAPSE") )
	{
		int Method = (*pParameters)("LAPSE_METHOD")->asInt(); bool bLimit = pParameter->asBool();

		pParameters->Set_Enabled("CONST_LAPSE"  , Method == 0 || (Method == 1 && bLimit));
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTemperature_Lapse_Interpolation::On_Execute(void)
{
	CSG_Shapes Points;

	if( !Get_Points(Points) )
	{
		return( false );
	}

	//-----------------------------------------------------
	// define the lapse rate

	double dT_lapse = Parameters("CONST_LAPSE")->asDouble() / 100.;

	switch( Parameters("LAPSE_METHOD")->asInt() )
	{
	default: // user defined
		break;

	case  1: // constant lapse rate from regression
		if( !Get_Regression(Points, dT_lapse) )
		{
			return( false );
		}
		break;
	}

	//-----------------------------------------------------
	// from surface temperatures to sea level temperatures

	for(sLong i=0; i<Points.Get_Count(); i++)
	{
		Points[i].Set_Value(0, Points[i].asDouble(0) + dT_lapse * Points[i].asDouble(1));
	}

	//-----------------------------------------------------
	// interpolate the sea level temperatures

	CSG_Grid SLT, *pSLT = Parameters("SLT")->asGrid();

	if( !pSLT )
	{
		pSLT = &SLT; SLT.Create(Get_System());
	}

	switch( Parameters("INTERPOLATION")->asInt() )
	{
	default:
		SG_RUN_TOOL_ExitOnError("grid_spline", 4, // Multlevel B-Spline Interpolation
			   SG_TOOL_PARAMETER_SET("SHAPES"           , &Points)
			&& SG_TOOL_PARAMETER_SET("FIELD"            , 0      )
			&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1      ) // grid or grid system
			&& SG_TOOL_PARAMETER_SET("TARGET_OUT_GRID"  , pSLT   )
		);
		break;

	case  1:
		SG_RUN_TOOL_ExitOnError("grid_gridding", 1, // Inverse Distance Weighted
			   SG_TOOL_PARAMETER_SET("POINTS"           , &Points)
			&& SG_TOOL_PARAMETER_SET("FIELD"            , 0      )
			&& SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1      ) // grid or grid system
			&& SG_TOOL_PARAMETER_SET("TARGET_OUT_GRID"  , pSLT   )
			&& SG_TOOL_PARAMETER_SET("SEARCH_RANGE"     , 1      ) // global
			&& SG_TOOL_PARAMETER_SET("SEARCH_POINTS_ALL", 1      ) // all points within search distance
		//	&& SG_TOOL_PARAMETER_SET("DW_WEIGHTING"     , 1      ) // inverse distance to a power
			&& SG_TOOL_PARAMETER_SET("DW_IDW_POWER"     , Parameters("IDW_POWER")->asDouble()) // power
		);
		break;
	}

	pSLT->Fmt_Name("%s [%s]", _TL("Sea Level Temperature"), Points.Get_Name());

	//-----------------------------------------------------
	// from sea level temperatures to surface temperatures

	CSG_Grid *pDEM = Parameters("DEM")->asGrid(), *pT = Parameters("TEMPERATURE")->asGrid();

	pT->Fmt_Name("%s [%s]", _TL("Temperature"), Points.Get_Name());

	for(int y=0; y<Get_NY() && Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pDEM->is_NoData(x, y) && !pSLT->is_NoData(x, y) )
			{
				pT->Set_Value(x, y, pSLT->asDouble(x, y) - dT_lapse * pDEM->asDouble(x, y));
			}
			else
			{
				pT->Set_NoData(x, y);
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
bool CTemperature_Lapse_Interpolation::Get_Points(CSG_Shapes &Points)
{
	CSG_Shapes *pPoints = Parameters("POINTS")->asShapes();

	int field_T = Parameters("FIELD_T")->asInt();
	int field_Z = Parameters("FIELD_Z")->asInt();

	Points.Create(SHAPE_TYPE_Point);
	
	Points.Fmt_Name("%s.%s", pPoints->Get_Field_Name(field_T), pPoints->Get_Name());

	Points.Add_Field("T", SG_DATATYPE_Double);
	Points.Add_Field("Z", SG_DATATYPE_Double);

	if( field_Z >= 0 )
	{
		for(sLong i=0; i<pPoints->Get_Count(); i++)
		{
			CSG_Shape *pPoint = pPoints->Get_Shape(i);

			if( !pPoint->is_NoData(field_T) && !pPoint->is_NoData(field_Z) )
			{
				CSG_Shape &Point = *Points.Add_Shape(pPoint, SHAPE_COPY_GEOM);

				Point.Set_Value(0, pPoint->asDouble(field_T));
				Point.Set_Value(1, pPoint->asDouble(field_Z));
			}
		}
	}
	else // if( field_Z < 0 )
	{
		CSG_Grid *pDEM = Parameters("DEM")->asGrid();

		for(sLong i=0; i<pPoints->Get_Count(); i++)
		{
			CSG_Shape *pPoint = pPoints->Get_Shape(i); double z;

			if( !pPoint->is_NoData(field_T) && pDEM->Get_Value(pPoint->Get_Point(), z) )
			{
				CSG_Shape &Point = *Points.Add_Shape(pPoint, SHAPE_COPY_GEOM);

				Point.Set_Value(0, pPoint->asDouble(field_T));
				Point.Set_Value(1, z);
			}
		}
	}

	return( Points.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTemperature_Lapse_Interpolation::Get_Regression(CSG_Shapes &Points, double &dT_lapse)
{
	int Method = Parameters("REGRS_LAPSE")->asInt();

	CSG_Vector Sample;

	switch( Method )
	{
	default: Sample.Create(2); break; // T = a + b*Z
	case  1: Sample.Create(4); break; // T = a + b*Z + c*X + d*Y
	case  2: Sample.Create(6); break; // T = a + b*Z + c*X + d*Y + e*X*X + f*Y*Y
	}

	const SG_Char *Vars[] = { SG_T("T"), SG_T("Z"), SG_T("X"), SG_T("Y"), SG_T("X^2"), SG_T("Y^2") };

	CSG_Strings	Names(Sample.Get_N(), Vars);

	//-----------------------------------------------------
	CSG_Matrix Samples;

	for(sLong i=0; i<Points.Get_Count(); i++)
	{
		CSG_Shape &Point = *Points.Get_Shape(i); CSG_Point p = Point.Get_Point();

		switch( Method )
		{
		case  2: // T = a + b*Z + c*X + d*Y + e*X*X + f*Y*Y
			Sample[5] = p.y*p.y;
			Sample[4] = p.x*p.x;

		case  1: // T = a + b*Z + c*X + d*Y
			Sample[3] = p.y;
			Sample[2] = p.x;

		default: // T = a + b*Z
			Sample[1] = Point.asDouble(1);
			Sample[0] = Point.asDouble(0);
		}

		Samples.Add_Row(Sample);
	}

	//-----------------------------------------------------
	CSG_Regression_Multiple	Regression;

	if( !Regression.Get_Model(Samples, &Names) )
	{
		Error_Set(_TL("Regression failed"));

		return( false );
	}

	Message_Add(Regression.Get_Info(), false);

	dT_lapse = -Regression.Get_RCoeff(0);

	if( Parameters("LIMIT_LAPSE")->asBool() )
	{
		double dT_min = Parameters("CONST_LAPSE")->asDouble() / 100.;

		if( dT_lapse < dT_min )
		{
			dT_lapse = dT_min;
		}
	}

	Message_Fmt("\n\n%s: %g", _TL("Constant lapse rate from regression"), dT_lapse * 100.);

	//-----------------------------------------------------
	CSG_Table *pSummary = Parameters("REGRS_SUMMARY")->asTable();

	if( pSummary )
	{
		pSummary->Destroy();
		pSummary->Fmt_Name("%s (%s: %s)", _TL("Lapse Rate"), _TL("Regression"), Points.Get_Name());
		pSummary->Add_Field(_TL("Parameter"), SG_DATATYPE_String);
		pSummary->Add_Field(_TL("Value"    ), SG_DATATYPE_Double);

		#define Add_Entry(name, value) { CSG_Table_Record &r = *pSummary->Add_Record(); r.Set_Value(0, name); r.Set_Value(1, value); }

		Add_Entry(_TL("Lapse Rate"), dT_lapse * 100.    );
		Add_Entry(_TL("R-squared" ), Regression.Get_R2());
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
