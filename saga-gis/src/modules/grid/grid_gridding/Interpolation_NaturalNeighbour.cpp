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
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//          Interpolation_NaturalNeighbour.cpp           //
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
#include "Interpolation_NaturalNeighbour.h"

#include "nn/nn.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_NaturalNeighbour::CInterpolation_NaturalNeighbour(void)
{
	Set_Name		(_TL("Natural Neighbour"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		"Natural Neighbour method for grid interpolation from irregular distributed points.")
	);

	Parameters.Add_Value(
		NULL	, "SIBSON"	, _TL("Sibson"),
		_TL(""),
		PARAMETER_TYPE_Bool	, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_NaturalNeighbour::Interpolate(void)
{
	int			i, n, x, y;
	double		zMin, zMax;
	TSG_Point	p;

	nn_rule		= Parameters("SIBSON")->asBool() ? SIBSON : NON_SIBSONIAN;

	//-----------------------------------------------------
	point	*pSrc	= (point  *)SG_Malloc(m_pShapes->Get_Count() * sizeof(point));
	double	*zSrc	= (double *)SG_Malloc(m_pShapes->Get_Count() * sizeof(double));

	for(i=0, n=0; i<m_pShapes->Get_Count() && Set_Progress(i, m_pShapes->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= m_pShapes->Get_Shape(i);

		if( !pShape->is_NoData(m_zField) )
		{
			pSrc[n].x	= pShape->Get_Point(0).x;
			pSrc[n].y	= pShape->Get_Point(0).y;
			pSrc[n].z	= zSrc[n]	= pShape->asDouble(m_zField);

			if( n == 0 )
				zMin	= zMax	= pSrc[n].z;
			else if( zMin > pSrc[n].z )
				zMin	= pSrc[n].z;
			else if( zMax < pSrc[n].z )
				zMax	= pSrc[n].z;

			n++;
		}
	}

	Process_Set_Text(_TL("triangulating"));
	delaunay	*pTIN	= delaunay_build(n, pSrc, 0, NULL, 0, NULL);

	//-----------------------------------------------------
	double	*xDst	= (double *)SG_Malloc(m_pGrid->Get_NCells() * sizeof(double));
	double	*yDst	= (double *)SG_Malloc(m_pGrid->Get_NCells() * sizeof(double));
	double	*zDst	= (double *)SG_Malloc(m_pGrid->Get_NCells() * sizeof(double));

	for(y=0, i=0, p.y=m_pGrid->Get_YMin(); y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, p.y+=m_pGrid->Get_Cellsize())
	{
		for(x=0, p.x=m_pGrid->Get_XMin(); x<m_pGrid->Get_NX(); x++, p.x+=m_pGrid->Get_Cellsize(), i++)
		{
			xDst[i]	= p.x;
			yDst[i]	= p.y;
			zDst[i]	= NaN;
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("creating interpolator"));
	nnai	*pNN	= nnai_build(pTIN, m_pGrid->Get_NCells(), xDst, yDst);

    Process_Set_Text(_TL("interpolating"));
    nnai_interpolate(pNN, zSrc, zDst);

	//-----------------------------------------------------
	for(y=0, i=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
	{
		for(x=0; x<m_pGrid->Get_NX(); x++, i++)
		{
			double	z	= zDst[i];

			if( zMin <= z && z <= zMax )
			{
				m_pGrid->Set_Value(x, y, z);
			}
			else
			{
				m_pGrid->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	nnai_destroy(pNN);

	delaunay_destroy(pTIN);

	SG_Free(xDst);
	SG_Free(yDst);
	SG_Free(zDst);
	SG_Free(zSrc);
	SG_Free(pSrc);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
