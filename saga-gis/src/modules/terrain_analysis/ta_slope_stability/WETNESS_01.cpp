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
//                    WETNESS_01.cpp                     //
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
#include "WETNESS_01.h"
#include <math.h>

//---------------------------------------------------------
#define RUN_MODULE(LIBRARY, MODULE, CONDITION)	{\
	bool	bResult;\
	SG_RUN_MODULE(bResult, LIBRARY, MODULE, CONDITION)\
	if( !bResult ) return( false );\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pModule->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWETNESS::CWETNESS(void)
{
	CSG_Parameters	P;

	//-----------------------------------------------------
	
	Set_Name		(_TL("WETNESS"));

	Set_Author		(SG_T("A. G&uuml;nther (c) 2012"));

	Set_Description	(_TW(
		"\n"
		"This module calculates a topographic wetness index (TWI) following Montgomery & Dietrich (1994) that can be used to estimate the degree of saturation of unconsolidated, permeable "
		"materials above (more or less) impermeable bedrock. In contrast to the common TOPMODEL (Beven & Kirkby, 1979) - based TWI, this index differs in such that it considers "
		"hydraulic conductivity to be constant in a soil mantle overlying relatively impermeable bedrock. Also, it uses the sine of the slope rather than its tangens, which is more correct "
		"and significantly matters for steeper slopes that give raise to landslides. "
		"For computation, a slope (in radians) and a catchment area (in m2) grid are required. "
		"Additionally, information on groundwater recharge (m/hr), material hydraulic conductivity (m/hr), "
		"and depth to potential shear plane (m) are required that can be specified either globally or through grids. "
		"The module produces a continuous wetness index (-) where cells with WI values > 1 (overland flow) set to 1, and optionally creates a classified WI grid rendering three saturation classes:.\n"
		"0): Low moisture (WI smaller 0.1)\n"
		"1): Partially wet (0.1 smaller WI smaller 1)\n"
		"2): Saturation zone (WI larger 1)\n"
		"\n"
		"References:\n"
		"<a href=\"http://www.tandfonline.com/doi/abs/10.1080/02626667909491834\">Beven, K.J., Kirkby, M.J. (1979) A physically-based variable contributing area model of basin hydrology. Hydrology Science Bulletin, 24, 43-69.</a>.\n"
		"\n"
		"<a href=\"http://www.agu.org/pubs/crossref/1994/93WR02979.shtml\">Montgomery D. R., Dietrich, W. E. (1994) A physically based model for the topographic control on shallow landsliding. Water Resources Research, 30, 1153-1171.</a>.\n"

	));

	Parameters.Add_Grid(
		NULL, "DEM", "DEM", "A DEM", PARAMETER_INPUT
		);
	
	/*Parameters.Add_Grid(
		NULL, "B", "Catchment area grid (m2)", "A catchment area grid (in square meters)", PARAMETER_INPUT
		);
	*/
	Parameters.Add_Grid(
		NULL, "Cmin", "Min hydraulic conductivity grid (m/hr) ", "A grid representing minimum material hydraulic conductivity (in m/hr)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Grid(
		NULL, "Cmax", "Max hydraulic conductivity grid (m/hr) ", "A grid representing maximum material hydraulic conductivity (in m/hr)", PARAMETER_INPUT_OPTIONAL
		);	

	Parameters.Add_Grid(
		NULL, "Dmin", "Min groundwater recharge grid (m/hr) ", "A grid representing minimum groundwater recharge (in m/hr)", PARAMETER_INPUT_OPTIONAL
		);
	
	Parameters.Add_Grid(
		NULL, "Dmax", "Max groundwater recharge grid (m/hr) ", "A grid representing maximum groundwater recharge (in m/hr)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Emin", "Min material depth grid (m)", "A grid representing minimum depth to potential shear plane (in m)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "Emax", "Max material depth grid (m)", "A grid representing maximum depth to potential shear plane (in m)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Value(
		NULL, "fCmin", "Min global material conductivity (m/hr)", "Constant value if no raster set", PARAMETER_TYPE_Double, 2.7					//Initialisierung eines fixen wertes vs Grid für density
		);

	Parameters.Add_Value(
		NULL, "fCmax", "Max global material conductivity (m/hr)", "Constant value if no raster set", PARAMETER_TYPE_Double, 2.7					//Initialisierung eines fixen wertes vs Grid für density
		);	
	
	Parameters.Add_Value(
		NULL, "fDmin", "Min global groundwater recharge (m/hr)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0.001						//Initialisierung eines fixen wertes vs Grid für conductivity
		);

	Parameters.Add_Value(
		NULL, "fDmax", "Max global groundwater recharge (m/hr)", "Constant value if no raster set", PARAMETER_TYPE_Double, 0.001						//Initialisierung eines fixen wertes vs Grid für conductivity
		);

	Parameters.Add_Value(
		NULL, "fEmin", "Min global material depth (m)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.0							//Initialisierung eines fixen wertes vs Grid für depth
		);

	Parameters.Add_Value(
		NULL, "fEmax", "Max global material depth (m)", "Constant value if no raster set", PARAMETER_TYPE_Double, 1.0							//Initialisierung eines fixen wertes vs Grid für depth
		);

	Parameters.Add_Value(
		NULL, "fH", "Parameter sampling runs", "Number of sampling cycles",PARAMETER_TYPE_Int, 1						//sampling cycles
		);

	Parameters.Add_Grid(
		NULL, "F", "WI values", "Resulting wetness index (-) grid", PARAMETER_OUTPUT
		);

	Parameters.Add_Grid(
		NULL, "G", "WI classes", "Classified wetness (-) grid", PARAMETER_OUTPUT_OPTIONAL
		);

	Parameters.Add_Choice(
		NULL	, "Method"		, _TL("Catchment Area Calculation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Braunschweiger Reliefmodell"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Multiple Triangular Flow Directon")
		), 4
	);

	Parameters.Add_Value(
		NULL	, "PREPROC"		, _TL("Preprocessing"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
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
	WI_NODATA			= 0,
	WI_LOW_MOISTURE,
	WI_PARTIALLY_WET,
	WI_SATURATION_ZONE,
	WI_COUNT
};

bool CWETNESS::On_Execute(void)
{
	double		fCmin	= Parameters("fCmin")->asDouble();
	double		fDmin	= Parameters("fDmin")->asDouble();
	double		fEmin	= Parameters("fEmin")->asDouble();
	double		fCmax	= Parameters("fCmax")->asDouble();
	double		fDmax	= Parameters("fDmax")->asDouble();
	double		fEmax	= Parameters("fEmax")->asDouble();
	double		fH		= Parameters("fH")->asInt();

	CSG_Grid	*pDEM, *pB, *pCmin, *pDmin, *pEmin, *pCmax, *pDmax, *pEmax,*pF, *pG;

	pDEM	= Parameters("DEM"	)->asGrid();		//DEM
	pCmin	= Parameters("Cmin"	)->asGrid();		//conductivity
	pDmin	= Parameters("Dmin"	)->asGrid();		//recharge
	pEmin	= Parameters("Emin"	)->asGrid();		//depth
	pCmax	= Parameters("Cmax"	)->asGrid();		//conductivity
	pDmax	= Parameters("Dmax"	)->asGrid();		//recharge
	pEmax	= Parameters("Emax"	)->asGrid();		//depth
	pF		= Parameters("F"	)->asGrid();		//output wetness index
	pG		= Parameters("G"	)->asGrid();		//output wetness classes

	//-----------------------------------------------------
	// get catchment area sizes
	CSG_Grid	B(*Get_System(), SG_DATATYPE_Float);

	pB	= &B;

	if( Parameters("PREPROC")->asBool() )
	{
		CSG_Grid	DEM(*Get_System(), SG_DATATYPE_Float);

		RUN_MODULE("ta_preprocessor"		, 2,
				SET_PARAMETER("DEM"			, pDEM)
			&&	SET_PARAMETER("DEM_PREPROC"	, &DEM)
		)

		RUN_MODULE("ta_hydrology"			, 0,
				SET_PARAMETER("ELEVATION"	, &DEM)
			&&	SET_PARAMETER("CAREA"		, pB)
			&&	SET_PARAMETER("Method"		, Parameters("Method"))
		)
	}
	else
	{
		RUN_MODULE("ta_hydrology"			, 0,
				SET_PARAMETER("ELEVATION"	, pDEM)
			&&	SET_PARAMETER("CAREA"		, pB)
			&&	SET_PARAMETER("Method"		, Parameters("Method"))
		)
	}


	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double a, b, c, d, e, f;					//a slope, b catchment area
			double cmin, dmin, emin;
			double cmax, dmax, emax;
			double cc, dd, ee;
			int cperc, dperc, eperc;
			int rand_int, h, n;
		
			b		=	pB->asDouble(x, y);						//Abfrage ob Raster oder Globalwerte:
			cmin	=	pCmin ? pCmin->asDouble(x, y) : fCmin;
			dmin	=	pDmin ? pDmin->asDouble(x, y) : fDmin;
			emin	=	pEmin ? pEmin->asDouble(x, y) : fEmin;
			cmax	=	pCmax ? pCmax->asDouble(x, y) : fCmax;
			dmax	=	pDmax ? pDmax->asDouble(x, y) : fDmax;
			emax	=	pEmax ? pEmax->asDouble(x, y) : fEmax;
			h		=	fH;

			if (pDEM->Get_Gradient(x, y, a, ee)==false
			   || (pCmin && pCmin->is_NoData(x, y))
			   || (pCmax && pCmax->is_NoData(x, y))
			   || (pDmin && pDmin->is_NoData(x, y))
			   || (pDmax && pDmax->is_NoData(x, y))
			   || (pEmin && pEmin->is_NoData(x, y))
			   || (pEmax && pEmax->is_NoData(x, y)) )
			{
				pF->Set_NoData(x, y);

				if (pG)

					pG->Set_Value(x, y, WI_NODATA);
			}

			else
			{

				cperc = ((cmax - cmin) / cmax) * 100;				//calculate parameter range %: conductivity
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

				dperc = ((dmax - dmin) / dmax) * 100;				//calculate parameter range %: recharge
				if (dperc > 0)
				{
					n = 0;
					dd = 0;
					while ( n < h)									//loop through specified random number iterations:
					{
						rand_int = rand() % dperc + 0;				//calculate random percentage
						d = ((dmax/100) * rand_int) + dmin;			//calculate value
						dd = dd + d;								//sum
						n = n + 1;
					}
					d = dd / n;										// calculate mean from random values
				}
				else 
				{
					d = dmax;
				}

				eperc = ((emax - emin) / emax) * 100;				//calculate parameter range %: depth
				if (eperc > 0)
				{
					n = 0;
					ee = 0;
					while ( n < h)									//loop through specified random number iterations:
					{
						rand_int = rand() % eperc + 0;				//calculate random percentage
						e = ((emax/100) * rand_int) + emin;			//calculate value
						ee = ee + e;								//sum
						n = n + 1;
					}
					e = ee / n;										// calculate mean from random values
				}
				else 
				{
					e = emax;
				}
		
			
				f	=	(d*(b/pB->Get_Cellsize()))/((c*e)*sin(a));		//Wetness index calculation
		
				if (f < 1)												//calculate wetness index grid
					pF->Set_Value(x, y, f);
				else if (f >= 1)
					pF->Set_Value(x, y, 1);
				else
					pF->Set_NoData(x, y);


				if (pG)													//calculate optional classified grid
				{
					if (f <= 0.1)
						pG->Set_Value(x, y, WI_LOW_MOISTURE);
					else if( f <= 1 )// if ((f > 0.1) && (f <= 1))
						pG->Set_Value(x, y, WI_PARTIALLY_WET);
					else //if (f > 1)
						pG->Set_Value(x, y, WI_SATURATION_ZONE);
				}
			}
		}
	}


	//-----------------------------------------------------

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pG, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int CR_Colors[WI_COUNT]	=
		{
			SG_GET_RGB(255, 255, 255),  // WI_NODATA
			SG_GET_RGB(255, 255,   0),	// WI_LOW_MOISTURE
			SG_GET_RGB(0,	255,   0),	// WI_PARTIALLY_WET
			SG_GET_RGB(0,     0, 255),	// WI_SATURATION_ZONE
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL("No data");							Desc	+= _TL("");
		Name	+= _TL("Low moisture (WI <= 0.1");			Desc	+= _TL("");
		Name	+= _TL("Partially wet (WI = 0.1 - 1)");		Desc	+= _TL("");
		Name	+= _TL("Saturation zone (WI > 1)");			Desc	+= _TL("");	

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<WI_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, CR_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pG, P);
	}

	return( true );

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
