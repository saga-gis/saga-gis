/**********************************************************
 * Version $Id: fill_sinks.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    fill_sinks.cpp                     //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                      Olaf Conrad                      //
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
//                University of Hamburg                  //
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
#include "fill_sinks.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CStack : public CSG_Stack
{
public:
	CStack(void) : CSG_Stack(3 * sizeof(int))	{}

	//-----------------------------------------------------
	virtual bool			Push			(int  x, int  y, int  i)
	{
		int	*Data	= (int *)Get_Record_Push();

		if( Data )
		{
			Data[0]	= x;
			Data[1]	= y;
			Data[2]	= i;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool			Pop				(int &x, int &y, int &i)
	{
		int	*Data	= (int *)Get_Record_Pop();

		if( Data )
		{
			x	= Data[0];
			y	= Data[1];
			i	= Data[2];

			return( true );
		}

		return( false );
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFill_Sinks::CFill_Sinks(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Fill Sinks (QM of ESP)"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Filling in pits and flats in a DEM."
	));

	Add_Reference("Pelletier, J.D.",
		"2008", "Quantitative Modeling of Earth Surface Processes",
		"Cambridge, 295p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"FILLED"	, _TL("DEM without Sinks"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"SINKS"		, _TL("Sinks"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"DZFILL"	, _TL("Fill Increment"),
		_TL(""),
		0.01, 0.0000001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFill_Sinks::On_Execute(void)
{
	CSG_Grid	*pDEM, *pSinks;

	pDEM		= Parameters("DEM"   )->asGrid();
	m_pDEM		= Parameters("FILLED")->asGrid();
	pSinks		= Parameters("SINKS" )->asGrid();
	m_dzFill	= Parameters("DZFILL")->asDouble();

	m_pDEM->Assign(pDEM);
	m_pDEM->Fmt_Name("%s [%s]", pDEM->Get_Name(), _TL("No Sinks"));

	if( !Fill_Sinks() )
	{
		return( false );
	}

	if( pSinks )
	{
		pSinks->Assign(m_pDEM);
		pSinks->Subtract(*pDEM);
		pSinks->Set_NoData_Value(0.0);
	}

	return( true );
}

//---------------------------------------------------------
bool CFill_Sinks::Fill_Sinks(CSG_Grid *pDEM, CSG_Grid *pFilled, double dzFill)
{
	if( is_Executing() || !pDEM || (pFilled && !pDEM->is_Compatible(pFilled)) || !Get_System()->Assign(pDEM->Get_System()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pFilled )
	{
		m_pDEM	= pFilled;
		m_pDEM->Assign(pDEM);
	}
	else
	{
		m_pDEM	= pDEM;
	}

	m_dzFill	= dzFill;


	return( Fill_Sinks() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFill_Sinks::Fill_Sinks(void)
{
	if( m_dzFill <= 0.0 )
	{
		return( false );
	}

	Process_Set_Text(_TL("Processing Sinks and Flats"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Fill_Sink(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CFill_Sinks::Fill_Sink(int x, int y)
{
	int		i;
	CStack	Stack;

	do
	{
		if( Fill_Cell(x, y) )
		{
			Stack.Push(x, y, 1);

			x	= Get_xTo(0, x);
			y	= Get_yTo(0, y);
		}
		else if( Stack.Pop(x, y, i) && i < 8 )
		{
			Stack.Push(x, y, 1 + i);

			x	= Get_xTo(i, x);
			y	= Get_yTo(i, y);
		}
	}
	while( Stack.Get_Size() > 0 );

	return( true );
}

//---------------------------------------------------------
bool CFill_Sinks::Fill_Cell(int x, int y)
{
	if( x > 0 && x < Get_NX() - 1 && y > 0 && y < Get_NY() - 1 && !m_pDEM->is_NoData(x, y) )
	{
		for(bool bFilled=false, bSingle=true; ; )
		{
			bool	bPit	= true;
			double	zMin	= m_pDEM->asDouble(x, y);

			for(int i=0; i<8; i++)
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				if( m_pDEM->is_InGrid(ix, iy) )
				{
					bSingle	= false;

					if( m_pDEM->asDouble(ix, iy) < zMin )
					{
						zMin	= m_pDEM->asDouble(ix, iy);
						bPit	= false;
					}
				}
			}

			if( !bSingle && bPit )
			{
				m_pDEM->Set_Value(x, y, zMin + m_dzFill);

				bFilled	= true;
			}
			else
			{
				return( bFilled );
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

/*/---------------------------------------------------------
void CFill_Sinks::Fill_Sinks(int x, int y)
{
	if( Fill_Cell(x, y) )
	{
		for(int i=0; i<8; i++)
		{
			Fill_Sinks(Get_xTo(i, x), Get_yTo(i, y));
		}
	}
}/**/

/*/---------------------------------------------------------
void CFill_Sinks::Fill_Sinks(int x, int y)
{
	if( x > 0 && x < Get_NX() - 1 && y > 0 && y < Get_NY() - 1 && !m_pDEM->is_NoData(x, y) )
	{
		int		i;
		double	zMin;

		for(i=0, zMin=m_pDEM->asDouble(x, y); i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && m_pDEM->asDouble(ix, iy) < zMin )
			{
				zMin	= m_pDEM->asDouble(ix, iy);
			}
		}

		if( m_pDEM->asDouble(x, y) <= zMin )
		{
			m_pDEM->Set_Value(x, y, zMin + m_dzFill);

			Fill_Sinks(x, y);

			for(i=0; i<8; i++)
			{
				Fill_Sinks(Get_xTo(i, x), Get_yTo(i, y));
			}
		}
	}
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
