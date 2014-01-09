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
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_05.cpp                     //
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
#include "Exercise_05.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_05::CExercise_05(void)
{
	//-----------------------------------------------------
	// Give some information about your module...

	Set_Name	(_TL("05: Direct neighbours - slope and aspect"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Simple neighbourhood analysis for grid cells.\n"
		"- Zevenbergen, L.W. / Thorne, C.R. (1987):\n"
		"    'Quantitative analysis of land surface topography',\n"
		"    Earth Surface Processes and Landforms, 12: 47-56.\n\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "ELEVATION"	, _TL("Input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "SLOPE"		, _TL("Slope"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "ASPECT"		, _TL("Aspect"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL("Choose a method"),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Steepest gradient (first version)"),
			_TL("Steepest gradient (second version)"),
			_TL("Zevenbergen & Thorne")
		)
	);
}

//---------------------------------------------------------
CExercise_05::~CExercise_05(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_05::On_Execute(void)
{
	bool	bResult;
	int		Method;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pDTM		= Parameters("ELEVATION")->asGrid();

	m_pSlope	= Parameters("SLOPE"    )->asGrid();
	m_pAspect	= Parameters("ASPECT"   )->asGrid();

	Method		= Parameters("METHOD")->asInt();


	m_pSlope	->Set_ZFactor(180.0 / M_PI);
	m_pSlope	->Set_Unit(SG_T("Degree"));

	m_pAspect	->Set_ZFactor(180.0 / M_PI);
	m_pAspect	->Set_Unit(SG_T("Degree"));


	//-----------------------------------------------------
	// Execute calculation...

	switch( Method )
	{
	case 0:
		bResult	= Method_01();
		break;

	case 1:
		bResult	= Method_02();
		break;

	default:
		bResult	= false;
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_05::Method_01(void)
{
	int		x, y, i, ix, iy, iMax;
	double	z, dz, dzMax,
			dx[2]	= { Get_Cellsize(), sqrt(2.0) * Get_Cellsize()	};

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_NoData(x, y) )
			{
				m_pSlope	->Set_NoData(x, y);
				m_pAspect	->Set_NoData(x, y);
			}
			else
			{
				z		= m_pDTM->asDouble(x, y);
				iMax	= -1;

				for(i=0; i<8; i++)
				{
					ix		= Get_xTo(i, x);
					iy		= Get_yTo(i, y);

					if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
					{
						dz	= (z - m_pDTM->asDouble(ix, iy)) / dx[i % 2];

						if( dz > 0.0 && (iMax < 0 || (iMax >= 0 && dzMax < dz)) )
						{
							iMax	= i;
							dzMax	= dz;
						}
					}
				}

				if( iMax < 0 )
				{
					m_pSlope	->Set_NoData(x, y);
					m_pAspect	->Set_NoData(x, y);
				}
				else
				{
					m_pSlope	->Set_Value(x, y, atan(dzMax));
					m_pAspect	->Set_Value(x, y, M_PI_045 * iMax);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_05::Method_02(void)
{
	int		x, y, i, ix, iy, iMax;
	double	z, dz, dzMax;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_NoData(x, y) )
			{
				m_pSlope	->Set_NoData(x, y);
				m_pAspect	->Set_NoData(x, y);
			}
			else
			{
				z		= m_pDTM->asDouble(x, y);
				iMax	= -1;

				for(i=0; i<8; i++)
				{
					ix		= Get_xTo(i, x);
					iy		= Get_yTo(i, y);

					if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
					{
						dz	= (z - m_pDTM->asDouble(ix, iy)) / Get_Length(i);

						if( dz > 0.0 && (iMax < 0 || (iMax >= 0 && dzMax < dz)) )
						{
							iMax	= i;
							dzMax	= dz;
						}
					}
				}

				if( iMax < 0 )
				{
					m_pSlope	->Set_NoData(x, y);
					m_pAspect	->Set_NoData(x, y);
				}
				else
				{
					m_pSlope	->Set_Value(x, y, atan(dzMax));
					m_pAspect	->Set_Value(x, y, M_PI_045 * iMax);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_05::Method_03(void)
{
	static int	x_To[]	= { 0, 1, 0, -1 },
				y_To[]	= { 1, 0, -1, 0 };

	int		x, y, i, ix, iy;
	double	z, dz[4], G, H, dx2;

	//-----------------------------------------------------
	dx2		= 2.0 * Get_Cellsize();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_NoData(x, y) )
			{
				m_pSlope	->Set_NoData(x, y);
				m_pAspect	->Set_NoData(x, y);
			}

			//---------------------------------------------
			else
			{
				z		= m_pDTM->asDouble(x, y);

				for(i=0; i<4; i++)
				{
					ix		= x + x_To[i];
					iy		= y + y_To[i];

					if( m_pDTM->is_NoData(ix, iy) )
					{
						dz[i]	= 0.0;
					}
					else
					{
						dz[i]	= m_pDTM->asDouble(ix, iy) - z;
					}
				}

				//-----------------------------------------
				G		= (dz[0] - dz[2]) / dx2;
				H		= (dz[1] - dz[3]) / dx2;

				m_pSlope		->Set_Value(x, y, atan(sqrt(G*G + H*H)));

				if( G != 0.0 )
				{
					m_pAspect	->Set_Value(x, y, M_PI_180 + atan2(H, G));
				}
				else if( H > 0.0 )
				{
					m_pAspect	->Set_Value(x, y, M_PI_270);
				}
				else if( H < 0.0 )
				{
					m_pAspect	->Set_Value(x, y, M_PI_090);
				}
				else
				{
					m_pAspect	->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}
