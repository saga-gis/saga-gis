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
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Pit_Router.cpp                     //
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
#include "Pit_Router.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	IS_Flat(a,b)		(a==b)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPit_Router::CPit_Router(void)
{
	Set_Name		(_TL("Sink Drainage Route Detection"));

	Set_Author		(SG_T("O. Conrad (c) 2001"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Sink Route"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Threshold"),
		_TL(""),
		PARAMETER_TYPE_Bool
	);

	Parameters.Add_Value(
		NULL	, "THRSHEIGHT"	, _TL("Threshold Height"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100
	);
}

//---------------------------------------------------------
CPit_Router::~CPit_Router(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Router::On_Execute(void)
{
	return( Get_Routes(
		Parameters("ELEVATION")->asGrid(),
		Parameters("SINKROUTE")->asGrid(),
		Parameters("THRESHOLD")->asBool() ? Parameters("THRSHEIGHT")->asDouble() : -1.0
	) >= 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPit_Router::Get_Routes(CSG_Grid *pDEM, CSG_Grid *pRoute, double Threshold)
{
	int			iPit, nPits, n;
	TPit_Outlet	*pOutlet, *pNext;

	//-----------------------------------------------------
	m_pDEM		= pDEM;
	m_pRoute	= pRoute;
	m_Threshold	= Threshold;

	//-----------------------------------------------------
	m_pPits		= NULL;
	m_Pit		= NULL;

	m_pFlats	= NULL;
	m_Flat		= NULL;

	m_Outlets	= NULL;

	//-----------------------------------------------------
	Get_System()->Assign(m_pDEM->Get_System());

	//-----------------------------------------------------
	if( Initialize() )
	{
		//-------------------------------------------------
		// 1. Pits/Flats finden...

		SG_UI_Process_Set_Text(_TL("Find Pits"));

		nPits	= Find_Pits();

		if( nPits > 0 )
		{
			//---------------------------------------------
			// 2. Pit/Flat-Zugehoerigkeiten u. pot. m_Outlets finden...

			SG_UI_Process_Set_Text(_TL("Find Outlets"));

			Find_Outlets(nPits);


			//---------------------------------------------
			// 3. Routing vornehmen...

			SG_UI_Process_Set_Text(_TL("Routing"));

			iPit	= 0;

			do
			{
				pOutlet	= m_Outlets;

				while( pOutlet && SG_UI_Process_Get_Okay(false) )
				{
					pNext	= pOutlet->Next;
					n		= Find_Route(pOutlet);

					if( n > 0 )
					{
						pOutlet	= m_Outlets;
						iPit	+= n;
						SG_UI_Process_Set_Progress(iPit, nPits);
					}
					else
					{
						pOutlet	= pNext;
					}
				}

				if( iPit < nPits )	// Thresholding may have prevented total removal of pits...
				{
					for(n=0; n<nPits; n++)
					{
						if( !m_Pit[n].bDrained )
						{
							m_Pit[n].bDrained	= true;
							iPit++;
							break;
						}
					}
				}
			}
			while( iPit < nPits && SG_UI_Process_Set_Progress(iPit, nPits) );
		}


		//-------------------------------------------------
		// 4. Threshold

		if( m_Threshold > 0.0 )
		{
			nPits	-= Process_Threshold();
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Finalize"));

	Finalize();

	if( Process_Get_Okay(false) )
	{
		if( nPits > 0 )
		{
			Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("number of processed sinks"), nPits));

			return( nPits );
		}
		else
		{
			Message_Add(_TL("No sinks have been detected."));
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Router::Initialize(void)
{
	if(	m_pDEM && m_pDEM->is_Valid()
	&&	m_pRoute && m_pRoute->is_Valid()
	&&	m_pDEM->Get_System() == m_pRoute->Get_System()	)
//	&&	m_pDEM->is_Compatible(m_pRoute->Get_System())	)
	{
		m_pRoute->Assign();

		m_pPits		= SG_Create_Grid(m_pDEM, SG_DATATYPE_Int);
		m_pPits->Assign();

		m_Pit		= NULL;

		m_pFlats	= NULL;
		m_Flat		= NULL;

		m_Outlets	= NULL;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CPit_Router::Finalize(void)
{
	TPit_Outlet	*pOutlet;

	if( m_pPits )
	{
		delete( m_pPits );
		m_pPits		= NULL;
	}

	if( m_Pit )
	{
		SG_Free(m_Pit);
		m_Pit		= NULL;
	}

	if( m_pFlats )
	{
		delete( m_pFlats );
		m_pFlats	= NULL;
	}

	if( m_Flat )
	{
		SG_Free(m_Flat);
		m_Flat		= NULL;
	}

	while( m_Outlets )
	{
		pOutlet		= m_Outlets->Next;
		SG_Free(m_Outlets);
		m_Outlets	= pOutlet;
	}

	m_Outlets	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPit_Router::Find_Pits(void)
{
	bool	bLower, bFlat;
	int		x, y, i, ix, iy, nFlats, nPits;
	long	n;
	double	z;
	TPit	*pPit;

	//-----------------------------------------------------
	nFlats		= 0;
	nPits		= 0;

	for(n=0; n<Get_NCells() && SG_UI_Process_Set_Progress(n, Get_NCells()); n++)
	{
		m_pDEM->Get_Sorted(n,x,y,false);	// von tief nach hoch...

		if(	x > 0 && x < Get_NX() - 1 && y > 0 && y < Get_NY() - 1	// Randzellen und Missing Values sind
		&&	!m_pDEM->is_NoData(x, y)								// per Definition drainiert (:= 0)...
		&&	m_pPits->asInt(x, y) == 0	)	// ...oder schon als m_Flat markiert sein...
		{
			z		= m_pDEM->asDouble(x,y);
			bLower	= false;
			bFlat	= false;

			for(i=0; i<8 && !bLower; i++)
			{
				ix	= Get_xTo(i,x);
				iy	= Get_yTo(i,y);

				if( !m_pDEM->is_InGrid(ix, iy) || z > m_pDEM->asDouble(ix, iy) )
				{
					bLower	= true;
				}
				else if( IS_Flat(z, m_pDEM->asDouble(ix,iy)) )
				{
					bFlat	= true;
				}
			}

			//-----------------------------------------
			if( !bLower )	// Pit or Flat...
			{
				nPits++;

				m_pPits->Set_Value(x,y, nPits);

				m_Pit				= (TPit *)SG_Realloc(m_Pit, nPits * sizeof(TPit));
				pPit			= m_Pit + nPits - 1;
				pPit->bDrained	= false;
				pPit->z			= z;

				if( bFlat )
				{
					nFlats++;

					m_Flat			= (TGEO_iRect *)SG_Realloc(m_Flat, nFlats * sizeof(TGEO_iRect));

					Mark_Flat(x, y, m_Flat + nFlats - 1, nFlats, nPits);
				}
			}
		}
	}

	return( nPits );
}

//---------------------------------------------------------
int CPit_Router::Find_Outlets(int nPits)
{
	bool	bOutlet, bExArea, bGoExArea;

	int		x, y, i, ix, iy, iMin,
			iID, j, jID, Pit_ID[8];

	long	n;

	double	z, dz, dzMin;

	TPit_Outlet	*pOutlet;

	//-----------------------------------------------------
	if( nPits > 0 && SG_UI_Process_Get_Okay(false) )
	{
		pOutlet		= NULL;

		m_nJunctions	= (int  *)SG_Calloc(nPits, sizeof(int  ));
		m_Junction	= (int **)SG_Calloc(nPits, sizeof(int *));

		//-------------------------------------------------
		for(n=0; n<Get_NCells() && SG_UI_Process_Set_Progress(n, Get_NCells()); n++)
		{
			if(	m_pDEM->Get_Sorted(n, x, y, false) && m_pPits->asInt(x,y) == 0 )
			{
				z			= m_pDEM->asDouble(x,y);
				iMin		= -1;

				bOutlet		= false;
				bGoExArea	= false;

				//-----------------------------------------
				for(i=0; i<8; i++)
				{
					ix		= Get_xTo(i,x);
					iy		= Get_yTo(i,y);

					bExArea	= !m_pDEM->is_InGrid(ix, iy);

					if( bExArea || z > m_pDEM->asDouble(ix,iy) )
					{
						Pit_ID[i]	= iID	= bExArea ? 0 : m_pPits->asInt(ix,iy);

						if( iID >= 0 )
						{
							for(j=0; j<i && !bOutlet; j++)
							{
								jID		= Pit_ID[j];

								if(	jID >= 0 && !Get_Junction(iID, jID) )
								{
									bOutlet		= true;
								}
							}
						}

						//---------------------------------
						if( !bGoExArea )
						{
							if( bExArea )
							{
								bGoExArea	= true;
								iMin		= i;
							}
							else
							{
								dz			= (z - m_pDEM->asDouble(ix,iy)) / Get_Length(i);

								if( iMin < 0 || dzMin < dz )
								{
									iMin	= i;
									dzMin	= dz;
								}
							}
						}
					}
					else
					{
						Pit_ID[i]	= -1;
					}
				}

				//-----------------------------------------
				if( bOutlet )
				{
					if( pOutlet )
					{
						pOutlet->Next		= (TPit_Outlet *)SG_Malloc(sizeof(TPit_Outlet));
						pOutlet->Next->Prev	= pOutlet;
						pOutlet				= pOutlet->Next;
					}
					else
					{
						m_Outlets	= pOutlet	= (TPit_Outlet *)SG_Malloc(sizeof(TPit_Outlet));
						m_Outlets->Prev		= NULL;
					}

					pOutlet->Next	= NULL;
					pOutlet->x		= x;
					pOutlet->y		= y;
					memcpy(pOutlet->Pit_ID, Pit_ID, 8 * sizeof(int));

					//-------------------------------------
					for(i=1; i<8; i++)
					{
						iID	= Pit_ID[i];

						if( iID >= 0 )
						{
							for(j=0; j<i; j++)
							{
								jID	= Pit_ID[j];

								if( jID >= 0 && !Get_Junction(iID, jID) )
								{
									Add_Junction(iID, jID);
								}
							}
						}
					}
				}

				//-----------------------------------------
				if( iMin >= 0 )
				{
					m_pPits->Set_Value(x,y, Pit_ID[iMin] );
				}
			}
		}

		//-------------------------------------------------
		for(i=0; i<nPits; i++)
		{
			if( m_Junction[i] )
			{
				SG_Free(m_Junction[i]);
			}
		}

		SG_Free(m_Junction);

		SG_Free(m_nJunctions);
	}

	return( 0 );
}

//---------------------------------------------------------
int CPit_Router::Find_Route(TPit_Outlet *pOutlet)
{
	bool	bDrained, bNotDrained;

	int		x, y, i, ix, iy, iMin,
			Pit_ID, nPitsDrained;

	double	z, dz, dzMin;


	//-----------------------------------------------------
	// 1. Ist Outlets Verbindung zw. Drained und Not Drained ???...

	bDrained		= false;
	bNotDrained		= false;

	for(i=0; i<8; i++)
	{
		Pit_ID	= pOutlet->Pit_ID[i];

		if( Pit_ID == 0 )
		{
			bDrained	= true;
		}
		else if( Pit_ID > 0 )
		{
			if( m_Pit[Pit_ID - 1].bDrained )
			{
				bDrained	= true;
			}
			else
			{
				bNotDrained	= true;
			}
		}
	}

	//-----------------------------------------------------
	nPitsDrained	= 0;

	if( bDrained )
	{
		if( bNotDrained )
		{
			x		= pOutlet->x;
			y		= pOutlet->y;
			z		= m_pDEM->asDouble(x,y);


			//---------------------------------------------
			// 2. Threshold ??!!...

		/*	if( m_Threshold > 0.0 )
			{
				for(i=0; i<8; i++)
				{
					Pit_ID	= pOutlet->Pit_ID[i];

					if( Pit_ID > 0 && !m_Pit[Pit_ID - 1].bDrained && m_Threshold < z - m_Pit[Pit_ID - 1].z )
					{
						pOutlet->Pit_ID[i]	= -1;
					}
				}
			}/**/


			//---------------------------------------------
			// 3.a) nach außen entwaessern...

			if( !m_pRoute->asChar(x,y) )
			{
				iMin	= -1;

				for(i=0; i<8; i++)
				{
					ix	= Get_xTo(i,x);
					iy	= Get_yTo(i,y);

					if( !m_pDEM->is_InGrid(ix, iy) || m_pRoute->asChar(ix, iy) > 0 )
					{
						iMin	= i;
						break;
					}
					else
					{
						Pit_ID	= pOutlet->Pit_ID[i];

						if(	Pit_ID == 0 || (Pit_ID > 0 && m_Pit[Pit_ID - 1].bDrained) )
						{
							dz		= (z - m_pDEM->asDouble(ix,iy)) / Get_Length(i);

							if( iMin < 0 || dzMin < dz )
							{
								iMin	= i;
								dzMin	= dz;
							}
						}
					}
				}

				if( iMin >= 0 )
				{
					m_pRoute->Set_Value(x,y, iMin > 0 ? iMin : 8 );
				}
				else
				{
					SG_UI_Msg_Add_Error(_TL("Routing Error"));
				}
			}


			//---------------------------------------------
			// 3.b) Pit(s)/Flat(s) drainieren...

			for(i=0; i<8; i++)
			{
				Pit_ID	= pOutlet->Pit_ID[i];

				if( Pit_ID > 0 && !m_Pit[Pit_ID - 1].bDrained )
				{
					m_Pit[Pit_ID - 1].bDrained	= true;

					Drain_Pit(x,y,Pit_ID);

					nPitsDrained++;
				}
			}
		}


		//-------------------------------------------------
		// 4. pOutlet entfernen...

		if( pOutlet->Prev )
		{
			pOutlet->Prev->Next	= pOutlet->Next;
		}
		else
		{
			m_Outlets			= pOutlet->Next;
		}

		if( pOutlet->Next )
		{
			pOutlet->Next->Prev	= pOutlet->Prev;
		}

		SG_Free(pOutlet);
	}

	return( nPitsDrained );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPit_Router::Add_Junction(int iID, int jID)
{
	int		i;

	if( iID != jID )
	{
		if( iID > jID )
		{
			i	= iID;
			iID	= jID;
			jID	= i;
		}

		m_nJunctions[iID]++;

		i	= m_nJunctions[iID];

		m_Junction[iID]			= (int *)SG_Realloc(m_Junction[iID], i * sizeof(int));
		m_Junction[iID][i-1]	= jID;
	}
}

//---------------------------------------------------------
bool CPit_Router::Get_Junction(int iID, int jID)
{
	int		i;

	if(	iID == jID )
	{
		return( true );
	}
	else
	{
		if( iID > jID )
		{
			i	= iID;
			iID	= jID;
			jID	= i;
		}

		for(i=0; i<m_nJunctions[iID]; i++)
		{
			if( m_Junction[iID][i] == jID )
			{
				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPit_Router::Drain_Pit(int x, int y, int Pit_ID)
{
	int		i, ix, iy, iMin;

	double	z, dz, dzMin;

	do
	{
		iMin	= -1;

		if( m_pFlats && m_pFlats->asInt(x,y) > 0 )
		{
			Drain_Flat(x,y);
		}
		else
		{
			z		= m_pDEM->asDouble(x,y);
			dzMin	= 0;

			for(i=0; i<8; i++)
			{
				ix	= Get_xTo(i,x);
				iy	= Get_yTo(i,y);

				if( m_pDEM->is_InGrid(ix,iy) && m_pPits->asInt(ix,iy) == Pit_ID && !m_pRoute->asChar(ix,iy) )
				{
					dz		= (z - m_pDEM->asDouble(ix,iy)) / Get_Length(i);

					if( dzMin < dz )
					{
						iMin	= i;
						dzMin	= dz;
					}
				}
			}

			if( iMin >= 0 )
			{
				x	+= Get_xTo(iMin);
				y	+= Get_yTo(iMin);

				i	= (iMin + 4) % 8;

				m_pRoute->Set_Value(x,y, i > 0 ? i : 8 );
			}
		}
	}
	while( iMin >= 0 );
}

//---------------------------------------------------------
void CPit_Router::Drain_Flat(int x, int y)
{
	bool		bContinue;

	int			i, ix, iy, j,
				n, nPlus,
				Flat_ID;

	TGEO_iRect	*pFlat;

	//-----------------------------------------------------
	Flat_ID	= m_pFlats->asInt(x,y);

	if( Flat_ID > 0 )
	{
		pFlat	= m_Flat + Flat_ID - 1;

		nPlus	= -1;

		m_pFlats->Set_Value(x,y, nPlus );

		//-------------------------------------------------
		do
		{
			bContinue	= false;
			n			= nPlus--;

			for(y=pFlat->yMin; y<=pFlat->yMax; y++)
			{
				for(x=pFlat->xMin; x<=pFlat->xMax; x++)
				{
					if( m_pFlats->asInt(x,y) == n )
					{
						for(i=0; i<8; i++)
						{
							ix	= Get_xTo(i,x);
							iy	= Get_yTo(i,y);

							if(	m_pDEM->is_InGrid(ix, iy) && Flat_ID == m_pFlats->asInt(ix, iy) )
							{
								bContinue	= true;

								j			= (i + 4) % 8;

								m_pRoute->Set_Value(ix,iy, j ? j : 8 );
								m_pFlats->Set_Value(ix,iy, nPlus );
							}
						}
					}
				}
			}
		}
		while( bContinue );

		//-------------------------------------------------
		for(y=pFlat->yMin; y<=pFlat->yMax; y++)
		{
			for(x=pFlat->xMin; x<=pFlat->xMax; x++)
			{
				if( m_pFlats->asInt(x,y) < 0 )
				{
					m_pFlats->Set_Value(x,y, 0 );
				}
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
void CPit_Router::Mark_Flat(int x, int y, TGEO_iRect *pFlat, int Flat_ID, int Pit_ID)
{
	bool	goStackDown;

	int		i, ix, iy,
			iStart, iStack, nStack,
			*xMem, *yMem, *iMem;

	double	z;

	//-----------------------------------------------------
	if( !m_pFlats )
	{
		m_pFlats		= SG_Create_Grid(m_pDEM, SG_DATATYPE_Int);
		//m_pFlats->Set_Cache(false);
	}

	z			= m_pDEM->asDouble(x,y);

	xMem		= NULL;
	yMem		= NULL;
	iMem		= NULL;

	iStart		= 0;
	iStack		= 0;
	nStack		= 0;

	pFlat->xMin	= pFlat->xMax	= x;
	pFlat->yMin	= pFlat->yMax	= y;

	m_pPits->Set_Value(	x, y, Pit_ID );
	m_pFlats->Set_Value(	x, y, Flat_ID );


	//-----------------------------------------------------
	do
	{
		goStackDown	= true;

		for(i=iStart; i<8 && goStackDown; i++)
		{
			ix	= Get_xTo(i,x);
			iy	= Get_yTo(i,y);

			if(	m_pDEM->is_InGrid(ix, iy) && !m_pPits->asInt(ix, iy) && IS_Flat(z, m_pDEM->asDouble(ix, iy)) )
			{
				goStackDown		= false;
				m_pPits->Set_Value(	ix, iy, Pit_ID );
				m_pFlats->Set_Value(	ix, iy, Flat_ID );
			}
		}

		//-------------------------------------------------
		if( goStackDown )
		{
			iStack--;

			if( iStack >= 0 )
			{
				x		= xMem[iStack];
				y		= yMem[iStack];
				iStart	= iMem[iStack];
			}
		}
		else
		{
			if( nStack <= iStack )
			{
				nStack	= iStack + 32;
				xMem	= (int  *)SG_Realloc(xMem, nStack * sizeof(int ));
				yMem	= (int  *)SG_Realloc(yMem, nStack * sizeof(int ));
				iMem	= (int  *)SG_Realloc(iMem, nStack * sizeof(int ));
			}

			xMem[iStack]	= x;
			yMem[iStack]	= y;
			iMem[iStack]	= i + 1;

			x				= ix;
			y				= iy;
			iStart			= 0;

			if( x < pFlat->xMin )
				pFlat->xMin	= x;
			else if( x > pFlat->xMax )
				pFlat->xMax	= x;

			if( y < pFlat->yMin )
				pFlat->yMin	= y;
			else if( y > pFlat->yMax )
				pFlat->yMax	= y;

			iStack++;
		}
	}
	while( iStack >= 0 );

	//-----------------------------------------------------
	if( nStack > 0 )
	{
		SG_Free(xMem);
		SG_Free(yMem);
		SG_Free(iMem);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPit_Router::Process_Threshold(void)
{
	int		x, y, n;
	long	i;

	m_Route.Create(*Get_System(), SG_DATATYPE_Char);

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_Route.Set_Value(x, y, -1);
			}
			else if( (i = m_pRoute->asInt(x, y)) > 0 )
			{
				m_Route.Set_Value(x, y, i % 8);
			}
			else
			{
				m_Route.Set_Value(x, y, m_pDEM->Get_Gradient_NeighborDir(x, y));
			}
		}
	}

	//-----------------------------------------------------
	Lock_Create();

	for(i=0, n=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
	{
		if( m_pDEM->Get_Sorted(i, x, y, false) && m_pPits->asInt(x, y) )
		{
			m_zThr	= m_pDEM->asDouble(x, y) + m_Threshold;
			m_zMax	= m_pDEM->asDouble(x, y);

			Check_Threshold(x, y);

			if( m_zMax > m_zThr )
				n++;
		}
	}

	Lock_Destroy();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			i	= m_Route.asInt(x, y);

			if( i < 0 || i == m_pDEM->Get_Gradient_NeighborDir(x, y) )
			{
				m_pRoute->Set_Value(x, y, 0);
			}
			else
			{
				m_pRoute->Set_Value(x, y, i == 0 ? 8 : i);
			}
		}
	}

	//-----------------------------------------------------
	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("number of pits above threshold level"), n));

	m_Route.Destroy();

	return( n );
}

//---------------------------------------------------------
void CPit_Router::Check_Threshold(int x, int y)
{
	if( Lock_Get(x, y) )
		return;

	Lock_Set(x, y);

	if( m_pDEM->asDouble(x, y) > m_zMax )
	{
		m_zMax	= m_pDEM->asDouble(x, y);
	}

	int		i	= m_Route.asInt(x, y);
	int		ix	= Get_xTo(i, x);
	int		iy	= Get_yTo(i, y);

	if( m_pDEM->is_InGrid(ix, iy) )
	{
		if( m_pDEM->asDouble(x, y) < m_pDEM->asDouble(ix, iy) || m_zMax < m_zThr )
		{
			Check_Threshold(ix, iy);
		}
	}

	if( m_zMax > m_zThr )
	{
		m_Route.Set_Value(x, y, (i + 4) % 8);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
