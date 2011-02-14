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
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Flow_RecursiveDown.cpp                 //
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
#include "Flow_RecursiveDown.h"

//---------------------------------------------------------
#define GET_OUTLET_DIAG__1(in, angle)		(1.0 - (1.0 - in) * tan(M_PI_090 - angle))
#define GET_OUTLET_CROSS_1(in, angle)		(in + tan(angle))

#define GET_OUTLET_DIAG__2(in, angle)		(in * tan(angle))
#define GET_OUTLET_CROSS_2(in, angle)		(in - tan(M_PI_090 - angle))

#define GET_LENGTH(a, b)					(sqrt((a)*(a) + (b)*(b)))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_RecursiveDown::CFlow_RecursiveDown(void)
{
	Set_Name(_TL("Flow Tracing"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"Flow tracing algorithms for calculations of flow accumulation and related parameters. "
		"These algorithms trace the flow of each cell in a DEM separately until it finally leaves the DEM or ends in a sink.\n\n"

		"References:\n\n"

		"Rho 8 (this implementation adopted the original algorithm only for the flow routing and will give quite different results):\n"
		"- Fairfield, J. / Leymarie, P. (1991):\n"
		"    'Drainage networks from grid digital elevation models',\n"
		"    Water Resources Research, 27:709-717\n\n"

		"Kinematic Routing Algorithm:\n"
		"- Lea, N.L. (1992):\n"
		"    'An aspect driven kinematic routing algorithm',\n"
		"    in: Parsons, A.J., Abrahams, A.D. (Eds.), 'Overland Flow: hydraulics and erosion mechanics', London, 147-175\n\n"

		"DEMON:\n"
		"- Costa-Cabral, M. / Burges, S.J. (1994):\n"
		"    'Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas',\n"
		"    Water Resources Research, 30:1681-1692\n\n")
	);


	//-----------------------------------------------------
	// Method...

	Parameters.Add_Choice(
		NULL	, "Method"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Rho 8"),
			_TL("Kinematic Routing Algorithm"),
			_TL("DEMON")
		), 1
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "MINDQV"		, _TL("DEMON - Min. DQV"),
		_TL("DEMON - Minium Drainage Quota Volume (DQV) for traced flow tubes"),
		PARAMETER_TYPE_Double	,	0.0, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "CORRECT"		, _TL("Flow Correction"),
		_TL(""),
		PARAMETER_TYPE_Bool
	);
}

//---------------------------------------------------------
CFlow_RecursiveDown::~CFlow_RecursiveDown(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::On_Initialize(void)
{
	int		x, y;
	double	Slope, Aspect;

	//-----------------------------------------------------
	Method			= Parameters("Method"	)->asInt();
	DEMON_minDQV	= Parameters("MINDQV"	)->asDouble();
	bFlowPathWeight	= Parameters("CORRECT"	)->asBool();

	pLinear			= 1 ? SG_Create_Grid(pDTM, SG_DATATYPE_Float) : NULL;

	//-----------------------------------------------------
	Lock_Create();

	//-----------------------------------------------------
	switch( Method )
	{
	case 0: default:	// Rho 8...

		pDir		= NULL;
		pDif		= NULL;

		break;

	//-----------------------------------------------------
	case 1:	case 2:		// KRA, DEMON...

		pDir		= SG_Create_Grid(pDTM, SG_DATATYPE_Char);
		pDif		= SG_Create_Grid(pDTM, SG_DATATYPE_Float);

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !pDTM->is_NoData(x, y) )
				{
					Get_Gradient(x, y, Slope, Aspect);

					if( Aspect >= 0.0 )
					{
						pDir->Set_Value(x, y, 2 * (((int)(Aspect / M_PI_090)) % 4));
						pDif->Set_Value(x, y, fmod(Aspect, M_PI_090));
					}
				}
			}
		}

		break;
	}
}

//---------------------------------------------------------
void CFlow_RecursiveDown::On_Finalize(void)
{
	int		x, y, dir;
	long	n;
	double	qFlow;

	//-----------------------------------------------------
	if( pDir )
	{
		delete(pDir);
	}

	if( pDif )
	{
		delete(pDif);
	}

	Lock_Destroy();

	//-----------------------------------------------------
	if( pLinear )
	{
		for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			;

			if( pDTM->Get_Sorted(n, x, y) && (qFlow = pLinear->asDouble(x, y)) > 0.0 )
			{
				Add_Flow(x, y, qFlow);

				if( (dir = pDTM->Get_Gradient_NeighborDir(x, y)) >= 0 )
				{
					x	= Get_xTo(dir, x);
					y	= Get_yTo(dir, y);

					if( pDTM->is_InGrid(x, y) )
					{
						pLinear->Add_Value(x, y, qFlow);
					}
				}
			}
		}

		delete(pLinear);

		pLinear	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_RecursiveDown::Calculate(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y+=Step)
	{	//if( !(y%2) )DataObject_Update(pFlow);
		for(int x=0; x<Get_NX(); x+=Step)
		{
			Calculate(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CFlow_RecursiveDown::Calculate(int x, int y)
{
	double 	Slope, Aspect, qFlow;

	if( !pDTM->is_NoData(x, y) && (qFlow = pWeight ? pWeight->asDouble(x, y) : 1.0) > 0.0 )
	{
		Get_Gradient(x, y, Slope, Aspect);

		Src_Height	= pDTM->asDouble(x,y);
		Src_Slope	= Slope;

		Add_Flow(x, y, qFlow);
		Lock_Set(x, y, 1);

		//-------------------------------------------------
		switch( Method )
		{
		case 0:
			Rho8_Start(		x, y, qFlow );
			break;

		case 1:
			KRA_Start(		x, y, qFlow );
			break;

		case 2:
			DEMON_Start(	x, y, qFlow );
			break;
		}

		Lock_Set(x, y, 0);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::Add_Flow(int x, int y, double Fraction)
{
	if( pCatch )
	{
		pCatch			->Add_Value(x, y, Fraction );
	}

	if( pCatch_Height )
	{
		pCatch_Height	->Add_Value(x, y, Fraction * Src_Height );
	}

	if( pCatch_Slope )
	{
		pCatch_Slope	->Add_Value(x, y, Fraction * Src_Slope );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::Rho8_Start(int x, int y, double qFlow)
{
	int		dir, ix, iy;
	double 	Slope, Aspect;

	//-----------------------------------------------------
	Get_Gradient(x, y, Slope, Aspect);

	if( Aspect >= 0.0 )
	{
		dir		= (int)(Aspect / M_PI_045);

		if( fmod(Aspect, M_PI_045) / M_PI_045 > rand() / (double)RAND_MAX )
		{
			dir++;
		}

		dir	%= 8;

		ix		= Get_xTo(dir, x);
		iy		= Get_yTo(dir, y);

		//-------------------------------------------------
		if( is_InGrid(ix, iy) )
		{
			if( is_Locked(ix, iy) )
			{
				if( pLinear )
				{
					pLinear->Add_Value(x, y, qFlow);
				}
			}
			else
			{
				Lock_Set( x,  y, 1);
				Add_Flow(ix, iy, qFlow);
				Rho8_Start(ix, iy, qFlow);
				Lock_Set( x,  y, 0);
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
void CFlow_RecursiveDown::KRA_Start(int x, int y, double qFlow)
{
	int		dir;
	double	dif;

	if( (dif = pDif->asDouble(x, y)) > M_PI_045 )	// to the right...
	{
		dir		= pDir->asInt(x, y) + 2;
		dif		= 0.5 - tan(M_PI_090 - dif) / 2.0;
	}
	else											// to the top...
	{
		dir		= pDir->asInt(x, y) + 0;
		dif		= 0.5 + tan(dif) / 2.0;
	}

	KRA_Trace(x, y, qFlow, dir, dif);
}

//---------------------------------------------------------
void CFlow_RecursiveDown::KRA_Trace(int x, int y, double qFlow, int Direction, double from)
{
	bool	bLinear;
	int		dir;
	double	dif, to, weight;

	Direction	%= 8;

	x	= Get_xTo(Direction, x);
	y	= Get_yTo(Direction, y);

	//-----------------------------------------------------
	if( pDTM->is_InGrid(x, y) && !is_Locked(x, y) )
	{
		Lock_Set(x, y, 1);

		bLinear	= false;
		weight	= 1.0;

		dir		= pDir->asInt(x, y);
		dif		= pDif->asDouble(x, y);

		//-------------------------------------------------
		if( Direction == dir )						// entering from the bottom...
		{
			if( from > 1.0 - tan(dif) )				// 1. to the right...
			{
				to		= GET_OUTLET_DIAG__1(from, dif);

				KRA_Trace(x, y, qFlow, dir + 2, to);

				if( bFlowPathWeight )
				{
					weight	= GET_LENGTH(1.0 - from, 1.0 - to);
				}
			}
			else									// 2. to the top...
			{
				to		= GET_OUTLET_CROSS_1(from, dif);

				KRA_Trace(x, y, qFlow, dir + 0, to);

				if( bFlowPathWeight )
				{
					weight	= GET_LENGTH(1.0, to - from);
				}
			}
		}
		else if( (8 + Direction - dir) % 8 == 2 )	// entering from the left...
		{
			if( from < tan(M_PI_090 - dif) )		// 1. to the top...
			{
				to		= GET_OUTLET_DIAG__2(from, dif);

				KRA_Trace(x, y, qFlow, dir + 0, to);

				if( bFlowPathWeight )
				{
					weight	= GET_LENGTH(from, to);
				}
			}
			else									// 2. to the right...
			{
				to		= GET_OUTLET_CROSS_2(from, dif);

				KRA_Trace(x, y, qFlow, dir + 2, to);

				if( bFlowPathWeight )
				{
					weight	= GET_LENGTH(1.0, from - to);
				}
			}
		}
		else										// go linear...
		{
			bLinear	= true;
		}

		//-------------------------------------------------
		if( bLinear && pLinear )
		{
			pLinear->Add_Value(x, y, qFlow);
		}
		else
		{
			Add_Flow(x, y, weight * qFlow);
		}

		Lock_Set(x, y, 0);
	}
}


///////////////////////////////////////////////////////////
//														 //
//		DEMON - Digital Elevation MOdel Network			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::DEMON_Start(int x, int y, double qFlow)
{
	double	dif, flow_A, flow_B;

	if( (dif = pDif->asDouble(x, y)) < M_PI_045 )	// mostly to the top...
	{
		flow_B	= tan(dif) / 2.0;
		flow_A	= 1.0 - flow_B;
	}
	else											// mostly to the right...
	{
		flow_A	= tan(M_PI_090 - dif) / 2.0;
		flow_B	= 1.0 - flow_A;
	}

	flow_A	*= qFlow;
	flow_B	*= qFlow;

	if( flow_A <= DEMON_minDQV )
	{
		DEMON_Trace(x, y, qFlow , pDir->asInt(x, y) + 2, 0.0, 1.0);	// all to the right...
	}
	else if( flow_B <= DEMON_minDQV )
	{
		DEMON_Trace(x, y, qFlow , pDir->asInt(x, y) + 0, 0.0, 1.0);	// all to the top...
	}
	else
	{
		DEMON_Trace(x, y, flow_A, pDir->asInt(x, y) + 0, 0.0, 1.0);	// to the top...
		DEMON_Trace(x, y, flow_B, pDir->asInt(x, y) + 2, 0.0, 1.0);	// to the right...
	}
}

//---------------------------------------------------------
void CFlow_RecursiveDown::DEMON_Trace(int x, int y, double qFlow, int Direction, double from_A, double from_B)
{
	bool	bLinear;
	int		dir;
	double	dif, to_A, to_B, flow_A, flow_B, weight;

	Direction	%= 8;

	x	= Get_xTo(Direction, x);
	y	= Get_yTo(Direction, y);

	//-----------------------------------------------------
	if( pDTM->is_InGrid(x, y) && !is_Locked(x, y) )
	{
		Lock_Set(x, y, 1);

		bLinear	= false;
		weight	= 1.0;

		dir		= pDir->asInt(x, y);
		dif		= pDif->asDouble(x, y);

		//-------------------------------------------------
		if( Direction == dir )						// entering from the bottom...
		{
			if( from_A >= 1.0 - tan(dif) )			// 1. completely to the right...
			{
				to_A	= GET_OUTLET_DIAG__1(from_A, dif);
				to_B	= GET_OUTLET_DIAG__1(from_B, dif);

				DEMON_Trace(x, y, qFlow, dir + 2, to_A, to_B);

				if( bFlowPathWeight )
				{
					//weight	= ((1.0 - from_A) * (1.0 - to_A) - (1.0 - from_B) * (1.0 - to_B)) / 2.0;	// area...
					weight	= GET_LENGTH(1.0 - (from_B + from_A) / 2.0, 1.0 - (to_B + to_A) / 2.0);
				}
			}
			else if( from_B < 1.0 - tan(dif) )		// 2. completely to the top...
			{
				to_A	= GET_OUTLET_CROSS_1(from_A, dif);
				to_B	= GET_OUTLET_CROSS_1(from_B, dif);

				DEMON_Trace(x, y, qFlow, dir + 0, to_A, to_B);

				if( bFlowPathWeight )
				{
					//weight	= from_B - from_A;	// area...
					weight	= GET_LENGTH(1.0, to_A - from_A);
				}
			}
			else									// 3. partly to the right, partly to the top...
			{
				to_A	= GET_OUTLET_CROSS_1(from_A, dif);
				to_B	= GET_OUTLET_DIAG__1(from_B, dif);

				dif		= 1.0 - tan(dif);

				flow_A	= dif - from_A;
				flow_B	= from_B - dif;
				flow_A	= qFlow * flow_A / (flow_A + flow_B);
				flow_B	= qFlow - flow_A;

				if( bFlowPathWeight )
				{
					//weight	= (dif - from_A) + ((1.0 - dif) - (1.0 - from_B) * (1.0 - to_B)) / 2.0;	// area...
					if( (weight = (from_A + from_B) / 2.0) < dif )	// to the top...
					{
						weight	= GET_LENGTH(1.0, to_A - from_A);
					}
					else											// to the right...
					{
						weight	= (1.0 - weight) / (1.0 - dif) * GET_LENGTH(1.0, to_A - from_A);
					}
				}

				if( flow_A <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, qFlow, dir + 2, 0.0, to_B);
				}
				else if( flow_B <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, qFlow, dir + 0, to_A, 1.0);
				}
				else
				{
					DEMON_Trace(x, y, flow_A, dir + 0, to_A, 1.0);
					DEMON_Trace(x, y, flow_B, dir + 2, 0.0, to_B);
				}
			}
		}
		else if( (8 + Direction - dir) % 8 == 2 )	// entering from the left...
		{
			if( from_B <= tan(M_PI_090 - dif) )		// 1. completely to the top...
			{
				to_A	= GET_OUTLET_DIAG__2(from_A, dif);
				to_B	= GET_OUTLET_DIAG__2(from_B, dif);

				DEMON_Trace(x, y, qFlow, dir + 0, to_A, to_B);

				if( bFlowPathWeight )
				{
					//weight	= (from_B * to_B - from_A * to_A) / 2.0;	// area...
					weight	= GET_LENGTH((from_A + from_B) / 2.0, (to_A + to_B) / 2.0);
				}
			}
			else if( from_A > tan(M_PI_090 - dif) )	// 2. completely to the right...
			{
				to_A	= GET_OUTLET_CROSS_2(from_A, dif);
				to_B	= GET_OUTLET_CROSS_2(from_B, dif);

				DEMON_Trace(x, y, qFlow, dir + 2, to_A, to_B);

				if( bFlowPathWeight )
				{
					//weight	= from_B - from_A;	// area...
					weight	= GET_LENGTH(1.0, from_A - to_A);
				}
			}
			else									// 3. partly to the top, partly to the right...
			{
				to_A	= GET_OUTLET_DIAG__2(from_A, dif);
				to_B	= GET_OUTLET_CROSS_2(from_B, dif);

				dif		= tan(M_PI_090 - dif);

				flow_A	= dif - from_A;
				flow_B	= from_B - dif;
				flow_A	= qFlow * flow_A / (flow_A + flow_B);
				flow_B	= qFlow - flow_A;

				if( bFlowPathWeight )
				{
					//weight	= (from_B - dif) + (dif - (from_A * to_A)) / 2.0;	// area...
					if( (weight = (from_A + from_B) / 2.0) > dif )
					{
						weight	= GET_LENGTH(1.0, from_B - to_B);
					}
					else
					{
						weight	= weight / dif * GET_LENGTH(1.0, from_B - to_B);
					}
				}

				if( flow_A <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, qFlow, dir + 2, 0.0, to_B);
				}
				else if( flow_B <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, qFlow, dir + 0, to_A, 1.0);
				}
				else
				{
					DEMON_Trace(x, y, flow_A, dir + 0, to_A, 1.0);
					DEMON_Trace(x, y, flow_B, dir + 2, 0.0, to_B);
				}
			}
		}
		else
		{
			bLinear	= true;
		}

		//-------------------------------------------------
		if( bLinear && pLinear )
		{
			pLinear->Add_Value(x, y, qFlow);
		}
		else
		{
			if( bFlowPathWeight )
			{
				Add_Flow(x, y, weight * qFlow);
				//Add_Flow(x, y, weight >= qFlow ? qFlow : weight * qFlow);
			}
			else
			{
				Add_Flow(x, y, qFlow);
			}
		}

		Lock_Set(x, y, 0);
	}
}
