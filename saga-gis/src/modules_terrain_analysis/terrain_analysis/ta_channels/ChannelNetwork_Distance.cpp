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
	Set_Name		(_TL("Overland Flow Distance to Channel Network"));

	Set_Author		(SG_T("O.Conrad (c) 2001-11"));

	Set_Description	(_TW(
		"This module calculates overland flow distances to a channel network "
		"based on gridded digital elevation data and channel network information.\n"
		"The flow algorithm may be either Deterministic 8 (O'Callaghan & Mark 1984) or Multiple Flow Direction (Freeman 1991)\n"
		"\n\nReferences:\n"
		"- Freeman, G.T., 1991: 'Calculating catchment area with divergent flow based on a regular grid', Computers and Geosciences, 17:413-22\n"
		"- O'Callaghan, J.F., Mark, D.M., 1984: 'The extraction of drainage networks from digital elevation data', Computer Vision, Graphics and Image Processing, 28:323-344\n"
		"- Nobre, A.D., Cuartas, L.A., Hodnett, M., Rennó, C.D., Rodrigues, G., Silveira, A., Waterloo, M., Saleska S. (2011): Height Above the Nearest Drainage – a hydrologically relevant new terrain model. "
		"Journal of Hydrology, Vol. 404, Issues 1-2, pp. 13-29, ISSN 0022-1694, 10.1016/j.jhydrol.2011.03.051. "
		"<a target=\"_blank\" href=\"http://www.sciencedirect.com/science/article/pii/S0022169411002599\">online</a>\n"
	));


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHANNELS"	, _TL("Channel Network"),
		_TW("A grid providing information about the channel network. It is assumed that no-data cells are not part "
		"of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "DISTANCE"	, _TL("Overland Flow Distance"),
		_TW("The overland flow distance in map units. "
		"It is assumed that the (vertical) elevation data use the same units as the (horizontal) grid coordinates."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTVERT"	, _TL("Vertical Overland Flow Distance"),
		_TL("This is the vertical component of the overland flow"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTHORZ"	, _TL("Horizontal Overland Flow Distance"),
		_TL("This is the horizontal component of the overland flow"),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Flow Algorithm"),
		_TL("Choose a flow routing algorithm that shall be used for the overland flow distance calculation:\n- D8\n- MFD"),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("D8"),
			_TL("MFD")
		), 1
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
	int			x, y, Method;
	CSG_Grid	*pChannels;

	//-----------------------------------------------------
	m_pDTM		= Parameters("ELEVATION")->asGrid();
	pChannels	= Parameters("CHANNELS")->asGrid();

	m_pDistance	= Parameters("DISTANCE")->asGrid();
	m_pDistVert	= Parameters("DISTVERT")->asGrid();
	m_pDistHorz	= Parameters("DISTHORZ")->asGrid();

	Method		= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	switch( Method )
	{
	case 0:	default:	Initialize_D8();	break;
	case 1:				Initialize_MFD();	break;
	}

	m_pDistance	->Assign_NoData();
	m_pDistVert	->Assign_NoData();
	m_pDistHorz	->Assign_NoData();

	m_pDTM		->Set_Index(true);

	//-----------------------------------------------------
	for(long n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		m_pDTM->Get_Sorted(n, x, y, false, false);

		if( !m_pDTM->is_NoData(x, y) && !(pChannels->is_NoData(x, y) && m_pDistance->is_NoData(x, y)) )
		{
			if( !pChannels->is_NoData(x, y) )
			{
				m_pDistance->Set_Value(x, y, 0.0);
				m_pDistVert->Set_Value(x, y, 0.0);
				m_pDistHorz->Set_Value(x, y, 0.0);
			}

			switch( Method )
			{
			case 0:	default:	Execute_D8(x, y);	break;
			case 1:				Execute_MFD(x, y);	break;
			}
		}
	}

	//-----------------------------------------------------
	m_Dir	.Destroy();

	for(int i=0; i<=8; i++)
	{
		m_Flow[i].Destroy();
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
	m_Dir.Create(*Get_System(), SG_DATATYPE_Char);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			m_Dir.Set_Value(x, y, m_pDTM->Get_Gradient_NeighborDir(x, y));
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_D8(int x, int y)
{
	double	zDist, zVert, zHorz, dVert, dHorz;

	zDist	= m_pDistance->asDouble(x, y);
	zVert	= m_pDistVert->asDouble(x, y);
	zHorz	= m_pDistHorz->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xFrom(i, x);
		int	iy	= Get_yFrom(i, y);

		if( m_pDTM->is_InGrid(ix, iy) && m_Dir.asInt(ix, iy) == i )
		{
			dVert	= m_pDTM->asDouble(ix, iy) - m_pDTM->asDouble(x, y);
			dHorz	= Get_Length(i);

			m_pDistVert->Set_Value(ix, iy, zVert + dVert);
			m_pDistHorz->Set_Value(ix, iy, zHorz + dHorz);
			m_pDistance->Set_Value(ix, iy, zDist + sqrt(dVert*dVert + dHorz*dHorz));
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

	int		i;

	for(i=0; i<=8; i++)
	{
		m_Flow[i].Create(*Get_System(), SG_DATATYPE_Float);
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDTM->is_NoData(x, y) )
			{
				double	z, dz, zSum, Flow[8];

				z		= m_pDTM->asDouble(x, y);
				zSum	= 0.0;

				for(i=0; i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( m_pDTM->is_InGrid(ix, iy) && z > (dz = m_pDTM->asDouble(ix, iy)) )
					{
						zSum	+= (Flow[i]	= pow((z - dz) / Get_Length(i), MFD_Convergence));
					}
					else
					{
						Flow[i]	= 0.0;
					}
				}

				if( zSum > 0.0 )
				{
					m_Flow[8].Set_Value(x, y, zSum);

					for(i=0; i<8; i++)
					{
						if( Flow[i] > 0.0 )
						{
							m_Flow[i].Set_Value(x, y, Flow[i] / zSum);
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_MFD(int x, int y)
{
	int		i, ix, iy;

	double	zDist, zVert, zHorz, dDist, dVert, dHorz, dFlow;

	dDist	= m_Flow[8].asDouble(x, y);

	if( dDist > 0.0 )
	{
		dDist	= 1.0 / dDist;
		m_pDistance->Mul_Value(x, y, dDist);
		m_pDistVert->Mul_Value(x, y, dDist);
		m_pDistHorz->Mul_Value(x, y, dDist);
	}

	zDist	= m_pDistance->asDouble(x, y);
	zVert	= m_pDistVert->asDouble(x, y);
	zHorz	= m_pDistHorz->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) && (dFlow = m_Flow[(i + 4) % 8].asDouble(ix, iy)) > 0.0 )
		{
			dVert	= m_pDTM->asDouble(ix, iy) - m_pDTM->asDouble(x, y);
			dHorz	= Get_Length(i);

			dDist	= dFlow * (zDist + sqrt(dVert*dVert + dHorz*dHorz));
			dVert	= dFlow * (zVert + dVert);
			dHorz	= dFlow * (zHorz + dHorz);

			if( m_pDistance->is_NoData(ix, iy) )
			{
				m_pDistVert->Set_Value(ix, iy, dVert);
				m_pDistHorz->Set_Value(ix, iy, dHorz);
				m_pDistance->Set_Value(ix, iy, dDist);
				m_Flow[8].Set_Value(ix, iy, dFlow);
			}
			else
			{
				m_pDistVert->Add_Value(ix, iy, dVert);
				m_pDistHorz->Add_Value(ix, iy, dHorz);
				m_pDistance->Add_Value(ix, iy, dDist);
				m_Flow[8].Add_Value(ix, iy, dFlow);
			}
		}
	}
}
