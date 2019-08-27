
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   statistics_points                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             GSPoints_Variogram_Surface.cpp            //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GSPoints_Variogram_Surface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Variogram_Surface::CGSPoints_Variogram_Surface(void)
{
	Set_Name		(_TL("Variogram Surface"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description(
		_TL("Calculates a variogram surface.")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Int("",
		"DISTCOUNT"	, _TL("Number of Distance Classes"),
		_TL(""),
		10, 1, true
	);

	Parameters.Add_Int("",
		"NSKIP"		, _TL("Skip Number"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Grid_Output("", "COUNT"     , _TL("Number of Pairs"   ), _TL(""));
	Parameters.Add_Grid_Output("", "VARIANCE"  , _TL("Variogram Surface" ), _TL(""));
	Parameters.Add_Grid_Output("", "COVARIANCE", _TL("Covariance Surface"), _TL(""));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSPoints_Variogram_Surface::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	int	Field		= Parameters("FIELD"    )->asInt();
	int	nSkip		= Parameters("NSKIP"    )->asInt();
	int	nDistances	= Parameters("DISTCOUNT")->asInt();

	double	Lag	= pPoints->Get_Extent().Get_XRange() < pPoints->Get_Extent().Get_YRange()
				? pPoints->Get_Extent().Get_XRange() / nDistances
				: pPoints->Get_Extent().Get_YRange() / nDistances;

	int	nx	= 1 + (int)(pPoints->Get_Extent().Get_XRange() / Lag);
	int	ny	= 1 + (int)(pPoints->Get_Extent().Get_YRange() / Lag);

	CSG_Grid	*pCount      = SG_Create_Grid(SG_DATATYPE_Int  , 1 + 2 * nx, 1 + 2 * ny, Lag, -nx * Lag, -ny * Lag);
	CSG_Grid	*pVariance   = SG_Create_Grid(SG_DATATYPE_Float, 1 + 2 * nx, 1 + 2 * ny, Lag, -nx * Lag, -ny * Lag);
	CSG_Grid	*pCovariance = SG_Create_Grid(SG_DATATYPE_Float, 1 + 2 * nx, 1 + 2 * ny, Lag, -nx * Lag, -ny * Lag);

	pCount		->Fmt_Name("%s [%s]"    , pPoints->Get_Name(), _TL("Count"             ));
	pVariance	->Fmt_Name("%s [%s: %s]", pPoints->Get_Name(), _TL("Variogram Surface" ), pPoints->Get_Field_Name(Field));
	pCovariance	->Fmt_Name("%s [%s: %s]", pPoints->Get_Name(), _TL("Covariance Surface"), pPoints->Get_Field_Name(Field));

	Parameters("COUNT"     )->Set_Value(pCount     );
	Parameters("VARIANCE"  )->Set_Value(pVariance  );
	Parameters("COVARIANCE")->Set_Value(pCovariance);

	//-----------------------------------------------------
	for(int i=0, n=0; i<pPoints->Get_Count() && Set_Progress(n, 0.5 * SG_Get_Square(pPoints->Get_Count() / nSkip)); i+=nSkip)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Field) )
		{
			TSG_Point	pi	= pPoint->Get_Point(0);
			double		zi	= pPoint->asDouble(Field);

			for(int j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Field) )
				{
					TSG_Point	pj	= pPoint->Get_Point(0);
					double		zj	= pPoint->asDouble(Field);

					double	v	= SG_Get_Square(zi - zj);
					double	c	= (zi - pPoints->Get_Mean(Field)) * (zj - pPoints->Get_Mean(Field));

					pj.x	= (pi.x - pj.x) / Lag;
					pj.y	= (pi.y - pj.y) / Lag;

					int	x	= (int)(pj.x + (pj.x > 0. ? 0.5 : -0.5));
					int	y	= (int)(pj.y + (pj.y > 0. ? 0.5 : -0.5));

					pCount     ->Add_Value(nx + x, ny + y, 1);
					pCount     ->Add_Value(nx - x, ny - y, 1);
					pVariance  ->Add_Value(nx + x, ny + y, v);
					pVariance  ->Add_Value(nx - x, ny - y, v);
					pCovariance->Add_Value(nx + x, ny + y, c);
					pCovariance->Add_Value(nx - x, ny - y, c);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(sLong iCell=0; iCell<pCount->Get_NCells(); iCell++)
	{
		if( pCount->asInt(iCell) > 0 )
		{
			pVariance  ->Mul_Value(iCell, 0.5 / pCount->asDouble(iCell));
			pCovariance->Mul_Value(iCell, 1.0 / pCount->asDouble(iCell));
		}
		else
		{
			pVariance  ->Set_NoData(iCell);
			pCovariance->Set_NoData(iCell);
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
