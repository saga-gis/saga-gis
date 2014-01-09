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
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               watershed_segmentation.cpp              //
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
#include "watershed_segmentation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	SEED_X	= 0,
	SEED_Y,
	SEED_Z,
	SEED_ID,
	SEED_JOIN
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWatershed_Segmentation::CWatershed_Segmentation(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Watershed Segmentation"));

	Set_Author		(SG_T("O.Conrad (c) 2002"));

	Set_Description	(_TW(
		"Watershed segmentation."
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "SEGMENTS"	, _TL("Segments"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "SEEDS"		, _TL("Seed Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid_Output(
		NULL	, "BORDERS"		, _TL("Borders"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		pNode	, "OUTPUT"		, _TL("Output"),
		_TL("The values of the resultant grid can be either the seed value (e.g. the local maximum) or the enumerated segment id."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Seed Value"),
			_TL("Segment ID")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "DOWN"		, _TL("Method"),
		_TL("Choose if you want to segmentate either on minima or on maxima."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Minima"),
			_TL("Maxima")
		), 1
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "JOIN"		, _TL("Join Segments based on Threshold Value"),
		_TL("Join segments based on threshold value."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("do not join"),
			_TL("seed to saddle difference"),
			_TL("seeds difference")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "THRESHOLD"	, _TL("Threshold"),
		_TL("Specify a threshold value as minimum difference between neighboured segments."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "EDGE"		, _TL("Allow Edge Pixels to be Seeds"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "BBORDERS"	, _TL("Borders"),
		_TL("Create borders between segments as new grid."),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWatershed_Segmentation::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrid		= Parameters("GRID")		->asGrid();
	m_pSeeds	= Parameters("SEEDS")		->asShapes();
	m_pSegments	= Parameters("SEGMENTS")	->asGrid();
	m_bDown		= Parameters("DOWN")		->asInt() == 1;

	//-----------------------------------------------------
	m_pSeeds->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), m_pGrid->Get_Name(), _TL("Seeds")));

	m_pSeeds->Add_Field(SG_T("XCELL")	, SG_DATATYPE_Int);		// SEED_X
	m_pSeeds->Add_Field(SG_T("YCELL")	, SG_DATATYPE_Int);		// SEED_Y
	m_pSeeds->Add_Field(SG_T("VALUE")	, SG_DATATYPE_Double);	// SEED_Z
	m_pSeeds->Add_Field(SG_T("ID")		, SG_DATATYPE_Int);		// SEED_ID
	m_pSeeds->Add_Field(SG_T("ID_JOIN")	, SG_DATATYPE_Int);		// SEED_JOIN

	//-----------------------------------------------------
	m_pSegments->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pGrid->Get_Name(), _TL("Segments")));
	m_pSegments->Set_NoData_Value(-999999.0);

	m_Dir.Create(*Get_System(), SG_DATATYPE_Char);

	//-----------------------------------------------------
	if( !Get_Seeds() )
	{
		Message_Add(_TL("no seed points identified"));

		return( false );
	}

	//-----------------------------------------------------
	Get_Segments();

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asInt() == 0 )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				int	ID	= m_pSegments->asInt(x, y);

				if( ID >= 0 )
				{
					m_pSegments->Set_Value(x, y, m_pSeeds->Get_Shape(ID)->asDouble(SEED_Z));
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("BBORDERS")->asBool() )
	{
		Get_Borders();
	}

	//-----------------------------------------------------
	m_Dir.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWatershed_Segmentation::Get_Seeds(void)
{
	Process_Set_Text(_TL("Seeds"));

	bool	bEdge, bEdge_Seeds;
	int		x, y, i, ix, iy, iMax;
	double	z, dz, dzMax;

	//-----------------------------------------------------
	bEdge_Seeds	= Parameters("EDGE")	->asBool();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !m_pGrid->is_InGrid(x, y) )
			{
				m_Dir.Set_Value(x, y, -1);
				m_pSegments->Set_NoData(x, y);
			}
			else
			{
				for(i=0, iMax=-1, dzMax=0.0, z=m_pGrid->asDouble(x, y), bEdge=false; i<8; i++)
				{
					if( !m_pGrid->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) )
					{
						bEdge	= true;
					}
					else if( dzMax < (dz = (m_bDown ? m_pGrid->asDouble(ix, iy) - z : z - m_pGrid->asDouble(ix, iy)) / Get_Length(i)) )
					{
						dzMax	= dz;
						iMax	= i;
					}
				}

				//---------------------------------------------
				m_Dir.Set_Value(x, y, iMax);

				if( iMax < 0 && (bEdge_Seeds || !bEdge) )
				{
					int			ID	= m_pSeeds->Get_Count();

					CSG_Shape	*pSeed	= m_pSeeds->Add_Shape();

					pSeed->Set_Point(Get_System()->Get_Grid_to_World(x, y), 0);

					pSeed->Set_Value(SEED_X		, x);
					pSeed->Set_Value(SEED_Y		, y);
					pSeed->Set_Value(SEED_Z		, z);
					pSeed->Set_Value(SEED_ID	, ID);
					pSeed->Set_Value(SEED_JOIN	, -1);
				
					m_pSegments->Set_Value(x, y, ID);
				}
				else
				{
					m_pSegments->Set_Value(x, y, -1);
				}
			}
		}
	}

	return( m_pSeeds->Get_Count() > 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWatershed_Segmentation::Get_Segments(void)
{
	Process_Set_Text(_TL("Segments"));

	double	Threshold	= Parameters("THRESHOLD")->asDouble();
	int		Join		= Threshold > 0.0 ? Parameters("JOIN")->asInt() : 0;

	//-----------------------------------------------------
	for(long n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)	
	{
		int		x, y, i, ID, iID;

		if( m_pGrid->Get_Sorted(n, x, y, m_bDown) && (i = m_Dir.asInt(x, y)) >= 0 )
		{
			m_pSegments->Set_Value(x, y, ID = m_pSegments->asInt(Get_xTo(i, x), Get_yTo(i, y)));

			if( Join != 0 && ID >= 0 )
			{
				double	z	= m_pGrid->asDouble(x, y);

				for(int i=0; i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( m_pSegments->is_InGrid(ix, iy) && (iID = m_pSegments->asInt(ix, iy)) >= 0 )	// Border < 0, Segment >= 0
					{
						if( ID != iID )
						{
							bool	bJoin;

							if( Join == 1 )
							{
								bJoin	=  (Threshold >= fabs(m_pSeeds->Get_Shape(iID)->asDouble(SEED_Z) - z))
										|| (Threshold >= fabs(m_pSeeds->Get_Shape( ID)->asDouble(SEED_Z) - z));
							}
							else
							{
								bJoin	=  Threshold >= fabs(m_pSeeds->Get_Shape(iID)->asDouble(SEED_Z) - m_pSeeds->Get_Shape(ID)->asDouble(SEED_Z));
							}

							if( bJoin )
							{
								if(	(m_bDown == true  && m_pSeeds->Get_Shape(iID)->asDouble(SEED_Z) < m_pSeeds->Get_Shape(ID)->asDouble(SEED_Z))
								||	(m_bDown == false && m_pSeeds->Get_Shape(iID)->asDouble(SEED_Z) > m_pSeeds->Get_Shape(ID)->asDouble(SEED_Z)) )
								{
									Segment_Change(iID, ID);
								}
								else
								{
									Segment_Change(ID, iID);

									ID	= iID;
								}
							}
						}
					}
				}
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
bool CWatershed_Segmentation::Segment_Change(int ID, int new_ID)
{
	bool	bContinue;
	int		ax, ay, bx, by;

	//-----------------------------------------------------
	CSG_Shape	*pSeed	= m_pSeeds->Get_Shape(ID);

	pSeed->Set_Value(SEED_JOIN, new_ID);

	ax	= bx	= pSeed->asInt(SEED_X);
	ay	= by	= pSeed->asInt(SEED_Y);

	do
	{
		bContinue	= false;

		for(int x=ax; x<=bx; x++)
		{
			if( m_pSegments->asInt( x, ay) == ID )
			{
				m_pSegments->Set_Value( x, ay, new_ID);

				bContinue	= true;
			}

			if( m_pSegments->asInt( x, by) == ID )
			{
				m_pSegments->Set_Value( x, by, new_ID);

				bContinue	= true;
			}
		}

		for(int y=ay; y<=by; y++)
		{
			if( m_pSegments->asInt(ax,  y) == ID )
			{
				m_pSegments->Set_Value(ax,  y, new_ID);

				bContinue	= true;
			}

			if( m_pSegments->asInt(bx,  y) == ID )
			{
				m_pSegments->Set_Value(bx,  y, new_ID);

				bContinue	= true;
			}
		}

		if( ax > 0 )			ax--;
		if( ay > 0 )			ay--;
		if( bx < Get_NX() - 1 )	bx++;
		if( by < Get_NY() - 1 )	by++;
	}
	while( bContinue );

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWatershed_Segmentation::Get_Borders(void)
{
	Process_Set_Text(_TL("Borders"));

	CSG_Grid	*pBorders	= SG_Create_Grid(SG_DATATYPE_Byte, Get_NX() + 2, Get_NY() + 2, Get_Cellsize(), Get_XMin() - 0.5 * Get_Cellsize(), Get_YMin() - 0.5 * Get_Cellsize());

	pBorders->Set_NoData_Value(0);

	Parameters("BORDERS")->Set_Value(pBorders);

	for(int y=0, yy=1; yy<Get_NY() && Set_Progress(yy); y++, yy++)
	{
		for(int x=0, xx=1; xx<Get_NX(); x++, xx++)
		{
			int		id	= m_pSegments->asInt(x, y);

			if( id != m_pSegments->asInt(xx,  y) )
			{
				pBorders->Set_Value(xx,  y, 1);
			}

			if( id != m_pSegments->asInt( x, yy) )
			{
				pBorders->Set_Value( x, yy, 1);
			}

			if( id != m_pSegments->asInt(xx, yy) )
			{
				pBorders->Set_Value(xx, yy, 1);
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
