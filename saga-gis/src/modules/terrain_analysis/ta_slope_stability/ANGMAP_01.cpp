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
//                    ANGMAP_01.cpp                      //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "ANGMAP_01.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CANGMAP::CANGMAP(void)
{
	Set_Name		(_TL("ANGMAP"));

	Set_Author		(SG_T("A. G&uuml;nther (c) 2013"));

	Set_Description	(_TW(
		"\n"
		"This tool computes the acute angle raster between the topographic surface defined by slope and aspect rasters internally derived from input elevation raster, and a structural plane defined by diop direction- and dip grids. "
		"Optionally, the dip direction and dip of the cutting line linears between the two planes can be calculated"
		"\n"
		"Reference: "
		"<a href=\"http://www.sciencedirect.com/science/article/pii/S0098300403000864\">G&uuml;nther, A. (2003). SLOPEMAP: programs for automated mapping of geometrical and kinematical properties of hard rock hill slopes. Computers & Geosciences, 29, 865 - 875</a>.\n"

	));

	Parameters.Add_Grid(
		NULL, "DEM", "Elevation", "A Digital Elvation Model (DEM)", PARAMETER_INPUT
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
		NULL, "E", "Angle", "Acute angle (degrees) grid", PARAMETER_OUTPUT
		);

	Parameters.Add_Grid(
		NULL, "F", "CL dipdir", "Dip direction cutting line (degrees)",PARAMETER_OUTPUT_OPTIONAL
		);

	Parameters.Add_Grid(
		NULL, "G", "CL dip", "Dip cutting line (degrees)",PARAMETER_OUTPUT_OPTIONAL
		);

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CANGMAP::On_Execute(void)
{
	double		fB	= Parameters("fB")->asDouble();
	double		fC	= Parameters("fC")->asDouble();
	
	CSG_Grid	*pDEM, *pC, *pD, *pE, *pF, *pG;

	pDEM	= Parameters("DEM")->asGrid();		//elevation
	pC		= Parameters("C"  )->asGrid();		//dip grid
	pD		= Parameters("D"  )->asGrid();		//dip dir grid
	pE		= Parameters("E"  )->asGrid();		//output angles
	pF		= Parameters("F"  )->asGrid();		//output ce dipdir
	pG		= Parameters("G"  )->asGrid();		//output ce dip

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double a, b, c, d, e, f, g;

			if( !pDEM->Get_Gradient(x, y, a, b)
			||  (pC && pC->is_NoData(x, y))
			||  (pD && pD->is_NoData(x, y))	)
			{
				if (pE)	pE->Set_NoData(x, y);
				if (pF)	pF->Set_NoData(x, y);
				if (pG)	pG->Set_NoData(x, y);
			}
			else
			{
				double	s1, s2, s3, s4, c1, c2, c3, c4;
				double	px, py, pz;
				double	p, pp, ff, aa;

				c	=	pC ? pC->asDouble(x, y) : fB;	//Abfrage ob Raster oder Globalwerte
				d	=	pD ? pD->asDouble(x, y) : fC;	//Abfrage ob Raster oder Globalwerte

				s1	= sin(b + M_PI);
				s2	= sin((d * M_DEG_TO_RAD) + M_PI);
				s3	= sin((M_PI/2) - a);
				s4	= sin((M_PI/2) - (c * M_DEG_TO_RAD));
				c1	= cos(b + M_PI);
				c2	= cos((d * M_DEG_TO_RAD) + M_PI);
				c3	= cos((M_PI/2) - a);
				c4	= cos((M_PI/2) - (c * M_DEG_TO_RAD));
		
				px = (s1 * c3 * s4) - (s3 * s2 * s4);
				py = (s3 * c2 * c4) - (c1 * c3 * c4);
				pz = (c1 * c3 * s2 * 4) - (s1 * c3 * c2 * c4);

				if (pz < 0)
				{
					px = px * -1;
					py = py * -1;
					pz = pz * -1;
				}
		 
				p	=	sqrt((px * px) + (py * py) + (pz * pz));
				pp  =	pz / p;
				ff	=	atan(pp / sqrt((1 - (pp * pp))));
				aa	=	atan(py / px);

				e	=	ff * M_RAD_TO_DEG;
				f	=	aa * M_RAD_TO_DEG;

				if (px < 0)
					f = f + 180;
				else if ((px < 0) && (py < 0))
					f = f + 360;

				if (f > 360)
					f = f - 360;
				if (f < 0)
					f = f + 360;

				g = sin(p) * M_RAD_TO_DEG;

				if (pE)	pE->Set_Value(x, y, e);
				if (pF)	pF->Set_Value(x, y, f);
				if (pG)	pG->Set_Value(x, y, g);
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
