
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_10.cpp                     //
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
#include "Exercise_10.h"
#include <time.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_10::CExercise_10(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name	(_TL("10: Dynamic Simulation - Life"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Conway's game of life. A cellular automate.\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Life"),
		"",
		PARAMETER_OUTPUT, true, GRID_TYPE_Byte
	);

	Parameters.Add_Value(
		NULL, "REFRESH"		, _TL("Refresh"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL, "COLORS"		, _TL("Fade Color Count"),
		"",
		PARAMETER_TYPE_Int, 64, 3, true, 255, true
	);
}

//---------------------------------------------------------
CExercise_10::~CExercise_10(void)
{}


///////////////////////////////////////////////////////////
//														 //
//	Run													 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_10::On_Execute(void)
{
	bool	bAlive;
	int		x, y, i;
	CSG_Colors	Colors;


	//-----------------------------------------------------
	// General initialisations...

	m_pLife		= Parameters("RESULT")->asGrid();

	m_nColors	= Parameters("COLORS")->asInt();

	Colors.Set_Count(m_nColors + 1);
	Colors.Set_Ramp(SG_GET_RGB(127, 127, 127), SG_GET_RGB(0, 0, 0));
	Colors.Set_Color(0, SG_GET_RGB(255, 255, 255));
	DataObject_Set_Colors(m_pLife, Colors);


	//-----------------------------------------------------
	// Initialise life's world...

	if( Parameters("REFRESH")->asBool() )
	{
		srand((unsigned)time(NULL));

		for(y=0; y<Get_NY(); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				m_pLife->Set_Value(x, y, rand() > RAND_MAX / 2 ? 0 : 1);
			}
		}
	}


	//-----------------------------------------------------
	// Execution...

	m_pTemp		= SG_Create_Grid(m_pLife, GRID_TYPE_Byte);

	for(i=1, bAlive=true; bAlive && Process_Get_Okay(true); i++)
	{
		Process_Set_Text(CSG_String::Format("%d %s", i, _TL("Life Cycle")));

		if( (bAlive = Next_Step()) == false )
		{
			Message_Add(CSG_String::Format("%s %d %s\n", _TL("Dead after"), i, _TL("Life Cycles")));
		}
	}

	delete(m_pTemp);


	//-----------------------------------------------------
	// Finish...

	return( true );
}

//---------------------------------------------------------
bool CExercise_10::Next_Step(void)
{
	bool	bAlive;
	int		x, y, i, ix, iy, n;

	bAlive	= false;

	//-----------------------------------------------------
	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			//---------------------------------------------
			// Count neighours...

			for(i=0, n=0; i<8; i++)
			{
				ix	= Get_xTo(i, x);
				if( ix < 0 )
					ix	= Get_NX() - 1;
				else if( ix >= Get_NX() )
					ix	= 0;

				iy	= Get_yTo(i, y);
				if( iy < 0 )
					iy	= Get_NY() - 1;
				else if( iy >= Get_NY() )
					iy	= 0;

				if( m_pLife->asByte(ix, iy) == 0 )
				{
					n++;
				}
			}


			//---------------------------------------------
			// Dead or alive...

			i	= m_pLife->asByte(x, y);

			switch( n )
			{
			case 2:		// keep status...
				if( i > 0 && i < m_nColors )	// color fading...
				{
					i++;
				}
				break;

			case 3:		// Birth...
				i	= 0;
				break;

			default:	// Dead...
				if( i < m_nColors )				// color fading...
				{
					i++;
				}
			}

			if( i > 1 && i < m_nColors )
			{
				bAlive	= true;
			}

			m_pTemp->Set_Value(x, y, i);
		}
	}

	//-----------------------------------------------------
	m_pLife->Assign(m_pTemp);

	DataObject_Update(m_pLife, 0, m_nColors, true);

	return( bAlive );
}
