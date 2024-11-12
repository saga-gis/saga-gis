
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     imagery_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   landsat_acca.cpp                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
//            Benjamin Bechtel & Olaf Conrad             //
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
#include "landsat_acca.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandsat_ACCA::CLandsat_ACCA(void)
{
	Set_Name		(_TL("Automated Cloud Cover Assessment"));

	Set_Author		("B.Bechtel, O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Automated Cloud-Cover Assessment (ACCA) for Landsat TM/ETM+ imagery as proposed by Irish (2000). "
		"This tool incorporates E.J. Tizado's GRASS GIS implementation (i.landsat.acca)."
	));

	Add_Reference("Irish, R.R.", "2000",
		"Landsat 7 Automatic Cloud Cover Assessment",
		"In: Shen, S.S., Descour, M.R. [Eds.]: Algorithms for Multispectral, Hyperspectral, and Ultraspectral Imagery VI. Proceedings of SPIE, 4049: 348-355.",
		SG_T("https://doi.org/10.1117/12.410358"), SG_T("doi:10.1117/12.410358")
	);

	Add_Reference("Irish, R.R., Barker, J.L., Goward, S.N., Arvidson, T.", "2006",
		"Characterization of the Landsat-7 ETM+ Automated Cloud-Cover Assessment (ACCA) Algorithm.",
		"Photogrammetric Engineering and Remote Sensing vol. 72(10): 1179-1188.",
		SG_T("https://doi.org/10.14358/PERS.72.10.1179"), SG_T("doi:10.14358/PERS.72.10.1179")
	);

	Add_Reference("Tizado, E.J.", "2010",
		"GRASS GIS i.landsat.acca",
		"E.J. Tizado's implementation of the method proposed by Irish (2000), Irish et al. (2006).",
		SG_T("https://github.com/OSGeo/grass/tree/main/imagery/i.landsat.acca"), SG_T("Source Code") // https://grass.osgeo.org/grass72/manuals/i.landsat.acca.html
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "BAND2", _TL("Green"  ), _TL("Reflectance"), PARAMETER_INPUT);
	Parameters.Add_Grid("", "BAND3", _TL("Red"    ), _TL("Reflectance"), PARAMETER_INPUT);
	Parameters.Add_Grid("", "BAND4", _TL("NIR"    ), _TL("Reflectance"), PARAMETER_INPUT);
	Parameters.Add_Grid("", "BAND5", _TL("SWIR"   ), _TL("Reflectance"), PARAMETER_INPUT);
	Parameters.Add_Grid("", "BAND6", _TL("Thermal"), _TL("Kelvin"     ), PARAMETER_INPUT, false);

	Parameters.Add_Grid("",
		"CLOUD" , _TL("Clouds"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"B56C"  , _TL("SWIR/Thermal Threshold"),
		_TL("Threshold for SWIR/Thermal Composite (step 6)."),
		225.
	);

	Parameters.Add_Double("",
		"B45R"  , _TL("Desert Detection Threshold"),
		_TL("Threshold for desert detection (step 10,  NIR/SWIR Ratio)."),
		1.
	);

	Parameters.Add_Int("",
		"HIST_N", _TL("Temperature Histogram"),
		_TL("Number of classes in the cloud temperature histogram."),
		100, 10, true
	);

	Parameters.Add_Bool("",
		"CSIG"  , _TL("Cloud Signature"),
		_TL("Always use cloud signature (step 14)."),
		true
	);

	Parameters.Add_Bool("",
		"PASS2" , _TL("Cloud Differentiation"),
		_TL("Differentiate between warm (not ambiguous) and cold clouds."),
		false
	);

	Parameters.Add_Bool("",
		"SHADOW", _TL("Shadows"),
		_TL("Include a category for cloud shadows."),
		true
	);

	Parameters.Add_Bool("",
		"FILTER", _TL("Filter"),
		_TL("Apply post-processing filter to remove small holes."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_ACCA::On_Execute(void)
{
	CSG_Grid *pCloud = Parameters("CLOUD")->asGrid();

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pCloud, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		#define LUT_SET_CLASS(id, name, color)	{ CSG_Table_Record *pR = pLUT->asTable()->Add_Record(); pR->Set_Value(0, color); pR->Set_Value(1, name); pR->Set_Value(3, id); pR->Set_Value(3, id); }

		if( Parameters("PASS2")->asBool() )
		{
			LUT_SET_CLASS(CACCA::IS_COLD_CLOUD, _TL("Cold Cloud"), SG_COLOR_BLUE);
			LUT_SET_CLASS(CACCA::IS_WARM_CLOUD, _TL("Warm Cloud"), SG_COLOR_BLUE_LIGHT);
		}
		else
		{
			LUT_SET_CLASS(CACCA::IS_COLD_CLOUD, _TL("Cloud"     ), SG_COLOR_BLUE_LIGHT);
		}

		LUT_SET_CLASS(CACCA::IS_SHADOW    , _TL("Shadow"    ), SG_COLOR_RED);

		DataObject_Set_Parameter(pCloud, pLUT);

		DataObject_Set_Parameter(pCloud, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	pCloud->Set_NoData_Value(0.);

	//-----------------------------------------------------
	CSG_Grid *pBands[5] = {
		Parameters("BAND2")->asGrid(),
		Parameters("BAND3")->asGrid(),
		Parameters("BAND4")->asGrid(),
		Parameters("BAND5")->asGrid(),
		Parameters("BAND6")->asGrid()
	};

	//-----------------------------------------------------
	CACCA ACCA;

	ACCA.acca_algorithm(pCloud, pBands,
		Parameters("PASS2" )->asBool() ? 0 : 1,
		Parameters("SHADOW")->asBool() ? 1 : 0,
		Parameters("CSIG"  )->asBool() ? 1 : 0,
		Parameters("HIST_N")->asInt()
	);

	if( Parameters("FILTER")->asBool() )
	{
		ACCA.filter_holes(pCloud);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/****************************************************************************
 *
 * TOOL:	   i.landsat.acca
 *
 * AUTHOR(S):	E. Jorge Tizado - ej.tizado@unileon.es
 *
 * PURPOSE:	  Landsat TM/ETM+ Automatic Cloud Cover Assessment
 *			   Adopted for GRASS 7 by Martin Landa <landa.martin gmail.com>
 *
 * COPYRIGHT:	(C) 2008, 2010 by the GRASS Development Team
 *
 *			   This program is free software under the GNU General Public
 *   			 License (>=v2). Read the file COPYING that comes with GRASS
 *   			 for details.
 *
 *****************************************************************************/

//---------------------------------------------------------
#define G_message printf

#define TRUE 1

enum
{
	BAND2 = 0, BAND3, BAND4, BAND5, BAND6
};

enum
{
	NO_CLOUD = 0, IS_CLOUD = 1, COLD_CLOUD = 30, WARM_CLOUD = 50
};

//---------------------------------------------------------
#define SCALE    200.
#define K_BASE   230.

 /* value and count */
#define TOTAL    0
#define WARM     1
#define COLD     2
#define SNOW     3
#define SOIL     4

 /* signa */
#define COVER    1
#define SUM_COLD 0
#define SUM_WARM 1
#define KMEAN    2
#define KMAX     3
#define KMIN     4

 /* reuse value */
#define KLOWER   0
#define KUPPER   1
#define MEAN     2
#define SKEW     3
#define DSTD     4

/**********************************************************
 *
 * Automatic Cloud Cover Assessment (ACCA): Irish 2000
 *
 **********************************************************/

/*--------------------------------------------------------
  CONSTANTS
  Usar esta forma para que via extern puedan modificarse
  como opciones desde el programa main.
 ---------------------------------------------------------*/

double th_1 = 0.08; /* Band 3 Brightness Threshold */
double th_1_b = 0.07;
double th_2[] = {-0.25, 0.70}; /* Normalized Snow Difference Index */

double th_2_b = 0.8;
double th_3 = 300.; /* Band 6 Temperature Threshold */
double th_4 = 225.; /* Band 5/6 Composite */
double th_4_b = 0.08;
double th_5 = 2.35;    /* Band 4/3 Ratio */
double th_6 = 2.16248; /* Band 4/2 Ratio */
double th_7 = 1.0;     /* Band 4/5 Ratio */

double th_8 = 210.; /* Band 5/6 Composite */

//---------------------------------------------------------
void CACCA::acca_algorithm(CSG_Grid *pCloud, CSG_Grid *band[], int single_pass, int with_shadow, int cloud_signature, int _hist_n)
{
	hist_n = _hist_n; CSG_Array_Int _hist_cold(_hist_n), _hist_warm(_hist_n);

	int		i, count[5], *hist_cold = _hist_cold.Get_Array(), *hist_warm = _hist_warm.Get_Array(), review_warm;
	double	max, value[5], signa[5], idesert, shift;

	/* Reset variables ... */
	for (i = 0; i < 5; i++) {
		count[i] = 0;
		value[i] = 0.;
	}
	for (i = 0; i < hist_n; i++) {
		hist_cold[i] = hist_warm[i] = 0;
	}

	/* FIRST FILTER ... */
	acca_first(pCloud, band, with_shadow, count, hist_cold, hist_warm, signa);
	/* CATEGORIES: NO_DEFINED, WARM_CLOUD, COLD_CLOUD, NULL (= NO_CLOUD) */

	value[WARM] = (double)count[WARM] / (double)count[TOTAL];
	value[COLD] = (double)count[COLD] / (double)count[TOTAL];
	value[SNOW] = (double)count[SNOW] / (double)count[TOTAL];
	value[SOIL] = (double)count[SOIL] / (double)count[TOTAL];

	value[0] = (double)(count[WARM] + count[COLD]);
	idesert = (value[0] == 0. ? 0. : value[0] / ((double)count[SOIL]));

	/* BAND-6 CLOUD SIGNATURE DEVELOPMENT */
	if (idesert <= .5 || value[SNOW] > 0.01) {
		/* Only the cold clouds are used
		if snow or desert soil is present */
		review_warm = 1;
	}
	else {
		/* The cold and warm clouds are combined
		and treated as a single population */
		review_warm = 0;
		count[COLD] += count[WARM];
		value[COLD] += value[WARM];
		signa[SUM_COLD] += signa[SUM_WARM];
		for (i = 0; i < hist_n; i++)
			hist_cold[i] += hist_warm[i];
	}

	signa[KMEAN] = SCALE * signa[SUM_COLD] / ((double)count[COLD]);
	signa[COVER] = ((double)count[COLD]) / ((double)count[TOTAL]);

	G_message(("Preliminary scene analysis:"));
	G_message(("* Desert index: %.2lf"), idesert);
	G_message(("* Snow cover: %.2lf %%"), 100. * value[SNOW]);
	G_message(("* Cloud cover: %.2lf %%"), 100. * signa[COVER]);
	G_message(("* Temperature of clouds:"));
	G_message(("** Maximum: %.2lf K"), signa[KMAX]);
	G_message(("** Mean (%s cloud): %.2lf K"), (review_warm ? "cold" : "all"),
		signa[KMEAN]);
	G_message(("** Minimum: %.2lf K"), signa[KMIN]);

	/* WARNING: reuse of the variable 'value' with new meaning */

	/* step 14 */

	/* To correct Irish2006: idesert has to be bigger than 0.5 to start pass 2
	processing (see Irish2000) because then we have no desert condition
	(thanks to Matthias Eder, Germany) */
	if (cloud_signature ||
		(idesert > .5 && signa[COVER] > 0.004 && signa[KMEAN] < 295.)) {
		G_message(("Histogram cloud signature:"));

		value[MEAN] = quantile(0.5, hist_cold) + K_BASE;
		value[DSTD] = sqrt(moment(2, hist_cold, 1));
		value[SKEW] = moment(3, hist_cold, 3) / pow(value[DSTD], 3);

		G_message(("* Mean temperature: %.2lf K"), value[MEAN]);
		G_message(("* Standard deviation: %.2lf"), value[DSTD]);
		G_message(("* Skewness: %.2lf"), value[SKEW]);
		G_message(("* Histogram classes: %d"), hist_n);

		shift = value[SKEW];
		if (shift > 1.)
			shift = 1.;
		else if (shift < 0.)
			shift = 0.;

		max = quantile(0.9875, hist_cold) + K_BASE;
		value[KUPPER] = quantile(0.975, hist_cold) + K_BASE;
		value[KLOWER] = quantile(0.835, hist_cold) + K_BASE;

		G_message(("* 98.75 percentile: %.2lf K"), max);
		G_message(("* 97.50 percentile: %.2lf K"), value[KUPPER]);
		G_message(("* 83.50 percentile: %.2lf K"), value[KLOWER]);

		/* step 17 & 18 */
		if (shift > 0.) {
			shift *= value[DSTD];

			if ((value[KUPPER] + shift) > max) {
				if ((value[KLOWER] + shift) > max) {
					value[KLOWER] += (max - value[KUPPER]);
				}
				else {
					value[KLOWER] += shift;
				}
				value[KUPPER] = max;
			}
			else {
				value[KLOWER] += shift;
				value[KUPPER] += shift;
			}
		}

		G_message(("Maximum temperature:"));
		G_message(("* Cold cloud: %.2lf K"), value[KUPPER]);
		G_message(("* Warm cloud: %.2lf K"), value[KLOWER]);
	}
	else {
		if (signa[KMEAN] < 295.) {
			/* Retained warm and cold clouds */
			G_message(("Result: Scene with clouds"));
			review_warm = 0;
			value[KUPPER] = 0.;
			value[KLOWER] = 0.;
		}
		else {
			/* Retained cold clouds */
			G_message(("Result: Scene cloud free"));
			review_warm = 1;
			value[KUPPER] = 0.;
			value[KLOWER] = 0.;
		}
	}

	/* SECOND FILTER ... */
	/* By-pass two processing but it retains warm and cold clouds */
	if (single_pass == TRUE) {
		review_warm = -1.;
		value[KUPPER] = 0.;
		value[KLOWER] = 0.;
	}
	acca_second(pCloud, band[BAND6], review_warm, value[KUPPER], value[KLOWER]);
	/* CATEGORIES: IS_WARM_CLOUD, IS_COLD_CLOUD, IS_SHADOW, NULL (= NO_CLOUD) */

	return;
}

//---------------------------------------------------------
void CACCA::acca_first(CSG_Grid *pCloud, CSG_Grid *band[], int with_shadow, int count[], int cold[], int warm[], double stats[])
{
	SG_UI_Msg_Add_Execution(_TL("Processing first pass..."), true);

	stats[SUM_COLD] = 0.;
	stats[SUM_WARM] = 0.;
	stats[KMAX] = 0.;
	stats[KMIN] = 10000.;

	for(int y=0; y<pCloud->Get_NY() && SG_UI_Process_Set_Progress(y, pCloud->Get_NY()); y++)
	{
		for(int x=0; x<pCloud->Get_NX(); x++)
		{
			char code = NO_DEFINED;
			double pixel[5], nsdi, rat56;

			for(int i=BAND2; i<=BAND6; i++)	// Null when null pixel in any band
			{
				if( pCloud->Get_System() == band[i]->Get_System() )
				{
					if( band[i]->is_NoData(x, y) )
					{
						code = NO_CLOUD;
						break;
					}

					pixel[i] = band[i]->asDouble(x, y);
				}
				else if( !band[i]->Get_Value(pCloud->Get_System().Get_Grid_to_World(x, y), pixel[i]) )
				{
					code = NO_CLOUD;
					break;
				}

				if( m_bCelsius && i == BAND6 && code == NO_DEFINED ) { pixel[i] -= 273.15; }
			}

			/* Determina los pixeles de sombras */
			if (code == NO_DEFINED && with_shadow) {
				code = shadow_algorithm(pixel);
			}
			/* Analiza el valor de los pixeles no definidos */
			if (code == NO_DEFINED) {
				code = NO_CLOUD;
				count[TOTAL]++;
				nsdi = (pixel[BAND2] - pixel[BAND5]) /
				       (pixel[BAND2] + pixel[BAND5]);
				/* ----------------------------------------------------- */
				/* step 1. Brightness Threshold: Eliminates dark images */
				if (pixel[BAND3] > th_1) {
					/* step 3. Normalized Snow Difference Index: Eliminates many
					* types of snow */
					if (nsdi > th_2[0] && nsdi < th_2[1]) {
						/* step 5. Temperature Threshold: Eliminates warm image
						* features */
						if (pixel[BAND6] < th_3) {
							rat56 = (1. - pixel[BAND5]) * pixel[BAND6];
							/* step 6. Band 5/6 Composite: Eliminates numerous
							* categories including ice */
							if (rat56 < th_4) {
								/* step 8. Eliminates growing vegetation */
								if ((pixel[BAND4] / pixel[BAND3]) < th_5) {
									/* step 9. Eliminates senescing vegetation
									*/
									if ((pixel[BAND4] / pixel[BAND2]) < th_6) {
										/* step 10. Eliminates rocks and desert
										*/
										count[SOIL]++;
										if ((pixel[BAND4] / pixel[BAND5]) >
											th_7) {
											/* step 11. Distinguishes warm
											* clouds from cold clouds */
											if (rat56 < th_8) {
												code = COLD_CLOUD;
												count[COLD]++;
												/* for statistic */
												stats[SUM_COLD] +=
													(pixel[BAND6] / SCALE);
												hist_put(pixel[BAND6] - K_BASE,
													cold);
											}
											else {
												code = WARM_CLOUD;
												count[WARM]++;
												/* for statistic */
												stats[SUM_WARM] +=
													(pixel[BAND6] / SCALE);
												hist_put(pixel[BAND6] - K_BASE,
													warm);
											}
											if (pixel[BAND6] > stats[KMAX])
												stats[KMAX] = pixel[BAND6];
											if (pixel[BAND6] < stats[KMIN])
												stats[KMIN] = pixel[BAND6];
										}
										else {
											code = NO_DEFINED;
										}
									}
									else {
										code = NO_DEFINED;
										count[SOIL]++;
									}
								}
								else {
									code = NO_DEFINED;
								}
							}
							else {
								/* step 7 */
								code = (pixel[BAND5] < th_4_b) ? NO_CLOUD
									: NO_DEFINED;
							}
						}
						else {
							code = NO_CLOUD;
						}
					}
					else {
						/* step 3 */
						code = NO_CLOUD;
						if (nsdi > th_2_b)
							count[SNOW]++;
					}
				}
				else {
					/* step 2 */
					code = (pixel[BAND3] < th_1_b) ? NO_CLOUD : NO_DEFINED;
				}
				/* ----------------------------------------------------- */
			}
			if (code == NO_CLOUD) {
				pCloud->Set_Value(x, y, NO_DEFINED);
			}
			else {
				pCloud->Set_Value(x, y, code);
			}
		}
	}

	return;
}

//---------------------------------------------------------
void CACCA::acca_second(CSG_Grid *pCloud, CSG_Grid *pThermal, int review_warm, double upper, double lower)
{
	if( m_bCelsius ) { upper -= 273.15; lower -= 273.15; }

	SG_UI_Process_Set_Text(upper == 0.
		? _TL("Removing ambiguous pixels...")
		: _TL("Pass two processing...")
	);

	//-----------------------------------------------------
	for(int y=0; y<pCloud->Get_NY() && SG_UI_Process_Set_Progress(y, pCloud->Get_NY()); y++)
	{
		double p_y = pCloud->Get_YMin() + y * pCloud->Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<pCloud->Get_NX(); x++)
		{
			double p_x = pCloud->Get_XMin() + x * pCloud->Get_Cellsize(), temp;

			if( pThermal->Get_Value(p_x, p_y, temp) )
			{
				int code = pCloud->asInt(x, y);
				/* Resolve ambiguous pixels */
				if(code == NO_DEFINED ) continue; if(
					(code == WARM_CLOUD && review_warm == 1)) {
					if (temp > upper) {
						pCloud->Set_Value(x, y, NO_DEFINED);
					}
					else {
						pCloud->Set_Value(x, y, (temp < lower) ? IS_WARM_CLOUD : IS_COLD_CLOUD);
					}
				}
				else
					/* Join warm (not ambiguous) and cold clouds */
					if (code == COLD_CLOUD || code == WARM_CLOUD) {
						pCloud->Set_Value(x, y,
							(code == WARM_CLOUD && review_warm == 0)
							? IS_WARM_CLOUD
							: IS_COLD_CLOUD
						);
					}
					else
						pCloud->Set_Value(x, y, IS_SHADOW);
			}
		}
	}

	//-----------------------------------------------------
	return;
}


///////////////////////////////////////////////////////////
//														 //
//					Cloud shadows						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CACCA::shadow_algorithm(double pixel[])
{
	// I think this filter is better but not in any paper
	if( pixel[BAND3] < 0.07 && (1 - pixel[BAND4]) * pixel[BAND6] > 240. && pixel[BAND4] / pixel[BAND2] > 1.
	&& (pixel[BAND3] - pixel[BAND5]) / (pixel[BAND3] + pixel[BAND5]) < 0.10 )
//	if( pixel[BAND3] < 0.07 && (1 - pixel[BAND4]) * pixel[BAND6] > 240. && pixel[BAND4] / pixel[BAND2] > 1. )
	{
		return IS_SHADOW;
	}

	return NO_DEFINED;
}


///////////////////////////////////////////////////////////
//														 //
//				  HISTOGRAM ANALYSIS					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Define un factor de escala = hist_n/100 con objeto
// de dividir el entero 1 por 100/hist_n partes y
// aumentar la precision.
//
// Afecta al almacenamiento en el histograma pero
// modifica el calculo de quantiles y momentos.
//---------------------------------------------------------

//---------------------------------------------------------
void CACCA::hist_put(double t, int hist[])
{
	int	i	= (int)(t * ((double)hist_n / 100.));	// scale factor

	if( i < 1 )
		i	= 1;
	else if (i > hist_n)
		i	= hist_n;

	hist[i - 1]++;
}

//---------------------------------------------------------
/* histogram moment */
double CACCA::moment(int n, int hist[], int k)
{
	int		i, total;
	double	value, mean;

	for(i=0, total=0, mean=0; i<hist_n; i++)
	{
		total	+= hist[i];
		mean	+= (double)(i * hist[i]);
	}

	mean	/= ((double)total);	/* histogram mean */

	for(i=0, value=0.; i<hist_n; i++)
	{
		value	+= (pow((i - mean), n) * ((double)hist[i]));
	}

	value	/= (double)(total);	// k	= 0;	// ???!!!
//	value	/= (double)(total - k);

	return( value / pow((double)hist_n / 100., n) );
}

//---------------------------------------------------------
/* Real data quantile */
double CACCA::quantile(double q, int hist[])
{
	int		i, total;
	double	value, qmax, qmin;

	for(i=0, total=0; i<hist_n; i++)
	{
		total += hist[i];
	}

	for(i=hist_n-1, value=0, qmax=1.; i>=0; i--)
	{
		qmin	= qmax - (double)hist[i] / (double)total;

		if( q >= qmin )
		{
			value = (q - qmin) / (qmax - qmin) + (i - 1);
			break;
		}

		qmax	= qmin;
	}

	/* remove scale factor */
	return (value / ((double)hist_n / 100.));
}


///////////////////////////////////////////////////////////
//														 //
//				FILTER HOLES OF CLOUDS					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// This a >=50% filter of 3x3
// if >= 50% vecinos cloud then pixel set to cloud
//---------------------------------------------------------
void CACCA::filter_holes(CSG_Grid *pCloud)
{
	if( pCloud->Get_NY() < 3 || pCloud->Get_NX() < 3 )
		return;

	SG_UI_Process_Set_Text(_TL("Filling small holes in clouds..."));

	CSG_Grid Cloud(*pCloud);

	//-----------------------------------------------------
	for(int y=0; y<pCloud->Get_NY() && SG_UI_Process_Set_Progress(y, pCloud->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pCloud->Get_NX(); x++)
		{
			int z = Cloud.asInt(x, y);

			if( z == 0 )
			{
				int cold = 0, warm = 0, shadow = 0, nulo = 0;

				for(int i=0; i<8; i++)
				{
					int ix = pCloud->Get_System().Get_xTo(i, x);
					int iy = pCloud->Get_System().Get_yTo(i, y);

					switch( Cloud.is_InGrid(ix, iy) ? Cloud.asInt(ix, iy) : -1 )
					{
					case IS_COLD_CLOUD: cold  ++; break;
					case IS_WARM_CLOUD: warm  ++; break;
					case IS_SHADOW    : shadow++; break;
					default           : nulo  ++; break;
					}
				}

				int lim = (cold + warm + shadow + nulo) / 2;

				// Entra pixel[0] = 0
				if( nulo < lim )
				{
					if( shadow >= (cold + warm) )
						z = IS_SHADOW;
					else
						z = (warm > cold) ? IS_WARM_CLOUD : IS_COLD_CLOUD;
				}
			}

			if( z != 0 )
			{
				pCloud->Set_Value(x, y, z);
			}
			else
			{
				pCloud->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return;
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
