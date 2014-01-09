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
//                       MRVBF.cpp                       //
//                                                       //
//                 Copyright (C) 2006 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mrvbf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMRVBF::CMRVBF(void)
{
	Set_Name		(_TL("Multiresolution Index of Valley Bottom Flatness (MRVBF)"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"Calculation of the 'multiresolution index of valley bottom flatness' (MRVBF) and "
		"the complementary 'multiresolution index of the ridge top flatness' (MRRTF). "
		"\n\n"
		"References:\n"
		"- Gallant, J.C., Dowling, T.I. (2003): "
		"  'A multiresolution index of valley bottom flatness for mapping depositional areas', "
		"  Water Resources Research, 39/12:1347-1359\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "MRVBF"		, _TL("MRVBF"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "MRRTF"		, _TL("MRRTF"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "T_SLOPE"		, _TL("Initial Threshold for Slope"),
		_TL(""),
		PARAMETER_TYPE_Double	, 16.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "T_PCTL_V"	, _TL("Threshold for Elevation Percentile (Lowness)"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.40, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "T_PCTL_R"	, _TL("Threshold for Elevation Percentile (Upness)"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.35, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "P_SLOPE"		, _TL("Shape Parameter for Slope"),
		_TL(""),
		PARAMETER_TYPE_Double	, 4.0
	);

	Parameters.Add_Value(
		NULL	, "P_PCTL"		, _TL("Shape Parameter for Elevation Percentile"),
		_TL(""),
		PARAMETER_TYPE_Double	, 3.0
	);

	Parameters.Add_Value(
		NULL	, "UPDATE"		, _TL("Update Views"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Value(
		NULL	, "CLASSIFY"	, _TL("Classify"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Value(
		NULL	, "MAX_RES"		, _TL("Maximum Resolution (Percentage)"),
		_TL("Maximum resolution as percentage of the diameter of the DEM."),
		PARAMETER_TYPE_Double	, 100.0, 0.0, true, 100.0, true
	);
}

//---------------------------------------------------------
CMRVBF::~CMRVBF(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define UPDATE_VIEWS(b)	if( bUpdate )	{	DataObject_Update(pMRRTF, b);	DataObject_Update(pMRVBF, b);	}

//---------------------------------------------------------
bool CMRVBF::On_Execute(void)
{
	bool		bUpdate;
	int			Level;
	double		T_Slope, Resolution, max_Resolution;
	CSG_Grid	*pDEM, *pMRVBF, *pMRRTF, CF, VF, RF, DEM, Slope, Pctl;

	//-----------------------------------------------------
	pDEM			= Parameters("DEM")			->asGrid();
	pMRVBF			= Parameters("MRVBF")		->asGrid();
	pMRRTF			= Parameters("MRRTF")		->asGrid();

	T_Slope			= Parameters("T_SLOPE")		->asDouble();

	m_T_Pctl_V		= Parameters("T_PCTL_V")	->asDouble();
	m_T_Pctl_R		= Parameters("T_PCTL_R")	->asDouble();

	m_P_Slope		= Parameters("P_SLOPE")		->asDouble();
	m_P_Pctl		= Parameters("P_PCTL")		->asDouble();

	bUpdate			= Parameters("UPDATE")		->asBool();

	max_Resolution	= Parameters("MAX_RES")		->asDouble() / 100.0;
	Resolution		= SG_Get_Length(Get_System()->Get_XRange(), Get_System()->Get_YRange());
	max_Resolution	= max_Resolution * Resolution;

	//-----------------------------------------------------
	if( 1 )
	{
	//	DataObject_Set_Colors(pMRVBF, 100, SG_COLORS_WHITE_BLUE		, false);
		DataObject_Set_Colors(pMRVBF, 100, SG_COLORS_RED_GREY_BLUE	, false);

	//	DataObject_Set_Colors(pMRRTF, 100, SG_COLORS_RED_GREY_BLUE	, true);
		DataObject_Set_Colors(pMRRTF, 100, SG_COLORS_WHITE_RED		, false);

		CSG_Grid	CF, VF, RF, DEM, Slopes, Percentiles;

		VF.Create(*Get_System(), SG_DATATYPE_Float);
		RF.Create(*Get_System(), SG_DATATYPE_Float);
		CF.Create(*Get_System(), SG_DATATYPE_Float);
		CF.Assign(1.0);

		DEM.Create(*pDEM);

		//-------------------------------------------------
		Level		= 1;
		Resolution	= Get_Cellsize();

		Process_Set_Text(CSG_String::Format(SG_T("%d. %s"), Level, _TL("step")));
		Message_Add(CSG_String::Format(SG_T("%s: %d, %s: %.2f, %s %.2f"), _TL("step"), Level, _TL("resolution"), Resolution, _TL("threshold slope"), T_Slope));

		Get_Slopes		(&DEM, &Slopes);
		Get_Percentiles	(&DEM, &Percentiles, 3);
		Get_Flatness	(&Slopes, &Percentiles, &CF, pMRVBF, pMRRTF, T_Slope);
		UPDATE_VIEWS	(true);

		//-------------------------------------------------
		T_Slope		/= 2.0;
		Level++;

		Process_Set_Text(CSG_String::Format(SG_T("%d. %s"), Level, _TL("step")));
		Message_Add(CSG_String::Format(SG_T("%s: %d, %s: %.2f, %s %.2f"), _TL("step"), Level, _TL("resolution"), Resolution, _TL("threshold slope"), T_Slope));

		Get_Percentiles	(&DEM, &Percentiles, 6);
		Get_Flatness	(&Slopes, &Percentiles, &CF, &VF, &RF, T_Slope);
		Get_MRVBF		(Level, pMRVBF, &VF, pMRRTF, &RF);
		UPDATE_VIEWS	(false);

		//-------------------------------------------------
		while( Process_Get_Okay(false) && Resolution < max_Resolution )
		{
			Resolution	*= 3.0;
			T_Slope		/= 2.0;
			Level++;

			Process_Set_Text(CSG_String::Format(SG_T("%d. %s"), Level, _TL("step")));
			Message_Add(CSG_String::Format(SG_T("%s: %d, %s: %.2f, %s %.2f"), _TL("step"), Level, _TL("resolution"), Resolution, _TL("threshold slope"), T_Slope));

			Get_Values		(&DEM, &Slopes, &Percentiles, Resolution);
			Get_Flatness	(&Slopes, &Percentiles, &CF, &VF, &RF, T_Slope);
			Get_MRVBF		(Level, pMRVBF, &VF, pMRRTF, &RF);
			UPDATE_VIEWS	(false);
		}

		if( Parameters("CLASSIFY")->asBool() )
		{
			Get_Classified(pMRVBF);
			Get_Classified(pMRRTF);
		}

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
inline double CMRVBF::Get_Transformation(double x, double t, double p)
{
	return( 1.0 / (1.0 + pow(x / t, p)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMRVBF::Get_Smoothed(CSG_Grid *pDEM, CSG_Grid *pSmoothed, int Radius, double Smoothing)
{
	if( pDEM && pSmoothed )
	{
		int		x, y, ix, iy, jx, jy, kx, ky;
		double	d, s;

		CSG_Grid	Kernel(SG_DATATYPE_Double, 1 + 2 * Radius, 1 + 2 * Radius);

		for(iy=-Radius, ky=0; iy<=Radius; iy++, ky++)
		{
			for(ix=-Radius, kx=0; ix<=Radius; ix++, kx++)
			{
				Kernel.Set_Value(kx, ky, 4.3565 * exp(-SG_Get_Square(sqrt((double)ix*ix + iy*iy) / 3.0)) );
			//	Kernel.Set_Value(kx, ky, exp(-(ix*ix + iy*iy) / (2.0 * s)) / (2.0 * M_PI * s));
			}
		}

		pSmoothed->Create(pDEM, SG_DATATYPE_Float);

		for(y=0; y<pDEM->Get_NY() && Set_Progress(y, pDEM->Get_NY()); y++)
		{
			for(x=0; x<pDEM->Get_NX(); x++)
			{
				for(iy=-Radius, jy=y-Radius, ky=0, s=d=0.0; iy<=Radius; iy++, jy++, ky++)
				{
					for(ix=-Radius, jx=x-Radius, kx=0; ix<=Radius; ix++, jx++, kx++)
					{
						if( pDEM->is_InGrid(jx, jy) )
						{
							s	+= Kernel.asDouble(kx, ky) * pDEM->asDouble(jx, jy);
							d	+= Kernel.asDouble(kx, ky);
						}
					}
				}

				if( d > 0.0 )
				{
					pSmoothed->Set_Value(x, y, s / d);
				}
				else
				{
					pSmoothed->Set_NoData(x, y);
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CMRVBF::Get_Values(CSG_Grid *pDEM, CSG_Grid *pSlopes, CSG_Grid *pPercentiles, double Resolution)
{
	if( pDEM && pDEM->is_Valid() && pSlopes && pPercentiles )
	{
		int			nx, ny;
		CSG_Grid	Smoothed;

		Get_Smoothed(pDEM, &Smoothed, 5, 3.0);
		Get_Slopes(&Smoothed, pSlopes);

		nx	= 2 + (int)(pDEM->Get_XRange() / Resolution);
		ny	= 2 + (int)(pDEM->Get_YRange() / Resolution);

		pDEM->Create(SG_DATATYPE_Float, nx, ny, Resolution, pDEM->Get_XMin(), pDEM->Get_YMin());
		pDEM->Assign(&Smoothed, GRID_INTERPOLATION_NearestNeighbour);

		Get_Percentiles(pDEM, pPercentiles, 6);

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
bool CMRVBF::Get_Percentile(CSG_Grid *pDEM, int x, int y, double &Percentile)
{
	if( pDEM && pDEM->is_Valid() && pDEM->is_InGrid(x, y, true) )
	{
		int		iRadius, iPoint, nPoints, nLower, ix, iy;
		double	z	= pDEM->asDouble(x, y);

		for(iRadius=0, nPoints=0, nLower=0; iRadius<m_Radius.Get_Maximum(); iRadius++)
		{
			for(iPoint=0; iPoint<m_Radius.Get_nPoints(iRadius); iPoint++)
			{
				m_Radius.Get_Point(iRadius, iPoint, ix, iy);

				ix	+= x;
				iy	+= y;

				if( pDEM->is_InGrid(ix, iy) )
				{
					nPoints++;

					if( pDEM->asDouble(ix, iy) < z )
					{
						nLower++;
					}
				}
			}
		}

		if( nPoints > 1 )
		{
			Percentile	= (double)nLower / (double)(nPoints - 1.0);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CMRVBF::Get_Percentiles(CSG_Grid *pDEM, CSG_Grid *pPercentiles, int Radius)
{
	if( pDEM && pDEM->is_Valid() )
	{
		pPercentiles->Create(pDEM->Get_System(), SG_DATATYPE_Float);

		m_Radius.Create(Radius);

		for(int y=0; y<pDEM->Get_NY() && Set_Progress(y, pDEM->Get_NY()); y++)
		{
			for(int x=0; x<pDEM->Get_NX(); x++)
			{
				double	Percentile;

				if( !Get_Percentile(pDEM, x, y, Percentile) )
				{
					pPercentiles->Set_NoData(x, y);
				}
				else
				{
					pPercentiles->Set_Value (x, y, Percentile);
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CMRVBF::Get_Slopes(CSG_Grid *pDEM, CSG_Grid *pSlopes)
{
	if( pDEM && pDEM->is_Valid() )
	{
		pSlopes->Create(pDEM->Get_System(), SG_DATATYPE_Float);

		for(int y=0; y<pDEM->Get_NY() && Set_Progress(y, pDEM->Get_NY()); y++)
		{
			for(int x=0; x<pDEM->Get_NX(); x++)
			{
				double	Slope, Aspect;

				if( !pDEM->Get_Gradient(x, y, Slope, Aspect) )
				{
					pSlopes->Set_NoData(x, y);
				}
				else
				{
					pSlopes->Set_Value (x, y, 100.0 * tan(Slope));
				}
			}
		}

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
bool CMRVBF::Get_Flatness(CSG_Grid *pSlopes, CSG_Grid *pPercentiles, CSG_Grid *pCF, CSG_Grid *pVF, CSG_Grid *pRF, double T_Slope)
{
//	const int	Interpolation	= GRID_INTERPOLATION_Bilinear;
	const int	Interpolation	= GRID_INTERPOLATION_BSpline;

	if( pSlopes && pSlopes->is_Valid() && pPercentiles && pPercentiles->is_Valid() )
	{
		int		x, y;
		double	xp, yp, Slope, Percentile, cf, vf, rf;

		for(y=0, yp=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, yp+=Get_Cellsize())
		{
			for(x=0, xp=Get_XMin(); x<Get_NX(); x++, xp+=Get_Cellsize())
			{
				if( pSlopes		->Get_Value(xp, yp, Slope     , Interpolation)
				&&	pPercentiles->Get_Value(xp, yp, Percentile, Interpolation) )
				{
					cf	= pCF->asDouble(x, y) * Get_Transformation(Slope, T_Slope, m_P_Slope);
					vf	= cf * Get_Transformation(      Percentile, m_T_Pctl_V, m_P_Pctl);
					rf	= cf * Get_Transformation(1.0 - Percentile, m_T_Pctl_R, m_P_Pctl);

					pCF->Set_Value	(x, y, cf);
					pVF->Set_Value	(x, y, 1.0 - Get_Transformation(vf, 0.3, 4.0));
					pRF->Set_Value	(x, y, 1.0 - Get_Transformation(rf, 0.3, 4.0));
				}
				else
				{
					pVF->Set_NoData	(x, y);
					pRF->Set_NoData	(x, y);
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CMRVBF::Get_MRVBF(int Level, CSG_Grid *pMRVBF, CSG_Grid *pVF, CSG_Grid *pMRRTF, CSG_Grid *pRF)
{
	if( pMRVBF && pVF && pMRRTF && pRF )
	{
		double	d, w, t, p;

		t	= 0.4;
		p	= log((Level - 0.5) / 0.1) / log(1.5);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pMRVBF->is_NoData(x, y) && !pVF->is_NoData(x, y) )
				{
					d	= pVF->asDouble(x, y);
					w	= 1.0 - Get_Transformation(d, t, p);
					pMRVBF->Set_Value(x, y, w * (Level - 1 + d) + (1.0 - w) * pMRVBF->asDouble(x, y));
				}

				if( !pMRRTF->is_NoData(x, y) && !pRF->is_NoData(x, y) )
				{
					d	= pRF->asDouble(x, y);
					w	= 1.0 - Get_Transformation(d, t, p);
					pMRRTF->Set_Value(x, y, w * (Level - 1 + d) + (1.0 - w) * pMRRTF->asDouble(x, y));
				}
			}
		}

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
bool CMRVBF::Get_Classified(CSG_Grid *pMRF)
{
	if( pMRF && pMRF->is_Valid() )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pMRF->is_NoData(x, y) )
				{
					double	d	= pMRF->asDouble(x, y);

					if		( d < 0.5 )
						pMRF->Set_Value(x, y, 0.0);
					else if	( d < 1.5 )
						pMRF->Set_Value(x, y, 1.0);
					else if	( d < 2.5 )
						pMRF->Set_Value(x, y, 2.0);
					else if	( d < 3.5 )
						pMRF->Set_Value(x, y, 3.0);
					else if	( d < 4.5 )
						pMRF->Set_Value(x, y, 4.0);
					else if	( d < 5.5 )
						pMRF->Set_Value(x, y, 5.0);
					else
						pMRF->Set_Value(x, y, 6.0);
				}
			}
		}

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
