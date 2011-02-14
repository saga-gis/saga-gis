
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
//                 Grid_Skeletonize.cpp                  //
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
#include "skeletonization.h"

//---------------------------------------------------------
#define	skNE			1
#define skJA			2

#define SEGMENT_END		1
#define SEGMENT_NODE	2
#define SEGMENT_LOCKED	3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSkeletonization::CSkeletonization(void)
{
	Set_Name		(_TL("Grid Skeletonization"));

	Set_Author		(SG_T("O.Conrad (c) 2002"));

	Set_Description	(_TW(
		"Simple skeletonisation methods for grids.\n"
	));

	Parameters.Add_Grid(
		NULL, "INPUT"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"			, _TL("Skeleton"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Shapes(
		NULL, "VECTOR"			, _TL("Skeleton"),
		_TL(""), PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL, "METHOD"			, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Standard"),
			_TL("Hilditch's Algorithm"),
			_TL("Channel Skeleton")
		), 0
	);

	Parameters.Add_Choice(
		NULL, "INIT_METHOD"		, _TL("Initialisation"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Less than"),
			_TL("Greater than") 
		),1
	);

	Parameters.Add_Value(
		NULL, "INIT_THRESHOLD"	, _TL("Threshold (Init.)"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	Parameters.Add_Value(
		NULL, "CONVERGENCE"		, _TL("Convergence"),
		_TL(""),
		PARAMETER_TYPE_Int, 3
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSkeletonization::On_Execute(void)
{
	int			n, Initiation;
	double		Threshold;
	CSG_Grid	*pInput;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT")			->asGrid();
	pResult		= Parameters("RESULT")			->asGrid();

	Initiation	= Parameters("INIT_METHOD")		->asInt();
	Threshold	= Parameters("INIT_THRESHOLD")	->asDouble();

	DataObject_Set_Colors(pResult, 3, SG_COLORS_BLACK_WHITE, true);
	pResult->Assign(0.0);

	//-----------------------------------------------------
	for(n=0; n<Get_NCells(); n++)
	{
		switch( Initiation )
		{
		case 0: default:
			if( pInput->asDouble(n) < Threshold )
			{
				pResult->Set_Value(n, 1);
			}
			break;

		case 1:
			if( pInput->asDouble(n) > Threshold )
			{
				pResult->Set_Value(n, 1);
			}
			break;
		}
	}

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	default:
		Standard_Execute();
		break;

	case 1:
		Hilditch_Execute();
		break;

	case 2:
		SK_Execute();
		break;
	}

	//-------------------------------------------------
	if( Parameters("VECTOR")->asShapes() )
	{
		Vectorize(Parameters("VECTOR")->asShapes());
	}

	if( 1 )
	{
		for(n=0; n<Get_NCells(); n++)
		{
			switch( Initiation )
			{
			case 0: default:
				if( pInput->asDouble(n) < Threshold )
				{
					pResult->Add_Value(n, 1);
				}
				break;

			case 1:
				if( pInput->asDouble(n) > Threshold )
				{
					pResult->Add_Value(n, 1);
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Helpers							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSkeletonization::Get_Neighbours(int x, int y, CSG_Grid *pGrid, bool Neighbours[8])
{
	int		i, ix, iy, nNeighbours;

	nNeighbours	= 0;

	for(i=0; i<8; i++)
	{
		ix	= Get_System()->Get_xTo(i, x);
		iy	= Get_System()->Get_yTo(i, y);

		if( pGrid->is_InGrid(ix, iy) && pGrid->asByte(ix, iy) )
		{
			Neighbours[i]	= true;

			nNeighbours++;
		}
		else
		{
			Neighbours[i]	= false;
		}
	}

	return( nNeighbours );
}


///////////////////////////////////////////////////////////
//														 //
//					Vectorization						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSkeletonization::Vectorize(CSG_Shapes *pShapes)
{
	bool	z[8], bPrev;

	int		x, y, i, ix, iy, n, nSegments;

	double	xMin, yMin, dx, dy;

	CSG_Shape	*pShape;

	//-----------------------------------------------------
	pShapes->Create(SHAPE_TYPE_Line, _TL("Skeleton"));
	pShapes->Add_Field("ID", SG_DATATYPE_Int);

	Lock_Create();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pResult->asByte(x, y) )
			{
				n	= Get_Neighbours(x, y, pResult, z);
				
				if( n == 1 )
				{
					Lock_Set(x, y, SEGMENT_END);
				}
				else if( n > 1 )
				{
					n	= 0;

					for(i=0, bPrev=z[7]; i<8; i++)
					{
						if( bPrev == false && z[i] == true )
						{
							n++;
						}

						bPrev	= z[i];
					}

					if( n > 2 )
					{
						Lock_Set(x, y, SEGMENT_NODE);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	nSegments	= 0;

	for(y=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		dx			= pResult->Get_Cellsize();
		xMin		= pResult->Get_XMin();// + 0.5 * dx;
		dy			= pResult->Get_Cellsize();
		yMin		= pResult->Get_YMin();// + 0.5 * dy;

		for(x=0; x<Get_NX(); x++)
		{
			if( Lock_Get(x, y) == SEGMENT_NODE || Lock_Get(x, y) == SEGMENT_END )
			{
				Lock_Set(x, y, SEGMENT_LOCKED);

				for(i=0; i<8; i++)
				{
					ix	= Get_System()->Get_xTo(i, x);
					iy	= Get_System()->Get_yTo(i, y);

					if( pResult->is_InGrid(ix, iy) && pResult->asByte(ix, iy) && !Lock_Get(ix, iy) )
					{
						pShape	= pShapes->Add_Shape();
						pShape->Set_Value(0, ++nSegments);
						pShape->Add_Point(xMin + dx * (double)x, yMin + dy * (double)y);

						Vectorize_Trace(ix, iy, pShape);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	Lock_Destroy();

	Message_Dlg(CSG_String::Format(SG_T("%d %s\n"), nSegments, _TL("segments identified")), Get_Name());

	return( nSegments );
}

//---------------------------------------------------------
bool CSkeletonization::Vectorize_Trace(int x, int y, CSG_Shape *pShape)
{
	bool	bContinue;

	int		i, ix, iy, iNext;

	double	xMin, yMin, dx, dy;

	//-----------------------------------------------------
	dx			= pResult->Get_Cellsize();
	xMin		= pResult->Get_XMin();// + 0.5 * dx;
	dy			= pResult->Get_Cellsize();
	yMin		= pResult->Get_YMin();// + 0.5 * dy;

	bContinue	= true;

	do
	{
		pShape->Add_Point(xMin + dx * (double)x, yMin + dy * (double)y);

		if( Lock_Get(x, y) == SEGMENT_NODE || Lock_Get(x, y) == SEGMENT_END )
		{
			bContinue	= false;
		}
		else
		{
			Lock_Set(x, y, SEGMENT_LOCKED);

			iNext	= -1;

			for(i=0; i<8; i+=2)
			{
				ix	= Get_System()->Get_xTo(i, x);
				iy	= Get_System()->Get_yTo(i, y);

				if( pResult->is_InGrid(ix, iy) && pResult->asByte(ix, iy) && Lock_Get(ix, iy) != SEGMENT_LOCKED )
				{
					iNext	= i;

					if( Lock_Get(ix, iy) == SEGMENT_NODE )
					{
						break;
					}
				}
			}

			if( iNext < 0 )
			{
				for(i=1; i<8; i+=2)
				{
					ix	= Get_System()->Get_xTo(i, x);
					iy	= Get_System()->Get_yTo(i, y);

					if( pResult->is_InGrid(ix, iy) && pResult->asByte(ix, iy) && Lock_Get(ix, iy) != SEGMENT_LOCKED )
					{
						iNext	= i;

						if( Lock_Get(ix, iy) == SEGMENT_NODE )
						{
							break;
						}
					}
				}
			}

			if( iNext < 0 )
			{
				bContinue	= false;
			}
			else
			{
				x	+= Get_System()->Get_xTo(iNext);
				y	+= Get_System()->Get_yTo(iNext);
			}
		}
	}
	while( bContinue );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Standard						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSkeletonization::Standard_Execute(void)
{
	int		i, nChanges;

	CSG_Grid	*pPrev, *pNext, *pTemp;

	//-----------------------------------------------------
	pPrev		= pResult;
	pNext		= SG_Create_Grid(pPrev);

	//-----------------------------------------------------
	do
	{
		DataObject_Update(pResult, 0, 1, true);

		nChanges	= 0;

		for(i=0; i<8; i++)
		{
			nChanges	+= Standard_Step(i, pPrev, pNext);

			pTemp		= pPrev;
			pPrev		= pNext;
			pNext		= pTemp;
		}
	}
	while( nChanges > 0 && Process_Get_Okay(true) );

	//-----------------------------------------------------
	if( pNext == pResult )
	{
		delete(pPrev);
	}
	else
	{
		pResult->Assign(pNext);

		delete(pNext);
	}
}

//---------------------------------------------------------
int CSkeletonization::Standard_Step(int iDir, CSG_Grid *pPrev, CSG_Grid *pNext)
{
	bool	z[8], bRemove;

	int		x, y, nNeighbours, nChanges;

	pNext->Assign();

	nChanges	= 0;

	for(y=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pPrev->asByte(x, y) )
			{
				bRemove		= false;
				nNeighbours	= Get_Neighbours(x, y, pPrev, z);

				if( nNeighbours > 1 && nNeighbours < 6 )
				{
					bRemove	= Standard_Check(iDir, z);
				}

				if( bRemove )
				{
					nChanges++;
				}
				else
				{
					pNext->Set_Value(x, y, 1);
				}
			}
		}
	}

	return( nChanges );
}

//---------------------------------------------------------
inline bool CSkeletonization::Standard_Check(int iDir, bool z[8])
{
	bool	bRemove;

	switch( iDir )
	{
	default:
		bRemove	= false;
		break;

	case 0:
		bRemove	=	!z[7] && !z[0] && !z[1]
				&&	 z[3] &&  z[4] &&  z[5];
		break;

	case 1:
		bRemove	=	!z[0] && !z[1] && !z[2]
				&&	 z[4]      &&      z[6];
		break;

	case 2:
		bRemove	=	!z[1] && !z[2] && !z[3]
				&&	 z[5] &&  z[6] &&  z[7];
		break;

	case 3:
		bRemove	=	!z[2] && !z[3] && !z[4]
				&&	 z[6]      &&      z[0];
		break;

	case 4:
		bRemove	=	!z[3] && !z[4] && !z[5]
				&&	 z[7] &&  z[0] &&  z[1];
		break;

	case 5:
		bRemove	=	!z[4] && !z[5] && !z[6]
				&&	 z[0]      &&      z[2];
		break;

	case 6:
		bRemove	=	!z[5] && !z[6] && !z[7]
				&&	 z[1] &&  z[2] &&  z[3];
		break;

	case 7:
		bRemove	=	!z[6] && !z[7] && !z[0]
				&&	 z[2]      &&      z[4];
		break;
	}

	return( bRemove );
}


///////////////////////////////////////////////////////////
//														 //
//						Hilditch						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSkeletonization::Hilditch_Execute(void)
{
	int		nChanges;

	CSG_Grid	*pPrev, *pNext, *pTemp, *pNC_Gaps;

	//-----------------------------------------------------
	pPrev		= pResult;
	pNext		= SG_Create_Grid(pPrev);
	pNC_Gaps	= SG_Create_Grid(pPrev, SG_DATATYPE_Char);

	//-----------------------------------------------------
	do
	{
		DataObject_Update(pResult, 0, 1, true);

		nChanges	= Hilditch_Step(pPrev, pNext, pNC_Gaps);

		pTemp		= pPrev;
		pPrev		= pNext;
		pNext		= pTemp;
	}
	while( nChanges > 0 && Process_Get_Okay(true) );

	//-----------------------------------------------------
	delete(pNC_Gaps);

	if( pNext == pResult )
	{
		delete(pPrev);
	}
	else
	{
		pResult->Assign(pNext);

		delete(pNext);
	}
}

//---------------------------------------------------------
int CSkeletonization::Hilditch_Step(CSG_Grid *pPrev, CSG_Grid *pNext, CSG_Grid *pNC_Gaps)
{
	bool	z[8], bPrev, bRemove;

	int		x, y, i, nChanges, nNeighbours;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			nChanges	= 0;

			if( pPrev->asByte(x, y) && Get_Neighbours(x, y, pPrev, z) > 0 )
			{
				for(i=0, bPrev=z[7]; i<8; i++)
				{
					if( bPrev == false && z[i] == true )
					{
						nChanges++;
					}

					bPrev	= z[i];
				}
			}

			pNC_Gaps->Set_Value(x, y, nChanges);
		}
	}

	//-----------------------------------------------------
	pNext->Assign();

	nChanges	= 0;

	for(y=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pPrev->asByte(x, y) )
			{
				bRemove		= false;
				nNeighbours	= Get_Neighbours(x, y, pPrev, z);

				//-----------------------------------------
				if( 2 <= nNeighbours && nNeighbours <= 6 && pNC_Gaps->asByte(x, y) == 1 )
				{
					bRemove	=	Hilditch_Check(pNC_Gaps, x, y, 0, z)
							||	Hilditch_Check(pNC_Gaps, x, y, 2, z)
							||	Hilditch_Check(pNC_Gaps, x, y, 4, z)
							||	Hilditch_Check(pNC_Gaps, x, y, 6, z)
							;
				}

				//-----------------------------------------
				if( bRemove )
				{
					nChanges++;
				}
				else
				{
					pNext->Set_Value(x, y, true);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( nChanges );
}

//---------------------------------------------------------
inline bool CSkeletonization::Hilditch_Check(CSG_Grid *pNC_Gaps, int x, int y, int i0, bool z[8])
{
	int		ix, iy, i2, i4, i6;

	i2		= (i0 + 2) % 8;
	i4		= (i0 + 4) % 8;
	i6		= (i0 + 6) % 8;

	if( z[i0] || z[i2] || z[i6] )
	{
		ix		= Get_System()->Get_xTo(i0, x);
		iy		= Get_System()->Get_yTo(i0, y);

		if( pNC_Gaps->is_InGrid(ix, iy) && pNC_Gaps->asByte(ix, iy) == 1 )
		{
			return( false );
		}
	}

	if( z[i0] || z[i2] || z[i4] )
	{
		ix		= Get_System()->Get_xTo(i2, x);
		iy		= Get_System()->Get_yTo(i2, y);

		if( pNC_Gaps->is_InGrid(ix, iy) && pNC_Gaps->asByte(ix, iy) == 1 )
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Channel Detection					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSkeletonization::SK_Execute(void)
{
	int		x, y, i, ix, iy, n,
			Convergence, NB[8];

	double	z, iz;

	CSG_Grid	*pInput;

	//-----------------------------------------------------
	Convergence	= Parameters("CONVERGENCE")->asInt();
	pInput		= Parameters("INPUT")->asGrid();

	pResult->Assign();


	//-----------------------------------------------------
	// 1.) Find definitive channel points...

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			z	= pInput->asDouble(x, y);
			n	= 0;

			for(i=0; i<8; i++)
			{
				ix	= Get_System()->Get_xTo(i, x);
				iy	= Get_System()->Get_yTo(i, y);

				if( pInput->is_InGrid(ix, iy) && z < pInput->asDouble(ix, iy) )
				{
					n++;
				}
			}

			if( Convergence > n )
			{
				pResult->Set_Value(x, y, 2);
			}
		}
	}


	//-----------------------------------------------------
	// 2.) Find channels...

	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{	
		if( pInput->Get_Sorted(n, x, y) )	// Von oben nach unten...
		{
			z	= pInput->asDouble(x, y);

			for(i=0; i<8; i++)
			{
				ix	= Get_System()->Get_xTo(i,x);
				iy	= Get_System()->Get_yTo(i,y);

				if( !pInput->is_InGrid(ix, iy) )
				{
					NB[i]	= skNE;
				}
				else
				{
					iz	= pInput->asDouble(ix, iy);

					if( iz < z )
					{
						NB[i]	= skNE;
					}
					else if( iz > z && pResult->asByte(ix, iy) )
					{
						NB[i]	= skJA;
					}
					else
					{
						NB[i]	= 0;
					}
				}
			}

			if( SK_Connectivity(NB) )
			{
				pResult->Set_Value(x, y, 1);
			}
		}
	}


	//-----------------------------------------------------
	/*/ 3.) Filter...

	Lock_Create();

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			SK_Filter(x, y);
		}
	}

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( Lock_Get(x, y) )
			{
				pResult->Set_Value(x, y, 0);
			}
		}
	}

	Lock_Destroy();/**/
}

//---------------------------------------------------------
int CSkeletonization::SK_Connectivity(int NB[8])
{
	int		i;

	for(i=0; i<8; i+=2)
	{
		if( !NB[i] )
		{
			if( !NB[(i+2)%8] )	// Diagonal Connection
			{
			/*
				if( ( NB[(i+1)%8]	)
				&&	( NB[(i+3)%8] || NB[(i+4)%8] || NB[(i+5)%8] || NB[(i+6)%8] || NB[(i+7)%8] ) )
					return(1);
				*/

				///*
				if( ( NB[(i+1)%8]==skNE	)
				&&	( NB[(i+3)%8]==skJA || NB[(i+4)%8]==skJA || NB[(i+5)%8]==skJA || NB[(i+6)%8]==skJA || NB[(i+7)%8]==skJA ) )
					return(1);

				if( ( NB[(i+1)%8]==skJA	)
				&&	( NB[(i+3)%8]==skNE || NB[(i+4)%8]==skNE || NB[(i+5)%8]==skNE || NB[(i+6)%8]==skNE || NB[(i+7)%8]==skNE ) )
					return(1);
				//*/
			}

			//---Orthogonal-Connection---------------------------------
			if(!NB[(i+4)%8])
			{	/*
				if(	( NB[(i+1)%8] || NB[(i+2)%8] || NB[(i+3)%8] )
				&&	( NB[(i+5)%8] || NB[(i+6)%8] || NB[(i+7)%8] ) )
					return(1);
				*/

				///*
				if(	( NB[(i+1)%8]==skJA || NB[(i+2)%8]==skJA || NB[(i+3)%8]==skJA )
				&&	( NB[(i+5)%8]==skNE || NB[(i+6)%8]==skNE || NB[(i+7)%8]==skNE ) )
					return(1);

				if(	( NB[(i+1)%8]==skNE || NB[(i+2)%8]==skNE || NB[(i+3)%8]==skNE )
				&&	( NB[(i+5)%8]==skJA || NB[(i+6)%8]==skJA || NB[(i+7)%8]==skJA ) )
					return(1);
				//*/
			}
		}
	}

	return( 0 );
}

//---------------------------------------------------------
bool CSkeletonization::SK_Filter(int x, int y)
{
	bool	z[8];

	if( !pResult->asByte(x, y) && Get_Neighbours(x, y, pResult, z) == 4 )
	{
		if( z[0] && z[2] && z[4] && z[6] )
		{
			Lock_Set(Get_System()->Get_xTo(0, x), Get_System()->Get_yTo(0, y));
			Lock_Set(Get_System()->Get_xTo(2, x), Get_System()->Get_yTo(2, y));
			Lock_Set(Get_System()->Get_xTo(4, x), Get_System()->Get_yTo(4, y));
			Lock_Set(Get_System()->Get_xTo(6, x), Get_System()->Get_yTo(6, y));

			return( true );
		}
	}

	return( false );
}
