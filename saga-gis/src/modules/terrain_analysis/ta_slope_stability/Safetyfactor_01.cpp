/**********************************************************
 * Version $Id: template.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Andis modules                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Safetyfactor_01.cpp               //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
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
#include "Safetyfactor_01.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSafetyfactor::CSafetyfactor(void)
{
	Set_Name		(_TL("SAFETYFACTOR"));

	Set_Author		(SG_T("A. Günther (c) 2012"));

	Set_Description	(_TW(
		"This module computes a slope stability (expressed as a factor-of-safety) raster according to the traditional infinite slope model theory (see cf Selby, 1993) "
		"The resulting raster represents the ratio of resisting forces/driving forces (fs) on a potential shear plane with fs lesser 1 unstable, fs greater 1 stable. "
		"Except for a slope raster (in radians), all input variables can be specified either globally or distributed (through grids). "
		"The module creates a continuous fs raster (values above 10 are truncated), and a binary stability grid with nodata = stable, 1 = unstable (optional)."
	));

	Parameters.Add_Grid(
		NULL, "A", "Slope grid (rad)", "A slope angle grid (in radíans)", PARAMETER_INPUT
		);

	Parameters.Add_Grid(
		NULL, "Bmin", "Min thickness grid (m) ", "A grid representing minimum cell layer thicknesses to potential shear plane (in meters)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Bmax", "Max thickness grid (m) ", "A grid representing maximum cell layer thicknesses to potential shear plane (in meters)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Cmin", "Min saturation grid (-) ", "A grid representing minimum cell relative water saturation of layer (dimensionless)", PARAMETER_INPUT_OPTIONAL
		);	

	Parameters.Add_Grid(
		NULL, "Cmax", "Max saturation grid (-) ", "A grid representing maximum cell relative water saturation of layer (dimensionless)", PARAMETER_INPUT_OPTIONAL
		);	

	Parameters.Add_Grid(
		NULL, "Dmin", "Min friction grid (°) ", "A grid representing minimum cell frictional shear strength of layer (in degrees)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Dmax", "Max friction grid (°) ", "A grid representing maximum cell frictional shear strength of layer (in degrees)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Emin", "Min density grid (g/cm3)", "A grid representing minimum cell bulk density of layer (in grams per cubiccentimeters)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Emax", "Max density grid (g/cm3)", "A grid representing maximum cell bulk density of layer (in grams per cubiccentimeters)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Fmin", "Min cohesion grid (MPa) ", "A grid representing minimum cell layer cohesion (in Megapascals)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Fmax", "Max cohesion grid (MPa) ", "A grid representing maximum cell layer cohesion (in Megapascals)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Value(
		NULL, "fBmin", "Min global thickness (m)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.0			//Initialisierung eines fixen wertes vs Grid für Mächtigkeit
		);

	Parameters.Add_Value(
		NULL, "fBmax", "Max global thickness (m)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.0			//Initialisierung eines fixen wertes vs Grid für Mächtigkeit
		);

	Parameters.Add_Value(
		NULL, "fCmin", "Min global saturation (-)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0.0			//Initialisierung eines fixen wertes vs Grid für wetness
		);

	Parameters.Add_Value(
		NULL, "fCmax", "Max global saturation (-)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0.0			//Initialisierung eines fixen wertes vs Grid für wetness
		);

	Parameters.Add_Value(
		NULL, "fDmin", "Min global friction (°)", "Constant value if no raster set", PARAMETER_TYPE_Double, 33.0			//Initialisierung eines fixen wertes vs Grid für friction
		);

	Parameters.Add_Value(
		NULL, "fDmax", "Max global friction (°)", "Constant value if no raster set", PARAMETER_TYPE_Double, 33.0			//Initialisierung eines fixen wertes vs Grid für friction
		);

	Parameters.Add_Value(
		NULL, "fEmin", "Min global density (g/cm3)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.6			//Initialisierung eines fixen wertes vs Grid für density
		);

	Parameters.Add_Value(
		NULL, "fEmax", "Max global density (g/cm3)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.6			//Initialisierung eines fixen wertes vs Grid für density
		);

	Parameters.Add_Value(
		NULL, "fFmin", "Min global cohesion (MPa)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0.0			//Initialisierung eines fixen wertes vs Grid für Cohesion
		);

	Parameters.Add_Value(
		NULL, "fFmax", "Max global cohesion (MPa)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0.0			//Initialisierung eines fixen wertes vs Grid für Cohesion
		);

	Parameters.Add_Value(
		NULL, "fI", "Parameter sampling runs", "Number of sampling cycles",PARAMETER_TYPE_Int, 1						//sampling cycles
		);

	Parameters.Add_Grid(
		NULL, "G", "FS values", "Resulting factor-of-safety (-) grid", PARAMETER_OUTPUT
		);

	Parameters.Add_Grid(
		NULL, "H", "FS classes", "Resulting stability (0/1) grid", PARAMETER_OUTPUT_OPTIONAL
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
	FS_NODATA		= 0,
	FS_UNSTABLE,
	FS_STAB1,
	FS_STAB2,
	FS_STAB3,
	FS_STAB4,
	FS_COUNT
};


bool CSafetyfactor::On_Execute(void)
{
	double		fBmin	= Parameters("fBmin")->asDouble();
	double		fCmin	= Parameters("fCmin")->asDouble();
	double		fDmin	= Parameters("fDmin")->asDouble();
	double		fEmin	= Parameters("fEmin")->asDouble();
	double		fFmin	= Parameters("fFmin")->asDouble();
	double		fBmax	= Parameters("fBmax")->asDouble();
	double		fCmax	= Parameters("fCmax")->asDouble();
	double		fDmax	= Parameters("fDmax")->asDouble();
	double		fEmax	= Parameters("fEmax")->asDouble();
	double		fFmax	= Parameters("fFmax")->asDouble();

	double		grav	= 9.81;
	int			fI		= Parameters("fI")->asInt();
	
	CSG_Grid	*pA, *pBmin, *pCmin, *pDmin, *pEmin, *pFmin, *pBmax, *pCmax, *pDmax, *pEmax, *pFmax, *pG, *pH;

	pA		= Parameters("A"	)->asGrid();		//slope
	pBmin	= Parameters("Bmin"	)->asGrid();		//thickness
	pCmin	= Parameters("Cmin"	)->asGrid();		//saturation
	pDmin	= Parameters("Dmin"	)->asGrid();		//friction
	pEmin	= Parameters("Emin"	)->asGrid();		//density
	pFmin	= Parameters("Fmin"	)->asGrid();		//cohesion
	pBmax	= Parameters("Bmax"	)->asGrid();
	pCmax	= Parameters("Cmax"	)->asGrid();		//saturation
	pDmax	= Parameters("Dmax"	)->asGrid();		//friction
	pEmax	= Parameters("Emax"	)->asGrid();		//density
	pFmax	= Parameters("Fmax"	)->asGrid();		//cohesion
	pG		= Parameters("G"	)->asGrid();
	pH		= Parameters("H"	)->asGrid();


	
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double a, b, c, d, e, f, g;
			double bmin, cmin, dmin, emin, fmin;
			double bmax, cmax, dmax, emax, fmax;
			double bb, cc, dd, ee, ff;
			int bperc, cperc, dperc, eperc, fperc;
			int rand_int, h, n;

			a		=	pA->asDouble(x, y);
			bmin	=	pBmin ? pBmin->asDouble(x, y) : fBmin;						//Abfrage ob Raster oder Globalwerte
			cmin	=	pCmin ? pCmin->asDouble(x, y) : fCmin;
			dmin	=	pDmin ? pDmin->asDouble(x, y) : fDmin;
			emin	=	pEmin ? pEmin->asDouble(x, y) : fEmin;
			fmin	=	pFmin ? pFmin->asDouble(x, y) : fFmin;
			bmax	=	pBmax ? pBmax->asDouble(x, y) : fBmax;						//Abfrage ob Raster oder Globalwerte
			cmax	=	pCmax ? pCmax->asDouble(x, y) : fCmax;
			dmax	=	pDmax ? pDmax->asDouble(x, y) : fDmax;
			emax	=	pEmax ? pEmax->asDouble(x, y) : fEmax;
			fmax	=	pFmax ? pFmax->asDouble(x, y) : fFmax;
			h		=	fI;

			if (pA->is_NoData(x, y))
			{
				pG->Set_NoData(x, y);

				if (pH)
					pH->Set_Value(x, y, FS_NODATA);
			}

			else if( (pBmin || pBmax || pCmin || pCmax || pDmin || pDmax || pEmin || pEmax || pFmin || pFmax)
			&&	(  pBmin && pBmax->is_NoData(x, y)
				|| pCmin && pCmax->is_NoData(x, y)
				|| pDmin && pDmax->is_NoData(x, y)
				|| pEmin && pEmax->is_NoData(x, y)
				|| pFmin && pFmax->is_NoData(x, y) ))
			{
				pG->Set_NoData(x, y);

				if (pH)
					pH->Set_Value(x, y, FS_NODATA);
			}

			else
			{
				bperc = ((bmax - bmin) / bmax) * 100;				//calculate parameter range %: density
				if (bperc > 0)
				{
					n = 0;
					bb = 0;
					while ( n < h)									//loop through specified random number iterations:
					{
						rand_int = rand() % bperc + 0;				//calculate random percentage
						b = ((bmax/100) * rand_int) + bmin;			//calculate value
						bb = bb + b;								//sum
						n = n + 1;
					}
					b = bb / n;										// calculate mean from random values
				}
				else 
				{
					b = bmax;
				}


				cperc = ((cmax - cmin) / cmax) * 100;				//calculate parameter range %: density
				if (cperc > 0)
				{
					n = 0;
					cc = 0;
					while ( n < h)									//loop through specified random number iterations:
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
					while ( n < h) 
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
					while ( n < h) 
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
					while ( n < h) 
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

				g	=	((f * 1000) + (((e * grav) - (c * grav)) *				//FS calculation
						(((b * pow(cos(a), 2) * tan(d/57.2958)))))) / 
						((e * grav) * b * sin(a) * cos(a));

				if (g < 10)														//truncate values fs>10
					pG->Set_Value(x, y, g);
				else
					pG->Set_Value(x, y, 10);

				if (pH)			//create optional binary stabilty grid
				{
					if (g < 1)
						pH->Set_Value(x, y, FS_UNSTABLE);
					else if ((g >= 1) && (g < 1.5))
						pH->Set_Value(x, y, FS_STAB1);
					else if ((g >= 1.5) && (g < 3))
						pH->Set_Value(x, y, FS_STAB2);
					else if ((g >= 3) && (g < 6))
						pH->Set_Value(x, y, FS_STAB3);
					else if (g >= 6)
						pH->Set_Value(x, y, FS_STAB4);
				}
			}
		}
	}

	//-----------------------------------------------------

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pH, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int TO_Colors[FS_COUNT]	=
		{
			SG_GET_RGB(255, 255, 255),	// FS_NODATA
			SG_GET_RGB(255,	  0,   0),	// FS_UNSTABLE
			SG_GET_RGB(255,	128,  64),	// FS_STAB1
			SG_GET_RGB(255,	255,   0),	// FS_STAB2
			SG_GET_RGB(  0,	128, 255),	// FS_STAB3
			SG_GET_RGB(  0,	255,   0),	// FS_STAB4
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL("NO DATA");			Desc	+= _TL("");
		Name	+= _TL("FS <1");			Desc	+= _TL("");
		Name	+= _TL("FS 1-1.5");			Desc	+= _TL("");
		Name	+= _TL("FS 1.5-3");			Desc	+= _TL("");
		Name	+= _TL("FS 3-6");			Desc	+= _TL("");
		Name	+= _TL("FS >6");			Desc	+= _TL("");

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<FS_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, TO_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

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
