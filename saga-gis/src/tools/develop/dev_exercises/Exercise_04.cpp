
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_04.cpp                     //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Exercise_04.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_04::CExercise_04(void)
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name		(_TL("04: Direct neighbours - more..."));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Simple neighbourhood analysis for grid cells."
	));


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		"", "INPUT"	, _TL("Input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			_TL("Arithmetic mean (version 1)"),
			_TL("Arithmetic mean (version 2)"),
			_TL("Difference from all neighbour's mean"),
			_TL("Variance (version 1)"),
			_TL("Variance (version 2)"),
			_TL("Standard deviation")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	m_pInput	= Parameters("INPUT" )->asGrid();
	m_pOutput	= Parameters("OUTPUT")->asGrid();


	//-----------------------------------------------------
	// Execute calculation...

	bool	bResult	= false;

	switch( Parameters("METHOD")->asInt() )
	{
	case  0:	bResult	= Method_01();	break;
	case  1:	bResult	= Method_02();	break;
	case  2:	bResult	= Method_03();	break;
	case  3:	bResult	= Method_04();	break;
	case  4:	bResult	= Method_05();	break;
	case  5:	bResult	= Method_06();	break;
	}


	//-----------------------------------------------------
	// Return 'true' if everything is okay...

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_01(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	s	= 0.;	int	n	= 0;

			for(int iy=y-1; iy<=y+1; iy++)
			{
				for(int ix=x-1; ix<=x+1; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						s	+= m_pInput->asDouble(ix, iy);
						n++;
					}
				}
			}

			if( n > 0 )
			{
				m_pOutput->Set_Value(x, y, s / n);
			}
			else
			{
				m_pOutput->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_02(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				double	s	= m_pInput->asDouble(x, y);	int	n	= 1;

				for(int i=0; i<8; i++)
				{
					int ix	= Get_xTo(i, x);
					int iy	= Get_yTo(i, y);

					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						s	+= m_pInput->asDouble(ix, iy);
						n++;
					}
				}

				m_pOutput->Set_Value(x, y, s / n);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_03(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				double	s	= 0.;	int	n	= 0;

				for(int i=0; i<8; i++)
				{
					int ix	= Get_xTo(i, x);
					int iy	= Get_yTo(i, y);

					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						s	+= m_pInput->asDouble(ix, iy);
						n++;
					}
				}

				if( n > 0 )
				{
					m_pOutput->Set_Value(x, y, m_pInput->asDouble(x, y) - s / n);
				}
				else
				{
					m_pOutput->Set_NoData(x, y);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_04(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	z[9];	int	n	= 0;

			for(int iy=y-1; iy<=y+1; iy++)
			{
				for(int ix=x-1; ix<=x+1; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						z[n++]	= m_pInput->asDouble(ix, iy);
					}
				}
			}

			if( n > 0 )
			{
				double	m = 0., v = 0.;

				for(int i=0; i<n; i++)
				{
					m	+= z[i];
				}

				m	/= n;

				for(int i=0; i<n; i++)
				{
					v	+= SG_Get_Square(m - z[i]);
				}

				v	/= n;

				m_pOutput->Set_Value(x, y, v);
			}
			else
			{
				m_pOutput->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_05(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	m = 0., v = 0.;	int	n = 0;

			for(int iy=y-1; iy<=y+1; iy++)
			{
				for(int ix=x-1; ix<=x+1; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						n++;

						double	z = m_pInput->asDouble(ix, iy);

						m	+= z;
						v	+= z * z;
					}
				}
			}

			if( n > 0 )
			{
				m	= m / n;
				v	= v / n - m * m;

				m_pOutput->Set_Value(x, y, v);
			}
			else
			{
				m_pOutput->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_06(void)
{
	if( Method_05() == true )
	{
		for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			if( m_pOutput->is_NoData(n) == false )
			{
				double	v	= m_pOutput->asDouble(n);

				m_pOutput->Set_Value(n, sqrt(v));
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
