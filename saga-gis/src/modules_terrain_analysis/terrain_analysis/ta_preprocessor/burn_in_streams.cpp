/**********************************************************
 * Version $Id: burn_in_streams.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
		""
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

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
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
bool CBurnIn_Streams::On_Execute(void)
{
	double		Epsilon;
	CSG_Grid	*pDEM, *pBurn, *pStream;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM")		->asGrid();
	pBurn		= Parameters("BURN")	->asGrid();
	pStream		= Parameters("STREAM")	->asGrid();
	Epsilon		= Parameters("EPSILON")	->asDouble();

	//-----------------------------------------------------
	if( pBurn )
	{
		pBurn	->Assign(pDEM);
		pBurn	->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pDEM->Get_Name(), _TL("Burned Streams")));
	}
	else
	{
		pBurn	= pDEM;
	}

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	// simple
		{
			for(int y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( !pStream->is_NoData(x, y) && !pBurn->is_NoData(x, y) )
					{
						pBurn->Add_Value(x, y, -Epsilon);
					}
				}
			}
		}
		break;

	case 1:	// assure lower than neighourhood
		{
			for(int y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( !pStream->is_NoData(x, y) && !pBurn->is_NoData(x, y) )
					{
						int		iMin	= -1;
						double	zMin;

						for(int i=0; i<8; i++)
						{
							int	ix	= Get_xTo(i, x);
							int	iy	= Get_yTo(i, y);

							if( pBurn->is_InGrid(ix, iy) && pStream->is_NoData(ix, iy) )
							{
								if( iMin < 0 || pBurn->asDouble(ix, iy) < zMin )
								{
									zMin	= pBurn->asDouble(ix, iy);
								}
							}
						}

						if( iMin < 0 )
						{
							pBurn->Add_Value(x, y, -Epsilon);
						}
						else
						{
							pBurn->Set_Value(x, y, zMin - Epsilon);
						}
					}
				}
			}
		}
		break;

	case 3:
		{
		}
		break;
	}

	//-----------------------------------------------------
	if( Parameters("BURN")->asGrid() == NULL )
	{
		DataObject_Update(pBurn);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
