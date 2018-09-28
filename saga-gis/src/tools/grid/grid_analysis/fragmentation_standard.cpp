/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Fragmentation_Standard.cpp              //
//                                                       //
//                 Copyright (C) 2008 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "fragmentation_standard.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VAL_NODATA	-1
#define VAL_NO		 0
#define VAL_YES		 1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFragmentation_Standard::CFragmentation_Standard(void)
	: CFragmentation_Base()
{
	Set_Name		(_TL("Fragmentation (Standard)"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Grid based fragmentation analysis after Riitters et al. (2000).\n"
		"\n"
		"(1) interior, if Density = 1.0\n"
		"(2) undetermined, if Density > 0.6 and Density = Connectivity\n"
		"(3) perforated, if Density > 0.6 and Density - Connectivity > 0\n"
		"(4) edge, if Density > 0.6 and Density - Connectivity < 0\n"
		"(5) transitional, if 0.4 < Density < 0.6\n"
		"(6) patch, if Density < 0.4\n"
	));

	Add_Reference("Riitters, K., Wickham, J., O'Neill, R., Jones, B., Smith, E.", "2000",
		"Global-scale patterns of forest fragmentation",
		"Conservation Ecology 4(2):3."
		SG_T("https://www.srs.fs.usda.gov/pubs/ja/ja_riitters006.pdf")
	);

	Parameters.Add_Choice(
		"", "CIRCULAR"	, _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("square"),
			_TL("circle")
		), 1
	);

	Parameters.Add_Bool(
		"", "DIAGONAL"	, _TL("Include diagonal neighbour relations"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Standard::Initialise(CSG_Grid *pClasses, int Class)
{
	int		x, y;

	m_bCircular		= Parameters("CIRCULAR")->asInt() == 1;
	m_bDiagonal		= Parameters("DIAGONAL")->asBool();

	//-----------------------------------------------------
	m_Grid.Create(Get_System(), SG_DATATYPE_Char);
	m_Grid.Set_NoData_Value(VAL_NODATA);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			m_Grid.Set_Value(x, y, pClasses->is_NoData(x, y) ? VAL_NODATA : pClasses->asInt(x, y) == Class ? VAL_YES : VAL_NO);
		}
	}

	//-----------------------------------------------------
	m_Radius.Create(SG_DATATYPE_Int, 1 + 2 * m_Radius_iMax, 1 + 2 * m_Radius_iMax);

	for(y=0; y<m_Radius.Get_NY(); y++)
	{
		for(x=0; x<m_Radius.Get_NX(); x++)
		{
			m_Radius.Set_Value(x, y, (int)(0.5 + M_GET_LENGTH(x - m_Radius_iMax, y - m_Radius_iMax)));
		}
	}

	//-----------------------------------------------------
	for(y=m_Radius_iMin; y<=m_Radius_iMax; y++)
	{
		Message_Fmt("\n%s %d: %f (%f)", _TL("Scale"), 1 + y - m_Radius_iMin, (1.0 + 2.0 * y) * Get_Cellsize(), 1.0 + 2.0 * y);
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CFragmentation_Standard::Finalise(void)
{
	m_Grid  .Destroy();
	m_Radius.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Standard::Get_Fragmentation(int x, int y, double &Density, double &Connectivity)
{
	if( !m_Grid.is_NoData(x, y) )
	{
		int		i, n;
		double	d, c;

		Density			= 0.0;
		Connectivity	= 0.0;

		for(i=m_Radius_iMin, n=0; i<=m_Radius_iMax; i++)
		{
			if( Get_Fragmentation(x, y, d, c, i) )
			{
				if( n == 0 )
				{
					Density			= d;
					Connectivity	= c;
				}
				else
				{
					switch( m_Aggregation )
					{
					default:
						Density			= 0.5 * (d + Density);
						Connectivity	= 0.5 * (c + Connectivity);
						break;

					case  1:
						Density			*= d;
						Connectivity	*= c;
						break;
					}
				}

				n++;
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Standard::Get_Fragmentation(int x, int y, double &Density, double &Connectivity, int Radius)
{
	if( m_Grid.is_InGrid(x, y) )
	{
		int		Value, dx, dy, nDensity, nConnectivity;

		Density			= 0.0;
		nDensity		= 0;

		Connectivity	= 0.0;
		nConnectivity	= 0;

		for(dy=-Radius; dy<=Radius; dy++)
		{
			for(dx=-Radius; dx<=Radius; dx++)
			{
				if( Get_Value(x, y, dx, dy, Radius, Value) )
				{
					nDensity++;

					if( Value == VAL_YES )
					{
						Density++;

						if( Get_Value(x, y, dx + 0, dy + 1, Radius, Value) )		// up
						{
							nConnectivity++;

							if( Value == VAL_YES )
								Connectivity++;
						}

						if( Get_Value(x, y, dx + 1, dy + 0, Radius, Value) )		// right
						{
							nConnectivity++;

							if( Value == VAL_YES )
								Connectivity++;
						}

						if( m_bDiagonal )
						{
							if( Get_Value(x, y, dx + 1, dy + 1, Radius, Value) )	// right-up
							{
								nConnectivity++;

								if( Value == VAL_YES )
									Connectivity++;
							}

							if( Get_Value(x, y, dx + 1, dy - 1, Radius, Value) )	// right-down
							{
								nConnectivity++;

								if( Value == VAL_YES )
									Connectivity++;
							}
						}
					}
					else
					{
						if( Get_Value(x, y, dx + 0, dy + 1, Radius, Value) && Value == VAL_YES )		// up
							nConnectivity++;

						if( Get_Value(x, y, dx + 1, dy + 0, Radius, Value) && Value == VAL_YES )		// right
							nConnectivity++;

						if( m_bDiagonal )
						{
							if( Get_Value(x, y, dx + 1, dy + 1, Radius, Value) && Value == VAL_YES )	// right-up
								nConnectivity++;

							if( Get_Value(x, y, dx + 1, dy - 1, Radius, Value) && Value == VAL_YES )	// right-down
								nConnectivity++;
						}
					}
				}
			}
		}

		//-------------------------------------------------
		if( nDensity > 0 )
		{
			Density			/= nDensity;
		}

		if( nConnectivity > 0 )
		{
			Connectivity	/= nConnectivity;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CFragmentation_Standard::in_Radius(int x, int y, int Radius)
{
	if( m_bCircular )
	{
		x	+= m_Radius_iMax;
		y	+= m_Radius_iMax;

		if( !m_Radius.is_InGrid(x, y) || m_Radius.asInt(x, y) > Radius )
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
inline bool CFragmentation_Standard::Get_Value(int x, int y, int dx, int dy, int Radius, int &Value)
{
	if( in_Radius(dx, dy, Radius) )
	{
		x	+= dx;
		y	+= dy;

		if( m_Grid.is_InGrid(x, y) )
		{
			Value	= m_Grid.asInt(x, y);

			return( true );
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
