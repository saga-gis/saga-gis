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
//                 Convergence_Radius.cpp                //
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
#include <memory.h>

#include "Convergence_Radius.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CConvergence_Radius::CConvergence_Radius(void)
{
	Set_Name	(_TL("Convergence Index (Search Radius)"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Reference:\n"
		"Koethe, R. / Lehmeier, F. (1996):\n'SARA – System zur Automatischen Relief-Analyse',\n"
		"User Manual, 2. Edition [Dept. of Geography, University of Goettingen, unpublished]\n"
	));

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Convergence_Radius Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int		, 10, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Standard"),
			_TL("Distance Weighted (Linear)"),
			_TL("Distance Weighted (Inverse)")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "SLOPE"		, _TL("Gradient"),
		_TL(""),
		PARAMETER_TYPE_Bool		, 0.0
	);

	Parameters.Add_Choice(
		NULL	, "DIFF"		, _TL("Difference"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("direction to the center cell"),
			_TL("center cell's aspect direcion")
		), 0
	);
}

//---------------------------------------------------------
CConvergence_Radius::~CConvergence_Radius(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvergence_Radius::On_Execute(void)
{
	CSG_Grid	*pDTM, *pConvergence_Radius;

	pDTM				= Parameters("ELEVATION")	->asGrid();
	pConvergence_Radius	= Parameters("RESULT")		->asGrid();

	if( Initialize(pDTM, Parameters("RADIUS")->asInt()) )
	{
		DataObject_Set_Colors(pConvergence_Radius, 100, SG_COLORS_RED_GREY_BLUE, true);

		pConvergence_Radius->Assign_NoData();

		Get_Convergence_Radius(
			pDTM,
			pConvergence_Radius,
			Parameters("SLOPE")	->asBool(),
			Parameters("DIFF")	->asInt() == 0 ? true : false,
			Parameters("METHOD")->asInt()
		);

		Finalize();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvergence_Radius::Initialize(CSG_Grid *pDTM, int Radius)
{
	int		x, y, ix, iy;
	double	Slope, Aspect, d;

	if( Radius > 0 )
	{
		//-------------------------------------------------
		m_Radius	= Radius;
		m_Diameter	= 1 + 2 * Radius;

		m_Dir.Create(SG_DATATYPE_Double, m_Diameter, m_Diameter);
		m_Dst.Create(SG_DATATYPE_Double, m_Diameter, m_Diameter);

		for(iy=0, y=-m_Radius; iy<m_Diameter; iy++, y++)
		{
			for(ix=0, x=-m_Radius; ix<m_Diameter; ix++, x++)
			{
				d	= M_GET_LENGTH(x, y);

				if( d < 1 || d > m_Radius )
				{
					m_Dir.Set_NoData(ix, iy);
					m_Dst.Set_Value	(ix, iy, 0);
				}
				else
				{
					m_Dir.Set_Value	(ix, iy, y != 0.0 ? M_PI_180 + atan2((double)x, (double)y) : (x > 0.0 ? M_PI_270 : M_PI_090));
					m_Dst.Set_Value	(ix, iy, d - 1);
				}
			}
		}

		//-------------------------------------------------
		m_Slope	.Create(pDTM, SG_DATATYPE_Float);
		m_Aspect.Create(pDTM, SG_DATATYPE_Float);

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( pDTM->is_InGrid(x, y) && pDTM->Get_Gradient(x, y, Slope, Aspect) && Aspect >= 0.0 )
				{
					m_Slope	.Set_Value(x, y, Slope);
					m_Aspect.Set_Value(x, y, Aspect);
				}
				else
				{
					m_Slope	.Set_NoData(x, y);
					m_Aspect.Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
		m_iSum		= (double *)malloc(m_Radius * sizeof(double));
		m_iCnt		= (int    *)malloc(m_Radius * sizeof(int   ));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CConvergence_Radius::Finalize(void)
{
	m_Dir	.Destroy();
	m_Dst	.Destroy();

	m_Slope	.Destroy();
	m_Aspect.Destroy();

	free(m_iSum);
	free(m_iCnt);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PI2PERC	(100.0 / M_PI_090)

//---------------------------------------------------------
void CConvergence_Radius::Get_Convergence_Radius(CSG_Grid *pDTM, CSG_Grid *pConvergence_Radius, bool bSlope, bool bCenterDiff, int Method)
{
	int		x, y, ix, iy, jx, jy, iDst;
	double	Slope, Aspect, d, dSum, dCnt, z, dSlope, Dir;

	dSlope	= Get_Cellsize();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_Aspect.is_InGrid(x, y) )
			{
				z		= pDTM->asDouble(x, y);

				memset(m_iSum, 0, m_Radius * sizeof(double));
				memset(m_iCnt, 0, m_Radius * sizeof(int   ));

				for(iy=0, jy=y-m_Radius; iy<m_Diameter; iy++, jy++)
				{
					for(ix=0, jx=x-m_Radius; ix<m_Diameter; ix++, jx++)
					{
						if( m_Dir.is_InGrid(ix, iy) && m_Aspect.is_InGrid(jx, jy) && (Aspect = m_Aspect.asDouble(jx, jy)) >= 0.0 )
						{
							Dir	= bCenterDiff
								? m_Dir		.asDouble(ix, iy)
								: m_Aspect	.asDouble( x,  y) + M_PI_360;

							if( bSlope )
							{
								Slope	= bCenterDiff
										? m_Slope.asDouble(jx, jy)
										: m_Slope.asDouble(jx, jy) - m_Slope.asDouble(x, y) + M_PI_360;

								d		= atan((pDTM->asDouble(jx, jy) - z) / (dSlope * m_Dst.asDouble(ix, iy)));
								d		= acos(sin(Slope) * sin(d) + cos(Slope) * cos(d) * cos(Dir - Aspect));
							}
							else
							{
								d		= Aspect - Dir;
							}

							//-----------------------------
							d		= fmod(d, M_PI_360);

							if( d < -M_PI_180 )
							{
								d	+= M_PI_360;
							}
							else if( d > M_PI_180 )
							{
								d	-= M_PI_360;
							}

							//-----------------------------
							iDst			= m_Dst.asInt(ix, iy);
							m_iSum[iDst]	+= fabs(d);
							m_iCnt[iDst]++;
						}
					}
				}

				//-----------------------------------------
				for(iDst=1; iDst<m_Radius; iDst++)
				{
					m_iCnt[iDst]	+= m_iCnt[iDst - 1];
					m_iSum[iDst]	+= m_iSum[iDst - 1];
				}

				switch( Method )
				{
				case 0:	// Standard
					if( (dCnt = m_iCnt[m_Radius - 1]) > 0 )
					{
						pConvergence_Radius->Set_Value(x, y, PI2PERC * (m_iSum[m_Radius - 1] / dCnt - M_PI_090));
					}
					break;

				case 1:	// Distance Weighted (Linear)
					for(iDst=0, dSum=0.0; iDst<m_Radius; iDst++)
					{
						if( m_iCnt[iDst] > 0 )
						{
							dSum	+=  PI2PERC * (m_iSum[iDst] / (double)m_iCnt[iDst] - M_PI_090);
						}
					}

					pConvergence_Radius->Set_Value(x, y, dSum / (double)m_Radius);
					break;

				case 2:	// Distance Weighted (Inverse)
					for(iDst=0, dSum=0.0, dCnt=0.0; iDst<m_Radius; iDst++)
					{
						if( m_iCnt[iDst] > 0 )
						{
							d		= 1.0 / (1.0 + iDst);

							dCnt	+= d;
							dSum	+= d * PI2PERC * (m_iSum[iDst] / (double)m_iCnt[iDst] - M_PI_090);
						}
					}

					pConvergence_Radius->Set_Value(x, y, dSum / (double)dCnt);
					break;
				}
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
