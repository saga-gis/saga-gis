/**********************************************************
 * Version $Id: Erosion_LS_Fields.cpp 1637 2013-03-23 11:52:10Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Erosion_LS_Fields.cpp                 //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//                University of Hamburg                  //
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
#include "Erosion_LS_Fields.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CErosion_LS_Fields::CErosion_LS_Fields(void)
{
	//-----------------------------------------------------
	Set_Name		("LS-Factor, Field Based");

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Calculation of slope length (LS) factor as used for the Universal Soil Loss Equation (USLE), "
		"based on slope and (specific) catchment area, latter as substitute for slope length. This tool "
		"takes only a Digital Elevation Model (DEM) as input and derives catchment areas according to "
		"Freeman (1991). Optionally field polygons can be supplied. Is this the case, calculations will "
		"be performed field by field, i.e. catchment area calculation is restricted to each field's area."
	));

	Add_Reference("Boehner, J. & Selige, T.", "2006",
		"Spatial Prediction of Soil Attributes Using Terrain Analysis and Climate Regionalisation",
		"In: Boehner, J., McCloy, K.R., Strobl, J.: 'SAGA - Analysis and Modelling Applications', "
		"Goettinger Geographische Abhandlungen, 115, 13-27."
	);

	Add_Reference("Desmet, P.J.J. & Govers, G.", "1996",
		"A GIS Procedure for Automatically Calculating the USLE LS Factor on Topographically Complex Landscape Units",
		"Journal of Soil and Water Conservation, 51(5), 427-433."
	);

	Add_Reference("Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22."
	);

	Add_Reference(
		"Kinnell, P.I.A.", "2005",
		"'Alternative Approaches for Determining the USLE-M Slope Length Factor for Grid Cells",
		"soil.scijournals.org, 69/3/674", SG_T("http://soil.scijournals.org/cgi/content/full/69/3/674")
	);

	Add_Reference(
		"Moore, I.D., Grayson, R.B., Ladson, A.R.", "1991",
		"Digital terrain modelling: a review of hydrogical, geomorphological, and biological applications",
		"Hydrological Processes, Vol.5, No.1."
	);

	Add_Reference("Moore, I.D., Nieber, J.L.", "1991",
		"Landscape assessment of soil erosion and nonpoint source pollution",
		"J. Minnesota Acad. Sci., 55, 18-25."
	);

	Add_Reference(
		"Wischmeier, W.H., Smith, D.D.", "1978",
		"Predicting rainfall erosion losses - A guide to conservation planning",
		"Agriculture Handbook No. 537: US Department of Agriculture, Washington DC."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"			, "Elevation",
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"FIELDS"		, "Fields",
		"",
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"STATISTICS"	, "Field Statistics",
		"",
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Grid("",
		"UPSLOPE_AREA"	, _TL("Upslope Length Factor"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"UPSLOPE_LENGTH", _TL("Effective Flow Length"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"UPSLOPE_SLOPE"	, _TL("Upslope Slope"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"LS_FACTOR"		, _TL("LS Factor"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"BALANCE"		, _TL("Sediment Balance"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"		, _TL("LS Calculation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Moore & Nieber 1989"),
			_TL("Desmet & Govers 1996"),
			_TL("Wischmeier & Smith 1978")
		), 0
	);

	Parameters.Add_Choice("",
		"METHOD_SLOPE"	, _TL("Type of Slope"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("local slope"),
			_TL("distance weighted average catchment slope")
		), 0
	);

	Parameters.Add_Choice("",
		"METHOD_AREA"	, _TL("Specific Catchment Area"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("specific catchment area (contour length simply as cell size)"),
			_TL("specific catchment area (contour length dependent on aspect)"),
			_TL("catchment length (square root of catchment area)"),
			_TL("effective flow length"),
			_TL("total catchment area")
		), 1
	);

	Parameters.Add_Bool("",
		"STOP_AT_EDGE"	, _TL("Stop at Edge"),
		"",
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"DESMET_GOVERS"	, _TL("Desmet & Govers"),
		_TL("")
	);

	Parameters.Add_Double("DESMET_GOVERS",
		"EROSIVITY"		, _TL("Rill/Interrill Erosivity"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Choice("DESMET_GOVERS",
		"STABILITY"		, _TL("Stability"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("stable"),
			_TL("instable (thawing)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CErosion_LS_Fields::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("DESMET_GOVERS", pParameter->asInt() == 1);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FIELDS") )
	{
		pParameters->Set_Enabled("STATISTICS"   , pParameter->asShapes() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CErosion_LS_Fields::On_Execute(void)
{
	//-----------------------------------------------------
	m_Method		= Parameters("METHOD"        )->asInt();
	m_Method_Slope	= Parameters("METHOD_SLOPE"  )->asInt();
	m_Method_Area	= Parameters("METHOD_AREA"   )->asInt();

	m_bStopAtEdge	= Parameters("STOP_AT_EDGE"  )->asBool();

	m_Erosivity		= Parameters("EROSIVITY"     )->asDouble();
	m_Stability		= Parameters("STABILITY"     )->asInt();

	m_pDEM			= Parameters("DEM"           )->asGrid();
	m_pUp_Area		= Parameters("UPSLOPE_AREA"  )->asGrid();
	m_pUp_Length	= Parameters("UPSLOPE_LENGTH")->asGrid();
	m_pUp_Slope		= Parameters("UPSLOPE_SLOPE" )->asGrid();
	m_pLS			= Parameters("LS_FACTOR"     )->asGrid();

	DataObject_Set_Colors(m_pUp_Area  , 11, SG_COLORS_WHITE_BLUE    , false);
	DataObject_Set_Colors(m_pUp_Length, 11, SG_COLORS_YELLOW_RED    , false);
	DataObject_Set_Colors(m_pUp_Slope , 11, SG_COLORS_YELLOW_RED    , false);
	DataObject_Set_Colors(m_pLS       , 11, SG_COLORS_RED_GREY_GREEN, true );

	if( m_pUp_Area   == NULL )	m_pUp_Area   = SG_Create_Grid(*Get_System(), SG_DATATYPE_Float);
	if( m_pUp_Length == NULL )	m_pUp_Length = SG_Create_Grid(*Get_System(), SG_DATATYPE_Float);
	if( m_pUp_Slope  == NULL )	m_pUp_Slope  = SG_Create_Grid(*Get_System(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	bool	bResult	= Set_Fields() && Get_Flow() && Get_LS();

	if( bResult )
	{
		Get_Statistics();

		Get_Balance();
	}

	//-----------------------------------------------------
	if( m_pUp_Area   && Parameters("UPSLOPE_AREA"  )->asGrid() == NULL ) delete(m_pUp_Area  );
	if( m_pUp_Length && Parameters("UPSLOPE_LENGTH")->asGrid() == NULL ) delete(m_pUp_Length);
	if( m_pUp_Slope  && Parameters("UPSLOPE_SLOPE" )->asGrid() == NULL ) delete(m_pUp_Slope );

	m_Fields.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CErosion_LS_Fields::Get_Flow(void)
{
	//-----------------------------------------------------
	if( !m_pDEM->Set_Index() )	// create index ...
	{
		return( false );
	}

	Process_Set_Text(_TL("Flow Accumulation"));

	m_pUp_Area  ->Assign(0.0);
	m_pUp_Length->Assign(0.0);
	m_pUp_Slope ->Assign(0.0);

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;
		double	dzSum, dz[8], Slope, Aspect;

		if( m_pDEM->Get_Sorted(n, x, y) && !m_Fields.is_NoData(x, y) && m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
		{
			double	Up_Area		= m_pUp_Area  ->asDouble(x, y) + Get_Cellarea();
			double	Up_Length	= m_pUp_Length->asDouble(x, y) + log(Up_Area);
			double	Up_Slope	= m_pUp_Slope ->asDouble(x, y) + log(Up_Area) * Slope;

			//---------------------------------------------
			if( (dzSum = Get_Flow(x, y, dz)) > 0.0 )
			{
				for(int i=0; i<8; i++)
				{
					if( dz[i] > 0.0 )
					{
						int	ix	= Get_xTo(i, x);
						int	iy	= Get_yTo(i, y);

						m_pUp_Area  ->Add_Value(ix, iy, Up_Area   * dz[i] / dzSum);
						m_pUp_Length->Add_Value(ix, iy, Up_Length * dz[i] / dzSum);
						m_pUp_Slope ->Add_Value(ix, iy, Up_Slope  * dz[i] / dzSum);
					}
				}
			}

			//---------------------------------------------
			switch( m_Method_Area )
			{
			case 0:	// specific catchment area (contour length simply as cell size)
				m_pUp_Area->Set_Value(x, y, Up_Area / (Get_Cellsize()));
				break;

			case 1:	// specific catchment area (contour length dependent on aspect)
				m_pUp_Area->Set_Value(x, y,	Up_Area / (Get_Cellsize() * (fabs(sin(Aspect)) + fabs(cos(Aspect)))));
				break;

			case 2:	// catchment length (square root of catchment area)
				m_pUp_Area->Set_Value(x, y, sqrt(Up_Area));
				break;

			case 3:	// effective flow length
				m_pUp_Area->Set_Value(x, y, Up_Length);
				break;

			case 4:	// total catchment area
				m_pUp_Area->Set_Value(x, y, Up_Area);
				break;
			}

			m_pUp_Length->Set_Value(x, y, Up_Length);
			m_pUp_Slope ->Set_Value(x, y, Up_Slope / (Up_Length > M_ALMOST_ZERO ? Up_Length : M_ALMOST_ZERO));
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
double CErosion_LS_Fields::Get_Flow(int x, int y, double dz[8])
{
	if( m_Fields.is_NoData(x, y) )
	{
		return( 0.0 );
	}

	double	d, z = m_pDEM->asDouble(x, y), dzSum = 0.0;

	int		ID	= m_Fields.asInt(x, y);

	for(int i=0; i<8; i++)
	{
		dz[i]	= 0.0;

		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if(	m_pDEM->is_InGrid(ix, iy) && (d = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
		{
			if( ID == m_Fields.asInt(ix, iy) )
			{
				dzSum	+= (dz[i] = pow(d / Get_Length(i), 1.1));
			}
			else if( m_bStopAtEdge )
			{
				dzSum	+= pow(d / Get_Length(i), 1.1);
			}
		}
	}

	return( dzSum );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CErosion_LS_Fields::Get_LS(void)
{
	Process_Set_Text(_TL("LS Factor"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	LS	= Get_LS(x, y);

			if( LS >= 0.0 )
			{
				m_pLS->Set_Value(x, y, LS);
			}
			else
			{
				m_pLS->Set_NoData(x, y);

				m_pUp_Area  ->Set_NoData(x, y);
				m_pUp_Length->Set_NoData(x, y);
				m_pUp_Slope ->Set_NoData(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
double CErosion_LS_Fields::Get_LS(int x, int y)
{
	double	LS, Slope, Aspect, Area, sin_Slope;

	//-----------------------------------------------------
	if( m_Fields.is_NoData(x, y) )
	{
		return( -1.0 );
	}

	if( !m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
	{
		return( -1.0 );
	}

	if( m_Method_Slope == 1 )	// distance weighted average up-slope slope
	{
		Slope	= m_pUp_Slope->asDouble(x, y);
	}

	if( Slope <= 0.0 )	Slope	= 0.000001;
	if( Aspect < 0.0 )	Aspect	= 0.0;

	sin_Slope	= sin(Slope);

	Area		= m_pUp_Area->asDouble(x, y);

	//-----------------------------------------------------
	switch( m_Method )
	{
	//-----------------------------------------------------
	default:	// Moore and Nieber
		{
			LS		= (0.4 + 1) * pow(Area / 22.13, 0.4) * pow(sin_Slope / 0.0896, 1.3);
		}
		break;

	//-----------------------------------------------------
	case 1:		// Desmet and Govers
		{
			double	L, S, m, x;

			m		= m_Erosivity * (sin_Slope / 0.0896) / (3.0 * pow(sin_Slope, 0.8) + 0.56);
			m		= m / (1.0 + m);

			x		= fabs(sin(Aspect)) + fabs(cos(Aspect));

			// x: coefficient that adjusts for width of flow at the center of the cell.
			// It has a value of 1.0 when the flow is toward a side and sqrt(2.0) when
			// the flow is toward a corner (Kinnel 2005).

			L		= (pow(Area + Get_Cellarea(), m + 1.0) - pow(Area, m + 1.0))
				/ (pow(Get_Cellsize(), m + 2.0) * pow(22.13, m) * pow(x, m));

			//-----------------------------------------------------
			if( Slope < 0.08975817419 )		// <  9% (= atan(0.09)), ca. 5 Degree
			{
				S	= 10.8 * sin_Slope + 0.03;	
			}
			else if( m_Stability == 0 )		// >= 9%, stable
			{
				S	= 16.8 * sin_Slope - 0.5;
			}
			else							// >= 9%, thawing, unstable
			{
				S	= pow(sin_Slope / 0.896, 0.6);
			}

			LS		= L * S;
		}
		break;

	//-----------------------------------------------------
	case 2:		// Wischmeier and Smith
		{
			if( Slope > 0.0505 )	// >  ca. 3°
			{
				LS	= sqrt(Area / 22.13)
					* (65.41 * sin_Slope * sin_Slope + 4.56 * sin_Slope + 0.065);
			}
			else					// <= ca. 3°
			{
				LS	= pow (Area / 22.13, 3.0 * pow(Slope, 0.6))
					* (65.41 * sin_Slope * sin_Slope + 4.56 * sin_Slope + 0.065);
			}
		}
		break;
	}

	return( LS );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CErosion_LS_Fields::Get_Balance(void)
{
	CSG_Grid	*pBalance	= Parameters("BALANCE")->asGrid();

	if( pBalance == NULL )
	{
		return( false );
	}

	DataObject_Set_Colors(pBalance, 11, SG_COLORS_RED_GREY_BLUE , false);

	//-----------------------------------------------------
	int		y;

	CSG_Grid	dzSum(*Get_System(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	Process_Set_Text(CSG_String::Format(SG_T("%s: %s 1"), _TL("Sediment Balance"), _TL("Pass")));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_Fields.is_NoData(x, y) )
			{
				int		ID	= m_Fields.asInt  (x, y);
				double	z	= m_pDEM->asDouble(x, y), iz;
				double	Sum	= 0.0;

				for(int i=0; i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( is_InGrid(ix, iy) && ID == m_Fields.asInt(ix, iy) && (iz = m_pDEM->asDouble(ix, iy)) < z )
					{
						Sum	+= atan((z - iz) / Get_Length(i));	// als Winkel !!!?
					}
				}

				dzSum.Set_Value(x, y, Sum);
			}
		}
	}


	//-----------------------------------------------------
	Process_Set_Text(CSG_String::Format(SG_T("%s: %s 2"), _TL("Sediment Balance"), _TL("Pass")));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_Fields.is_NoData(x, y) )
			{
				int		ID	= m_Fields.asInt  (x, y);
				double	z	= m_pDEM->asDouble(x, y), iz;
				double	Sum	= -m_pLS->asDouble(x, y);

				for(int i=0; i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( is_InGrid(ix, iy) && ID == m_Fields.asInt(ix, iy) && dzSum.asDouble(ix, iy) > 0.0 && (iz = m_pDEM->asDouble(ix, iy)) > z )
					{
						iz	 = atan((z - iz) / Get_Length(i));	// als Winkel !!!?
						Sum	+= (-iz / dzSum.asDouble(ix, iy)) * m_pLS->asDouble(ix, iy);
					}
				}

				if( Sum > 0.0 )
				{
					z	=  log(1.0 + Sum);

					pBalance->Set_Value (x, y, z >  5.0 ?  5.0 : z);
				}
				else if( Sum < 0.0 )
				{
					z	= -log(1.0 - Sum);

					pBalance->Set_Value (x, y, z < -5.0 ? -5.0 : z);
				}
				else
				{
					pBalance->Set_NoData(x, y);
				}
			}
			else
			{
				pBalance->Set_NoData(x, y);
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
bool CErosion_LS_Fields::Get_Statistics(void)
{
	CSG_Shapes	*pFields		= Parameters("FIELDS"    )->asShapes();
	CSG_Shapes	*pStatistics	= Parameters("STATISTICS")->asShapes();

	//-----------------------------------------------------
	if( !pStatistics || !pFields || m_nFields <= 0 || pFields->Get_Count() != m_nFields )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		i;

	CSG_Simple_Statistics	*Statistics	= new CSG_Simple_Statistics[m_nFields];

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pLS->is_NoData(x, y) && (i = m_Fields.asInt(x, y)) >= 0 && i < m_nFields )
			{
				Statistics[i]	+= m_pLS->asDouble(x, y);
			}
		}
	}

	//-----------------------------------------------------
	pStatistics->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pFields->Get_Name(), _TL("LS")));

	pStatistics->Add_Field("NCELLS", SG_DATATYPE_Int   );
	pStatistics->Add_Field("MEAN"  , SG_DATATYPE_Double);
	pStatistics->Add_Field("MIN"   , SG_DATATYPE_Double);
	pStatistics->Add_Field("MAX"   , SG_DATATYPE_Double);
	pStatistics->Add_Field("STDDEV", SG_DATATYPE_Double);

	for(i=0; i<pFields->Get_Count() && Set_Progress(i, pFields->Get_Count()); i++)
	{
		CSG_Shape	*pField	= pStatistics->Add_Shape(pFields->Get_Shape(i));

		if( Statistics[i].Get_Count() > 0 )
		{
			pField->Set_Value(0, Statistics[i].Get_Count  ());
			pField->Set_Value(1, Statistics[i].Get_Mean   ());
			pField->Set_Value(2, Statistics[i].Get_Minimum());
			pField->Set_Value(3, Statistics[i].Get_Maximum());
			pField->Set_Value(4, Statistics[i].Get_StdDev ());
		}
		else for(int j=0; j<pFields->Get_Field_Count(); j++)
		{
			pField->Set_NoData(j);
		}
	}

	//-------------------------------------------------
	delete[](Statistics);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CErosion_LS_Fields::Set_Fields(void)
{
	CSG_Shapes	*pFields	= Parameters("FIELDS")->asShapes();

	//-----------------------------------------------------
	if( !pFields || pFields->Get_Count() <= 0 )
	{
		m_Fields.Create(*Get_System(), SG_DATATYPE_Char);
	//	m_Fields.Set_NoData_Value(1.0);
	//	m_Fields.Assign(0.0);

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !m_pDEM->is_InGrid(x, y) )
				{
					m_Fields.Set_NoData(x, y);
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing Fields"));

	m_nFields	= pFields->Get_Count();

	m_Fields.Create(*Get_System(), m_nFields < pow(2.0, 16.0) - 1.0 ? SG_DATATYPE_Word : SG_DATATYPE_DWord);
	m_Fields.Set_NoData_Value(m_nFields);
	m_Fields.Assign_NoData();

	//-----------------------------------------------------
	for(int iField=0; iField<pFields->Get_Count() && Set_Progress(iField, pFields->Get_Count()); iField++)
	{
		CSG_Shape_Polygon	*pField	= (CSG_Shape_Polygon *)pFields->Get_Shape(iField);

		int	xMin	= Get_System()->Get_xWorld_to_Grid(pField->Get_Extent().Get_XMin()) - 1; if( xMin <  0        ) xMin = 0;
		int	xMax	= Get_System()->Get_xWorld_to_Grid(pField->Get_Extent().Get_XMax()) + 1; if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
		int	yMin	= Get_System()->Get_yWorld_to_Grid(pField->Get_Extent().Get_YMin()) - 1; if( yMin <  0        ) yMin = 0;
		int	yMax	= Get_System()->Get_yWorld_to_Grid(pField->Get_Extent().Get_YMax()) + 1; if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

		for(int y=yMin; y<=yMax; y++)
		{
			for(int x=xMin; x<=xMax; x++)
			{
				if( m_pDEM->is_InGrid(x, y) && pField->Contains(Get_System()->Get_Grid_to_World(x, y)) )
				{
					m_Fields.Set_Value(x, y, iField);
				}
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
