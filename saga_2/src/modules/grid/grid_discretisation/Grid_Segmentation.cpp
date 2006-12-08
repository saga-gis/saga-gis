
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Segmentation.cpp                 //
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
#include "Grid_Segmentation.h"


///////////////////////////////////////////////////////////
//														 //
//						CSegment						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSegment::CSegment(int aSegment, double aValue, int axSeed, int aySeed)
{
	iSegment		= aSegment;
	Value			= aValue;
	xSeed			= axSeed;
	ySeed			= aySeed;

	nConnects		= 0;
	maxConnects		= 10;

	Connect			= (int *)SG_Malloc(maxConnects * sizeof(int));
	Segment			= (int *)SG_Malloc(maxConnects * sizeof(int));
}

//---------------------------------------------------------
CSegment::~CSegment(void)
{
	SG_Free(Connect);
	SG_Free(Segment);
}

//---------------------------------------------------------
inline int CSegment::Get_Segment(int jSegment)
{
	int		i;

	for(i=0; i<nConnects; i++)
		if( Segment[i] == jSegment )
			return( Connect[i] );

	return(0);
}

//---------------------------------------------------------
inline void CSegment::Set_Segment(int jSegment, int jConnect)
{
	if( nConnects >= maxConnects )
	{
		maxConnects		+= 10;
		Connect			= (int *)SG_Realloc(Connect,maxConnects * sizeof(int));
		Segment			= (int *)SG_Realloc(Segment,maxConnects * sizeof(int));
	}

	Connect[nConnects]	= jConnect;
	Segment[nConnects]	= jSegment;

	nConnects++;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Segmentation::CGrid_Segmentation(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Grid Segmentation"));

	Set_Author		(_TL("Copyrights (c) 2002 by Olaf Conrad"));

	Set_Description	(_TW(
		"Segmentation with the local maximum method.\n"
	));


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Segments"),
		_TL(""),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// 3. General Parameters...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Choose if you want to segmentate either on minima or on maxima."),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Minima"),
			_TL("Maxima")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "BORDERS"		, _TL("Borders"),
		_TL("Keep the borders between segments as special values."),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		Parameters("RESULT"), "OUTPUT_TYPE", _TL("Output"),
		_TL("The values of the resultant grid can be either the seed value (e.g. the local maximum) or the enumerated segment id."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Seed Value"),
			_TL("Segment ID")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Threshold"),
		_TL("Specify a threshold value as minimum difference between neighboured segments."),
		PARAMETER_TYPE_Double
	);
}

//---------------------------------------------------------
CGrid_Segmentation::~CGrid_Segmentation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Segmentation::On_Execute(void)
{
	bool	bDown, bBorder, bWriteID;

	int		x, y;

	double	Threshold;

	//-----------------------------------------------------
	pGrid		= Parameters("INPUT")->asGrid();
	pSegments	= Parameters("RESULT")->asGrid();

	bDown		= Parameters("METHOD")->asInt() == 1;
	bBorder		= Parameters("BORDERS")->asBool();
	bWriteID	= Parameters("OUTPUT_TYPE")->asInt() == 1;
	Threshold	= Parameters("THRESHOLD")->asDouble();

	//-----------------------------------------------------
	if( !bDown )
	{
		pGrid->Invert();
	}

	//-----------------------------------------------------
	pSegments->Assign();

	//-----------------------------------------------------
	Do_Grid_Segmentation( Threshold );

	//-----------------------------------------------------
	if( bBorder )
		UnPrepareBorders();
	else
		UnPrepareNoBorders();

	//-----------------------------------------------------
	if( Segments )
	{
		if( !bWriteID )
		{
			for(y=0; y<Get_NY(); y++)
				for(x=0; x<Get_NX(); x++)
					if( pSegments->asInt(x,y) > 0 )
						pSegments->Set_Value(x,y, Segments[pSegments->asInt(x,y)-1]->Get_Value() );
		}

		for(y=0; y<nSegments; y++)
			delete(Segments[y]);

		SG_Free(Segments);
	}

	//-----------------------------------------------------
	if( !bDown )
	{
		pGrid->Invert();
		pGrid->Sort_Discard();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Segmentation::Do_Grid_Segmentation(double Threshold)
{
	int		x, y, i,
			iConnect, nConnects,
			SegmentA, SegmentB;

	long	n;

	double	aVal, bVal;

	//-----------------------------------------------------
	if( Get_Initials() )
	{
		nConnects	= -2;

		for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			pGrid->Get_Sorted(n,x,y);

			if( !Get_System()->is_InGrid(x, y, 1) )
			{
				pSegments->Set_Value(x,y,-1);
			}
			else if(pSegments->asInt(x,y) <= 0)
			{
				iConnect	= 0;
				SegmentB	= 0;

				for(i=0; i<8; i++)
				{
					SegmentA	= pSegments->asInt(Get_System()->Get_xTo(i,x),Get_System()->Get_yTo(i,y));

					if( SegmentA >0 )	// <0=Border, 0=noch undefined, >0=iSegment
					{
						if( SegmentB == 0 )
						{
							SegmentB	= SegmentA;
						}
						else if( SegmentB != SegmentA )
						{
							iConnect	= Segments[SegmentA-1]->Get_Segment(SegmentB);

							if( iConnect==0 )
							{
								aVal	= Segments[SegmentA-1]->Get_Value();
								bVal	= Segments[SegmentB-1]->Get_Value();

								if( fabs(aVal - bVal) < Threshold)
								{
									if( aVal < bVal )
									{
										Segment_Change(SegmentA,SegmentB);
									}
									else
									{
										Segment_Change(SegmentB,SegmentA);
										SegmentB	= SegmentA;
									}
								}
								else
								{
									iConnect	= nConnects--;
									Segments[SegmentA-1]->Set_Segment(SegmentA,iConnect);
									Segments[SegmentB-1]->Set_Segment(SegmentB,iConnect);
								}
							}

							break;
						}
					}
				}

				pSegments->Set_Value(x,y, iConnect ? iConnect : SegmentB);
			}
		}

		Get_Junctions();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Segmentation::Get_Initials(void)
{
	bool	bSegment;
	int		x, y, ix, iy, i;
	long	n;
	double	d;

	nSegments	= 0;
	Segments	= NULL;

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		pGrid->Get_Sorted(n,x,y);

		if( Get_System()->is_InGrid(x,y,1) )
		{
			d			= pGrid->asDouble(x,y);
			bSegment	= true;

			for(i=0; i<8; i++)
			{
				ix	= Get_System()->Get_xTo(i,x);
				iy	= Get_System()->Get_yTo(i,y);

				if( Get_System()->is_InGrid(ix,iy,1) )
				{
					if( d < pGrid->asDouble(ix,iy) )
						bSegment	= false;
				}
			}

			//---------------------------------------------
			if( bSegment )
			{
				nSegments++;
				pSegments->Set_Value(x,y, nSegments);
				Segments				= (CSegment **)SG_Realloc(Segments,nSegments * sizeof(CSegment *));
				Segments[nSegments-1]	= (CSegment  *)new CSegment(nSegments,d,x,y);
			}
		}
	}

	return( nSegments > 1 );
}

//---------------------------------------------------------
void CGrid_Segmentation::Get_Junctions(void)
{
	int		x, y, ix, iy, i,
			p, p1, p2;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pSegments->asInt(x,y) < 0 )
			{
				p1	= p2	= 0;

				for(i=0; i<8; i++)
				{
					ix	= Get_System()->Get_xTo(i,x);
					iy	= Get_System()->Get_yTo(i,y);

					p	= !Get_System()->is_InGrid(ix,iy) ? 1 : pSegments->asInt(ix,iy);

					if( p > 0 )
					{
						if(!p1)
						{
							p1	= p;
						}
						else if(p1!=p)
						{
							if(!p2)
							{
								p2	= p;
							}
							else if(p2!=p)
							{
								pSegments->Set_Value(x,y,-1);
								break;
							}
						}
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Segmentation::Segment_Change(int iFrom, int iTo)
{
	bool	bContinue;
	int		ax, ay, bx, by, x, y;

	//-----------------------------------------------------
	ax	= bx	= Segments[iFrom-1]->Get_xSeed();
	ay	= by	= Segments[iFrom-1]->Get_ySeed();

	do
	{
		bContinue	= false;

		for(x=ax; x<=bx; x++)
		{
			if(Segment_Change(x,ay,iFrom,iTo))
				bContinue		= true;

			if(Segment_Change(x,by,iFrom,iTo))
				bContinue		= true;
		}

		for(y=ay; y<=by; y++)
		{
			if(Segment_Change(ax,y,iFrom,iTo))
				bContinue		= true;

			if(Segment_Change(bx,y,iFrom,iTo))
				bContinue		= true;
		}

		if(ax>0)
			ax--;
		if(ay>0)
			ay--;
		if(bx<Get_NX()-1)
			bx++;
		if(by<Get_NY()-1)
			by++;
	}
	while( bContinue );
}

//---------------------------------------------------------
inline bool CGrid_Segmentation::Segment_Change(int x, int y, int iFrom, int iTo)
{
	if( pSegments->asInt(x,y) == iFrom )
	{
		pSegments->Set_Value(x,y,iTo);
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
void CGrid_Segmentation::UnPrepareNoBorders(void)
{
	bool	bRepeat, bRoger;
	int		x, y, i, ix, iy, nRepeats;

	nRepeats	= 0;

	do
	{
		bRepeat	= false;

		for(y=0; y<Get_NY(); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( pSegments->asInt(x,y) < 0 )
				{
					bRoger	= false;

					for(i=0; i<8; i++)
					{
						ix	= Get_System()->Get_xTo(i,x);
						iy	= Get_System()->Get_yTo(i,y);

						if(Get_System()->is_InGrid(ix,iy))
						{
							if( pSegments->asInt(ix,iy) > 0 )
							{
								pSegments->Set_Value(x,y,pSegments->asInt(ix,iy));
								bRoger	= true;
								break;
							}
						}
					}

					if(!bRoger)
						bRepeat	= true;
				}
			}
		}
	}
	while( bRepeat && nRepeats++ < 10 );
}

//---------------------------------------------------------
void CGrid_Segmentation::UnPrepareBorders(void)
{
	bool	bKill;
	int		x, y, i, ix, iy, z;

	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pSegments->asInt(x,y) < 0 )
			{
				z		= -1;
				bKill	= true;

				for(i=0; i<8; i++)
				{
					ix	= Get_System()->Get_xTo(i,x);
					iy	= Get_System()->Get_yTo(i,y);

					if( Get_System()->is_InGrid(ix,iy) )
					{
						if( pSegments->asInt(ix,iy) > 0 )
						{
							if( z < 0 )
							{
								z	= pSegments->asInt(ix,iy);
							}
							else if( z != pSegments->asInt(ix,iy) )
							{
								bKill	= false;
								break;
							}
						}
					}
				}

				pSegments->Set_Value(x,y, z > 0 && bKill ? z : -1);
			}
		}
	}
}
