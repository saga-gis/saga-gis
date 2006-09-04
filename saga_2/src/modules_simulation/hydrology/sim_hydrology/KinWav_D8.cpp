
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    Kinematic_Wave                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    KinWav_D8.cpp                      //
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
#include "KinWav_D8.h"

//---------------------------------------------------------
#define Beta		(3.0 / 5.0)
#define Beta_1		(3.0 / 5.0 - 1.0)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKinWav_D8::CKinWav_D8(void)
{
	CParameter	*pNode;

	Set_Name	(_TL("Overland Flow - Kinematic Wave D8"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Overland Flow - Kinematic Wave D8"
		"\n\n"
		"Reference:\n"
		"Johnson, D.L., Miller, A.C. (1997):"
		" A spatially distributed hydrological model utilizing raster data structures,"
		" Computers & Geosciences, Vol.23, No.3, pp.267-272"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DTM"			, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "RUNOFF"		, _TL("Runoff"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL	, "GAUGES"		, _TL("Outlets"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "TIME_END"	, _TL("Simulation Time [h]"),
		"",
		PARAMETER_TYPE_Double, 24.0
	);

	Parameters.Add_Value(
		NULL	, "TIME_STEP"	, _TL("Simulation Time Step [h]"),
		"",
		PARAMETER_TYPE_Double, 0.1
	);

	Parameters.Add_Value(
		NULL	, "ROUGHNESS"	, _TL("Manning's Roughness"),
		"",
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NEWTON", _TL("Newton-Raphson"), "");

	Parameters.Add_Value(
		pNode	, "NEWTON_MAXITER"	, _TL("Max. Iterations"),
		"",
		PARAMETER_TYPE_Int		, 100		, 1		, true
	);

	Parameters.Add_Value(
		pNode	, "NEWTON_EPSILON"	, _TL("Epsilon"),
		"",
		PARAMETER_TYPE_Double	, 0.0001	, 0.0	, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "PRECIP"		, _TL("Precipitation"),
		_TL("Kind of initializing Precipitation Event"),

		CSG_String::Format("%s|%s|%s|",
			_TL("Homogenous"),
			_TL("Above Elevation"),
			_TL("Left Half")
		)
	);

	Parameters.Add_Value(
		pNode	, "THRESHOLD"	, _TL("Threshold Elevation"),
		"",
		PARAMETER_TYPE_Double, 0.0
	);
}

//---------------------------------------------------------
CKinWav_D8::~CKinWav_D8(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKinWav_D8::Initialize(void)
{
	int			x, y, i, ix, iy, iMax;
	double		z, dz, dzMax, Roughness, t;
	CSG_Colors	Colors;
	CSG_String	s;


	//-----------------------------------------------------
	// 1. Get DTM...

	pDTM			= Parameters("DTM")->asGrid();

	Roughness		= Parameters("ROUGHNESS")->asDouble();

	Newton_MaxIter	= Parameters("NEWTON_MAXITER")->asInt();
	Newton_Epsilon	= Parameters("NEWTON_EPSILON")->asDouble();


	//-----------------------------------------------------
	// 2. Gauges Table...

	pos_Gauges	= NULL;

	if( (pGauges = Parameters("GAUGES")->asTable()) != NULL )
	{
		if( (int)pGauges == 1 )
		{
			Parameters("GAUGES")->Set_Value((pGauges = new CTable));
		}

		pGauges->Destroy();
		pGauges->Set_Name(_TL("Outlet Hydrographs"));
		pGauges->Add_Field("TIME", TABLE_FIELDTYPE_Double);
	}


	//-----------------------------------------------------
	// 3. Calculate Alphas...

	pAlpha		= new CGrid(pDTM, GRID_TYPE_Float);
	pFlow_Dir	= new CGrid(pDTM, GRID_TYPE_Char);

	for(y=0; y<pDTM->Get_NY(); y++)
	{
		for(x=0; x<pDTM->Get_NX(); x++)
		{
			if( pDTM->is_NoData(x, y) )
			{
				pAlpha->Set_NoData(x, y);
				pFlow_Dir->Set_Value(x, y, -1);
			}
			else
			{
				dzMax	= 0.0;
				iMax	= -1;
				z		= pDTM->asDouble(x, y);

				for(i=0; i<8; i++)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if( !is_InGrid(ix, iy) )
					{
					}
					else if( (dz = pDTM->asDouble(ix, iy)) < z )
					{
						dz	= (z - dz) / Get_Length(i);

						if( iMax < 0 || dz > dzMax )
						{
							dzMax	= dz;
							iMax	= i;
						}
					}
				}

				if( iMax >= 0 )
				{
					pAlpha->Set_Value(x, y, pow(Roughness / sqrt(dzMax), Beta));
				}
				else
				{
					pAlpha->Set_NoData(x, y);

					if( pGauges )
					{
						i				= pGauges->Get_Field_Count() - 1;
						s.Printf("GAUGE_%02d", i + 1);
						pGauges->Add_Field(s, TABLE_FIELDTYPE_Double);

						pos_Gauges		= (TPoint *)SG_Realloc(pos_Gauges, (i + 1) * sizeof(TPoint));
						pos_Gauges[i].x	= x;
						pos_Gauges[i].y	= y;
					}
				}

				pFlow_Dir->Set_Value(x, y, iMax);
			}
		}
	}


	//-----------------------------------------------------
	// 4. Get and initialize Flow grids...

	pFlow		= Parameters("RUNOFF")->asGrid();
	pFlow->Assign();

	Colors.Set_Ramp(SG_GET_RGB(255, 255, 200), SG_GET_RGB(0, 65, 128));
	DataObject_Set_Colors(pFlow, Colors);

	pFlow_Last	= new CGrid(pDTM, GRID_TYPE_Float);

	//-----------------------------------------------------
	switch( Parameters("PRECIP")->asInt() )
	{
	case 0:
		pFlow->Assign(100.0);
		break;

	case 1:
		t	= Parameters("THRESHOLD")->asDouble();

		for(y=0; y<pDTM->Get_NY(); y++)
		{
			for(x=0; x<pDTM->Get_NX(); x++)
			{
				pFlow->Set_Value(x, y, !pDTM->is_NoData(x, y) && pDTM->asDouble(x, y) > t ? 100.0 : 0.0);
			}
		}
		break;

	case 2:
		for(y=0; y<pDTM->Get_NY(); y++)
		{
			for(x=0; x<pDTM->Get_NX() / 2; x++)
			{
				pFlow->Set_Value(x, y, !pDTM->is_NoData(x, y) ? 100.0 : 0.0);
			}
		}
		break;
	}

	DataObject_Update(pFlow, 0, 200, true);

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CKinWav_D8::Finalize(void)
{
	delete(pFlow_Last);
	delete(pFlow_Dir);
	delete(pAlpha);

	if( pos_Gauges )
	{
		SG_Free(pos_Gauges);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKinWav_D8::On_Execute(void)
{
	int				x, y, n;
	double			Time, Time_End;
	CSG_String		s;
	CTable_Record	*pRecord;

	//-----------------------------------------------------
	if( Initialize() )
	{
		Time_End	= Parameters("TIME_END")->asDouble();
		Time_Step	= Parameters("TIME_STEP")->asDouble();

		P			= 0.0;
//		pFlow->Assign(0);

		for(Time=0.0; Time<=Time_End && Set_Progress(Time, Time_End); Time+=Time_Step)
		{
//			P	= 2 * (1 - cos(GRAD2PI * Time * 360 / 1));
//			if( Time < 1.0 )
//			{
//				P	= 50 * (1.0 - Time);
//			}
//			else	P	= 0.0;

			s.Printf(_TL("Simulation Time [h]: %f / %f"), Time, Time_End);
			Process_Set_Text(s);

			//---------------------------------------------
			pFlow_Last->Assign(pFlow);
			pFlow->Assign();

			for(n=0; n<pDTM->Get_NCells(); n++)
			{
				pDTM->Get_Sorted(n, x, y);

				if( pDTM->is_NoData(x, y) )
				{
					pFlow->Set_NoData(x, y);
				}
				else
				{
					Set_Runoff(x, y);
				}
			}

			//---------------------------------------------
			if( pGauges && pGauges->Get_Field_Count() > 1 )
			{
				pRecord	= pGauges->Add_Record();
				pRecord->Set_Value(0, Time);

				for(n=0; n<pGauges->Get_Field_Count() - 1; n++)
				{
					x	= pos_Gauges[n].x;
					y	= pos_Gauges[n].y;

					pRecord->Set_Value(n + 1, pFlow->asDouble(x, y));

					pFlow->Set_Value(x, y, 0.0);
				}
			}

			DataObject_Update_All();
		}

		//-------------------------------------------------
		Finalize();

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
void CKinWav_D8::Set_Runoff(int x, int y)
{
	int		i, ix, iy;
	double	q;

	if( (i = pFlow_Dir->asChar(x, y)) >= 0 )
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		q	= Get_Runoff(pFlow->asDouble(x, y), pFlow_Last->asDouble(x, y), Get_UnitLength(i), pAlpha->asDouble(x, y), P, P);

		pFlow->Set_Value(x, y, q);
		pFlow->Add_Value(ix, iy, pFlow_Last->asDouble(x, y));
	}
}

//---------------------------------------------------------
double CKinWav_D8::Get_Runoff(double q_Up, double q_Last, double dL, double alpha, double r, double r_Last)
{
	int		i;
	double	dTdL, d, c, q, Res, dRes, dR;

	//-----------------------------------------------------
	dTdL	= Time_Step / dL;
	dR		= Time_Step / 2.0 * (r + r_Last);


	//-----------------------------------------------------
	// 1. Initial estimation of q...

	if( q_Last + q_Up != 0.0 )
	{
		d	= alpha * Beta * pow((q_Last + q_Up) / 2.0, Beta_1);
		q	= ( dTdL * q_Up + q_Last * d + dR ) / ( dTdL + d );
	}
	else
	{
		q	= dR;
	}


	//-----------------------------------------------------
	// 2. Newton-Raphson...

	c	= dTdL * q_Up + alpha * pow(q_Last, Beta) + dR;

	for(i=0; i<Newton_MaxIter; i++)
	{
		if( q <= 0 )
		{
			return( dR );
		}

		Res		= dTdL * q + alpha		  * pow(q, Beta) - c;
		dRes	= dTdL     + alpha * Beta * pow(q, Beta_1);
//		if( dRes == 0.0 )	{	return( 0.0 );	}

		d		= Res / dRes;
		q		-= d;

		if( fabs(d) < Newton_Epsilon )
		{
			break;
		}
	}

	return( q );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
