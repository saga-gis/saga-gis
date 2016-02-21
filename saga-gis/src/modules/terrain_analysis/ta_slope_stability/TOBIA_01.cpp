/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  ta_slope_stability                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      TOBIA_01.cpp                     //
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
#include "TOBIA_01.h"
#include <math.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTOBIA::CTOBIA(void)
{
	Set_Name		(_TL("TOBIA"));

	Set_Author		(SG_T("A. G&uuml;nther (c) 2012"));

	Set_Description	(_TW(
		"\n"
		"This module computes both a continuous and a categorical TOBIA (""Topography Bedding Intersection Angle"") Index according to Meentemeyer & Moody (2000) "
		"For computation, a slope and a aspect raster (both in radians) determining slope face orientations are required. "
		"The categorical TOBIA classifies the alignement of a geological structure to Topography into seven classes:\n"
		"0) Underdip slope\n"
		"1) Dip slope\n"
		"2) Overdip slope\n"
		"3) Steepened escarpmemt\n"
		"4) Normal escarpment\n"
		"5) Subdued escarpment\n"
		"6) Orthoclinal slope\n"
		"The continuous TOBIA index ranges from -1 to 1 (parallel orientation)\n" 
		"The structure TOBIA should be calculated with can be set either distributed (through dip direction and dip grids, in degrees!), "
		"or globally using integers (dip and dip direction, in degrees!). The module creates a TOBIA class integer grid, and (optionally) a continuous TOBIA index grid.\n"
		"\n"
		"Reference: "
		"<a href=\"http://www.sciencedirect.com/science/article/pii/S009830040000011X\">Meentemeyer R. K., Moody A. (2000). Automated mapping of conformity between topographic and geological surfaces. Computers & Geosciences, 26, 815 - 829</a>.\n"

	));

	Parameters.Add_Grid(
		NULL, "A", "Slope grid (rad)", "A slope angle grid (in radíans)", PARAMETER_INPUT
		);
	
	Parameters.Add_Grid(
		NULL, "B", "Aspect grid (rad)", "A aspect angle grid (in radíans)", PARAMETER_INPUT
		);
	
	Parameters.Add_Grid(
		NULL, "C", "Dip grid (degrees) ", "A grid representing the dip of the structure plane (in degrees)", PARAMETER_INPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "D", "Dip direction grid (degrees) ", "A grid representing the dip direction of the structure plane (in degrees)", PARAMETER_INPUT_OPTIONAL
		);	
	
	Parameters.Add_Value(
		NULL, "fB", "Global structure dip (degrees)", "Constant value if no raster set", PARAMETER_TYPE_Double, 45.0			//Initialisierung eines fixen wertes vs Grid für dip
		);

	Parameters.Add_Value(
		NULL, "fC", "Global structure dip direction (degrees)", "Constant value if no raster set", PARAMETER_TYPE_Double, 90.0			//Initialisierung eines fixen wertes vs Grid für dipdir
		);

	Parameters.Add_Grid(
		NULL, "E", "TOBIA classes", "Resulting TOBIA classes (1-7) grid", PARAMETER_OUTPUT
		);

	Parameters.Add_Grid(
		NULL, "F", "TOBIA index", "Resulting TOBIA index (-) grid",PARAMETER_OUTPUT_OPTIONAL
		);
		
}

enum
{
	TO_UNDERDIP_SLOPE		= 0,
	TO_DIP_SLOPE,
	TO_OVERDIP_SLOPE,
	TO_STEEPENED_ESCARPMENT,
	TO_NORMAL_ESCARPMENT,
	TO_SUBDUED_ESCARPMENT,
	TO_ORTHOCLINAL_SLOPE,
	TO_COUNT
};

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTOBIA::On_Execute(void)
{
	double		fB	= Parameters("fB")->asDouble();
	double		fC	= Parameters("fC")->asDouble();
	
	CSG_Grid	*pA, *pB, *pC, *pD, *pE, *pF;

	pA	= Parameters("A"	)->asGrid();		//slope
	pB	= Parameters("B"	)->asGrid();		//aspect
	pC	= Parameters("C"	)->asGrid();		//dip grid
	pD	= Parameters("D"	)->asGrid();		//dip dir grid
	pE	= Parameters("E"	)->asGrid();		//output TOBIA classes
	pF	= Parameters("F"	)->asGrid();		//output TOBIA index
	

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double a, b, c, d, e, f;
		
			a	=	pA->asDouble(x, y);
			b	=	pB->asDouble(x, y);						//Abfrage ob Raster oder Globalwerte
			c	=	pC ? pC->asDouble(x, y) : fB;
			d	=	pC ? pD->asDouble(x, y) : fC;

			if (pA->is_NoData(x, y))
			{
				pE->Set_NoData(x, y);

				if (pF)
					pF->Set_NoData(x, y);

			}

			else if ((pC || pD) && (pC->is_NoData(x, y) || (pD->is_NoData(x, y))))
			{
				pE->Set_NoData(x, y);

				if (pF)
					pF->Set_NoData(x, y);

			}

			else
			{

				e	=	pow(pow((cos(d/57.2958) - cos(b)), 2) + pow((sin(d/57.2958) - sin(b)), 2), 0.5);												//TOBIA-classes
		
				if (((0 <= e) && (e < 0.7654)) && ((c - (a*57.2958)) > 5))
					pE->Set_Value(x, y, TO_UNDERDIP_SLOPE);
				else if (((0 <= e) && (e < 0.7654)) && ((-5 <= (c - (a*57.2958))) && ((c - (a*57.2958) <= 5))))
					pE->Set_Value(x, y, TO_DIP_SLOPE);
				else if (((0 <= e) && (e < 0.7654)) && ((c - (a*57.2958)) < -5))
					pE->Set_Value(x, y, TO_OVERDIP_SLOPE);
				else if (((1.8478 < e) && (e <= 2)) && ((c - (a*57.2958)) < -5))
					pE->Set_Value(x, y, TO_STEEPENED_ESCARPMENT);
				else if (((1.8478 < e) && (e <= 2)) && ((-5 <= (c - (a*57.2958))) && ((c - (a*57.2958) <= 5))))
					pE->Set_Value(x, y, TO_NORMAL_ESCARPMENT);
				else if (((1.8478 < e) && (e <= 2)) && ((c - (a*57.2958)) > 5))
					pE->Set_Value(x, y, TO_SUBDUED_ESCARPMENT);
				else if ((0.7654 < e) && (e <= 1.8478))
					pE->Set_Value(x, y, TO_ORTHOCLINAL_SLOPE);
				else
					pE->Set_NoData_Value(0);

				if (pF)
				{
					f	=	(cos((c/57.2958)) * (cos(a))) + (sin(c/57.2958) * sin(a) * ((cos((d/57.2958) - b))));				//TOBIA-index
					pF->Set_Value(x, y, f);
				}

			}
		}
	}

	//-----------------------------------------------------

	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pE, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int TO_Colors[TO_COUNT]	=
		{
			SG_GET_RGB(255, 255,   0),	// TO_UNDERDIP
			SG_GET_RGB(255,	128,   0),	// TO_DIP
			SG_GET_RGB(255,   0,   0),	// TO_OVERDIP
			SG_GET_RGB(  0,   0, 128),	// TO_STEEPENED
			SG_GET_RGB(  0, 128, 255),	// TO_NORMAL
			SG_GET_RGB(128, 255, 255),	// TO_SUBDUED
			SG_GET_RGB(  0, 255,  64),  // TO_ORTHOCLINAL
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL("Underdip slope");			Desc	+= _TL("");
		Name	+= _TL("Dip slope");				Desc	+= _TL("");
		Name	+= _TL("Overdip slope");			Desc	+= _TL("");
		Name	+= _TL("Steepened escarpment");		Desc	+= _TL("");
		Name	+= _TL("Normal escarpment");		Desc	+= _TL("");
		Name	+= _TL("Subdued escarpment");		Desc	+= _TL("");
		Name	+= _TL("Orthoclinal slope");		Desc	+= _TL("");
		

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<TO_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, TO_Colors[i]);
			pRecord->Set_Value(1, Name[i].c_str());
			pRecord->Set_Value(2, Desc[i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pE, P);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
