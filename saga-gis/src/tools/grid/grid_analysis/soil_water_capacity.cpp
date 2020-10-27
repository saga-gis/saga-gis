
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    grid_analysis                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 soil_water_capacity.cpp               //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include "soil_water_capacity.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double	CSoil_Water_Capacity::s_Coefficients[4][12]	= {	// pedotransfer coefficients developed by Hodnett and Tomasella (2002)
	{ -2.294,  0.   , -3.526,  0.   ,  2.44 ,   0.   , -0.076, -11.331, 0.019, 0.    ,  0.   ,  0.     }, // ln(alpha)
	{ 62.986,  0.   ,  0.   , -0.833, -0.529,   0.   ,  0.   ,   0.593, 0.   , 0.007 , -0.014,  0.     }, // ln(n)
	{ 81.799,  0.   ,  0.   ,  0.099,  0.   , -31.42 ,  0.018,   0.451, 0.   , 0     ,  0.   , -5.e-04 }, // theta_s
	{ 22.733, -0.164,  0.   ,  0.   ,  0.   ,   0.   ,  0.235,  -0.831, 0.   , 0.0018,  0.   ,  0.0026 }  // theta_r
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoil_Water_Capacity::CSoil_Water_Capacity(bool bGrids)
	: CSG_Tool_Grid(), m_bGrids(bGrids)
{
	Set_Name		(CSG_String::Format(m_bGrids ? "%s (%s)" : "%s", _TL("Soil Water Capacity"), _TL("Grid Collections")));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"This tool derives the soil water capacity for the given "
		"soil moisture potentials (psi) based on pedo-transfer functions.\n"
		"Suggested psi values for field capacity estimation range between "
		"60 hPa (pF=1.8) and 316 hPa (pF=2.5). "
		"For permanent wilting point estimation take a psi "
		"value of about 15850 hPa (pF=4.2). "
		"This tool re-implements the R-script AWCPTF by Hengl as well as the "
		"regression approach by Toth et al. (2015). "
		"See Hengl et al. (2017), Woesten & Verzandvoort (2013) "
		"and Toth et al. (2015) for more details. "
	));

	Add_Reference("Hengl, T., Mendes de Jesus, J., Heuvelink, G.B.M., Ruiperez Gonzalez, M., Kilibarda, M., Blagotic, A., et al.", "2017",
		"SoilGrids250m: Global gridded soil information based on machine learning",
		"PLoS ONE 12(2): e0169748.",
		SG_T("https://doi.org/10.1371/journal.pone.0169748"), SG_T("doi:10.1371/journal.pone.0169748")
	);

	Add_Reference("Hodnett, M.G., Tomasella, J.", "2002",
		"Marked differences between van Genuchten soil water-retention parameters for temperate and tropical soils: A new water-retention pedo-transfer functions developed for tropical soils",
		"Geoderma 108(3):155-180.",
		SG_T("https://doi.org/10.1016/S0016-7061(02)00105-2"), SG_T("doi:10.1016/S0016-7061(02)00105-2")
	);

	Add_Reference("Hodnett, M.G., Tomasella, J.", "2002",
		"Marked differences between van Genuchten soil water-retention parameters for temperate and tropical soils: A new water-retention pedo-transfer functions developed for tropical soils",
		"Geoderma 108(3):155-180.",
		SG_T("https://doi.org/10.1016/S0016-7061(02)00105-2"), SG_T("doi:10.1016/S0016-7061(02)00105-2")
	);

	Add_Reference("Toth, B., Weynants, M., Nemes, A., Mako, A., Bilas, G., Toth, G.", "2015",
		"New generation of hydraulic pedotransfer functions for Europe",
		"European Journal of Soil Science, 66, 226–238.",
		SG_T("https://doi.org/10.1111/ejss.12192"), SG_T("doi:10.1111/ejss.12192")
	);

	Add_Reference("Toth, B., Weynants, M., Pasztor, L, Hengl, T.", "2017",
		"3D soil hydraulic database of Europe at 250 m resolution",
		"Hydrological Processes, 31:2662–2666.",
		SG_T("https://doi.org/10.1002/hyp.11203"), SG_T("doi:10.1002/hyp.11203")
	);

	Add_Reference(
		"https://github.com/cran/GSIF/blob/master/R/AWCPTF.R",
		SG_T("R-Script AWCPTF by Tomislav Hengl version 0.5-5")
	);

	//-----------------------------------------------------
	if( m_bGrids )
	{
		Parameters.Add_Grids        ("", "SAND"   , _TL("Sand"                        ), _TL("[%]"       ), PARAMETER_INPUT                );
		Parameters.Add_Grids        ("", "SILT"   , _TL("Silt"                        ), _TL("[%]"       ), PARAMETER_INPUT                );
		Parameters.Add_Grids        ("", "CLAY"   , _TL("Clay"                        ), _TL("[%]"       ), PARAMETER_INPUT                );
		Parameters.Add_Grids        ("", "CORG"   , _TL("Soil Organic Carbon"         ), _TL("[permille]"), PARAMETER_INPUT                );
		Parameters.Add_Grids        ("", "BULK"   , _TL("Bulk Density"                ), _TL("[kg/m^3]"  ), PARAMETER_INPUT                );
		Parameters.Add_Grids        ("", "CEC"    , _TL("Cation Exchange Capacity"    ), _TL("[cmol/kg]" ), PARAMETER_INPUT                );
		Parameters.Add_Grids        ("", "PH"     , _TL("pH"                          ), _TL(""          ), PARAMETER_INPUT                );

		Parameters.Add_Grids        ("", "FC"     , _TL("Field Capacity"              ), _TL(""          ), PARAMETER_OUTPUT               );
		Parameters.Add_Grids        ("", "PWP"    , _TL("Permanent Wilting Point"     ), _TL(""          ), PARAMETER_OUTPUT               );
		Parameters.Add_Grids        ("", "THETA_S", _TL("Water Capacity at Saturation"), _TL(""          ), PARAMETER_OUTPUT_OPTIONAL      );
	}
	else
	{
		Parameters.Add_Grid_or_Const("", "SAND"   , _TL("Sand"                        ), _TL("[%]"       ),   15.0,   0., true,  100., true);
		Parameters.Add_Grid_or_Const("", "SILT"   , _TL("Silt"                        ), _TL("[%]"       ),   37.0,   0., true,  100., true);
		Parameters.Add_Grid_or_Const("", "CLAY"   , _TL("Clay"                        ), _TL("[%]"       ),   48.0,   0., true,  100., true);
		Parameters.Add_Grid_or_Const("", "CORG"   , _TL("Soil Organic Carbon"         ), _TL("[permille]"),   15.0,   0., true, 1000., true);
		Parameters.Add_Grid_or_Const("", "BULK"   , _TL("Bulk Density"                ), _TL("[kg/m^3]"  ), 1350.0, 100., true, 2650., true);
		Parameters.Add_Grid_or_Const("", "CEC"    , _TL("Cation Exchange Capacity"    ), _TL("[cmol/kg]" ),   45.0,   0., true             );
		Parameters.Add_Grid_or_Const("", "PH"     , _TL("pH"                          ), _TL(""          ),    6.4,   0., true,   14., true);

		Parameters.Add_Grid         ("", "FC"     , _TL("Field Capacity"              ), _TL(""          ), PARAMETER_OUTPUT               );
		Parameters.Add_Grid         ("", "PWP"    , _TL("Permanent Wilting Point"     ), _TL(""          ), PARAMETER_OUTPUT               );
		Parameters.Add_Grid         ("", "THETA_S", _TL("Water Capacity at Saturation"), _TL(""          ), PARAMETER_OUTPUT_OPTIONAL      );
	}

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"UNIT"			, _TL("Output Unit"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("cubic-meter per cubic-meter"),
			_TL("percentage of volume")
		), 1
	);

	Parameters.Add_Choice("",
		"FUNCTION"		, _TL("Pedo-Transfer Function"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Hodnett & Tomasella 2002"),
			_TL("Toth et al. 2015")
		)
	);

	//-----------------------------------------------------
	Parameters.Add_Double("FC" , "PSI_FC" , _TL("Soil Moisture Potential"), _TL("[hPa]"),   316., 0., true);
	Parameters.Add_Double("PWP", "PSI_PWP", _TL("Soil Moisture Potential"), _TL("[hPa]"), 15850., 0., true);

	Parameters.Add_Bool("",
		"ADJUST"		, _TL("Adjustments"),
		_TL("Specifies whether to correct values of textures and bulk density to avoid creating nonsensical values."),
		true
	);

	Parameters.Add_Bool("",
		"USERDEF"		, _TL("User Defined Coefficients"),
		_TL(""),
		true
	);

	CSG_Table	&a = *Parameters.Add_FixedTable("USERDEF",
		"COEFFICIENTS"	, _TL("User Defined Coefficients"),
		_TL("")
	)->asTable();

	a.Destroy();
	a.Set_Name(_TL("User Defined Coefficients"));
	a.Add_Field("ln(alpha)", SG_DATATYPE_Double);
	a.Add_Field("ln(n)"    , SG_DATATYPE_Double);
	a.Add_Field("theta s"  , SG_DATATYPE_Double);
	a.Add_Field("theta r"  , SG_DATATYPE_Double);
	a.Set_Count(12);

	for(int j=0; j<12; j++)
	{
		for(int i=0; i<4; i++)
		{
			a[j].Set_Value(i, s_Coefficients[i][j]);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoil_Water_Capacity::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FUNCTION") )
	{
		pParameters->Set_Enabled("SAND"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("BULK"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("CEC"     , pParameter->asInt() == 0);
		pParameters->Set_Enabled("PH"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("THETA_S" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("PSI_FC"  , pParameter->asInt() == 0);
		pParameters->Set_Enabled("PSI_PWP" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("ADJUST"  , pParameter->asInt() == 0);
		pParameters->Set_Enabled("USERDEF" , pParameter->asInt() == 0);
	}

	if( pParameter->Cmp_Identifier("USERDEF") )
	{
		pParameters->Set_Enabled("COEFFICIENTS", pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Capacity::On_Execute(void)
{
	switch( Parameters("FUNCTION")->asInt() )
	{
	default: return( Get_HodnettTomasella() );
	case  1: return( Get_Toth            () );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Capacity::Get_HodnettTomasella(void)
{
	double	Scale	= Parameters("UNIT")->asInt() == 1 ? 100. : 1.;

	double	psi_FC	= Parameters("PSI_FC" )->asDouble() / 10.;	// [hPa] -> [kPa]
	double	psi_PWP	= Parameters("PSI_PWP")->asDouble() / 10.;	// [hPa] -> [kPa]

	bool	bAdjust	= Parameters("ADJUST")->asBool();

	//-----------------------------------------------------
	m_Coefficients.Create(12, 4);

	switch( Parameters("FUNCTION")->asInt() )
	{
	default: {
		for(int j=0; j<12; j++)
		{
			for(int i=0; i<4; i++)
			{
				m_Coefficients[i][j]	= s_Coefficients[i][j];
			}
		}
		break; }

	case  1: {
		CSG_Table &a = *Parameters("COEFFICIENTS")->asTable();

		if( a.Get_Count() != 12 )
		{
			Error_Set(_TL("User defined table needs to provide not less than 12 rows."));

			return( false );
		}

		for(int j=0; j<12; j++)
		{
			for(int i=0; i<4; i++)
			{
				m_Coefficients[i][j]	= a[j][i];
			}
		}
		break; }
	}

	//-----------------------------------------------------
	if( m_bGrids )
	{
		int	nz	= 0;

		#define CHECK_NZ(pGrids) if( pGrids && pGrids->Get_NZ() && (!nz || nz > pGrids->Get_NZ()) ) { nz = pGrids->Get_NZ(); }
		CSG_Grids *pSand = Parameters("SAND"  )->asGrids();	CHECK_NZ(pSand);
		CSG_Grids *pSilt = Parameters("SILT"  )->asGrids();	CHECK_NZ(pSilt);
		CSG_Grids *pClay = Parameters("CLAY"  )->asGrids();	CHECK_NZ(pClay);
		CSG_Grids *pBulk = Parameters("BULK"  )->asGrids();	CHECK_NZ(pBulk);
		CSG_Grids *pCorg = Parameters("CORG"  )->asGrids();	CHECK_NZ(pCorg);
		CSG_Grids *pCEC  = Parameters("CEC"   )->asGrids();	CHECK_NZ(pCEC );
		CSG_Grids *ppH   = Parameters("PH"    )->asGrids();	CHECK_NZ(ppH  );
		#undef CHECK_NZ

		if( nz == 0 )
		{
			Error_Set(_TL("Each grid collection must provide at least one z level."));

			return( false );
		}

		CSG_Grids *pFC	= Parameters("FC"     )->asGrids();	if( pFC  ) pFC ->Set_Grid_Count(nz);
		CSG_Grids *pPWP	= Parameters("PWP"    )->asGrids();	if( pPWP ) pPWP->Set_Grid_Count(nz);
		CSG_Grids *pT_s	= Parameters("THETA_S")->asGrids();	if( pT_s ) pT_s->Set_Grid_Count(nz);

		//-------------------------------------------------
		for(int z=0; z<nz && Process_Get_Okay(); z++)
		{
			Process_Set_Text("%s [%d/%d]", _TL("processing"), z + 1, nz);

			for(int y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				#pragma omp parallel for
				for(int x=0; x<Get_NX(); x++)
				{
					#define SET_NODATA(x, y, z) {\
						if( pFC  ) pFC ->Set_NoData(x, y, z);\
						if( pPWP ) pPWP->Set_NoData(x, y, z);\
						if( pT_s ) pT_s->Set_NoData(x, y, z);\
						continue;\
					}

					if(	(pSand && pSand->is_NoData(x, y, z))
					||	(pSilt && pSilt->is_NoData(x, y, z))
					||	(pClay && pClay->is_NoData(x, y, z))
					||	(pBulk && pBulk->is_NoData(x, y, z))
					||	(pCorg && pCorg->is_NoData(x, y, z))
					||	(pCEC  && pCEC ->is_NoData(x, y, z))
					||	(ppH   && ppH  ->is_NoData(x, y, z)) )
					{
						SET_NODATA(x, y, z);
					}

					//-------------------------------------
					double	Sand = pSand->asDouble(x, y, z);
					double	Silt = pSilt->asDouble(x, y, z);
					double	Clay = pClay->asDouble(x, y, z);
					double	Bulk = pBulk->asDouble(x, y, z);
					double	Corg = pCorg->asDouble(x, y, z);
					double	CEC  = pCEC ->asDouble(x, y, z);
					double	pH   = ppH  ->asDouble(x, y, z);

					//-------------------------------------
					double	alpha, n, theta_s, theta_r;

					if( !Get_HodnettTomasella(alpha, n, theta_s, theta_r, Sand, Silt, Clay, Bulk, Corg, CEC, pH, bAdjust) )
					{
						SET_NODATA(x, y, z);
					}

					if( pFC  ) pFC ->Set_Value(x, y, z, Scale * van_Genuchten(psi_FC , alpha, n, theta_s, theta_r));
					if( pPWP ) pPWP->Set_Value(x, y, z, Scale * van_Genuchten(psi_PWP, alpha, n, theta_s, theta_r));
					if( pT_s ) pT_s->Set_Value(x, y, z, Scale * theta_s);

					#undef SET_NODATA
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Grid *pSand = Parameters("SAND"   )->asGrid(); double cSand = Parameters("SAND")->asDouble();
		CSG_Grid *pSilt = Parameters("SILT"   )->asGrid(); double cSilt = Parameters("SILT")->asDouble();
		CSG_Grid *pClay = Parameters("CLAY"   )->asGrid(); double cClay = Parameters("CLAY")->asDouble();
		CSG_Grid *pBulk = Parameters("BULK"   )->asGrid(); double cBulk = Parameters("BULK")->asDouble();
		CSG_Grid *pCorg = Parameters("CORG"   )->asGrid(); double cCorg = Parameters("CORG")->asDouble();
		CSG_Grid *pCEC  = Parameters("CEC"    )->asGrid(); double cCEC  = Parameters("CEC" )->asDouble();
		CSG_Grid *ppH   = Parameters("PH"     )->asGrid(); double cpH   = Parameters("PH"  )->asDouble();

		CSG_Grid *pFC	= Parameters("FC"     )->asGrid();
		CSG_Grid *pPWP	= Parameters("PWP"    )->asGrid();
		CSG_Grid *pT_s	= Parameters("THETA_S")->asGrid();

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				#define SET_NODATA(x, y) {\
					if( pFC  ) pFC ->Set_NoData(x, y);\
					if( pPWP ) pPWP->Set_NoData(x, y);\
					if( pT_s ) pT_s->Set_NoData(x, y);\
					continue;\
				}

				if(	(pSand && pSand->is_NoData(x, y))
				||	(pSilt && pSilt->is_NoData(x, y))
				||	(pClay && pClay->is_NoData(x, y))
				||	(pBulk && pBulk->is_NoData(x, y))
				||	(pCorg && pCorg->is_NoData(x, y))
				||	(pCEC  && pCEC ->is_NoData(x, y))
				||	(ppH   && ppH  ->is_NoData(x, y)) )
				{
					SET_NODATA(x, y);
				}

				//-----------------------------------------
				double	Sand = pSand ? pSand->asDouble(x, y) : cSand;
				double	Silt = pSilt ? pSilt->asDouble(x, y) : cSilt;
				double	Clay = pClay ? pClay->asDouble(x, y) : cClay;
				double	Bulk = pBulk ? pBulk->asDouble(x, y) : cBulk;
				double	Corg = pCorg ? pCorg->asDouble(x, y) : cCorg;
				double	CEC  = pCEC  ? pCEC ->asDouble(x, y) : cCEC ;
				double	pH   = ppH   ? ppH  ->asDouble(x, y) : cpH  ;

				//-----------------------------------------
				double	alpha, n, theta_s, theta_r;

				if( !Get_HodnettTomasella(alpha, n, theta_s, theta_r, Sand, Silt, Clay, Bulk, Corg, CEC, pH, bAdjust) )
				{
					SET_NODATA(x, y);
				}

				if( pFC  ) pFC ->Set_Value(x, y, Scale * van_Genuchten(psi_FC , alpha, n, theta_s, theta_r));
				if( pPWP ) pPWP->Set_Value(x, y, Scale * van_Genuchten(psi_PWP, alpha, n, theta_s, theta_r));
				if( pT_s ) pT_s->Set_Value(x, y, Scale * theta_s);

				#undef SET_NODATA
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
inline double CSoil_Water_Capacity::van_Genuchten(double psi, double alpha, double n, double theta_s, double theta_r)
{
	return( theta_r + (theta_s - theta_r) / pow(1. + pow(alpha * psi, n), 1. - 1. / n) );
}

//---------------------------------------------------------
inline bool CSoil_Water_Capacity::Get_HodnettTomasella(double &alpha, double &n, double &theta_s, double &theta_r, double Sand, double Silt, double Clay, double Bulk, double Corg, double CEC, double pH, bool bAdjust)
{
	if( bAdjust )	// standardize sand, silt, clay and check bulk density...
	{
		double	Sum	= (Clay + Silt + Sand) / 100.;

		if( Sum <= 0. )
		{
			return( false );
		}

		Clay /= Sum; Silt /= Sum; Sand /= Sum;

		if( Bulk < 100.  ) { Bulk =  100.; } else if( Bulk > 2650. ) { Bulk = 2650.; } // density of quartz [kg/m^3]
	}

	//-----------------------------------------------------
	double	X[4];

	for(int i=0; i<4; i++)
	{
		X[i]	= // Note: Formula available from [http://www.sciencedirect.com/science/article/pii/S001670611200417X]
			( m_Coefficients[i][ 0]
			+ m_Coefficients[i][ 1] * Sand
			+ m_Coefficients[i][ 2] * Silt
			+ m_Coefficients[i][ 3] * Clay
			+ m_Coefficients[i][ 4] * Corg /   10.
			+ m_Coefficients[i][ 5] * Bulk / 1000.
			+ m_Coefficients[i][ 6] * CEC
			+ m_Coefficients[i][ 7] * pH
			+ m_Coefficients[i][ 8] * Silt*Silt
			+ m_Coefficients[i][ 9] * Clay*Clay
			+ m_Coefficients[i][10] * Sand*Silt
			+ m_Coefficients[i][11] * Sand*Clay
			) / 100.;
	}

	alpha   = exp(X[0]);
	n       = exp(X[1]);

	theta_s = X[2] > 100. ? 100. : X[2];
	theta_r = X[3] <   0. ?   0. : X[3];

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Capacity::Get_Toth(void)
{
	double	Scale	= Parameters("UNIT")->asInt() == 1 ? 100. : 1.;

	//-----------------------------------------------------
	if( m_bGrids )
	{
		int	nz	= 0;

		#define CHECK_NZ(pGrids) if( pGrids && pGrids->Get_NZ() && (!nz || nz > pGrids->Get_NZ()) ) { nz = pGrids->Get_NZ(); }
		CSG_Grids *pSilt = Parameters("SILT"  )->asGrids();	CHECK_NZ(pSilt);
		CSG_Grids *pClay = Parameters("CLAY"  )->asGrids();	CHECK_NZ(pClay);
		CSG_Grids *pCorg = Parameters("CORG"  )->asGrids();	CHECK_NZ(pCorg);
		#undef CHECK_NZ

		if( nz == 0 )
		{
			Error_Set(_TL("Each grid collection must provide at least one z level."));

			return( false );
		}

		CSG_Grids *pFC	 = Parameters("FC"  )->asGrids();	if( pFC  ) pFC ->Set_Grid_Count(nz);
		CSG_Grids *pPWP	 = Parameters("PWP" )->asGrids();	if( pPWP ) pPWP->Set_Grid_Count(nz);

		//-------------------------------------------------
		for(int z=0; z<nz && Process_Get_Okay(); z++)
		{
			Process_Set_Text("%s [%d/%d]", _TL("processing"), z + 1, nz);

			for(int y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				#pragma omp parallel for
				for(int x=0; x<Get_NX(); x++)
				{
					#define SET_NODATA(x, y, z) {\
						if( pFC  ) pFC ->Set_NoData(x, y, z);\
						if( pPWP ) pPWP->Set_NoData(x, y, z);\
						continue;\
					}

					if(	(pSilt && pSilt->is_NoData(x, y, z))
					||	(pClay && pClay->is_NoData(x, y, z))
					||	(pCorg && pCorg->is_NoData(x, y, z)) )
					{
						SET_NODATA(x, y, z);
					}

					double	Silt = pSilt->asDouble(x, y, z);
					double	Clay = pClay->asDouble(x, y, z);
					double	Corg = pCorg->asDouble(x, y, z);

					double	FC, PWP;

					if( !Get_Toth(FC, PWP, Silt, Clay, Corg) )
					{
						SET_NODATA(x, y, z);
					}

					if( pFC  ) pFC ->Set_Value(x, y, z, Scale * FC );
					if( pPWP ) pPWP->Set_Value(x, y, z, Scale * PWP);

					#undef SET_NODATA
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Grid *pSilt = Parameters("SILT")->asGrid(); double cSilt = Parameters("SILT")->asDouble();
		CSG_Grid *pClay = Parameters("CLAY")->asGrid(); double cClay = Parameters("CLAY")->asDouble();
		CSG_Grid *pCorg = Parameters("CORG")->asGrid(); double cCorg = Parameters("CORG")->asDouble();

		CSG_Grid *pFC	= Parameters("FC" )->asGrid();
		CSG_Grid *pPWP	= Parameters("PWP")->asGrid();

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				#define SET_NODATA(x, y) {\
					if( pFC  ) pFC ->Set_NoData(x, y);\
					if( pPWP ) pPWP->Set_NoData(x, y);\
					continue;\
				}

				if(	(pSilt && pSilt->is_NoData(x, y))
				||	(pClay && pClay->is_NoData(x, y))
				||	(pCorg && pCorg->is_NoData(x, y)) )
				{
					SET_NODATA(x, y);
				}

				//-----------------------------------------
				double	Silt = pSilt ? pSilt->asDouble(x, y) : cSilt;
				double	Clay = pClay ? pClay->asDouble(x, y) : cClay;
				double	Corg = pCorg ? pCorg->asDouble(x, y) : cCorg;

				double	FC, PWP;

				if( !Get_Toth(FC, PWP, Silt, Clay, Corg) )
				{
					SET_NODATA(x, y);
				}

				if( pFC  ) pFC ->Set_Value(x, y, Scale * FC );
				if( pPWP ) pPWP->Set_Value(x, y, Scale * PWP);

				#undef SET_NODATA
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
inline bool CSoil_Water_Capacity::Get_Toth(double &FC, double &PWP, double Silt, double Clay, double Corg)
{
	Corg	= 1. / (1. + Corg);

	FC	= 0.2449  - 0.1887   * Corg + 0.004527  * Clay + 0.001535 * Silt + 0.001442   * Silt * Corg - 0.00005110 * Silt * Clay + 0.0008676 * Clay * Corg;
	PWP	= 0.09878 + 0.002127 * Clay - 0.0008366 * Silt - 0.07670  * Corg + 0.00003853 * Silt * Clay + 0.002330   * Clay * Corg + 0.0009498 * Silt * Corg;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//# R-Script by Tomislav Hengl version 0.5-5: https://github.com/cran/GSIF/blob/master/R/AWCPTF.R
//
//# Note: Formula available from [http://www.sciencedirect.com/science/article/pii/S001670611200417X]
//
//AWCPTF <- function(SNDPPT, SLTPPT, CLYPPT, ORCDRC, BLD=1400, CEC, PHIHOX, h1=-10, h2=-20, h3=-31.6, pwp=-1585, PTF.coef, fix.values=TRUE, print.coef=TRUE){
// ## pedotransfer coefficients developed by Hodnett and Tomasella (2002)
// if(missing(PTF.coef)){
//   PTF.coef <- data.frame(
//     lnAlfa = c(-2.294, 0, -3.526, 0, 2.44, 0, -0.076, -11.331, 0.019, 0, 0, 0),
//     lnN = c(62.986, 0, 0, -0.833, -0.529, 0, 0, 0.593, 0, 0.007, -0.014, 0),
//     tetaS = c(81.799, 0, 0, 0.099, 0, -31.42, 0.018, 0.451, 0, 0, 0, -5e-04),
//     tetaR = c(22.733, -0.164, 0, 0, 0, 0, 0.235, -0.831, 0, 0.0018, 0, 0.0026)
//   )
// }
// ## standardize sand silt clay:
// if(fix.values){
//   sum.tex <- CLYPPT+SLTPPT+SNDPPT
//   CLYPPT <- CLYPPT/(sum.tex)*100
//   SLTPPT <- SLTPPT/(sum.tex)*100
//   SNDPPT <- SNDPPT/(sum.tex)*100
//   BLD[BLD<100] <- 100
//   BLD[BLD>2650] <- 2650  ## weight of quartz
// }
// ## rows:
// clm <- data.frame(SNDPPT, SLTPPT, CLYPPT, ORCDRC/10, BLD*0.001, CEC, PHIHOX, SLTPPT^2, CLYPPT^2, SNDPPT*SLTPPT, SNDPPT*CLYPPT)
// alfa <- apply(clm, 1, function(x){ exp((PTF.coef$lnAlfa[1] + sum(PTF.coef$lnAlfa[-1] * x))/100) })
// N <- apply(clm, 1, function(x){ exp((PTF.coef$lnN[1] + sum(PTF.coef$lnN[-1] * x))/100) })
// tetaS <- apply(clm, 1, function(x){ (PTF.coef$tetaS[1] + sum(PTF.coef$tetaS[-1] * x))/100 })
// tetaR <- apply(clm, 1, function(x){ (PTF.coef$tetaR[1] + sum(PTF.coef$tetaR[-1] * x))/100 })
// ## change negative of tetaR to 0
// tetaR[tetaR < 0] <- 0
// tetaS[tetaS > 100] <- 100
// m <- 1-1/N
// tetah1 <- tetaR + (tetaS-tetaR)/((1+(alfa*-1*h1)^N))^m
// tetah2 <- tetaR + (tetaS-tetaR)/((1+(alfa*-1*h2)^N))^m
// tetah3 <- tetaR + (tetaS-tetaR)/((1+(alfa*-1*h3)^N))^m
// WWP <- tetaR + (tetaS-tetaR)/((1+(alfa*-1*pwp)^N))^m
// if(fix.values){
//   ## if any of the tetah values is smaller than WWP, then replace:
//   sel <- which(WWP > tetah1 | WWP > tetah2 | WWP > tetah3)
//   if(length(sel)>0){ 
//     WWP[sel] <- apply(data.frame(tetah1[sel], tetah2[sel], tetah3[sel]), 1, function(x){min(x, na.rm=TRUE)}) 
//     warning(paste("Wilting point capacity for", length(sel), "points higher than h1, h2 and/or h3"))
//   }
// }
// AWCh1 <- tetah1 - WWP
// AWCh2 <- tetah2 - WWP
// AWCh3 <- tetah3 - WWP
// out <- data.frame(AWCh1=signif(AWCh1,3), AWCh2=signif(AWCh2,3), AWCh3=signif(AWCh3,3), WWP=signif(WWP,3), tetaS=signif(tetaS,3))
// if(print.coef==TRUE){
//   attr(out, "coef") <- as.list(PTF.coef)
//   attr(out, "PTF.names") <- list(variable=c("ai1", "sand", "silt", "clay", "oc", "bd", "cec", "ph", "silt^2", "clay^2", "sand*silt", "sand*clay"))
// }
// return(out)
//}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
