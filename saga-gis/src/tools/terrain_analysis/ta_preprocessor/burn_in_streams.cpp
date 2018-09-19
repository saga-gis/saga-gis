/**********************************************************
 * Version $Id: burn_in_streams.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  burn_in_streams.cpp                  //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "burn_in_streams.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBurnIn_Streams::CBurnIn_Streams(void)
{
	Set_Name		(_TL("Burn Stream Network into DEM"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Burns a stream network into a Digital Elevation Model (DEM). "
		"Stream cells have to be coded with valid data values, all other "
		"cells should be set to no data value. "
		"First two methods decrease . "
		"The third method ensures a steady downstream gradient. An elevation "
		"decrease is only applied, if a downstream cell is equally high or higher. "
		"You should provide a grid with flow directions for determination of "
		"downstream cells. The 'Sink Drainage Route Detection' tool offers "
		"such flow directions. "
	));

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "BURN"		, _TL("Processed DEM"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STREAM"		, _TL("Streams"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "FLOWDIR"		, _TL("Flow Direction"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("simply decrease cell's value by epsilon"),
			_TL("lower cell's value to neighbours minimum value minus epsilon"),
			_TL("trace stream network downstream")
		), 0
	);

	Parameters.Add_Value(
		NULL, "EPSILON"		, _TL("Epsilon"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CBurnIn_Streams::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Get_Parameter("FLOWDIR")->Set_Enabled(pParameter->asInt() == 2);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBurnIn_Streams::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("BURN"   )->asGrid();
	m_pStream	= Parameters("STREAM" )->asGrid();
	m_Epsilon	= Parameters("EPSILON")->asDouble();
	int	Method	= Parameters("METHOD" )->asInt();

	//-----------------------------------------------------
	if( !m_pDEM )
	{
		m_pDEM	= Parameters("DEM")->asGrid();
	}
	else
	{
		m_pDEM->Assign(Parameters("DEM")->asGrid());
		m_pDEM->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("DEM")->asGrid()->Get_Name(), _TL("Burned Streams")));
	}

	//-----------------------------------------------------
	bool	bResult	= false;

	switch( Method )
	{
	case 0:	bResult	= Burn_Simple(false);	break;	// simply decrease cell's value by epsilon
	case 1:	bResult	= Burn_Simple( true);	break;	// lower cell's value to neighbours minimum value minus epsilon
	case 2:	bResult	= Burn_Trace();			break;	// trace stream network downstream
	}

	//-----------------------------------------------------
	if( Parameters("BURN")->asGrid() == NULL )
	{
		DataObject_Update(m_pDEM);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBurnIn_Streams::Burn_Simple(bool bNeighbours)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pStream->is_NoData(x, y) && !m_pDEM->is_NoData(x, y) )
			{
				double	zMin	= m_pDEM->asDouble(x, y);

				if( bNeighbours )	// assure lower than neighourhood
				{
					for(int i=0; i<8; i++)
					{
						int	ix	= Get_xTo(i, x);
						int	iy	= Get_yTo(i, y);

						if( m_pDEM->is_InGrid(ix, iy) && m_pStream->is_NoData(ix, iy) && m_pDEM->asDouble(ix, iy) < zMin )
						{
							zMin	= m_pDEM->asDouble(ix, iy);
						}
					}
				}

				m_pDEM->Set_Value(x, y, zMin - m_Epsilon);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBurnIn_Streams::Burn_Trace(void)
{
	int			y;

	//-----------------------------------------------------
	CSG_Grid	Count (*Get_System(), SG_DATATYPE_Char);
	CSG_Grid	Stream(*Get_System(), SG_DATATYPE_Char);
	CSG_Grid	*pDir	= Parameters("FLOWDIR")->asGrid();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pStream->is_NoData(x, y) && !m_pDEM->is_NoData(x, y) )
			{
				int	ix, iy, i	= pDir->is_NoData(x, y) ? m_pDEM->Get_Gradient_NeighborDir(x, y) : pDir->asInt(x, y);

				Stream.Set_Value(x, y, i);

				if( i >= 0 && Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy) )
				{
					Count.Add_Value(ix, iy, 1);
				}
			}
			else
			{
				Stream.Set_NoData(x, y);
			}
		}
	}

	m_pStream	= &Stream;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pStream->is_NoData(x, y) && Count.asInt(x, y) == 0 )
			{
				Lock_Create();

				Burn_Trace(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
void CBurnIn_Streams::Burn_Trace(int x, int y)
{
	if( !is_Locked(x, y) )
	{
		Lock_Set(x, y);

		int	ix, iy;

		if( Get_System()->Get_Neighbor_Pos(m_pStream->asInt(x, y), x, y, ix, iy) && !m_pStream->is_NoData(ix, iy) )
		{
			if( m_pDEM->asDouble(ix, iy) >= m_pDEM->asDouble(x, y) )
			{
				m_pDEM->Set_Value(ix, iy, m_pDEM->asDouble(x, y) - m_Epsilon);
			}

			Burn_Trace(ix, iy);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
