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
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  ChannelNetwork.cpp                   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "ChannelNetwork.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChannelNetwork::CChannelNetwork(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Channel Network"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"This tool derives a channel network based on gridded digital elevation data.\n"
		"Use the initiation options to determine under which conditions channels shall start.\n"
		"\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Flow Direction"),
		_TW(
		"An optional grid that provides information about flow directions. \n"
		"Values between 1 to 8 force the flow of a cell to be given to one its adjacent neighbor cells "
		"(1->NE, 2->E, 3->SE, 4->S, 5->SW, 6->W, 7->NW, 8->N). "
		"In case of other values the algorithm will use its own routing scheme. \n"
		"This option is in particular useful to supply the algorithm with routes that lead the flow through closed depression. "),
		PARAMETER_INPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "CHNLNTWRK"	, _TL("Channel Network"),
		_TW("If a cell is part of a channel its value equals the channel order. "
		"Otherwise the cell is marked as no-data."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHNLROUTE"	, _TL("Channel Direction"),
		_TW("If a cell is part of a channel then its value shows the flow direction of the channel "
		"(1->NE, 2->E, 3->SE, 4->S, 5->SW, 6->W, 7->NW, 8->N). "
		"Otherwise the cell is marked as no-data."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Channel Network"),
		_TL("This shapes layer will contain the resulting channel network in vector format (lines)."),
		PARAMETER_OUTPUT		, SHAPE_TYPE_Line
	);


	//-----------------------------------------------------
	// Initiation...

	pNode	= Parameters.Add_Grid(
		NULL	, "INIT_GRID"	, _TL("Initiation Grid"),
		_TW("Dependent on the chosen 'Initiation Type' and 'Initiation Threshold' "
		"the values of this grid control where a channel is initiated."),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		pNode	, "INIT_METHOD"	, _TL("Initiation Type"),
		_TL("Options:\n - Less than\n - Equals\n - Greater than\nControls under which condition a channel is initiated."),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Less than"),
			_TL("Equals"),
			_TL("Greater than")
		), 2
	);

	Parameters.Add_Value(
		pNode	, "INIT_VALUE"	, _TL("Initiation Threshold"),
		_TL("Dependent on the chosen 'Initiation Grid' and 'Initiation Type' this value controls under which condition a channel is initiated."),
		PARAMETER_TYPE_Double	, 0.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid(
		NULL	, "DIV_GRID"	, _TL("Divergence"),
		_TL("Tracing: Convergence"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "DIV_CELLS"	, _TL("Tracing: Max. Divergence"),
		_TL("Tracing: Stop after x cells with divergent flow"),
		PARAMETER_TYPE_Int		, 5, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "TRACE_WEIGHT", _TL("Tracing: Weight"),
		_TL("Tracing: Weight"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "MINLEN"		, _TL("Min. Segment Length"),
		_TL("Minimum Segment Length (Cells)"),
		PARAMETER_TYPE_Int		, 10
	);
}

//---------------------------------------------------------
CChannelNetwork::~CChannelNetwork(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork::On_Execute(void)
{
	int			x, y, ID, Trace_Method, Init_Method;
	sLong		n;
	double		Init_Threshold;
	CSG_Grid	*Trace_pRoute, *Trace_pWeight, *Init_pGrid;


	//-----------------------------------------------------
	pDTM				= Parameters("ELEVATION")->asGrid();
	pConvergence		= Parameters("DIV_GRID" )->asGrid();

	pChannels			= Parameters("CHNLNTWRK")->asGrid();
	pChannelRoute		= Parameters("CHNLROUTE")->asGrid();
	pShapes				= Parameters("SHAPES"   )->asShapes();

	minLength			= Parameters("MINLEN"   )->asInt();

	maxDivCells			= Parameters("DIV_GRID" )->asGrid() ? Parameters("DIV_CELLS")->asInt() : -1;

	if( !pDTM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	// 1. Flow Direction...

	Process_Set_Text(_TL("Channel Network: Pass 1"));

	pChannels->Assign();

	Trace_pRoute		= Parameters("SINKROUTE")	->asGrid();
	Trace_pWeight		= Parameters("TRACE_WEIGHT")->asGrid();
	Trace_Method		= Trace_pWeight ? 1 : 0;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( Trace_pRoute && (ID = Trace_pRoute->asChar(x, y)) >= 1 && ID <= 8 )
			{
				pChannels->Set_Value(x, y, ID);
			}
			else
			{
				switch( Trace_Method )
				{
				default:
					Set_Route_Standard(x, y);
					break;

				case 1:
					Set_Route_Weighted(x, y, Trace_pWeight, 0.0);
					break;
				}
			}
		}
	}


	//-----------------------------------------------------
	// 2. Initiation...

	Process_Set_Text(_TL("Channel Network: Pass 2"));

	pStart				= SG_Create_Grid(pDTM, SG_DATATYPE_Char);
	Init_pGrid			= Parameters("INIT_GRID")	->asGrid();
	Init_Method			= Parameters("INIT_METHOD")	->asInt();
	Init_Threshold		= Parameters("INIT_VALUE")	->asDouble();

	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		switch( Init_Method )
		{
		case 0:
			if( Init_pGrid->asDouble(n) <= Init_Threshold )
				pStart->Set_Value(n, 1);
			break;

		case 1:
			if( Init_pGrid->asDouble(n) == Init_Threshold )
				pStart->Set_Value(n, 1);
			break;

		case 2:
			if( Init_pGrid->asDouble(n) >= Init_Threshold )
				pStart->Set_Value(n, 1);
			break;
		}
	}


	//-----------------------------------------------------
	// 3. Trace Channel Routes...

	Process_Set_Text(_TL("Channel Network: Pass 3"));

	pChannelRoute->Assign();

	Direction			= NULL;
	Direction_Buffer	= 0;

	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( pDTM->Get_Sorted(n,x,y) )
		{
			Set_Channel_Route(x,y);
		}
	}

	if( Direction )
	{
		SG_Free( Direction );
	}

	pChannels->Assign();

	delete(pStart);


	//-----------------------------------------------------
	Process_Set_Text(_TL("Channel Network: Pass 4"));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Set_Channel_Order(x,y);
		}
	}


	//-----------------------------------------------------
	Process_Set_Text(_TL("Channel Network: Pass 5"));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Set_Channel_Mouth(x,y);
		}
	}


	//-----------------------------------------------------
	if( pShapes )
	{
		Process_Set_Text(_TL("Channel Network: Pass 6"));

		pShapes->Create(SHAPE_TYPE_Line, _TL("Channel Network"));

		pShapes->Add_Field("SegmentID"	,SG_DATATYPE_Int);
		pShapes->Add_Field("Order"		,SG_DATATYPE_Int);
		pShapes->Add_Field("Length"		,SG_DATATYPE_Double);

		Lock_Create();

		for(y=0, ID=1; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				Set_Vector(x, y, ID++);
			}
		}

		Lock_Destroy();
	}


	//-----------------------------------------------------
	for(n=0; n<Get_NCells(); n++)
	{
		if( pChannels->asInt(n) == 0 )
		{
			pChannels->Set_NoData(n);
			pChannelRoute->Set_NoData(n);
		}
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
void CChannelNetwork::Set_Route_Standard(int x, int y)
{
	int		i, ix, iy, iMin;

	double	z, dz, dzMin;

	z		= pDTM->asDouble(x,y);
	iMin	= 0;

	for(i=1; i<=8; i++)
	{
		ix		= Get_xTo(i,x);
		iy		= Get_yTo(i,y);

		if( !pDTM->is_InGrid(ix,iy) )
		{
			iMin	= i;
			break;
		}
		else
		{
			dz		= (z - pDTM->asDouble(ix,iy)) / Get_Length(i);

			if( iMin <= 0 || dzMin < dz )
			{
				iMin	= i;
				dzMin	= dz;
			}
		}
	}

	pChannels->Set_Value(x,y, iMin );
}

//---------------------------------------------------------
void CChannelNetwork::Set_Route_Weighted(int x, int y, CSG_Grid *pWeight, double Weight_Threshold)
{
	bool	bMin;

	int		i, ix, iy, iMin;

	double	z, dz, dzMin, w, wMin;

	z		= pDTM->asDouble(x,y);
	iMin	= 0;

	for(i=1; i<=8; i++)
	{
		ix		= Get_xTo(i,x);
		iy		= Get_yTo(i,y);

		if( !pDTM->is_InGrid(ix, iy) )
		{
			iMin	= i;
			break;
		}
		else
		{
			dz		= (z - pDTM->asDouble(ix,iy)) / Get_Length(i);

			if( dz > 0 )
			{
				bMin	= false;
				w		= pWeight->asDouble(ix,iy);

				if(	iMin <= 0 )
				{
					bMin	= true;
				}
				else if( Weight_Threshold > 0 )
				{
					if( w > Weight_Threshold && wMin > Weight_Threshold )
					{
						if( dzMin < dz )
						{
							bMin	= true;
						}
					}
					else if( wMin < w )
					{
						bMin	= true;
					}
				}
				else if( wMin < w )
				{
					bMin	= true;
				}

				if( bMin )
				{
					iMin	= i;
					wMin	= w;
					dzMin	= dz;
				}
			}
		}
	}

	pChannels->Set_Value(x,y, iMin );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CChannelNetwork::Set_Channel_Route(int x, int y)
{
	const int	BUFFER_GROWSIZE	= 256;

	int		xStart, yStart, i, ix, iy, goDir, m, n, nDiv;

	double	z, dz, dzMin, Length;

	//-----------------------------------------------------
	if( pStart->asChar(x,y) && !pChannelRoute->asChar(x,y) )
	{
		Lock_Create();

		n		= 0;
		nDiv	= 0;
		Length	= 0;
		xStart	= x;
		yStart	= y;

		do
		{
			//---------------------------------------------
			// 1. Divergence ?!...

			if( pConvergence )
			{
				if( pConvergence->asDouble(x,y) > -1.0 )
				{
					nDiv++;
				}
				else
				{
					nDiv	= 0;
				}
			}

			if( pConvergence && nDiv > maxDivCells )
			{
				goDir	= -1;
			}
			else
			{
				//-----------------------------------------
				// 2. Is there any channel around ?!...

				goDir	= 0;
				z		= pDTM->asDouble(x,y);

				for(i=1; i<=8; i++)
				{
					ix		= Get_xTo(i,x);
					iy		= Get_yTo(i,y);

					if( pDTM->is_InGrid(ix,iy) && !is_Locked(ix,iy) && pChannelRoute->asChar(ix,iy) )
					{
						dz		= (z - pDTM->asDouble(ix,iy)) / Get_Length(i);

						if( goDir <= 0 || dzMin < dz )
						{
							goDir	= i;
							dzMin	= dz;
						}
					}
				}

				if( goDir <= 0 )	// ...if not then go as usual...
				{
					goDir	= pChannels->asInt(x,y);
				}


				//-----------------------------------------
				// 3. Go to Drainage Direction !...

				if(	goDir > 0 )
				{
					Lock_Set(x,y);

					x		= Get_xTo(goDir,x);
					y		= Get_yTo(goDir,y);

					Length	+= Get_UnitLength(goDir);

					if( n >= Direction_Buffer )
					{
						Direction_Buffer	+= BUFFER_GROWSIZE;
						Direction			= (int *)SG_Realloc(Direction, Direction_Buffer * sizeof(int));
					}

					Direction[n++]	= goDir;
				}
			}
		}
		while( goDir > 0 && pDTM->is_InGrid(x,y) && !is_Locked(x,y) && !pChannelRoute->asChar(x,y) );


		//-------------------------------------------------
		if( Length >= minLength )
		{
			x	= xStart;
			y	= yStart;

			if( goDir < 0 )
			{
				n	-= nDiv;
			}

			for(m=0; m<n; m++)
			{
				goDir	= Direction[m];

				pChannelRoute->Set_Value(x,y,goDir);

				for(i=0; i<8; i++)	// Don't start new channels beside existing ones...
				{
					ix		= Get_xTo(i,x);
					iy		= Get_yTo(i,y);

					if( pDTM->is_InGrid(ix,iy) )
					{
						pStart->Set_Value(ix,iy,0);
					}
				}

				x	= Get_xTo(goDir,x);
				y	= Get_yTo(goDir,y);
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
void CChannelNetwork::Set_Channel_Order(int x, int y)
{
	int		i, ix, iy, j, n;

	//-----------------------------------------------------
	if( pChannelRoute->asChar(x,y) > 0 )
	{
		for(i=n=0, j=4; i<8; i++, j=(j+1)%8)
		{
			ix	= Get_xTo(i,x);
			iy	= Get_yTo(i,y);

			if( pDTM->is_InGrid(ix,iy) && pChannelRoute->asChar(ix,iy) && j == pChannelRoute->asChar(ix,iy) % 8 )
			{
				n++;
			}
		}

		//-----------------------------------------------------
		if( n == 0 )
		{
			Lock_Create();

			do
			{
				Lock_Set(x,y);

				pChannels->Add_Value(x,y, 1 );

				i	= pChannelRoute->asChar(x,y);

				if( i > 0 )
				{
					x	= Get_xTo(i,x);
					y	= Get_yTo(i,y);
				}
			}
			while( pDTM->is_InGrid(x,y) && i > 0 && !is_Locked(x,y) );
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork::Set_Channel_Mouth(int x, int y)
{
	int		Order, goDir, ix, iy;

	Order	= pChannels->asInt(x,y);

	if( Order > 0 )
	{
		goDir	= pChannelRoute->asChar(x,y);

		if( goDir > 0 )
		{
			ix	= Get_xTo(goDir,x);
			iy	= Get_yTo(goDir,y);

			if( !pDTM->is_InGrid(ix,iy) || (pChannels->asInt(ix,iy) > 0 && Order != pChannels->asInt(ix,iy)) )
			{
				pChannels->Set_Value(x,y, -1 );
			}
		}
		else
		{
			pChannels->Set_Value(x,y, -1 );
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CChannelNetwork::Set_Vector(int x, int y, int ID)
{
	bool	bContinue;

	int		i, ix, iy, j, Order;

	double	xMin, yMin, Length;

	CSG_Shape	*pShape;

	//-----------------------------------------------------
	if( (Order = pChannels->asInt(x,y)) > 0 )
	{
		bContinue	= true;

		for(i=0, j=4; i<8 && bContinue; i++, j=(j+1)%8)
		{
			ix	= Get_xTo(i,x);
			iy	= Get_yTo(i,y);

			if(	pDTM->is_InGrid(ix,iy)
			&&	pChannels->asInt(ix,iy) == Order
			&&	pChannelRoute->asChar(ix,iy) && j == pChannelRoute->asChar(ix,iy) % 8 )
			{
				bContinue	= false;
			}
		}

		//-------------------------------------------------
		if( bContinue )	// Startpunkt gefunden...
		{
			xMin	= pDTM->Get_XMin(),
			yMin	= pDTM->Get_YMin();
			ID		= 1;

			pShape	= pShapes->Add_Shape();

			do
			{
				bContinue	= false;
				Length		= 0;

				Lock_Set(x,y);

				pShape->Add_Point(xMin + x * Get_Cellsize(), yMin + y * Get_Cellsize());

				i	= pChannelRoute->asChar(x,y);

				if( i > 0 )
				{
					ix		= Get_xTo(i,x);
					iy		= Get_yTo(i,y);

					Length	+= Get_Length(i);

					if( pDTM->is_InGrid(ix,iy) )
					{
						if(	!is_Locked(ix,iy)
						&&	(	pChannels->asInt(ix,iy) == Order
							||	pChannels->asInt(ix,iy) < 0) )
						{
							x			= ix;
							y			= iy;
							bContinue	= true;
						}
						else
						{
							pShape->Add_Point(xMin + ix * Get_Cellsize(), yMin + iy * Get_Cellsize());
						}
					}
				}
			}
			while( bContinue );

			pShape->Set_Value(0, ID );
			pShape->Set_Value(1, Order );
			pShape->Set_Value(2, Length );
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
