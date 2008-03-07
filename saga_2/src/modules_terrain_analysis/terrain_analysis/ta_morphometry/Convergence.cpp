
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Convergence.cpp                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Convergence.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CConvergence::CConvergence(void)
{
	Set_Name(_TL("Convergence Index"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"Reference:\n"
		"Koethe, R. / Lehmeier, F. (1996):\n'SARA – System zur Automatischen Relief-Analyse',\n"
		"Benutzerhandbuch, 2. Auflage [Geogr. Inst. Univ. Goettingen, unveroeffentl.]\n")
	);

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Convergence Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Aspect"),
			_TL("Gradient")
		),0
	);
}

//---------------------------------------------------------
CConvergence::~CConvergence(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvergence::On_Execute(void)
{
	pDTM			= Parameters("ELEVATION")	->asGrid();
	pConvergence	= Parameters("RESULT")		->asGrid();

	pConvergence->Assign_NoData();

	DataObject_Set_Colors(pConvergence, 100, SG_COLORS_RED_GREY_BLUE, true);

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	Do_Aspect();	break;
	case 1:	Do_Gradient();	break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CConvergence::Do_Aspect(void)
{
	int		x, y, i, ix, iy, n;
	double	Slope, Aspect, iAspect, d, dSum;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pDTM->is_InGrid(x, y) )
			{
				for(i=0, n=0, dSum=0.0, iAspect=-M_PI_180; i<8; i++, iAspect+=M_PI_045)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if( pDTM->is_InGrid(ix, iy) && pDTM->Get_Gradient(ix, iy, Slope, Aspect) && Aspect >= 0.0 )
					{
						d		= Aspect - iAspect;

						//---------------------------------
						d		= fmod(d, M_PI_360);

						if( d < -M_PI_180 )
						{
							d	+= M_PI_360;
						}
						else if( d > M_PI_180 )
						{
							d	-= M_PI_360;
						}

						//---------------------------------
						dSum	+= fabs(d);
						n++;
					}
				}

				pConvergence->Set_Value(x, y, n > 0 ? (dSum / (double)n - M_PI_090) * 100.0 / M_PI_090 : 0.0);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CConvergence::Do_Gradient(void)
{
	int		x, y, i, ix, iy, n;
	double	z, Slope, Aspect, iSlope, iAspect, d, dSum;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pDTM->is_InGrid(x, y) )
			{
				z	= pDTM->asDouble(x, y);

				for(i=0, n=0, dSum=0.0, iAspect=-M_PI_180; i<8; i++, iAspect+=M_PI_045)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if( pDTM->is_InGrid(ix, iy) && pDTM->Get_Gradient(ix, iy, Slope, Aspect) && Aspect >= 0.0 )
					{
						iSlope	= atan((pDTM->asDouble(ix, iy) - z) / Get_Length(i));

						// Nach dem Seiten-Kosinus-Satz...
						d		= acos(sin(Slope) * sin(iSlope) + cos(Slope) * cos(iSlope) * cos(iAspect - Aspect));

						//---------------------------------
						d		= fmod(d, M_PI_360);

						if( d < -M_PI_180 )
						{
							d	+= M_PI_360;
						}
						else if( d > M_PI_180 )
						{
							d	-= M_PI_360;
						}

						//---------------------------------
						dSum	+= fabs(d);
						n++;
					}
				}

				pConvergence->Set_Value(x, y, n > 0 ? (dSum / (double)n - M_PI_090) * 100.0 / M_PI_090 : 0.0);
			}
		}
	}
}
