
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              ChannelNetwork_Distance.cpp              //
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
#include "ChannelNetwork_Distance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChannelNetwork_Distance::CChannelNetwork_Distance(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Overland Flow Distance to Channel Network"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"This module calculates overland flow distances to a channel network "
		"based on gridded digital elevation data and channel network information.\n"
		"The flow algorithm may be either Deterministic 8 (O'Callaghan & Mark 1984) or Multiple Flow Direction (Freeman 1991)\n"
		"\n\nReferences:\n"
		"\n- Freeman, G.T., 1991: 'Calculating catchment area with divergent flow based on a regular grid', Computers and Geosciences, 17:413-22\n"
		"\n- O'Callaghan, J.F., Mark, D.M., 1984: 'The extraction of drainage networks from digital elevation data', Computer Vision, Graphics and Image Processing, 28:323-344\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		"A grid that contains elevation data.",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHANNELS"	, _TL("Channel Network"),
		_TL("A grid providing information about the channel network. It is assumed that no-data cells are not part "
		"of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "DISTANCE"	, _TL("Overland Flow Distance"),
		"The overland flow distance in map units. "
		"It is assumed that the (vertical) elevation data use the same units as the (horizontal) grid coordinates.",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTVERT"	, _TL("Vertical Overland Flow Distance"),
		"This is the vertical component of the overland flow",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTHORZ"	, _TL("Horizontal Overland Flow Distance"),
		"This is the horizontal component of the overland flow",
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Flow Algorithm"),
		_TL("Choose a flow routing algorithm that shall be used for the overland flow distance calculation:\n- D8\n- MFD"),
		_TL("D8|"
		"MFD|")	, 1
	);
}

//---------------------------------------------------------
CChannelNetwork_Distance::~CChannelNetwork_Distance(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Distance::On_Execute(void)
{
	int		x, y, n, Method;

	CGrid	*pChannels;

	//-----------------------------------------------------
	pDTM		= Parameters("ELEVATION")->asGrid();
	pChannels	= Parameters("CHANNELS")->asGrid();

	pDistance	= Parameters("DISTANCE")->asGrid();
	pDistVert	= Parameters("DISTVERT")->asGrid();
	pDistHorz	= Parameters("DISTHORZ")->asGrid();

	pDistance->Assign_NoData();
	pDistVert->Assign_NoData();
	pDistHorz->Assign_NoData();

	Method		= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	switch( Method )
	{
	case 0:	default:
		Initialize_D8();
		break;

	case 1:
		Initialize_MFD();
		break;
	}

	pDTM->Get_Sorted(0, x, y);

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		pDTM->Get_Sorted(n, x, y, false);

		if( pDTM->is_NoData(x, y) )
		{
			pDistance->Set_NoData(x, y);
			pDistVert->Set_NoData(x, y);
			pDistHorz->Set_NoData(x, y);
		}
		else if( !(pChannels->is_NoData(x, y) && pDistance->is_NoData(x, y)) )
		{
			if( !pChannels->is_NoData(x, y) )
			{
				pDistance->Set_Value(x, y, 0.0);
				pDistVert->Set_Value(x, y, 0.0);
				pDistHorz->Set_Value(x, y, 0.0);
			}

			switch( Method )
			{
			case 0:	default:
				Execute_D8(x, y);
				break;

			case 1:
				Execute_MFD(x, y);
				break;
			}
		}
	}

	//-----------------------------------------------------
	switch( Method )
	{
	case 0:	default:
		Finalize_D8();
		break;

	case 1:
		Finalize_MFD();
		break;
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
void CChannelNetwork_Distance::Initialize_D8(void)
{
	int		x, y;

	pFlow	= SG_Create_Grid(pDTM, GRID_TYPE_Char);

	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			pFlow->Set_Value(x, y, pDTM->Get_Gradient_NeighborDir(x, y));
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Finalize_D8(void)
{
	delete(pFlow);
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_D8(int x, int y)
{
	int		i, ix, iy;

	double	zDist, zVert, zHorz, dVert, dHorz;

	zDist	= pDistance->asDouble(x, y);
	zVert	= pDistVert->asDouble(x, y);
	zHorz	= pDistHorz->asDouble(x, y);

	for(i=1; i<=8; i++)
	{
		ix		= Get_xFrom(i, x);
		iy		= Get_yFrom(i, y);

		if( pDTM->is_InGrid(ix, iy) && pFlow->asInt(ix, iy) == i )
		{
			dVert	= pDTM->asDouble(ix, iy) - pDTM->asDouble(x, y);
			dHorz	= Get_Length(i);

			pDistVert->Set_Value(ix, iy, zVert + dVert);
			pDistHorz->Set_Value(ix, iy, zHorz + dHorz);
			pDistance->Set_Value(ix, iy, zDist + sqrt(dVert*dVert + dHorz*dHorz));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_MFD(void)
{
	const double	MFD_Convergence	= 1.1;

	int		x, y, i, ix, iy;

	double	*Flow, z, dz, zSum;

	pFlow	= SG_Create_Grid(pDTM, GRID_TYPE_Long);
	Flow	= (double *)SG_Calloc(8 * Get_NCells(), sizeof(double));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++, Flow+=8)
		{
			if( pDTM->is_NoData(x, y) )
			{
				pFlow->Set_Value(x, y, NULL);
			}
			else
			{
				pFlow->Set_Value(x, y, (long)Flow);

				z		= pDTM->asDouble(x, y);
				zSum	= 0.0;

				for(i=0; i<8; i++)
				{
					ix		= Get_xTo(i, x);
					iy		= Get_yTo(i, y);

					if( pDTM->is_InGrid(ix, iy) && z > (dz = pDTM->asDouble(ix, iy)) )
					{
						zSum	+= (Flow[i]	= pow((z - dz) / Get_Length(i), MFD_Convergence));
					}
				}

				if( zSum > 0.0 )
				{
					for(i=0; i<8; i++)
					{
						Flow[i]	/= zSum;
					}
				}
			}
		}
	}

	pMFDSum	= SG_Create_Grid(pDTM, GRID_TYPE_Float);
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Finalize_MFD(void)
{
	SG_Free((double *)pFlow->asLong(0));

	delete(pFlow);

	delete(pMFDSum);
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_MFD(int x, int y)
{
	int		i, ix, iy;

	double	zDist, zVert, zHorz, dDist, dVert, dHorz, dFlow;

	dDist	= pMFDSum->asDouble(x, y);

	if( dDist > 0.0 )
	{
		dDist	= 1.0 / dDist;
		pDistance->Mul_Value(x, y, dDist);
		pDistVert->Mul_Value(x, y, dDist);
		pDistHorz->Mul_Value(x, y, dDist);
	}

	zDist	= pDistance->asDouble(x, y);
	zVert	= pDistVert->asDouble(x, y);
	zHorz	= pDistHorz->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		ix		= Get_xTo(i, x);
		iy		= Get_yTo(i, y);

		if( pDTM->is_InGrid(ix, iy) && (dFlow = ((double *)pFlow->asLong(ix, iy))[(i + 4) % 8]) > 0.0 )
		{
			dVert	= pDTM->asDouble(ix, iy) - pDTM->asDouble(x, y);
			dHorz	= Get_Length(i);

			dDist	= dFlow * (zDist + sqrt(dVert*dVert + dHorz*dHorz));
			dVert	= dFlow * (zVert + dVert);
			dHorz	= dFlow * (zHorz + dHorz);

			if( pDistance->is_NoData(ix, iy) )
			{
				pDistVert->Set_Value(ix, iy, dVert);
				pDistHorz->Set_Value(ix, iy, dHorz);
				pDistance->Set_Value(ix, iy, dDist);
				pMFDSum->Set_Value(ix, iy, dFlow);
			}
			else
			{
				pDistVert->Add_Value(ix, iy, dVert);
				pDistHorz->Add_Value(ix, iy, dHorz);
				pDistance->Add_Value(ix, iy, dDist);
				pMFDSum->Add_Value(ix, iy, dFlow);
			}
		}
	}
}
