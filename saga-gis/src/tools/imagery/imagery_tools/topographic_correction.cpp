
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               topographic_correction.cpp              //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "topographic_correction.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTopographic_Correction::CTopographic_Correction(void)
{
	Set_Name		(_TL("Topographic Correction"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Topographic correction tools for satellite imagery using a "
		"digital elevation model (DEM) and the position of the Sun "
		"at acquisition time to estimate and remove shading effects. "
	));

	Add_Reference(
		"Civco, D. L.", "1989",
		"Topographic Normalization of Landsat Thematic Mapper Digital Imagery",
		"Photogrammetric Engineering and Remote Sensing, 55(9), pp.1303-1309."
	);

	Add_Reference(
		"Law, K.H., Nichol, J.", "2004",
		"Topographic Correction for Differential Illumination Effects on Ikonos Satellite Imagery",
		"ISPRS 2004 International Society for Photogrammetry and Remote Sensing.",
		SG_T("http://www.cartesia.org/geodoc/isprs2004/comm3/papers/347.pdf")
	);

	Add_Reference(
		"Phua, M.-H., Saito, H.", "2003",
		"Estimation of biomass of a mountainous tropical forest using Landsat TM data",
		"Canadian Journal of Remote Sensing, 29(4), pp.429-440."
	);

	Add_Reference(
		"Riano, D., Chuvieco, E. Salas, J., Aguado, I.", "2003",
		"Assessment of Different Topographic Corrections in Landsat-TM Data for Mapping Vegetation Types",
		"IEEE Transactions on Geoscience and Remote Sensing, 41(5), pp.1056-1061.",
		SG_T("http://www.geogra.uah.es/~emilio/pdf/Riano2003b.pdf")
	);

	Add_Reference(
		"Teillet, P.M., Guindon, B., Goodenough, D.G.", "1982",
		"On the slope-aspect correction of multispectral scanner data",
		"Canadian Journal of Remote Sensing, 8(2), pp.1537-1540."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid_List("",
		"BANDS"		, _TL("Bands"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"CORRECTED"	, _TL("Corrected Bands"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Node("", "SOLAR_POSITION", _TL("Solar Position"), _TL(""));

	Parameters.Add_Double("SOLAR_POSITION",
		"AZIMUTH"	, _TL("Azimuth"),
		_TL("direction of sun (degree, clockwise from North)"),
		180., 0., true, 360., true
	);

	Parameters.Add_Double("SOLAR_POSITION",
		"HEIGHT"	, _TL("Height"),
		_TL("height of sun above horizon (degree)"),
		 45., 0., true,  90., true
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s",
			_TL("Cosine Correction (Teillet et al. 1982)"),
			_TL("Cosine Correction (Civco 1989)"),
			_TL("Minnaert Correction"),
			_TL("Minnaert Correction with Slope (Riano et al. 2003)"),
			_TL("Minnaert Correction with Slope (Law & Nichol 2004)"),
			_TL("C Correction"),
			_TL("Normalization (after Civco, modified by Law & Nichol)")
		), 2
	);

	Parameters.Add_Double("METHOD",
		"MINNAERT"	, _TL("Minnaert Correction"),
		_TL(""),
		0.5, 0., true, 1., true
	);

	Parameters.Add_Int("METHOD",
		"MAXCELLS"	, _TL("Maximum Number of Cells"),
		_TL("Maximum number of grid cells used for trend analysis as required by C correction."),
		100000, 1000, true
	);

	Parameters.Add_Bool("",
		"LIMIT"		, _TL("Limit Value Range"),
		_TL("Avoid over and undershootings specifying value range limits."),
		false
	);

	Parameters.Add_Double("LIMIT",
		"LIMIT_MIN"	, _TL("Minimum"),
		_TL(""),
		1.
	);

	Parameters.Add_Double("LIMIT",
		"LIMIT_MAX"	, _TL("Maximum"),
		_TL(""),
		255.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTopographic_Correction::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("BANDS") && pParameter->asList()->Get_Item_Count() > 0 )
	{
		for(int i=0; i<pParameter->asList()->Get_Item_Count(); i++)
		{
			CSG_MetaData &MD = pParameter->asList()->Get_Item(i)->Get_Owner()
				? pParameter->asList()->Get_Item(i)->Get_Owner()->Get_MetaData()
				: pParameter->asList()->Get_Item(i)             ->Get_MetaData();

			if( MD("LANDSAT") )
			{
				double Azimuth, Height;

				if( MD["LANDSAT"].Get_Content("SUN_AZIMUTH"  , Azimuth)
				&&  MD["LANDSAT"].Get_Content("SUN_ELEVATION", Height ) )
				{
					pParameters->Set_Parameter("AZIMUTH", Azimuth);
					pParameters->Set_Parameter("HEIGHT" , Height );

					break;
				}
			}
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CTopographic_Correction::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("MINNAERT", pParameter->asInt() == 2 || pParameter->asInt() == 3 || pParameter->asInt() == 4);

		pParameters->Set_Enabled("MAXCELLS", pParameter->asInt() == 5);
	}

	if( pParameter->Cmp_Identifier("LIMIT") )
	{
		pParameter->Set_Children_Enabled(pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Correction::On_Execute(void)
{
	CSG_Parameter_Grid_List *pBands = Get_Bands();

	if( !pBands || pBands->Get_Grid_Count() < 1 || !Get_Illumination() )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Method    = Parameters("METHOD")->asInt();

	m_Value_Min = Parameters("LIMIT")->asBool() ? Parameters("LIMIT_MIN")->asDouble() : 0.;
	m_Value_Max = Parameters("LIMIT")->asBool() ? Parameters("LIMIT_MAX")->asDouble() : 0.;

	m_Minnaert  = Parameters("MINNAERT")->asDouble();

	//-----------------------------------------------------
	for(int i=0; i<pBands->Get_Grid_Count() && Process_Get_Okay(); i++)
	{
		Process_Set_Text("%s [%d/%d]", _TL("Topographic Correction"), i + 1, pBands->Get_Grid_Count());

		CSG_Grid *pBand = pBands->Get_Grid(i);

		if( Get_Model(pBand) )
		{
			for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
			{
				#pragma omp parallel for
				for(int x=0; x<Get_NX(); x++)
				{
					if( !pBand->is_NoData(x, y) )
					{
						pBand->Set_Value(x, y, Get_Correction(
							m_Illumination[0].asDouble(x, y),
							m_Illumination[1].asDouble(x, y), pBand->asDouble(x, y)
						));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Illumination[0].Destroy();
	m_Illumination[1].Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CTopographic_Correction::Get_Correction(double Slope, double Incidence, double Value)
{
	switch( m_Method )
	{
	default:	// Cosine Correction (Teillet et al. 1982)
		if( Incidence > 0. )
		{
			Value = Value * m_cosTz / Incidence;
		}
		break;

	case  1:	// Cosine Correction (Civco 1989)
		Value = Value + (Value * ((m_Illumination[1].Get_Mean() - Incidence) / m_Illumination[1].Get_Mean()));
		break;

	case  2:	// Minnaert Correction
		if( Incidence > 0. )
		{
			Value = Value * pow(m_cosTz / Incidence, m_Minnaert);
		}
		break;

	case  3:	// Minnaert Correction with Slope (Riano et al. 2003)
		if( Incidence > 0. )
		{
			Value = Value * cos(Slope) * pow(m_cosTz / (Incidence * cos(Slope)), m_Minnaert);
		}
		break;

	case  4:	// Minnaert Correction with Slope (Law & Nichol 2004)
		if( Incidence > 0. )
		{
			Value = Value * cos(Slope) / pow(Incidence * cos(Slope), m_Minnaert);
		}
		break;

	case  5:	// C Correction
		Value = Value * (m_cosTz + m_C) / (Incidence + m_C);
		break;

	case  6:	// Normalization
		Value = Value + ((Value * ((m_Illumination[1].Get_Mean() - Incidence) / m_Illumination[1].Get_Mean())) * m_C);
		break;
	}

	if( m_Value_Min < m_Value_Max )
	{
		if( Value < m_Value_Min ) { return( m_Value_Min ); }
		if( Value > m_Value_Max ) { return( m_Value_Max ); }
	}

	return( Value );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Correction::Get_Model(CSG_Grid *pImage)
{
	switch( m_Method )
	{
	case  1:	// Cosine Correction (Civco 1989)
		return( m_Illumination[1].Get_Mean() );

	//-----------------------------------------------------
	case  5: {	// C Correction
		CSG_Regression R;

		sLong Step = Get_NCells() < Parameters("MAXCELLS")->asInt() ? 1 : Get_NCells() / Parameters("MAXCELLS")->asInt();

		for(sLong i=0; i<Get_NCells() && Set_Progress_Cells(i); i+=Step)
		{
			R.Add_Values(pImage->asDouble(i), m_Illumination[1].asDouble(i));
		}

		if( !R.Calculate() || !R.Get_Constant() )
		{
			SG_UI_Msg_Add_Error(_TL("Regression failed"));

			return( false );
		}

		m_C = R.Get_Coefficient() / R.Get_Constant();

		Message_Add(R.asString());

		break; }

	//-----------------------------------------------------
	case  6:	// Normalization
		m_C = 1.; return( m_Illumination[1].Get_Mean() );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Correction::Get_Illumination(void)
{
	Process_Set_Text(_TL("calculating illumination"));

	//-----------------------------------------------------
	CSG_Grid DEM, *pDEM = Parameters("DEM")->asGrid();

	if( !pDEM->Get_Extent().Intersects(Get_System().Get_Extent()) )
	{
		Error_Set(_TL("Extents of elevation model and images do not intersect!"));

		return( false );
	}

	if( !pDEM->Get_System().is_Equal(Get_System()) )
	{
		DEM.Create(Get_System());
		DEM.Assign(pDEM, pDEM->Get_Cellsize() > Get_Cellsize() ? GRID_RESAMPLING_BSpline : GRID_RESAMPLING_Mean_Cells);
		pDEM = &DEM;
	}

	//-----------------------------------------------------
	double Azimuth = Parameters("AZIMUTH")->asDouble() * M_DEG_TO_RAD;
	double Height  = Parameters("HEIGHT" )->asDouble() * M_DEG_TO_RAD;

	m_cosTz = cos(M_PI_090 - Height);
	m_sinTz = sin(M_PI_090 - Height);

	m_Illumination[0].Create(Get_System()); m_Illumination[1].Create(Get_System());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Slope, Aspect;

			if( pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_Illumination[0].Set_Value(x, y, Slope);
				m_Illumination[1].Set_Value(x, y, cos(Slope) * m_cosTz + sin(Slope) * m_sinTz * cos(Azimuth - Aspect));
			}
			else
			{
				m_Illumination[0].Set_Value(x, y, 0.);
				m_Illumination[1].Set_Value(x, y, m_cosTz);
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
CSG_Parameter_Grid_List * CTopographic_Correction::Get_Bands(void)
{
	CSG_Parameter_Grid_List *pBands = Parameters("BANDS")->asGridList();

	if( pBands->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no images in input list"));

		return( NULL );
	}

	Parameters("CORRECTED")->asGridList()->Del_Items();

	for(int i=0; i<pBands->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		CSG_Data_Object	*pInput = pBands->Get_Item(i), *pOutput;

		switch( pInput->Get_ObjectType() )
		{
		default: {
			CSG_Grid  *pGrid  = (CSG_Grid  *)pInput; pOutput = SG_Create_Grid (*pGrid );

			((CSG_Grid  *)pOutput)->Set_Scaling(pGrid ->Get_Scaling(), pGrid ->Get_Offset());

			break; }

		case SG_DATAOBJECT_TYPE_Grids: {
			CSG_Grids *pGrids = (CSG_Grids *)pInput; pOutput = SG_Create_Grids(*pGrids);

			((CSG_Grids *)pOutput)->Set_Scaling(pGrids->Get_Scaling(), pGrids->Get_Offset());

			break; }
		}

		DataObject_Add(pOutput); DataObject_Set_Parameters(pOutput, pInput);

		pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Topographic Correction"));
		pOutput->Set_NoData_Value_Range(pInput->Get_NoData_Value(), pInput->Get_NoData_Value(true));

		pOutput->Set_Description      (pInput->Get_Description());
		pOutput->Get_MetaData().Assign(pInput->Get_MetaData   ());

		Parameters("CORRECTED")->asGridList()->Add_Item(pOutput);
	}

	return( Parameters("CORRECTED")->asGridList() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
