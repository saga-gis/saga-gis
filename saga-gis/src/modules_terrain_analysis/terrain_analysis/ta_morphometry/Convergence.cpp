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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Convergence Index"));

	Set_Author		(SG_T("O.Conrad (c) 2001"));

	Set_Description	(_TW(
		"Reference:\n"
		"Koethe, R. & Lehmeier, F. (1996): SARA - System zur Automatischen Relief-Analyse. "
		"User Manual, 2. Edition [Dept. of Geography, University of Goettingen, unpublished]\n\n"
	));

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

	Parameters.Add_Choice(
		NULL	, "NEIGHBOURS"	, _TL("Gradient Calculation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("2 x 2"),
			_TL("3 x 3")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvergence::On_Execute(void)
{
	bool		bGradient;
	int			Neighbours;
	CSG_Grid	*pConvergence;

	m_pDTM			= Parameters("ELEVATION")	->asGrid();
	pConvergence	= Parameters("RESULT")		->asGrid();
	Neighbours		= Parameters("NEIGHBOURS")	->asInt();
	bGradient		= Parameters("METHOD")		->asInt() == 1;

	DataObject_Set_Colors(pConvergence, 100, SG_COLORS_RED_GREY_BLUE, true);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_InGrid(x, y) )
			{
				switch( Neighbours )
				{
				case 0: default:	pConvergence->Set_Value(x, y, Get_2x2(x, y, bGradient));	break;
				case 1:				pConvergence->Set_Value(x, y, Get_9x9(x, y, bGradient));	break;
				}
			}
			else
			{
				pConvergence->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CConvergence::Get_2x2_Gradient(int x, int y, int i, double &Slope, double &Aspect, double &Height)
{
	double	z[4];

	switch( i % 4 )
	{
	case 0:
		z[0]	= m_pDTM->is_InGrid(x    , y + 1) ? m_pDTM->asDouble(x    , y + 1) : m_pDTM->asDouble(x, y);
		z[1]	= m_pDTM->is_InGrid(x + 1, y + 1) ? m_pDTM->asDouble(x + 1, y + 1) : m_pDTM->asDouble(x, y);
		z[2]	= m_pDTM->is_InGrid(x    , y    ) ? m_pDTM->asDouble(x    , y    ) : m_pDTM->asDouble(x, y);
		z[3]	= m_pDTM->is_InGrid(x + 1, y    ) ? m_pDTM->asDouble(x + 1, y    ) : m_pDTM->asDouble(x, y);
		break;

	case 1:
		z[0]	= m_pDTM->is_InGrid(x    , y    ) ? m_pDTM->asDouble(x    , y    ) : m_pDTM->asDouble(x, y);
		z[1]	= m_pDTM->is_InGrid(x + 1, y    ) ? m_pDTM->asDouble(x + 1, y    ) : m_pDTM->asDouble(x, y);
		z[2]	= m_pDTM->is_InGrid(x    , y - 1) ? m_pDTM->asDouble(x    , y - 1) : m_pDTM->asDouble(x, y);
		z[3]	= m_pDTM->is_InGrid(x + 1, y - 1) ? m_pDTM->asDouble(x + 1, y - 1) : m_pDTM->asDouble(x, y);
		break;

	case 2:
		z[0]	= m_pDTM->is_InGrid(x - 1, y    ) ? m_pDTM->asDouble(x - 1, y    ) : m_pDTM->asDouble(x, y);
		z[1]	= m_pDTM->is_InGrid(x    , y    ) ? m_pDTM->asDouble(x    , y    ) : m_pDTM->asDouble(x, y);
		z[2]	= m_pDTM->is_InGrid(x - 1, y - 1) ? m_pDTM->asDouble(x - 1, y - 1) : m_pDTM->asDouble(x, y);
		z[3]	= m_pDTM->is_InGrid(x    , y - 1) ? m_pDTM->asDouble(x    , y - 1) : m_pDTM->asDouble(x, y);
		break;

	case 3:
		z[0]	= m_pDTM->is_InGrid(x - 1, y + 1) ? m_pDTM->asDouble(x - 1, y + 1) : m_pDTM->asDouble(x, y);
		z[1]	= m_pDTM->is_InGrid(x    , y + 1) ? m_pDTM->asDouble(x    , y + 1) : m_pDTM->asDouble(x, y);
		z[2]	= m_pDTM->is_InGrid(x - 1, y    ) ? m_pDTM->asDouble(x - 1, y    ) : m_pDTM->asDouble(x, y);
		z[3]	= m_pDTM->is_InGrid(x    , y    ) ? m_pDTM->asDouble(x    , y    ) : m_pDTM->asDouble(x, y);
		break;
	}

	double	a = ((z[1] + z[0]) - (z[3] + z[2])) / (2.0 * Get_Cellsize());
	double	b = ((z[3] + z[1]) - (z[2] + z[0])) / (2.0 * Get_Cellsize());

	Height	= (z[0] + z[1] + z[2] + z[3]) / 4.0;

	Slope	= atan(sqrt(a*a + b*b));

	if( a != 0.0 )
	{
		Aspect	= M_PI_180 + atan2(b, a);
	}
	else if( b > 0.0 )
	{
		Aspect	= M_PI_270;
	}
	else if( b < 0.0 )
	{
		Aspect	= M_PI_090;
	}
	else
	{
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
double CConvergence::Get_2x2(int x, int y, bool bGradient)
{
	int		i, n;
	double	Height, Slope, Aspect, iSlope, iAspect, d, dSum;

	for(i=0, n=0, dSum=0.0, iAspect=-M_PI_135; i<4; i++, iAspect+=M_PI_090)
	{
		if( Get_2x2_Gradient(x, y, i, Slope, Aspect, Height) )
		{
			d		= Aspect - iAspect;

			if( bGradient )
			{
				iSlope	= atan((Height - m_pDTM->asDouble(x, y)) / Get_Length(1));
				d		= acos(sin(Slope) * sin(iSlope) + cos(Slope) * cos(iSlope) * cos(d));	// Nach dem Seiten-Kosinus-Satz...
			}

			d		= fmod(d, M_PI_360);

			if( d < -M_PI_180 )
			{
				d	+= M_PI_360;
			}
			else if( d > M_PI_180 )
			{
				d	-= M_PI_360;
			}

			dSum	+= fabs(d);
			n++;
		}
	}

	return( n > 0 ? (dSum / (double)n - M_PI_090) * 100.0 / M_PI_090 : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CConvergence::Get_9x9(int x, int y, bool bGradient)
{
	int		i, ix, iy, n;
	double	Slope, Aspect, iSlope, iAspect, d, dSum;

	for(i=0, n=0, dSum=0.0, iAspect=-M_PI_180; i<8; i++, iAspect+=M_PI_045)
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) && m_pDTM->Get_Gradient(ix, iy, Slope, Aspect) && Aspect >= 0.0 )
		{
			d		= Aspect - iAspect;

			if( bGradient )
			{
				iSlope	= atan((m_pDTM->asDouble(ix, iy) - m_pDTM->asDouble(x, y)) / Get_Length(i));
				d		= acos(sin(Slope) * sin(iSlope) + cos(Slope) * cos(iSlope) * cos(d));	// Nach dem Seiten-Kosinus-Satz...
			}

			d		= fmod(d, M_PI_360);

			if( d < -M_PI_180 )
			{
				d	+= M_PI_360;
			}
			else if( d > M_PI_180 )
			{
				d	-= M_PI_360;
			}

			dSum	+= fabs(d);
			n++;
		}
	}

	return( n > 0 ? (dSum / (double)n - M_PI_090) * 100.0 / M_PI_090 : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
