
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        tpi.cpp                        //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "tpi.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTPI::CTPI(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Topographic Position Index (TPI)"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Topographic Position Index (TPI) calculation as proposed by Guisan et al. (1999). "
		"This is literally the same as the difference to the mean calculation (residual analysis) "
		"proposed by Wilson & Gallant (2000). "
		"The bandwidth parameter for distance weighting is given as percentage of the (outer) radius."
	));

	Add_Reference(
		"Guisan, A., Weiss, S.B., Weiss, A.D.", "1999",
		"GLM versus CCA spatial modeling of plant species distribution",
		"Plant Ecology 143: 107-122."
	);

	Add_Reference(
		"Weiss, A.D.", "2000",
		"Topographic Position and Landforms Analysis",
		"Poster", SG_T("http://www.jennessent.com/downloads/tpi-poster-tnc_18x22.pdf")
	);

	Add_Reference(
		"Wilson, J.P. & Gallant, J.C.", "2000",
		"Primary Topographic Attributes",
		"In: Wilson, J.P. & Gallant, J.C. [Eds.]: Terrain Analysis: Principles and Applications, John Wiley & Sons, p.51-85."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "TPI"		, _TL("Topographic Position Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "STANDARD"	, _TL("Standardize"),
		_TL(""),
		false
	);

	Parameters.Add_Range(
		"", "RADIUS"	, _TL("Scale"),
		_TL("kernel radius in map units; defines an annulus if inner radius is greater than zero"),
		0., 100., 0., true
	);

	m_Kernel.Get_Weighting().Set_BandWidth(75.);	// 75%
	m_Kernel.Get_Weighting().Create_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTPI::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Kernel.Get_Weighting().Enable_Parameters(*pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI::On_Execute(void)
{
	m_pDEM	= Parameters("DEM")->asGrid();
	m_pTPI	= Parameters("TPI")->asGrid();

	DataObject_Set_Colors(m_pTPI, 11, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	double	r_inner	= Parameters("RADIUS")->asRange()->Get_Min() / Get_Cellsize();
	double	r_outer	= Parameters("RADIUS")->asRange()->Get_Max() / Get_Cellsize();

	m_Kernel.Get_Weighting().Set_Parameters(Parameters);
	m_Kernel.Get_Weighting().Set_BandWidth(r_outer * m_Kernel.Get_Weighting().Get_BandWidth() / 100.);

	if( !m_Kernel.Set_Annulus(r_inner, r_outer) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Statistics(x, y);
		}
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	if( Parameters("STANDARD")->asBool() )
	{
		m_pTPI->Standardise();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI::Get_Statistics(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int		i, ix, iy;
		double	z, id, iw;

		CSG_Simple_Statistics	Statistics;

		for(i=0, z=m_pDEM->asDouble(x, y); i<m_Kernel.Get_Count(); i++)
		{
			if( m_Kernel.Get_Values(i, ix = x, iy = y, id, iw, true) && id >= 0. && m_pDEM->is_InGrid(ix, iy) )
			{
				Statistics.Add_Value(m_pDEM->asDouble(ix, iy), iw);
			}
		}

		//-------------------------------------------------
		if( Statistics.Get_Weights() > 0. )
		{
			m_pTPI->Set_Value(x, y, z - Statistics.Get_Mean());

			return( true );
		}
	}

	//-----------------------------------------------------
	m_pTPI->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTPI_Classification::CTPI_Classification(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("TPI Based Landform Classification"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Topographic Position Index (TPI) calculation as proposed by Guisan et al. (1999). "
		"This is literally the same as the difference to the mean calculation (residual analysis) "
		"proposed by Wilson & Gallant (2000). "
		"The bandwidth parameter for distance weighting is given as percentage of the (outer) radius."
	));

	Add_Reference(
		"Guisan, A., Weiss, S.B., Weiss, A.D.", "1999",
		"GLM versus CCA spatial modeling of plant species distribution",
		"Plant Ecology 143: 107-122."
	);

	Add_Reference(
		"Weiss, A.D.", "2000",
		"Topographic Position and Landforms Analysis",
		"Poster", SG_T("http://www.jennessent.com/downloads/tpi-poster-tnc_18x22.pdf")
	);

	Add_Reference(
		"Wilson, J.P. & Gallant, J.C.", "2000",
		"Primary Topographic Attributes",
		"In: Wilson, J.P. & Gallant, J.C. [Eds.]: Terrain Analysis: Principles and Applications, John Wiley & Sons, p.51-85."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "LANDFORMS"	, _TL("Landforms"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Range(
		"", "RADIUS_A"	, _TL("Small Scale"),
		_TL("kernel radius in map units; defines an annulus if inner radius is greater than zero"),
		0., 100., 0., true
	);

	Parameters.Add_Range(
		"", "RADIUS_B"	, _TL("Large Scale"),
		_TL("kernel radius in map units; defines an annulus if inner radius is greater than zero"),
		0., 1000., 0., true
	);

	m_Weighting.Set_BandWidth(75.);	// 75%
	m_Weighting.Create_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	LF_CANYON	= 1,
	LF_MID_SLOPE,
	LF_UPLAND,
	LF_VALLEY,
	LF_PLAIN,
	LF_OPEN_SLOPE,
	LF_UPPER_SLOPE,
	LF_LOCAL_RIDGE,
	LF_MIDSLOPE_RIDGE,
	LF_HIGH_RIDGE,
	LF_COUNT	= LF_HIGH_RIDGE
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTPI_Classification::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Weighting.Enable_Parameters(*pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI_Classification::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pDEM		= Parameters("DEM"      )->asGrid();
	CSG_Grid	*pLandforms	= Parameters("LANDFORMS")->asGrid();

	pLandforms->Set_NoData_Value(0);

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pLandforms, "LUT");

	if( pLUT )
	{
		const int LF_Colors[LF_COUNT]	=
		{
			SG_GET_RGB(  0,   0, 127),	// LF_CANYON
			SG_GET_RGB(200, 200, 255),	// LF_MID_SLOPE
			SG_GET_RGB(  0, 200, 255),	// LF_UPLAND
			SG_GET_RGB(127, 127, 255),	// LF_VALLEY
			SG_GET_RGB(255, 255, 128),	// LF_PLAIN
			SG_GET_RGB(128, 255,   0),	// LF_OPEN_SLOPE
			SG_GET_RGB(  0, 255,   0),	// LF_UPPER_SLOPE
			SG_GET_RGB(255, 200, 127),	// LF_LOCAL_RIDGE
			SG_GET_RGB(255, 127,   0),	// LF_MIDSLOPE_RIDGE
			SG_GET_RGB(255,   0,   0)	// LF_HIGH_RIDGE
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL("Streams"           );	Desc	+= _TL("Canyons, Deeply Incised Streams"       );
		Name	+= _TL("Midslope Drainages");	Desc	+= _TL("Midslope Drainages, Shallow Valleys"   );
		Name	+= _TL("Upland Drainages"  );	Desc	+= _TL("Upland Drainages, Headwaters"          );
		Name	+= _TL("Valleys"           );	Desc	+= _TL("U-shaped Valleys"                      );
		Name	+= _TL("Plains"            );	Desc	+= _TL("Plains"                                );
		Name	+= _TL("Open Slopes"       );	Desc	+= _TL("Open Slopes"                           );
		Name	+= _TL("Upper Slopes"      );	Desc	+= _TL("Upper Slopes, Mesas"                   );
		Name	+= _TL("Local Ridges"      );	Desc	+= _TL("Local Ridges, Hills in Valleys"        );
		Name	+= _TL("Midslope Ridges"   );	Desc	+= _TL("Midslope Ridges, Small Hills in Plains");
		Name	+= _TL("High Ridges"       );	Desc	+= _TL("Mountain Tops, High Ridges"            );

		//-------------------------------------------------
		CSG_Table	*pTable	= pLUT->asTable();

		pLUT->asTable()->Del_Records();

		for(int i=0; i<LF_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pLUT->asTable()->Add_Record();

			pRecord->Set_Value(0, LF_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i + 1);
			pRecord->Set_Value(4, i + 1);
		}

		DataObject_Set_Parameter(pLandforms, pLUT);
		DataObject_Set_Parameter(pLandforms, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	CTPI	Calculator;	Calculator.Set_Manager(NULL);

	Calculator.Get_Parameters()->Assign_Values(&Parameters);	// set DEM and Weighting scheme

	Calculator.Set_Parameter("STANDARD", true);

	//-----------------------------------------------------
	CSG_Grid	gA(Get_System());

	Calculator.Set_Parameter("TPI"   , &gA);
	Calculator.Set_Parameter("RADIUS", Parameters("RADIUS_A"));

	if( !Calculator.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	gB(Get_System());

	Calculator.Set_Parameter("TPI"   , &gB);
	Calculator.Set_Parameter("RADIUS", Parameters("RADIUS_B"));

	if( !Calculator.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) )
			{
				pLandforms->Set_Value(x, y, 0.);
			}
			else
			{
				double	A	= gA.asDouble(x, y);
				double	B	= gB.asDouble(x, y);

				if( A <= -1. )
				{
					if( B <= -1. )
					{	// Canyons, Deeply Incised Streams
						pLandforms->Set_Value(x, y, LF_CANYON);
					}
					else if( B < 1. )
					{	// Midslope Drainages, Shallow Valleys
						pLandforms->Set_Value(x, y, LF_MID_SLOPE);
					}
					else // if( B >= 1. )
					{	// Upland Drainages, Headwaters
						pLandforms->Set_Value(x, y, LF_UPLAND);
					}
				}
				else if( A < 1. )
				{
					if( B <= -1. )
					{	// U-shaped Valleys
						pLandforms->Set_Value(x, y, LF_VALLEY);
					}
					else if( B < 1. )
					{
						double	Slope, Aspect;

						pDEM->Get_Gradient(x, y, Slope, Aspect);

						if( Slope <= 5. * M_DEG_TO_RAD )
						{	// Plains
							pLandforms->Set_Value(x, y, LF_PLAIN);
						}
						else
						{	// Open Slopes
							pLandforms->Set_Value(x, y, LF_OPEN_SLOPE);
						}
					}
					else // if( B >= 1. )
					{	// Upper Slopes, Mesas
						pLandforms->Set_Value(x, y, LF_UPPER_SLOPE);
					}
				}
				else // if( A >= 1. )
				{
					if( B <= -1. )
					{	// Local Ridges, Hills in Valleys
						pLandforms->Set_Value(x, y, LF_LOCAL_RIDGE);
					}
					else if( B < 1. )
					{	// Midslope Ridges, Small Hills in Plains
						pLandforms->Set_Value(x, y, LF_MIDSLOPE_RIDGE);
					}
					else // if( B >= 1. )
					{	// Mountain Tops, High Ridges
						pLandforms->Set_Value(x, y, LF_HIGH_RIDGE);
					}
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
CTPI_MultiScale::CTPI_MultiScale(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Multi-Scale Topographic Position Index (TPI)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Topographic Position Index (TPI) calculation as proposed by Guisan et al. (1999).\n"
		"\n"
		"This implementation calculates the TPI for different scales and integrates these into one "
		"single grid. The hierarchical integration is achieved by starting with the standardized "
		"TPI values of the largest scale, then adding standardized values from smaller scales "
		"where the (absolute) values from the smaller scale exceed those from the larger scale. "
		"This integration scheme has been proposed by N. Zimmermann."
	));

	Add_Reference(
		"Guisan, A., Weiss, S.B., Weiss, A.D.", "1999",
		"GLM versus CCA spatial modeling of plant species distribution",
		"Plant Ecology 143: 107-122."
	);

	Add_Reference(
		"Weiss, A.D.", "2000",
		"Topographic Position and Landforms Analysis",
		"Poster", SG_T("http://www.jennessent.com/downloads/tpi-poster-tnc_18x22.pdf")
	);

	Add_Reference(
		"Wilson, J.P. & Gallant, J.C.", "2000",
		"Primary Topographic Attributes",
		"In: Wilson, J.P. & Gallant, J.C. [Eds.]: Terrain Analysis: Principles and Applications, John Wiley & Sons, p.51-85."
	);

	Add_Reference(
		"www.wsl.ch/staff/niklaus.zimmermann/programs/aml4_1.html",
		SG_T("toposcale.aml script by N.Zimmermann")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "TPI"		, _TL("Topographic Position Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		"", "SCALE_MIN"	, _TL("Minimum Scale"),
		_TL("kernel radius in cells"),
		1, 1, true
	);

	Parameters.Add_Int(
		"", "SCALE_MAX"	, _TL("Maximum Scale"),
		_TL("kernel radius in cells"),
		8, 1, true
	);

	Parameters.Add_Int(
		"", "SCALE_NUM"	, _TL("Number of Scales"),
		_TL(""),
		3, 2, true
	);

	Parameters.Add_Bool(
		"", "UPDATE"	, _TL("Update"),
		_TL("update view for each integration step"),
		false
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTPI_MultiScale::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SCALE_MIN") )
	{
		if( pParameter->asInt() > pParameters->Get_Parameter("SCALE_MAX")->asInt() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("SCALE_MAX")->asInt());
		}
	}

	if( pParameter->Cmp_Identifier("SCALE_MAX") )
	{
		if( pParameter->asInt() < pParameters->Get_Parameter("SCALE_MIN")->asInt() )
		{
			pParameter->Set_Value(pParameters->Get_Parameter("SCALE_MIN")->asInt());
		}
	}

	//-----------------------------------------------------
	int	Range	= pParameters->Get_Parameter("SCALE_MAX")->asInt()
				- pParameters->Get_Parameter("SCALE_MIN")->asInt();

	if( Range > 0 && Range + 1 < pParameters->Get_Parameter("SCALE_NUM")->asInt() )
	{
		pParameters->Set_Parameter("SCALE_NUM", Range + 1);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CTPI_MultiScale::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("SCALE_NUM",
		pParameters->Get_Parameter("SCALE_MIN")->asInt() <
		pParameters->Get_Parameter("SCALE_MAX")->asInt()
	);

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTPI_MultiScale::On_Execute(void)
{
	//-----------------------------------------------------
	int	Scale_Min	= Parameters("SCALE_MIN")->asInt();
	int	Scale_Max	= Parameters("SCALE_MAX")->asInt();
	int	nScales		= Parameters("SCALE_NUM")->asInt();

	if( Scale_Min > Scale_Max || nScales < 2 )
	{
		Error_Fmt("%s (min=%d, max=%d, num=%d)", _TL("invalid parameters"), Scale_Min, Scale_Max, nScales);

		return( false );
	}

	double	Scale	= Get_Cellsize() *  Scale_Max;
	double	dScale	= Get_Cellsize() * (Scale_Max - Scale_Min) / (nScales - 1.);

	if( dScale <= 0. )
	{
		nScales	= 1;
	}

	//-----------------------------------------------------
	CSG_Grid	TPI(Get_System()), *pTPI	= Parameters("TPI")->asGrid();

	CTPI	Calculator;	Calculator.Set_Manager(NULL);

	Calculator.Set_Parameter("DEM"     , Parameters("DEM")->asGrid());
	Calculator.Set_Parameter("TPI"     , pTPI);
	Calculator.Set_Parameter("STANDARD", true);

	Calculator.Get_Parameters()->Get_Parameter("RADIUS")->asRange()->Set_Min(  0.);
	Calculator.Get_Parameters()->Get_Parameter("RADIUS")->asRange()->Set_Max(Scale);

	Process_Set_Text(  "%s: %.*f [%d/%d]", _TL("Scale"), SG_Get_Significant_Decimals(Scale), Scale, 1, nScales);
	Message_Fmt     ("\n%s: %.*f [%d/%d]", _TL("Scale"), SG_Get_Significant_Decimals(Scale), Scale, 1, nScales);

	SG_UI_Msg_Lock(true);
	Calculator.Execute();
	SG_UI_Msg_Lock(false);

	Calculator.Set_Parameter("TPI", &TPI);

	//-----------------------------------------------------
	for(int iScale=1; iScale<nScales && Process_Get_Okay(); iScale++)
	{
		if( Parameters("UPDATE")->asBool() )
		{
			DataObject_Update(pTPI);
		}

		(*Calculator.Get_Parameters())("RADIUS")->asRange()->Set_Max(Scale = Scale - dScale);

		Process_Set_Text(  "%s: %.*f [%d/%d]", _TL("Scale"), SG_Get_Significant_Decimals(Scale), Scale, 1 + iScale, nScales);
		Message_Fmt     ("\n%s: %.*f [%d/%d]", _TL("Scale"), SG_Get_Significant_Decimals(Scale), Scale, 1 + iScale, nScales);

		SG_UI_Msg_Lock(true);
		Calculator.Execute();
		SG_UI_Msg_Lock(false);

		//-------------------------------------------------
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pTPI->is_NoData(x, y) && fabs(pTPI->asDouble(x, y)) < fabs(TPI.asDouble(x, y)) )
				{
					pTPI->Set_Value(x, y, TPI.asDouble(x, y));
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
