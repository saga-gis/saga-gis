/**********************************************************
 * Version $Id: ChannelNetwork_Distance.cpp 1921 2014-01-09 10:24:11Z oconrad $
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

	Set_Author		(SG_T("O.Conrad (c) 2001-14"));

	Set_Description	(_TW(
		"This module calculates overland flow distances to a channel network "
		"based on gridded digital elevation data and channel network information.\n"
		"The flow algorithm may be either Deterministic 8 (O'Callaghan & Mark 1984) or Multiple Flow Direction (Freeman 1991)\n"
		"\n\nReferences:\n"
		"- Freeman, G.T., 1991: 'Calculating catchment area with divergent flow based on a regular grid', Computers and Geosciences, 17:413-22\n"
		"- O'Callaghan, J.F., Mark, D.M., 1984: 'The extraction of drainage networks from digital elevation data', Computer Vision, Graphics and Image Processing, 28:323-344\n"
		"- Nobre, A.D., Cuartas, L.A., Hodnett, M., Renno, C.D., Rodrigues, G., Silveira, A., Waterloo, M., Saleska S. (2011): Height Above the Nearest Drainage - a hydrologically relevant new terrain model. "
		"Journal of Hydrology, Vol. 404, Issues 1-2, pp. 13-29, ISSN 0022-1694, 10.1016/j.jhydrol.2011.03.051. "
		"<a target=\"_blank\" href=\"http://www.sciencedirect.com/science/article/pii/S0022169411002599\">online</a>\n"
	));


	//-----------------------------------------------------
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

	Parameters.Add_Grid(
		NULL	, "ROUTE"		, _TL("Preferred Routing"),
		_TL("Downhill flow is bound to preferred routing cells, where these are not no-data. Helps to model e.g. small ditches, that are not well represented in the elevation data."),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
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
	Parameters.Add_Grid(
		NULL	, "FIELDS"		, _TL("Fields"),
		_TL("If set, output is given about the number of fields a flow path visits downhill. For D8 only."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "PASSES"		, _TL("Fields Visited"),
		_TL("Number of fields a flow path visits downhill starting at a cell. For D8 only."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Flow Algorithm"),
		_TL("Choose a flow routing algorithm that shall be used for the overland flow distance calculation:\n- D8\n- MFD"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("D8"),
			_TL("MFD")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CChannelNetwork_Distance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Get_Parameter("FIELDS")->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("PASSES")->Set_Enabled(pParameter->asInt() == 0 && pParameters->Get_Parameter("FIELDS")->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FIELDS") )
	{
		pParameters->Get_Parameter("PASSES")->Set_Enabled(pParameter->is_Enabled() && pParameter->asGrid() != NULL);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Distance::On_Execute(void)
{
	CSG_Grid	*pChannels;

	//-----------------------------------------------------
	m_pDEM		= Parameters("ELEVATION")->asGrid();
	m_pRoute	= Parameters("ROUTE"    )->asGrid();
	pChannels	= Parameters("CHANNELS" )->asGrid();

	m_pDistance	= Parameters("DISTANCE" )->asGrid();
	m_pDistVert	= Parameters("DISTVERT" )->asGrid();
	m_pDistHorz	= Parameters("DISTHORZ" )->asGrid();

	int	Method	= Parameters("METHOD"   )->asInt ();

	//-----------------------------------------------------
	switch( Method )
	{
	default:	Initialize_D8 ();	break;
	case  1:	Initialize_MFD();	break;
	}

	m_pDistance	->Assign_NoData();
	m_pDistVert	->Assign_NoData();
	m_pDistHorz	->Assign_NoData();

	m_pDEM		->Set_Index(true);

	//-----------------------------------------------------
	for(long n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;

		if( m_pDEM->Get_Sorted(n, x, y, false, true) && !(pChannels->is_NoData(x, y) && m_pDistance->is_NoData(x, y)) )
		{
			if( !pChannels->is_NoData(x, y) )
			{
				m_pDistance->Set_Value(x, y, 0.0);
				m_pDistVert->Set_Value(x, y, 0.0);
				m_pDistHorz->Set_Value(x, y, 0.0);

				if( m_pFields )
				{
					m_pPasses->Set_Value(x, y, 0.0);
				}
			}

			switch( Method )
			{
			default:	Execute_D8 (x, y);	break;
			case  1:	Execute_MFD(x, y);	break;
			}
		}
	}

	//-----------------------------------------------------
	m_Dir.Destroy();

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

	if( (m_pFields = Parameters("FIELDS")->asGrid()) != NULL )
	{
		m_pPasses	= Parameters("PASSES")->asGrid();
		m_pPasses	->Set_NoData_Value(-1.0);
		m_pPasses	->Assign_NoData();
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Initialize_D8(x, y);
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_D8(int x, int y)
{
	int		i, iMax, iRoute;
	double	z, dz, dzMax, dzRoute;

	for(i=0, iMax=-1, dzMax=0.0, iRoute=-1, dzRoute=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( is_InGrid(ix, iy) && (dz = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > 0.0 )
		{
			if( dz > dzMax )
			{
				iMax	= i;
				dzMax	= dz;
			}

			if( m_pRoute && !m_pRoute->is_NoData(ix, iy) && dz > dzRoute )
			{
				iRoute	= i;
				dzRoute	= dz;
			}
		}
	}

	m_Dir.Set_Value(x, y, iRoute >= 0 ? iRoute : iMax);
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_D8(int x, int y)
{
	int		nPasses	= m_pFields ? m_pPasses->asInt   (x, y) : 0;
	double	Field	= m_pFields ? m_pFields->asDouble(x, y) : 0;

	double	zDist, zVert, zHorz, dVert, dHorz;

	zDist	= m_pDistance->asDouble(x, y);
	zVert	= m_pDistVert->asDouble(x, y);
	zHorz	= m_pDistHorz->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xFrom(i, x);
		int	iy	= Get_yFrom(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && m_Dir.asInt(ix, iy) == i )
		{
			dVert	= m_pDEM->asDouble(ix, iy) - m_pDEM->asDouble(x, y);
			dHorz	= Get_Length(i);

			m_pDistVert->Set_Value(ix, iy, zVert + dVert);
			m_pDistHorz->Set_Value(ix, iy, zHorz + dHorz);
			m_pDistance->Set_Value(ix, iy, zDist + sqrt(dVert*dVert + dHorz*dHorz));

			if( m_pFields )
			{
				m_pPasses->Set_Value(ix, iy, Field != m_pFields->asDouble(ix, iy) ? nPasses + 1 : nPasses);
			}
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
	m_pFields	= NULL;
	m_pPasses	= NULL;

	for(int i=0; i<=8; i++)
	{
		m_Flow[i].Create(*Get_System(), SG_DATATYPE_Float);
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Initialize_MFD(x, y);
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_MFD(int x, int y)
{
	const double	MFD_Convergence	= 1.1;

	double	Flow[8], dz, zSum = 0.0, z = m_pDEM->asDouble(x, y);

	if( m_pRoute )
	{
		for(int i=0, ix, iy; i<8; i++)
		{
			if( m_pDEM->is_InGrid(ix=Get_xTo(i, x), iy=Get_yTo(i, y)) && !m_pRoute->is_NoData(ix, iy) && (dz = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				zSum	+= (Flow[i]	= pow(dz / Get_Length(i), MFD_Convergence));
			}
			else
			{
				Flow[i]	= 0.0;
			}
		}
	}

	if( zSum == 0.0 )
	{
		for(int i=0, ix, iy; i<8; i++)
		{
			if( m_pDEM->is_InGrid(ix=Get_xTo(i, x), iy=Get_yTo(i, y)) && (dz = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				zSum	+= (Flow[i]	= pow(dz / Get_Length(i), MFD_Convergence));
			}
			else
			{
				Flow[i]	= 0.0;
			}
		}
	}

	if( zSum > 0.0 )
	{
		m_Flow[8].Set_Value(x, y, zSum);

		for(int i=0; i<8; i++)
		{
			if( Flow[i] > 0.0 )
			{
				m_Flow[i].Set_Value(x, y, Flow[i] / zSum);
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_MFD(int x, int y)
{
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

		if( m_pDEM->is_InGrid(ix, iy) && (dFlow = m_Flow[(i + 4) % 8].asDouble(ix, iy)) > 0.0 )
		{
			dVert	= m_pDEM->asDouble(ix, iy) - m_pDEM->asDouble(x, y);
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
