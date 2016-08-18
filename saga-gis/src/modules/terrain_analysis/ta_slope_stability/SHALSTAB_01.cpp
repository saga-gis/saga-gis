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
//                  ta_slope_stability                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    SHALSTAB_01.cpp                    //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                     Andreas Günther                   //
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
//    e-mail:     a.guenther@bgr.de                      //
//                                                       //
//    contact:    Andreas Günther                        //
//                B2.2								     //
//                BGR				                     //
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
#include "SHALSTAB_01.h"
#include <math.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSHALSTAB::CSHALSTAB(void)
{
	CSG_Parameters	P;

	//-----------------------------------------------------
	
	Set_Name		(_TL("SHALSTAB"));

	Set_Author		(SG_T("A. G&uuml;nther (c) 2012"));

	Set_Description	(_TW(
		"\n"
		"This tool is a realization of the SHALSTAB (""Shallow Slope Stability"") model from Montgomery & Dietrich (1994). "
		"The model computes grid cell critical shallow groundwater recharge values (CR in mm/day) as a measure for relative shallow slope stability, utilizing "
		"a simple model that combines a steady-state hydrologic model (a topographic wetness index) to predict groundwater pressures with an infinite slope stability model. "
		"For computation, a slope (in radians) and a catchment area (in m2) grid are required. "
		"Additionally, information on material density (g/cm3), material friction angle (&deg;), material hydraulic conductivity (m/hr), bulk cohesion (MPa) "
		"and depth to potential shear plane (m) are required that can be specified either globally or through grids. "
		"The tool produces a continuous CR (mm/day) raster with unconditionally stable cells blanked, and unconditionally unstable cells as CR = 0. "
		"Optionally, a classified CR grid can be calculated representing seven stability classes.\n"
		"\n"
		"Reference: "
		"<a href=\"http://www.agu.org/pubs/crossref/1994/93WR02979.shtml\">Montgomery D. R., Dietrich, W. E. (1994) A physically based model for the topographic control on shallow landsliding. Water Resources Research, 30, 1153-1171.</a>.\n"

	));

	Parameters.Add_Grid(
		NULL, "A", "Slope grid (rad)", "A slope angle grid (in radíans)", PARAMETER_INPUT
		);
	
	Parameters.Add_Grid(
		NULL, "B", "Catchment area grid (m2)", "A catchment area grid (in square meters)", PARAMETER_INPUT
		);
	
	Parameters.Add_Grid(
		NULL, "Cmin", "Min Density grid (g/cm3)", "A grid representing minimum material density (in g/cm3)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Cmax", "Max Density grid (g/cm3)", "A grid representing maximum material density (in g/cm3)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Dmin", "Min Hydraulic conductivity grid (m/hr) ", "A grid representing minimum material hydraulic conductivity (in m/hr)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Dmax", "Max Hydraulic conductivity grid (m/hr) ", "A grid representing maximum material hydraulic conductivity (in m/hr)", PARAMETER_INPUT_OPTIONAL
		);	

	Parameters.Add_Grid(
		NULL, "Emin", "Min Thickness grid (m)", "A grid representing minimum material thickness (in m)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Emax", "Max Thickness grid (m)", "A grid representing maximum material thickness (in m)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Fmin", "Min Friction angle grid (degree) ", "A grid representing minimum material friction angle (in degrees)", PARAMETER_INPUT_OPTIONAL
		);		

	Parameters.Add_Grid(
		NULL, "Fmax", "Max Friction angle grid (degree) ", "A grid representing maximum material friction angle (in degrees)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Jmin", "Min Bulk cohesion grid (MPa) ", "A grid representing minimum bulk cohesion", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Jmax", "Max Bulk cohesion grid (MPa) ", "A grid representing maximum bulk cohesion", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Value(
		NULL, "fCmin", "Global minimum density (g/cm3)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.6					//Initialisierung eines fixen wertes vs Grid für density
		);

	Parameters.Add_Value(
		NULL, "fCmax", "Global maximum density (g/cm3)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.6					//Initialisierung eines fixen wertes vs Grid für density
		);

	Parameters.Add_Value(
		NULL, "fDmin", "Global minimum conductivity (m/hr)", "Constant value if no raster set", PARAMETER_TYPE_Double, 2.7				//Initialisierung eines fixen wertes vs Grid für conductivity
		);

	Parameters.Add_Value(
		NULL, "fDmax", "Global maximum conductivity (m/hr)", "Constant value if no raster set", PARAMETER_TYPE_Double, 2.7				//Initialisierung eines fixen wertes vs Grid für conductivity
		);

	Parameters.Add_Value(
		NULL, "fEmin", "Global minimum thickness (m)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.0							//Initialisierung eines fixen wertes vs Grid für depth
		);

	Parameters.Add_Value(
		NULL, "fEmax", "Global maximum thickness (m)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.0							//Initialisierung eines fixen wertes vs Grid für depth
		);
	
	Parameters.Add_Value(
		NULL, "fFmin", "Global minimum friction angle (degree)", "Constant value if no raster set", PARAMETER_TYPE_Double, 33.0				//Initialisierung eines fixen wertes vs Grid für friction
		);

	Parameters.Add_Value(
		NULL, "fFmax", "Global maximum friction angle (degree)", "Constant value if no raster set", PARAMETER_TYPE_Double, 33.0				//Initialisierung eines fixen wertes vs Grid für friction
		);

	Parameters.Add_Value(
		NULL, "fJmin", "Global minimum bulk cohesion (MPa)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0							//Initialisierung eines fixen wertes vs Grid für cohesion
		);

	Parameters.Add_Value(
		NULL, "fJmax", "Global maximum bulk cohesion (MPa)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0							//Initialisierung eines fixen wertes vs Grid für cohesion
		);
	
	Parameters.Add_Value(
		NULL, "fK", "Parameter sampling runs", "Number of sampling cycles",PARAMETER_TYPE_Int, 1						//Initialisierung eines fixen wertes vs Grid für cohesion
		);
	
	Parameters.Add_Grid(
		NULL, "G", "CR values", "Resulting critical recharge (m/day) grid", PARAMETER_OUTPUT
		);

	Parameters.Add_Grid(
		NULL, "H", "CR classes", "Classified critical recharge (-) grid", PARAMETER_OUTPUT_OPTIONAL
		);

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CR_NODATA		= 0,
	CR_UNSTABLE,
	CR_0_TO_50,
	CR_50_TO_100,
	CR_100_TO_200,
	CR_200_TO_400,
	CR_GR_400,
	CR_STABLE,
	CR_COUNT
};

bool CSHALSTAB::On_Execute(void)
{
	double		fCmin	= Parameters("fCmin")->asDouble();
	double		fDmin	= Parameters("fDmin")->asDouble();
	double		fEmin	= Parameters("fEmin")->asDouble();
	double		fFmin	= Parameters("fFmin")->asDouble();	
	double		fJmin	= Parameters("fJmin")->asDouble();
	double		fCmax	= Parameters("fCmax")->asDouble();
	double		fDmax	= Parameters("fDmax")->asDouble();
	double		fEmax	= Parameters("fEmax")->asDouble();
	double		fFmax	= Parameters("fFmax")->asDouble();	
	double		fJmax	= Parameters("fJmax")->asDouble();
	double		fK		= Parameters("fK")->asInt();
	double		grav	= 9.81;

	CSG_Grid	*pA, *pB, *pCmin, *pDmin, *pEmin, *pFmin, *pG, *pH, *pJmin, *pCmax, *pDmax, *pEmax, *pFmax, *pJmax;

	pA		= Parameters("A"	)->asGrid();		//slope
	pB		= Parameters("B"	)->asGrid();		//flow accumulation
	pCmin	= Parameters("Cmin"	)->asGrid();		//density
	pDmin	= Parameters("Dmin"	)->asGrid();		//conductivity
	pEmin	= Parameters("Emin"	)->asGrid();		//depth
	pFmin	= Parameters("Fmin"	)->asGrid();		//friction
	pCmax	= Parameters("Cmax"	)->asGrid();		//density
	pDmax	= Parameters("Dmax"	)->asGrid();		//conductivity
	pEmax	= Parameters("Emax"	)->asGrid();		//depth
	pFmax	= Parameters("Fmax"	)->asGrid();		//friction
	pG		= Parameters("G"	)->asGrid();		//output critical recharge
	pH		= Parameters("H"	)->asGrid();		//optional output classified grid
	pJmin	= Parameters("Jmin"	)->asGrid();		//bulk cohesion
	pJmax	= Parameters("Jmax"	)->asGrid();		//bulk cohesion


	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double a, b, c, d, e, f, g, j, l;
			double cmin, dmin, emin, fmin, jmin;
			double cmax, dmax, emax, fmax, jmax;
			double cc, dd, ee, ff, jj;
			int cperc, dperc, eperc, fperc, jperc;
			int rand_int, k, n;

			a		=	pA->asDouble(x, y);
			b		=	pB->asDouble(x, y);						//Abfrage ob Raster oder Globalwerte:
			cmin	=	pCmin ? pCmin->asDouble(x, y) : fCmin;
			dmin	=	pDmin ? pDmin->asDouble(x, y) : fDmin;
			emin	=	pEmin ? pEmin->asDouble(x, y) : fEmin;
			fmin	=	pFmin ? pFmin->asDouble(x, y) : fFmin;
			jmin	=	pJmin ? pJmin->asDouble(x, y) : fJmin;
			cmax	=	pCmax ? pCmax->asDouble(x, y) : fCmax;
			dmax	=	pDmax ? pDmax->asDouble(x, y) : fDmax;
			emax	=	pEmax ? pEmax->asDouble(x, y) : fEmax;
			fmax	=	pFmax ? pFmax->asDouble(x, y) : fFmax;
			jmax	=	pJmax ? pJmax->asDouble(x, y) : fJmax;
			k		=	fK;

			if (pA->is_NoData(x, y) || pB->is_NoData(x, y))
			{
				pG->Set_NoData(x, y);

				if (pH)

					pH->Set_Value(x, y, CR_NODATA);
			}

			else if ((pCmin || pCmax || pDmin || pDmax || pEmin || pEmax || pFmin || pFmax || pJmin || pJmax) &&
				(pCmin && pCmax->is_NoData(x, y) || pDmin && pDmax->is_NoData(x, y) || pEmin && pEmax->is_NoData(x, y) ||
				pFmin && pFmax->is_NoData(x, y) || pJmin && pJmax->is_NoData(x, y)))
			{
				pG->Set_NoData(x, y);

				if (pH)

					pH->Set_Value(x, y, CR_NODATA);
			}

			else
			{

				cperc = ((cmax - cmin) / cmax) * 100;				//calculate parameter range %: density
				if (cperc > 0)
				{
					n = 0;
					cc = 0;
					while ( n < k)									//loop through specified random number iterations:
					{
						rand_int = rand() % cperc + 0;				//calculate random percentage
						c = ((cmax/100) * rand_int) + cmin;			//calculate value
						cc = cc + c;								//sum
						n = n + 1;
					}
					c = cc / n;										// calculate mean from random values
				}
				else 
				{
					c = cmax;
				}

				dperc = ((dmax - dmin) / dmax) * 100;
				if (dperc > 0)
				{
					n = 0;
					dd = 0;
					while ( n < k) 
					{
						rand_int = rand() % dperc + 0;
						d = ((dmax/100) * rand_int) + dmin;
						dd = dd + d;
						n = n + 1;
					}
					d = dd / n;
				}
				else 
				{
					d = dmax;
				}

				eperc = ((emax - emin) / emax) * 100;
				if (eperc > 0)
				{
					n = 0;
					ee = 0;
					while ( n < k) 
					{
						rand_int = rand() % eperc + 0;
						e = ((emax/100) * rand_int) + emin;
						ee = ee + e;
						n = n + 1;
					}
					e = ee / n;
				}
				else 
				{
					e = emax;
				}

				fperc = ((fmax - fmin) / fmax) * 100;
				if (fperc > 0)
				{
					n = 0;
					ff = 0;
					while ( n < k) 
					{
						rand_int = rand() % fperc + 0;
						f = ((fmax/100) * rand_int) + fmin;
						ff = ff + f;
						n = n + 1;
					}
					f = ff / n;
				}
				else 
				{
					f = fmax;
				}

				jperc = ((jmax - jmin) / jmax) * 100;
				if (jperc > 0)
				{
					n = 0;
					jj = 0;
					while ( n < k) 
					{
						rand_int = rand() % jperc + 0;
						j = ((jmax/100) * rand_int) + jmin;
						jj = jj + j;
						n = n + 1;
					}
					j = jj / n;
				}
				else 
				{
					j = jmax;
				}
				
					
				l = j/((e/cos(a))*c*grav);											//calculate dimensionless cohesion 

				//g	=	((((sin(a) * (d*24*e)) * c)/(b/pB->Get_Cellsize())) * (1-(tan(a)/tan(f/57.2958)))) *1000;								//cohesionless SHALSTAB calculation
				g	=	((((sin(a) * (d*24*e)) * c)/(b/pB->Get_Cellsize())) * (1-((sin(a) - l)/(cos(a)*tan(f/57.2958))))) *1000;				//SHALSTAB calculation
		
			
				//if ((tan(a) > (tan(f/57.2958) * (1-(1/c)))))							//general stability criterion
				if (tan(a) > ((tan(f/57.2958) * (1-(1/c)))+(j/cos(a))))							
				{
					if (g > 0)															//critical recharge							
						pG->Set_Value(x, y, g);
					else
						pG->Set_Value(x, y, 0);											//unconditionally unstable = 0
				}

				//else if (tan(a) <= (tan(f/57.2958) * (1-(1/c))))
				else if (tan(a) <= ((tan(f/57.2958) * (1-(1/c)))+(l/cos(a))))
				{
					pG->Set_NoData(x, y);													//unconditionally stable = nodata
				}

				if (pH)																	//calculate optional classified grid
				{
				
					//if ((tan(a) > (tan(f/57.2958) * (1-(1/c)))))
					if (tan(a) > ((tan(f/57.2958) * (1-(1/c)))+(l/cos(a))))
					{
						if (g <= 0)
							pH->Set_Value(x, y, CR_UNSTABLE);
						else if ((g > 0) && (g <= 50))
							pH->Set_Value(x, y, CR_0_TO_50);
						else if ((g > 50) && (g <= 100))
							pH->Set_Value(x, y, CR_50_TO_100);
						else if ((g > 100) && (g <= 200))
							pH->Set_Value(x, y, CR_100_TO_200);
						else if ((g > 200) && (g <= 400))
							pH->Set_Value(x, y, CR_200_TO_400);
						else if (g > 400)
							pH->Set_Value(x, y, CR_GR_400);
					}

					//else if (tan(a) <= (tan(f/57.2958) * (1-(1/c))))
					else if (tan(a) <= ((tan(f/57.2958) * (1-(1/c)))+(l/cos(a))))
					{
						pH->Set_Value(x, y, CR_STABLE);
					}
				}
			}
		}
	}

	//-----------------------------------------------------

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pH, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int CR_Colors[CR_COUNT]	=
		{
			SG_GET_RGB(255, 255, 255),  // CR_NODATA
			SG_GET_RGB(255,   0,   0),	// CR_UNSTAB
			SG_GET_RGB(255,	128,  64),	// CR_0-50
			SG_GET_RGB(255, 255,   0),	// CR_50-100
			SG_GET_RGB(  0, 255,   0),	// CR_100-200
			SG_GET_RGB(128, 255, 255),	// CR_200-400
			SG_GET_RGB(  0,   0, 255),	// CR_>400
			SG_GET_RGB(128, 128, 128),  // CR_STABLE
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL("No data");				Desc	+= _TL("");
		Name	+= _TL("Uncond. unstable");		Desc	+= _TL("");
		Name	+= _TL("0-50 mm/day");			Desc	+= _TL("");
		Name	+= _TL("50-100 mm/day");		Desc	+= _TL("");
		Name	+= _TL("100-200 mm/day");		Desc	+= _TL("");
		Name	+= _TL("200-400 mm/day");		Desc	+= _TL("");
		Name	+= _TL(">400 mm/day");			Desc	+= _TL("");
		Name	+= _TL("Uncond. stable");		Desc	+= _TL("");

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<CR_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, CR_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);				// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pH, P);
	}

	return( true );

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
