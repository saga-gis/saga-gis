
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
//                       Life.cpp                        //
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
#include "Life.h"
#include <time.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLife::CLife(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Conway's Life"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Conway's Life - a cellular automat.\n\n"

		"Reference:\n"
		"- Eigen, M., Winkler, R. (1985): "
		"'Das Spiel - Naturgesetze steuern den Zufall', "
		"Muenchen, 404p.\n")
	);


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		""
	);

	Parameters.Add_Value(
		NULL	, "NX"		, _TL("Width (Cells)"),
		"",
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NY"		, _TL("Height (Cells)"),
		"",
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		NULL, "FADECOLOR"	, _TL("Fade Color Count"),
		"",
		PARAMETER_TYPE_Int, 63, 1, true, 255, true
	);
}

//---------------------------------------------------------
CLife::~CLife(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLife::On_Execute(void)
{
	int		x, y, i;
	CSG_Colors	Colors;

	//-----------------------------------------------------
	pLife	= SG_Create_Grid(GRID_TYPE_Byte, Parameters("NX")->asInt(), Parameters("NY")->asInt());
	pLife->Set_Name(_TL("Conway's Life"));
	Parameters("GRID")->Set_Value(pLife);

	nColors	= Parameters("FADECOLOR")->asInt();
	Colors.Set_Palette(COLORS_PALETTE_YELLOW_BLUE, false, nColors);
	DataObject_Set_Colors(pLife, Colors);

	pCount	= SG_Create_Grid(pLife);

	//-----------------------------------------------------
	srand((unsigned)time(NULL));

	pLife->Assign(0.0);

	for(y=0; y<pLife->Get_NY(); y++)
	{
		for(x=0; x<pLife->Get_NX(); x++)
		{
			if( rand() > RAND_MAX / 2 )
			{
				pLife->Set_Value(x, y, 1);
			}
		}
	}

	Next_Cycle();
	DataObject_Update(pLife, 0, nColors, true);

	//-----------------------------------------------------
	for(i=1; Process_Get_Okay(true) && Next_Cycle(); i++)
	{
		DataObject_Update(pLife);

		Process_Set_Text(CSG_String::Format(_TL("%d. Life Cycle"), i));
	}

	//-----------------------------------------------------
	delete(pCount);

	if( is_Progress() )
	{
		Message_Add(CSG_String::Format(_TL("Dead after %d Life Cycles\n"), i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLife::Next_Cycle(void)
{
	bool	bContinue;
	int		x, y, i, ix, iy;

	//-----------------------------------------------------
	bContinue	= false;

	pCount->Assign();

	//-----------------------------------------------------
	for(y=0; y<pLife->Get_NY(); y++)
	{
		for(x=0; x<pLife->Get_NX(); x++)
		{
			if( (i = pLife->asByte(x, y)) == 0 )
			{
				for(i=0; i<8; i++)
				{
					ix	= pLife->Get_System().Get_xTo(i, x);
					if( ix < 0 )
					{
						ix	= pLife->Get_NX() - 1;
					}
					else if( ix >= pLife->Get_NX() )
					{
						ix	= 0;
					}

					iy	= pLife->Get_System().Get_yTo(i, y);
					if( iy < 0 )
					{
						iy	= pLife->Get_NY() - 1;
					}
					else if( iy >= pLife->Get_NY() )
					{
						iy	= 0;
					}

					pCount->Add_Value(ix, iy, 1);
				}
			}
			else if( i > 1 && i < nColors )
			{
				bContinue	= true;
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<pLife->Get_NY(); y++)
	{
		for(x=0; x<pLife->Get_NX(); x++)
		{
			i	= pCount->asByte(x, y);

			switch( i )
			{
			case 2:
				i	= pLife->asByte(x, y);

				if( i > 0 && i < nColors )
				{
					pLife->Add_Value(x, y, 1);
				}
				break;

			case 3:
				pLife->Set_Value(x, y, 0);
				break;

			default:
				i	= pLife->asByte(x, y);

				if( i < 1 )
				{
					pLife->Set_Value(x, y, 1);
				}
				else if( i < nColors )
				{
					pLife->Add_Value(x, y, 1);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bContinue );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
