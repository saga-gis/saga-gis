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
//                    WEDGEFAIL_01.cpp                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                     Andreas G�nther                   //
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
//    e-mail:     a.guenther@bgr.de                      //
//                                                       //
//    contact:    Andreas G�nther                        //
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
#include "WEDGEFAIL_01.h"
#include <math.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWEDGEFAIL::CWEDGEFAIL(void)
{
	CSG_Parameters	P;

	//-----------------------------------------------------
	
	Set_Name		(_TL("WEDGEFAIL"));

	Set_Author		(SG_T("A. G&uuml;nther (c) 2012"));

	Set_Description	(_TW(
		"\n"
		"This tool determines terrain elements where failure (slide- or topple movements) on geological discontinuies are kinematically possible "
		"through the spatial application of common frictional feasibility criteria (G&uuml;nther et al. 2012 and references therein). Both the orientation of slope elements "
		"specified through aspect- and dip grids (in radians) are required together with the orientation of one planar structure defined through global- or grid dip direction and dip data, "
		"or two planar structures defined by plunge direction and plunge information of their intersection line (in degrees). The shear strength of the discontinuities is specified "
		"using global or grid-based friction angle data. Optionally, a cone value can be set allowing for some variance in discontinuity dip orientations. "
		"The tool operates in slide (testing for plane and wedge sliding) or topple (testing for plane and wedge toppling) modes.\n"
		"\n"
	));

	Add_Reference("G&uuml;nther A., Wienh&ouml;fer J., Konietzky H.", "2012",
		"Automated mapping of rock slope geometry, kinematics and stability with RSS-GIS",
		"Natural Hazards, 61, pp. 29-49.",
		SG_T("https://link.springer.com/article/10.1007/s11069-011-9771-2")
	);

	Parameters.Add_Grid(
		NULL, "DEM", "DEM", "A DEM", PARAMETER_INPUT
		);
	
	/*Parameters.Add_Grid(
		NULL, "B", "Aspect grid (rad)", "A aspect angle grid (in radians)", PARAMETER_INPUT
		);*/
	
	Parameters.Add_Grid(
		NULL, "C", "Dip/Plunge direction grid (degree) ", "A dip- or plunge direction grid (in degrees)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "D", "Dip/Plunge grid (degree) ", "A dip- or plunge grid (in degrees)", PARAMETER_INPUT_OPTIONAL
		);
	
	Parameters.Add_Grid(
		NULL, "Emin", "Min friction angle grid (degree) ", "A minimum discontinuity friction angle grid (in degrees)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Emax", "Max friction angle grid (degree) ", "A maximum discontinuity friction angle grid (in degrees)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Value(
		NULL, "fC", "Global dip/plunge direction (degree)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0					//Initialisierung eines fixen wertes vs Grid f�r density
		);

	Parameters.Add_Value(
		NULL, "fD", "Global dip/plunge (degree)", "Constant value if no raster set", PARAMETER_TYPE_Double, 35						//Initialisierung eines fixen wertes vs Grid f�r conductivity
		);

	Parameters.Add_Value(
		NULL, "fEmin", "Min global friction angle (degree)", "Constant value if no raster set", PARAMETER_TYPE_Double, 35							//Initialisierung eines fixen wertes vs Grid f�r depth
		);

	Parameters.Add_Value(
		NULL, "fEmax", "Max global friction angle (degree)", "Constant value if no raster set", PARAMETER_TYPE_Double, 35							//Initialisierung eines fixen wertes vs Grid f�r depth
		);

	Parameters.Add_Value(
		NULL, "ff", "Cone radius (degree)", "Radius of optional cone variance (in degrees)", PARAMETER_TYPE_Int, 0							//Initialisierung eines fixen wertes vs Grid f�r depth
		);	
	
	Parameters.Add_Grid(
		NULL, "F", "Failures", "Resulting failure cells (-) grid", PARAMETER_OUTPUT
		);

	Parameters.Add_Value(
		NULL, "fI", "Parameter sampling runs", "Number of sampling cycles",PARAMETER_TYPE_Int, 1						//sampling cycles
		);

	Parameters.Add_Choice(
		NULL, "METHOD"	, "Mode",
		"Set failure mode",
		"Slide|Topple"
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
	FI_NODATA			= 0,
	FI_FAILURE,
	FI_NO_FAILURE,
	FI_COUNT
};

bool CWEDGEFAIL::On_Execute(void)
{
	int			Method	= Parameters("METHOD"	)->asInt();
	int			ff		= Parameters("ff"		)->asInt();
	double		fC		= Parameters("fC"		)->asDouble();
	double		fD		= Parameters("fD"		)->asDouble();
	double		fEmin	= Parameters("fEmin"	)->asDouble();
	double		fEmax	= Parameters("fEmax"	)->asDouble();
	int			fI		= Parameters("fI"		)->asInt();

	CSG_Grid	*pDEM, *pC, *pD, *pEmin, *pEmax, *pF;

	pDEM			= Parameters("DEM"		)->asGrid();		//DEM
	//pB			= Parameters("B"		)->asGrid();		//aspect
	pC			= Parameters("C"		)->asGrid();		//dip dir
	pD			= Parameters("D"		)->asGrid();		//dip
	pEmin		= Parameters("Emin"		)->asGrid();		//friction
	pEmax		= Parameters("Emax"		)->asGrid();		//friction
	pF			= Parameters("F"		)->asGrid();		//output failure
	

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double a, b, c, d, e, f, g;
			double emin, emax;
			int rand_int, eperc, h, n, ee;
	
			//a		=	pA->asDouble(x, y);
			//b		=	pB->asDouble(x, y);						//Abfrage ob Raster oder Globalwerte:
			c		=	pC ? pC->asDouble(x, y) : fC;
			d		=	pD ? pD->asDouble(x, y) : fD;
			emin	=	pEmin ? pEmin->asDouble(x, y) : fEmin;
			emax	=	pEmax ? pEmax->asDouble(x, y) : fEmax;
			h		=	fI;

			if (pDEM->Get_Gradient(x, y, a, b)==false)
			{
				pF->Set_Value(x, y, FI_NODATA);
			}

			else if ((pC || pD) && (pC->is_NoData(x, y) || pD->is_NoData(x, y)))
			{
				pF->Set_Value(x, y, FI_NODATA);
			}

			else
			{
				eperc = (int)(((emax - emin) / emax) * 100.0);
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


				if ((b*M_RAD_TO_DEG) >= c)												//Azimuthaldifferenz in grad
					f	=	(b*M_RAD_TO_DEG) - c;
				else if ((b*M_RAD_TO_DEG) < c)
					f  =	c - (b*M_RAD_TO_DEG);
				else
					f = 0;

				g = atan((cos(f/M_RAD_TO_DEG)) * tan(a+(ff/M_RAD_TO_DEG))) * M_RAD_TO_DEG;			//Berechung apparent slope dip in direction plane/cedge

				switch (Method)
				{

				case 0:				//wedge/plane slides

					if ((e <= d) && (d <= g))												
						pF->Set_Value(x, y, FI_FAILURE);
					else
						pF->Set_Value(x, y, FI_NO_FAILURE);
					break;

				case 1:				//wedge/plane topples

					if (g >= ((e+90)-d))												
						pF->Set_Value(x, y, FI_FAILURE);
					else
						pF->Set_Value(x, y, FI_NO_FAILURE);
					break;

				}
			}
		}
	}


	//-----------------------------------------------------

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pF, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int CR_Colors[FI_COUNT]	=
		{
			SG_GET_RGB(255, 255, 255),	// FI_NO_DATA
			SG_GET_RGB(255,   0,   0),	// FI_FAILURE
			SG_GET_RGB(255,	255, 128),	// FI_NO_FAILURE
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		
		Name	+= _TL("No Data");			Desc	+= _TL("");
		Name	+= _TL("Failure");			Desc	+= _TL("");
		Name	+= _TL("No Failure");		Desc	+= _TL("");

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<FI_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, CR_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pF, P);
	}

	return( true );

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
