/**********************************************************
 * Version $Id: GSPoints_Variogram_Surface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Variogram Surface"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description(
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "COUNT"		, _TL("Number of Pairs"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "VARIANCE"	, _TL("Variogram Surface"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "COVARIANCE"	, _TL("Covariance Surface"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "DISTCOUNT"	, _TL("Number of Distance Classes"),
		_TL(""),
		PARAMETER_TYPE_Int		, 10, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NSKIP"		, _TL("Skip Number"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSPoints_Variogram_Surface::On_Execute(void)
{
	sLong				i;
	int					j, x, y, n, nx, ny, nSkip, Attribute, nDistances;
	double				zi, zj, zMean, v, c, lagDistance;
	TSG_Point			Pt_i, Pt_j;
	CSG_Shape			*pPoint;
	CSG_Shapes			*pPoints;
	CSG_Grid			*pVariance, *pCovariance, *pCount;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	Attribute	= Parameters("FIELD")		->asInt();
	nSkip		= Parameters("NSKIP")		->asInt();
	nDistances	= Parameters("DISTCOUNT")	->asInt();

	lagDistance	= pPoints->Get_Extent().Get_XRange() < pPoints->Get_Extent().Get_YRange()
				? pPoints->Get_Extent().Get_XRange() / nDistances
				: pPoints->Get_Extent().Get_YRange() / nDistances;

	nx			= 1 + (int)(pPoints->Get_Extent().Get_XRange() / lagDistance);
	ny			= 1 + (int)(pPoints->Get_Extent().Get_YRange() / lagDistance);
	zMean		= pPoints->Get_Mean(Attribute);

	pCount		= SG_Create_Grid(SG_DATATYPE_Int  , 1 + 2 * nx, 1 + 2 * ny, lagDistance, -nx * lagDistance, -ny * lagDistance);
	pVariance	= SG_Create_Grid(SG_DATATYPE_Float, 1 + 2 * nx, 1 + 2 * ny, lagDistance, -nx * lagDistance, -ny * lagDistance);
	pCovariance	= SG_Create_Grid(SG_DATATYPE_Float, 1 + 2 * nx, 1 + 2 * ny, lagDistance, -nx * lagDistance, -ny * lagDistance);

	pCount		->Set_Name("%s [%s]"    , pPoints->Get_Name(), _TL("Count"));
	pVariance	->Set_Name("%s [%s: %s]", pPoints->Get_Name(), _TL("Variogram Surface") , pPoints->Get_Field_Name(Attribute));
	pCovariance	->Set_Name("%s [%s: %s]", pPoints->Get_Name(), _TL("Covariance Surface"), pPoints->Get_Field_Name(Attribute));

	//-----------------------------------------------------
	for(i=0, n=0; i<pPoints->Get_Count() && Set_Progress(n, SG_Get_Square(pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Attribute) )
		{
			Pt_i	= pPoint->Get_Point(0);
			zi		= pPoint->asDouble(Attribute);

			for(j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Attribute) )
				{
					Pt_j	= pPoint->Get_Point(0);

					zj	= pPoint->asDouble(Attribute);

					v	= SG_Get_Square(zi - zj);
					c	= (zi - zMean) * (zj - zMean);

					Pt_j.x	= (Pt_i.x - Pt_j.x) / lagDistance;
					Pt_j.y	= (Pt_i.y - Pt_j.y) / lagDistance;

					x	= (int)(Pt_j.x + (Pt_j.x > 0.0 ? 0.5 : -0.5));
					y	= (int)(Pt_j.y + (Pt_j.y > 0.0 ? 0.5 : -0.5));

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
	for(i=0; i<pCount->Get_NCells(); i++)
	{
		if( pCount->asInt(i) > 0 )
		{
			pVariance  ->Mul_Value(i, 0.5 / pCount->asInt(i));
			pCovariance->Mul_Value(i, 1.0 / pCount->asInt(i));
		}
		else
		{
			pVariance  ->Set_NoData(i);
			pCovariance->Set_NoData(i);
		}
	}

	DataObject_Add(pCount);
	DataObject_Add(pVariance);
	DataObject_Add(pCovariance);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
