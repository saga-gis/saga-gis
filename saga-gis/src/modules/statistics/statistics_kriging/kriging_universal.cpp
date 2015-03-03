/**********************************************************
 * Version $Id: kriging_universal.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Kriging_Universal.cpp                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "kriging_universal.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Universal::CKriging_Universal(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Universal Kriging"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Universal Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Node(
		NULL	, "NODE_UK"		, _TL("Universal Kriging"),
		_TL("")
	);

	Parameters.Add_Grid_List(
		pNode	, "PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Choice(
		pNode	,"INTERPOL"		, _TL("Grid Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Value(
		pNode	, "COORDS"		, _TL("Coordinates"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::On_Initialize(void)
{
	m_pGrids		= Parameters("PREDICTORS")->asGridList();
	m_Interpolation	= Parameters("INTERPOL"  )->asInt();
	m_bCoords		= Parameters("COORDS"    )->asBool();

	//-----------------------------------------------------
	if( m_Search.Do_Use_All(true) )	// global
	{
		m_Data.Clear();

		for(int i=0; i<m_pPoints->Get_Count(); i++)
		{
			CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

			if( !pPoint->is_NoData(m_zField) )
			{
				bool	bAdd	= true;	// for better performance, make sure all predictors supply a value now

				for(int j=0; bAdd && j<m_pGrids->Get_Count(); j++)
				{
					bAdd	= m_pGrids->asGrid(j)->is_InGrid_byPos(pPoint->Get_Point(0));
				}

				if( bAdd )
				{
					m_Data.Add(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, m_bLog ? log(pPoint->asDouble(m_zField)) : pPoint->asDouble(m_zField));
				}
			}
		}

		return( Get_Weights(m_Data, m_W) );
	}

	//-----------------------------------------------------
	if( m_bLog )
	{
		CSG_Shapes	Points(SHAPE_TYPE_Point); Points.Add_Field("Z", SG_DATATYPE_Double);

		for(int iPoint=0; iPoint<m_pPoints->Get_Count() && Set_Progress(iPoint, m_pPoints->Get_Count()); iPoint++)
		{
			CSG_Shape	*pPoint	= m_pPoints->Get_Shape(iPoint);

			if( !pPoint->is_NoData(m_zField) )
			{
				bool	bAdd	= true;	// for better performance, make sure all predictors supply a value now

				for(int j=0; bAdd && j<m_pGrids->Get_Count(); j++)
				{
					bAdd	= m_pGrids->asGrid(j)->is_InGrid_byPos(pPoint->Get_Point(0));
				}

				if( bAdd )
				{
					Points.Add_Shape(pPoint, SHAPE_COPY_GEOM)->Set_Value(0, log(pPoint->asDouble(m_zField)));
				}
			}
		}

		return( m_Search.Initialize(&Points, 0) );
	}

	//-----------------------------------------------------
	return( m_Search.Initialize(m_pPoints, m_zField) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Get_Weights(const CSG_Points_Z &Points, CSG_Matrix &W)
{
	int	n	= Points.Get_Count();

	if( n > 0 )
	{
		int	i, j, k;

		int	nCoords	= m_bCoords ? 2 : 0;
		int	nGrids	= m_pGrids->Get_Count();

		W.Create(n + 1 + nGrids + nCoords, n + 1 + nGrids + nCoords);

		//-------------------------------------------------
		for(i=0; i<n; i++)
		{
			W[i][i]	= 0.0;				// diagonal...
			W[i][n]	= W[n][i]	= 1.0;	// edge...

			for(j=i+1; j<n; j++)
			{
				W[i][j]	= W[j][i]	= Get_Weight(Points.Get_X(i), Points.Get_Y(i), Points.Get_X(j), Points.Get_Y(j));
			}

			for(k=0, j=n+1; k<nGrids; k++, j++)
			{
				W[i][j]	= W[j][i]	= m_pGrids->asGrid(k)->Get_Value(Points.Get_X(i), Points.Get_Y(i), m_Interpolation);
			}

			for(k=0, j=n+nGrids+1; k<nCoords; k++, j++)
			{
				W[i][j]	= W[j][i]	= k == 0 ? Points.Get_X(i) : Points.Get_Y(i);
			}
		}

		for(i=n; i<=n+nGrids+nCoords; i++)
		{
			for(j=n; j<=n+nGrids+nCoords; j++)
			{
				W[i][j]	= 0.0;
			}
		}

		return( W.Set_Inverse(!m_Search.Do_Use_All(), n + 1 + nGrids + nCoords) );
	}	

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Get_Value(const TSG_Point &p, double &z, double &v)
{
	//-----------------------------------------------------
	int				i, j, n;
	double			**W;
	CSG_Matrix		_W;
	CSG_Points_Z	_Data, *pData;

	if( m_Search.Do_Use_All() )	// global
	{
		pData	= &m_Data;
		W		= m_W.Get_Data();
	}
	else if( m_Search.Get_Points(p, _Data) && Get_Weights(_Data, _W) )	// local
	{
		pData	= &_Data;
		W		= _W.Get_Data();
	}
	else
	{
		return( false );
	}

	//-----------------------------------------------------
	if(	(n = pData->Get_Count()) > 0 )
	{
		int	nCoords	= m_bCoords ? 2 : 0;
		int	nGrids	= m_pGrids->Get_Count();

		CSG_Vector	G(n + 1 + nGrids + nCoords);

		for(i=0; i<n; i++)
		{
			G[i]	= Get_Weight(p, pData->Get_Point(i));
		}

		G[n]	= 1.0;

		for(i=0, j=n+1; i<nGrids; i++, j++)
		{
			if( !m_pGrids->asGrid(i)->Get_Value(p, G[j], m_Interpolation) )
			{
				return( false );
			}
		}

		if( m_bCoords )
		{
			G[n + 1 + nGrids]	= p.x;
			G[n + 2 + nGrids]	= p.y;
		}

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			double	Lambda	= 0.0;

			for(j=0; j<=n+nGrids+nCoords; j++)
			{
				Lambda	+= W[i][j] * G[j];
			}

			z	+= Lambda * pData->Get_Z(i);
			v	+= Lambda * G[i];
		}

		//-------------------------------------------------
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
