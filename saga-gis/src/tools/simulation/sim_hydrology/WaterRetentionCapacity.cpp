/**********************************************************
 * Version $Id: WaterRetentionCapacity.cpp 966 2011-03-25 00:40:53Z johanvdw $
 *********************************************************/
/*******************************************************************************
    WaterRetentionCapacity.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "WaterRetentionCapacity.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWaterRetentionCapacity::CWaterRetentionCapacity(void)
{
	Set_Name		(_TL("Water Retention Capacity"));

	Set_Author		("V. Olaya (c) 2004");

	Set_Description	(_TW(
		"Water Retention Capacity. "
		"Plot hole input data must provide five attributes for each soil horizon in the following order and meaning:\n"
		"horizon depth, TF, L, Ar, Mo."
	));

	Add_Reference(
		"Gandullo, J. M.", "1994", "Climatología y ciencia del suelo", "No. 551.55 G3."
	);

	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Plot Holes"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"OUTPUT"	, _TL("Final Parameters"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
	
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RETENTION"	, _TL("Water Retention Capacity"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"INTERPOL"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Multilevel B-Spline Interpolation"),
			_TL("Inverse Distance Weighted")
		), 0
	);

	Parameters.Add_Bool("",
		"SLOPECORR"	, _TL("Slope Correction"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWaterRetentionCapacity::On_Execute(void)
{
	CSG_Shapes	*pInput		= Parameters("SHAPES")->asShapes();
	CSG_Shapes	*pOutput	= Parameters("OUTPUT")->asShapes();

	if( pInput->Get_Field_Count() < 5 )
	{
		Error_Set(_TL("Plot hole data has to provide at the very least five attributes (horizon depth, TF, L, Ar, Mo)."));

		return( false );
	}

	pOutput->Create(SHAPE_TYPE_Point, _TL("Water Retention Capacity"));

	pOutput->Add_Field("CCC"                     , SG_DATATYPE_Double);
	pOutput->Add_Field("CIL"                     , SG_DATATYPE_Double);
	pOutput->Add_Field("Permeability"            , SG_DATATYPE_Double);
	pOutput->Add_Field("Equivalent Moisture"     , SG_DATATYPE_Double);
	pOutput->Add_Field("Water Retention Capacity", SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();

	CSG_Matrix	Data(5, pInput->Get_Field_Count() / 5);

	for(int iPoint=0; iPoint<pInput->Get_Count(); iPoint++)
	{
		CSG_Shape	*pPoint	= pInput->Get_Shape(iPoint);

		for(int iHorizon=0, n=0; iHorizon<Data.Get_NRows(); iHorizon++, n+=5)
		{
			for(int i=0; i<5; i++)
			{
				Data[iHorizon][i]	= pPoint->asDouble(n + i);
			}
		}

		double	Slope, Aspect;

		if( !pDEM->Get_Gradient(pPoint->Get_Point(0), Slope, Aspect, GRID_RESAMPLING_BSpline) )
		{
			Slope	= 0.0;
		}

		Get_WaterRetention(Data, 1. - tan(Slope), pOutput->Add_Shape(pPoint, SHAPE_COPY_GEOM));
	}

	//-----------------------------------------------------
	CSG_Grid	*pRetention	= Parameters("RETENTION")->asGrid();

	if( pRetention )
	{
		switch( Parameters("INTERPOL")->asInt() )
		{
		default:	// Multlevel B-Spline Interpolation
			SG_RUN_TOOL_ExitOnError("grid_spline", 4,
					SG_TOOL_PARAMETER_SET("SHAPES"           , pOutput)
				&&  SG_TOOL_PARAMETER_SET("FIELD"            , pOutput->Get_Field_Count() - 1)
				&&  SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
				&&  SG_TOOL_PARAMETER_SET("TARGET_OUT_GRID"  , pRetention)
			);
			break;

		case  1:	// Inverse Distance Weighted
			SG_RUN_TOOL_ExitOnError("grid_gridding", 1,
					SG_TOOL_PARAMETER_SET("SHAPES"           , pOutput)
				&&  SG_TOOL_PARAMETER_SET("FIELD"            , pOutput->Get_Field_Count() - 1)
				&&  SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
				&&  SG_TOOL_PARAMETER_SET("TARGET_OUT_GRID"  , pRetention)
				&&  SG_TOOL_PARAMETER_SET("SEARCH_RANGE"     , 1)	// global
				&&  SG_TOOL_PARAMETER_SET("SEARCH_POINTS_ALL", 1)	// all points within search distance
			);
			break;
		}

		if( Parameters("SLOPECORR")->asBool() )
		{
			#pragma omp parallel for
			for(int y=0; y<Get_NY(); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( !pRetention->is_NoData(x, y) )
					{
						double	Slope, Aspect;

						if( !pDEM->Get_Gradient(x, y, Slope, Aspect) )
						{
							Slope	= 0.0;
						}

						pRetention->Mul_Value(x, y, 1. - tan(Slope));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWaterRetentionCapacity::Get_WaterRetention(CSG_Matrix &Data, double fC, CSG_Shape *pPoint)
{
	int		i;

	double	fWRC = 0, fPerm = 0, fHe = 0, fK = 0, fCCC = 0, fCIL = 0, fTotalDepth = 0;

	CSG_Vector	CCC (Data.Get_NRows());
	CSG_Vector	CIL (Data.Get_NRows());
	CSG_Vector	K   (Data.Get_NRows());
	CSG_Vector	Perm(Data.Get_NRows());
	CSG_Vector	He  (Data.Get_NRows());
	CSG_Vector	CRA (Data.Get_NRows());

	for(i=0; i<Data.Get_NRows(); i++)
	{
		CCC [i]	= Get_CCC(Data[i]);
		CIL [i]	= Get_CIL(Data[i]);
		He  [i]	= Get_He (Data[i]);

		Perm[i]	= Get_Permeability(CCC[i], CIL[i]);

		if( i > 0 )
		{
			K[i] = Get_K(Perm[i - 1], Perm[i], fC);
		}

		CRA[i]	= (double)((12.5 * He[i] + 12.5 * (50. - He[i]) * K[i] / 2.) * Data[i][1] / 100.);

		fTotalDepth	+= Data[i][0];
	}

	for(i=0; i<Data.Get_NRows(); i++)
	{
		fWRC	+= Data[i][0] / fTotalDepth * CRA [i];
		fCCC	+= Data[i][0] / fTotalDepth * CCC [i];
		fCIL	+= Data[i][0] / fTotalDepth * CIL [i];
		fPerm	+= Data[i][0] / fTotalDepth * Perm[i];
		fHe		+= Data[i][0] / fTotalDepth * He  [i];
		fK		+= Data[i][0] / fTotalDepth * K   [i];
	}

	pPoint->Set_Value(0, fCCC );
	pPoint->Set_Value(1, fCIL );
	pPoint->Set_Value(2, fPerm);
	pPoint->Set_Value(3, fHe  );
	pPoint->Set_Value(4, fWRC );
}

//---------------------------------------------------------
double CWaterRetentionCapacity::Get_He(double *Horizon)
{
	double fL	= Horizon[2];
	double fTF	= Horizon[1];
	double fAr	= Horizon[3];
	double fMO	= Horizon[4];

	return( 4.6 + 0.43 * fAr + 0.25 * fL + 1.22 * fMO );
}

//---------------------------------------------------------
double CWaterRetentionCapacity::Get_CIL(double *Horizon)
{
	double fL	= Horizon[2];
	double fTF	= Horizon[1];

	return( (fL * fTF) /10000. );
}

//---------------------------------------------------------
double CWaterRetentionCapacity::Get_CCC(double *Horizon)
{
	double fL	= Horizon[2];
	double fTF	= Horizon[1];
	double fAr	= Horizon[3];
	double fMO	= Horizon[4];

	return( (fAr - 4.*fMO) / fTF );
}

//---------------------------------------------------------
double CWaterRetentionCapacity::Get_K(double PermI, double PermS, double fC)
{
	double fAi = (PermI - 1) * .2;
	double fAs = (PermS - 1) * .2;

	return( (1. - fAi - (1 + fAs) * (1. - fC)) );
}

//---------------------------------------------------------
int CWaterRetentionCapacity::Get_Permeability(double fCCC, double fCIL)
{
	int	iPerm;

	if( fCCC < 0.15 )
	{
		iPerm = 5 - (int)((fCIL-0.1) / 0.15);
	}
	else if( fCIL < 0.2 )
	{
		iPerm = 5 - (int)(fCCC / 0.15);
	}
	else
	{
		iPerm = (int)(5 - (fCCC + fCIL - 0.1) / 0.15);
	}

	return( iPerm < 1 ? 1 : iPerm );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
