/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Visibility_BASE.cpp                  //
//                                                       //
//               Copyright (C) 2003, 2013 by             //
//               Olaf Conrad, Volker Wichmann            //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Visibility_BASE.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
void CVisibility_BASE::Initialize(CSG_Grid *pVisibility, int iMethod)
{
	CSG_Colors	Colors;

	switch( iMethod )
	{
	case 0:		// Visibility
		pVisibility->Assign(0.0);
		Colors.Set_Count(2);
		Colors.Set_Ramp(SG_GET_RGB(0, 0, 0), SG_GET_RGB(255, 255, 255));
		break;

	case 1:		// Shade
		pVisibility->Assign(M_PI_090);
		pVisibility->Set_Unit(_TL("radians"));
		Colors.Set_Ramp(SG_GET_RGB(255, 255, 255), SG_GET_RGB(0, 0, 0));
		break;

	case 2:		// Distance
		pVisibility->Assign_NoData();
		Colors.Set_Ramp(SG_GET_RGB(255, 255, 191), SG_GET_RGB(0, 95, 0));
		break;

	case 3:		// Size
		pVisibility->Assign_NoData();
		pVisibility->Set_Unit(_TL("radians"));
		Colors.Set_Ramp(SG_GET_RGB(0, 95, 0), SG_GET_RGB(255, 255, 191));
		break;
	}

	SG_UI_DataObject_Colors_Set(pVisibility, &Colors);

	return;
}

//---------------------------------------------------------
void CVisibility_BASE::Set_Visibility(CSG_Grid *pDTM, CSG_Grid *pVisibility, int x_Pos, int y_Pos, double z_Pos, double dHeight, int iMethod)
{
	double		Exaggeration	= 1.0;

	double		aziDTM, decDTM,
				aziSrc, decSrc,
				d, dx, dy, dz;

	for(int y=0; y<pDTM->Get_NY() && SG_UI_Process_Set_Progress(y, pDTM->Get_NY()); y++)
	{
		for(int x=0; x<pDTM->Get_NX(); x++)
		{
			if( pDTM->is_NoData(x, y) )
			{
				pVisibility->Set_NoData(x, y);
			}
			else
			{
				dx		= x_Pos - x;
				dy		= y_Pos - y;
				dz		= z_Pos - pDTM->asDouble(x, y);

				//-----------------------------------------
				if( Trace_Point(pDTM, x, y, dx, dy, dz) )
				{
					switch( iMethod )
					{
					case 0:		// Visibility
						pVisibility->Set_Value(x, y, 1);
						break;

					case 1:		// Shade
						pDTM->Get_Gradient(x, y, decDTM, aziDTM);
						decDTM	= M_PI_090 - atan(Exaggeration * tan(decDTM));

						decSrc	= atan2(dz, sqrt(dx*dx + dy*dy));
						aziSrc	= atan2(dx, dy);

						d		= acos(sin(decDTM) * sin(decSrc) + cos(decDTM) * cos(decSrc) * cos(aziDTM - aziSrc));

						if( d > M_PI_090 )
							d = M_PI_090;

						if( pVisibility->asDouble(x, y) > d )
							pVisibility->Set_Value(x, y, d);
						break;

					case 2:		// Distance
						d		= pDTM->Get_Cellsize() * sqrt(dx*dx + dy*dy);

						if( pVisibility->is_NoData(x, y) || pVisibility->asDouble(x, y) > d )
							pVisibility->Set_Value(x, y, d);
						break;

					case 3:		// Size
						if( (d = pDTM->Get_Cellsize() * sqrt(dx*dx + dy*dy)) > 0.0 )
						{
							d	= atan2(dHeight, d);
							if( pVisibility->is_NoData(x, y) || pVisibility->asDouble(x, y) < d )
								pVisibility->Set_Value(x, y, d);
						}
						break;
					}
				}
			}
		}
	}

	return;
}


//---------------------------------------------------------
bool CVisibility_BASE::Trace_Point(CSG_Grid *pDTM, int x, int y, double dx, double dy, double dz)
{
	double	ix, iy, iz, id, d, dist;

	d		= fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);

	if( d > 0 )
	{
		dist	= sqrt(dx*dx + dy*dy);

		dx		/= d;
		dy		/= d;
		dz		/= d;

		d		= dist / d;

		id		= 0.0;
		ix		= x + 0.5;
		iy		= y + 0.5;
		iz		= pDTM->asDouble(x, y);

		while( id < dist )
		{
			id	+= d;

			ix	+= dx;
			iy	+= dy;
			iz	+= dz;

			x	= (int)ix;
			y	= (int)iy;

			if( !pDTM->is_InGrid(x, y) )
			{
				return( true );
			}
			else if( iz < pDTM->asDouble(x, y) )
			{
				return( false );
			}
			else if( iz > pDTM->Get_Max() )
			{
				return( true );
			}
		}
	}

	return( true );
}


//---------------------------------------------------------
void CVisibility_BASE::Finalize(CSG_Grid *pVisibility, int iMethod)
{
	CSG_Parameters	Parameters;

	switch( iMethod )
	{
	case 0:		// Visibility
		Parameters.Add_Range("", "METRIC_ZRANGE", SG_T(""), SG_T(""), 0.0, 1.0);
		SG_UI_DataObject_Update(pVisibility, true, &Parameters);
		break;

	case 1:		// Shade
		Parameters.Add_Range("", "METRIC_ZRANGE", SG_T(""), SG_T(""), 0.0, M_PI_090);
		SG_UI_DataObject_Update(pVisibility, true, &Parameters);
		break;

	case 2:		// Distance
	case 3:		// Size
		SG_UI_DataObject_Show(pVisibility, true);
		break;
	}

	return;
}


//---------------------------------------------------------

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
