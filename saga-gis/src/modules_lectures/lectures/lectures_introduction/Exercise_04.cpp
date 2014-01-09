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
	// Give some information about your module...

	Set_Name	(_TL("04: Direct neighbours - more..."));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Simple neighbourhood analysis for grid cells.\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "INPUT"	, _TL("Input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "METHOD"	, _TL("Method"),
		_TL("Choose a method"),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Arithmetic mean (version 1)"),
			_TL("Arithmetic mean (version 2)"),
			_TL("Difference from all neighbour's mean"),
			_TL("Variance (version 1)"),
			_TL("Variance (version 2)"),
			_TL("Standard deviation")
		)
	);
}

//---------------------------------------------------------
CExercise_04::~CExercise_04(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::On_Execute(void)
{
	bool	bResult;
	int		Method;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pInput	= Parameters("INPUT" )->asGrid();
	m_pOutput	= Parameters("OUTPUT")->asGrid();

	Method		= Parameters("METHOD")->asInt();


	//-----------------------------------------------------
	// Execute calculation...

	switch( Method )
	{
	case 0:
		bResult	= Method_01();
		break;

	case 1:
		bResult	= Method_02();
		break;

	case 2:
		bResult	= Method_03();
		break;

	case 3:
		bResult	= Method_04();
		break;

	case 4:
		bResult	= Method_05();
		break;

	case 5:
		bResult	= Method_06();
		break;

	default:
		bResult	= false;
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_04::Method_01(void)
{
	int		x, y, ix, iy, n;
	double	s;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			s	= 0.0;
			n	= 0;

			for(iy=y-1; iy<=y+1; iy++)
			{
				for(ix=x-1; ix<=x+1; ix++)
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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_04::Method_02(void)
{
	int		x, y, i, ix, iy, n;
	double	s;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				s	= m_pInput->asDouble(x, y);
				n	= 1;

				for(i=0; i<8; i++)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_04::Method_03(void)
{
	int		x, y, i, ix, iy, n;
	double	s;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				s	= 0.0;
				n	= 0;

				for(i=0; i<8; i++)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_04::Method_04(void)
{
	int		x, y, i, ix, iy, n;
	double	m, v, z[9], dz;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0, ix=1; x<Get_NX(); x++, ix++)
		{
			for(n=0, iy=y-1; iy<=y+1; iy++)
			{
				for(ix=x-1; ix<=x+1; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						z[n++]	= m_pInput->asDouble(ix, iy);
					}
				}
			}

			if( n > 0 )
			{
				for(m=0.0, i=0; i<n; i++)
				{
					m	+= z[i];
				}

				m	/= n;

				for(v=0.0, i=0; i<n; i++)
				{
					dz	 = m - z[i];
					v	+= dz * dz;
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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_04::Method_05(void)
{
	int		x, y, ix, iy, n;
	double	m, v, z;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			n	= 0;
			m	= 0.0;
			v	= 0.0;

			for(iy=y-1; iy<=y+1; iy++)
			{
				for(ix=x-1; ix<=x+1; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						n++;

						z	 = m_pInput->asDouble(ix, iy);

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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_04::Method_06(void)
{
	long	n;
	double	v;

	//-----------------------------------------------------
	if( Method_05() == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( m_pOutput->is_NoData(n) == false )
		{
			v	= m_pOutput->asDouble(n);

			m_pOutput->Set_Value(n, sqrt(v));
		}
	}

	//-----------------------------------------------------
	return( true );
}
