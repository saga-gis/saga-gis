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
//                   Cellular_Automata                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Wator.cpp                       //
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
#include "Wator.h"
#include <time.h>

//---------------------------------------------------------
#define FISH	1
#define SHARK	2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWator::CWator(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	Set_Name		(_TL("Wa-Tor"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Wa-Tor - an ecological simulation of predator-prey populations - "
		"is based upon A. K. Dewdney's 'Computer Recreations' article "
		"in the December 1984 issue of Scientific American."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"			, _TL("Grid"),
		_TL("")
	);

	pNode_0	= Parameters.Add_Node(
		NULL	, "NODE_GRID"		, _TL("New Grid Dimensions"),
		_TL("If grid is not set, a new one will be created using chosen width and height.")
	);

	Parameters.Add_Value(
		pNode_0	, "NX"				, _TL("Width (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		pNode_0	, "NY"				, _TL("Height (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	pNode_0	= Parameters.Add_Grid(
		NULL	, "RESULT"			, _TL("Wa-Tor"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL	, true, SG_DATATYPE_Byte
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "REFRESH"			, _TL("Refresh"),
		_TL(""),
		PARAMETER_TYPE_Bool			, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "INIT_FISH"		, _TL("Initial Number of Fishes [%]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 30.0, 0.0, true, 100.0, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "INIT_SHARK"		, _TL("Initial Number of Sharks [%]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 7.5, 0.0, true, 100.0, true
	);

	pNode_0	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Cycles"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "FISH_BIRTH"		, _TL("Birth Rate of Fishes"),
		_TL(""),
		PARAMETER_TYPE_Int			,  3.0, 0.0, true
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "SHARK_BIRTH"		, _TL("Birth Rate of Sharks"),
		_TL(""),
		PARAMETER_TYPE_Int			, 12.0, 0.0, true
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "SHARK_STARVE"	, _TL("Max. Starvation Time for Sharks"),
		_TL(""),
		PARAMETER_TYPE_Int			,  4.0, 0.0, true
	);
}

//---------------------------------------------------------
CWator::~CWator(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWator::On_Execute(void)
{
	bool			bRefresh;
	int				x, y, i;
	double			perc, Fish_perc, Shark_perc;
	CSG_Colors			Colors;
	CSG_Table			*pTable;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	if( (pWator = Parameters("RESULT")->asGrid()) == NULL )
	{
		bRefresh	= true;
		pWator		= SG_Create_Grid(SG_DATATYPE_Byte, Parameters("NX")->asInt(), Parameters("NY")->asInt());
		Parameters("GRID")->Set_Value(pWator);
	}
	else
	{
		bRefresh	= Parameters("REFRESH")->asBool();
	}

	pWator->Set_Name(_TL("Wa-Tor"));

	Colors.Set_Count(3);
	Colors.Set_Color(0, SG_GET_RGB(  0,   0,   0));
	Colors.Set_Color(1, SG_GET_RGB(  0, 255,   0));
	Colors.Set_Color(2, SG_GET_RGB(255,   0,   0));
	DataObject_Set_Colors(pWator, Colors);
	DataObject_Update(pWator, 0, 3, SG_UI_DATAOBJECT_SHOW);

	//-----------------------------------------------------
	Fish_Birth		= Parameters("FISH_BIRTH")	->asInt();
	Shark_Birth		= Parameters("SHARK_BIRTH")	->asInt();
	Shark_Starve	= Parameters("SHARK_STARVE")->asInt();

	pTable			= Parameters("TABLE")		->asTable();
	pTable->Destroy();
	pTable->Set_Name(_TL("Wa-Tor"));
	pTable->Add_Field("Cycle"	, SG_DATATYPE_Int);
	pTable->Add_Field("Fishes"	, SG_DATATYPE_Int);
	pTable->Add_Field("Sharks"	, SG_DATATYPE_Int);

	pNext			= SG_Create_Grid(pWator, SG_DATATYPE_Byte);
	pAge			= SG_Create_Grid(pWator, SG_DATATYPE_Byte);
	pStarve			= SG_Create_Grid(pWator, SG_DATATYPE_Byte);

	srand((unsigned)time(NULL));

	//-----------------------------------------------------
	if( bRefresh )
	{
		pWator->Assign(0.0);

		Fish_perc		= Parameters("INIT_FISH" )->asDouble();
		Shark_perc		= Parameters("INIT_SHARK")->asDouble() + Fish_perc;

		for(y=0; y<pWator->Get_NY(); y++)
		{
			for(x=0; x<pWator->Get_NX(); x++)
			{
				perc	= 100.0 * (double)rand() / (double)RAND_MAX;

				if( perc <= Fish_perc )
				{
					pWator	->Set_Value(x, y, FISH);
				}
				else if( perc <= Shark_perc )
				{
					pWator	->Set_Value(x, y, SHARK);
				}
			}
		}
	}

	//-----------------------------------------------------
	pAge	->Assign();
	pStarve	->Assign();

	for(y=0; y<pWator->Get_NY(); y++)
	{
		for(x=0; x<pWator->Get_NX(); x++)
		{
			switch( pWator->asByte(x, y) )
			{
			case FISH:
				pAge	->Set_Value(x, y, Fish_Birth	* (double)rand() / (double)RAND_MAX);
				break;

			case SHARK:
				pAge	->Set_Value(x, y, Shark_Birth	* (double)rand() / (double)RAND_MAX);
				pStarve	->Set_Value(x, y, Shark_Starve	* (double)rand() / (double)RAND_MAX);
				break;
			}
		}
	}

	//-----------------------------------------------------
	SG_UI_Progress_Lock(true);

	for(i=1; Process_Get_Okay(true) && Next_Cycle(); i++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %d"), _TL("Life Cycle"), i));

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, i);
		pRecord->Set_Value(1, nFishes);
		pRecord->Set_Value(2, nSharks);

		DataObject_Update(pWator, 0, 3);
		DataObject_Update(pTable);
	}

	SG_UI_Progress_Lock(false);

	//-----------------------------------------------------
	delete(pNext);
	delete(pAge);
	delete(pStarve);

	if( is_Progress() )
	{
		Message_Add(CSG_String::Format(SG_T("%s %d %s"), _TL("Dead after"), i, _TL("Life Cycles")));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_NEIGHBOR			{	ix = pWator->Get_System().Get_xTo(i, x); if( ix < 0 ) ix = pWator->Get_NX() - 1; else if( ix >= pWator->Get_NX() ) ix = 0;\
									iy = pWator->Get_System().Get_yTo(i, y); if( iy < 0 ) iy = pWator->Get_NY() - 1; else if( iy >= pWator->Get_NY() ) iy = 0;	}

#define GET_NEIGHBOR_RANDOMLY	{	i = iNeighbor[(int)((double)rand() * nNeighbors / (double)RAND_MAX)];\
									ix = pWator->Get_System().Get_xTo(i, x); if( ix < 0 ) ix = pWator->Get_NX() - 1; else if( ix >= pWator->Get_NX() ) ix = 0;\
									iy = pWator->Get_System().Get_yTo(i, y); if( iy < 0 ) iy = pWator->Get_NY() - 1; else if( iy >= pWator->Get_NY() ) iy = 0;	}

//---------------------------------------------------------
bool CWator::Next_Cycle(void)
{
	static int	iDir	= 0;

	int		x, y, i, ix, iy, xx, yy, ax, ay, dx, dy,
			iNeighbor[8], nNeighbors,
			Age, Starve;

	//-----------------------------------------------------
	nFishes	= 0;
	nSharks	= 0;

	pNext->Assign(0.0);

	switch( iDir )
	{
	default:
	case 3:	ay	= pWator->Get_NY() - 1;	dy	= -1;	ax	= pWator->Get_NX() - 1;	dx	= -1;	iDir=0;	break;
	case 2:	ay	= 0;					dy	=  1;	ax	= pWator->Get_NX() - 1;	dx	= -1;	iDir++;	break;
	case 1:	ay	= pWator->Get_NY() - 1;	dy	= -1;	ax	= 0;					dx	=  1;	iDir++;	break;
	case 0:	ay	= 0;					dy	=  1;	ax	= 0;					dx	=  1;	iDir++;	break;
	}

	//-----------------------------------------------------
	for(yy=0, y=ay; yy<pWator->Get_NY(); yy++, y+=dy)
	{
		for(xx=0, x=ax; xx<pWator->Get_NX(); xx++, x+=dx)
		{
			if( pWator->asByte(x, y) == FISH )
			{
				nFishes++;

				Age		= pAge->asInt(x, y) + 1;
				pAge->Set_Value(x, y, 0);

				for(i=0, nNeighbors=0; i<8; i++)
				{
					GET_NEIGHBOR;

					if( pWator->asByte(ix, iy) == 0 && pNext->asByte(ix, iy) == 0 )
					{
						iNeighbor[nNeighbors++]	= i;
					}
				}

				if( nNeighbors > 0 )
				{
					GET_NEIGHBOR_RANDOMLY;

					pNext	->Set_Value(ix, iy, FISH);
					pAge	->Set_Value(ix, iy, Age >= Fish_Birth ? 0 : Age);

					if( Age >= Fish_Birth )
					{
						pNext	->Set_Value( x,  y, FISH);
						pAge	->Set_Value( x,  y, Fish_Birth * (double)rand() / (double)RAND_MAX);
					}
					else
					{
						pWator	->Set_Value( x,  y, 0);
					}
				}
				else
				{
					pNext	->Set_Value( x,  y, FISH);
					pAge	->Set_Value( x,  y, Age >= Fish_Birth ? 0 : Fish_Birth);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(yy=0, y=ay; yy<pWator->Get_NY(); yy++, y+=dy)
	{
		for(xx=0, x=ax; xx<pWator->Get_NX(); xx++, x+=dx)
		{
			if( pWator->asByte(x, y) == SHARK )
			{
				nSharks++;

				Age		= pAge->asInt(x, y) + 1;
				pAge->Set_Value(x, y, 0);

				Starve	= pStarve->asInt(x, y) + 1;
				pStarve->Set_Value(x, y, 0);

				for(i=0, nNeighbors=0; i<8; i++)
				{
					GET_NEIGHBOR;

					if( pNext->asByte(ix, iy) == FISH )
					{
						iNeighbor[nNeighbors++]	= i;
					}
				}

				if( nNeighbors > 0 )
				{
					GET_NEIGHBOR_RANDOMLY;

					nFishes--;
					pWator	->Set_Value(ix, iy, 0);

					pNext	->Set_Value(ix, iy, SHARK);
					pAge	->Set_Value(ix, iy, Age >= Shark_Birth ? 0 : Age);
					pStarve	->Set_Value(ix, iy, 0);

					if( Age >= Shark_Birth )
					{
						pNext	->Set_Value( x,  y, SHARK);
						pAge	->Set_Value( x,  y, Shark_Birth * (double)rand() / (double)RAND_MAX);
						pStarve	->Set_Value( x,  y, 0);
					}
					else
					{
						pWator	->Set_Value( x,  y, 0);
					}
				}
				else if( Starve <= Shark_Starve )
				{
					for(i=0, nNeighbors=0; i<8; i++)
					{
						GET_NEIGHBOR;

						if( pWator->asByte(ix, iy) == 0 && pNext->asByte(ix, iy) == 0 )
						{
							iNeighbor[nNeighbors++]	= i;
						}
					}

					if( nNeighbors > 0 )
					{
						GET_NEIGHBOR_RANDOMLY;

						pNext	->Set_Value(ix, iy, SHARK);
						pAge	->Set_Value(ix, iy, Age >= Shark_Birth ? 0 : Age);
						pStarve	->Set_Value(ix, iy, Starve);

						if( Age >= Shark_Birth )
						{
							pNext	->Set_Value( x,  y, SHARK);
							pAge	->Set_Value( x,  y, Shark_Birth * (double)rand() / (double)RAND_MAX);
							pStarve	->Set_Value( x,  y, Starve);
						}
						else
						{
							pWator	->Set_Value( x,  y, 0);
						}
					}
					else
					{
						pNext	->Set_Value( x,  y, SHARK);
						pAge	->Set_Value( x,  y, Age >= Shark_Birth ? 0 : Shark_Birth);
						pStarve	->Set_Value( x,  y, Starve);
					}
				}
				else
				{
					nSharks--;
				}
			}
		}
	}

	pWator->Assign(pNext);

	return( (nFishes > 0 && nFishes < pWator->Get_NCells()) || nSharks > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
