
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Classes_To_Shapes.cpp              //
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
#include "Grid_Classes_To_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Classes_To_Shapes::CGrid_Classes_To_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Vectorising Grid Classes"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad, (c) 2005 by Hartmut Linke"));

	Set_Description(_TL(
		"Vectorising grid classes.")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Input Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "CONTOUR"		, _TL("Shapes"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "OUTPUT_TYPE"	, _TL("Output as..."),
		"",
		_TL(
		"Lines|"
		"Polygons|"), 1
	);
}

//---------------------------------------------------------
CGrid_Classes_To_Shapes::~CGrid_Classes_To_Shapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::On_Execute(void)
{
	pGrid	= Parameters("INPUT")->asGrid();
	pLayer	= Parameters("CONTOUR")->asShapes();

	switch( Parameters("OUTPUT_TYPE")->asInt() )
	{
	case 0:
		pLayer->Create(SHAPE_TYPE_Line		, pGrid->Get_Name());
		break;

	case 1: default:
		pLayer->Create(SHAPE_TYPE_Polygon	, pGrid->Get_Name());
		break;
	}

	pLayer->Get_Table().Add_Field("ID"				, TABLE_FIELDTYPE_Int);
	pLayer->Get_Table().Add_Field(pGrid->Get_Name(), TABLE_FIELDTYPE_Double);
	pLayer->Get_Table().Add_Field(_TL("Name")			, TABLE_FIELDTYPE_String);

	Discrete_Create();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Classes_To_Shapes::Discrete_Create(void)
{
	int		x, y, ID;

	//-----------------------------------------------------
	Lock	= (int  **)SG_Calloc(Get_NY()    , sizeof(int  *));
	Area	= (char **)SG_Calloc(Get_NY() + 1, sizeof(char *));
	for(y=0; y<Get_NY(); y++)
	{
		Lock[y]	= (int  *)SG_Calloc(Get_NX()    , sizeof(int ));
		Area[y]	= (char *)SG_Calloc(Get_NX() + 1, sizeof(char));
	}
	Area[Get_NY()]	= (char *)SG_Calloc(Get_NX() + 1, sizeof(char));

	//-----------------------------------------------------
	for(y=0, ID=1; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x,y) && !Lock[y][x] )
			{
				Discrete_Lock(x, y, ID);
				Discrete_Area(x, y, ID);
				ID++;
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY(); y++)
	{
		SG_Free(Lock[y]);
		SG_Free(Area[y]);
	}

	SG_Free(Area[Get_NY()]);
	SG_Free(Lock);
	SG_Free(Area);
}

//---------------------------------------------------------
void CGrid_Classes_To_Shapes::Discrete_Lock(int x, int y, int ID)
{
	const int	xTo[]	= { 0,  1,  0, -1 },
				yTo[]	= { 1,  0, -1,  0 };

	const char	goDir[]	= { 1, 2, 4, 8	};

	bool	isBorder, doRecurse;

	char	goTemp,
			*goStack	= NULL;

	int		i, ix, iy,
			iStack		= 0,
			nStack		= 0,
			*xStack		= NULL,
			*yStack		= NULL;

	double	Value;

	//-----------------------------------------------------
	Value	= pGrid->asDouble(x,y);

	for(iy=0; iy<=Get_NY(); iy++)
	{
		for(ix=0; ix<=Get_NX(); ix++)
		{
			Area[iy][ix]	= 0;
		}
	}

	//-----------------------------------------------------
	do
	{
		if( !Lock[y][x] )
		{
			if( nStack <= iStack )
			{
				nStack	+= 50;
				goStack	= (char *)SG_Realloc(goStack	,nStack * sizeof(char));
				xStack	= (int  *)SG_Realloc(xStack	,nStack * sizeof(int ));
				yStack	= (int  *)SG_Realloc(yStack	,nStack * sizeof(int ));
			}

			goStack[iStack]	= 0;
			Lock[y][x]		= ID;

			//---------------------------------------------
			for(i=0; i<4; i++)
			{
				ix	= x + xTo[i];
				iy	= y + yTo[i];

				isBorder	= true;

				//-----------------------------------------
				if( ix>=0 && ix<Get_NX() && iy>=0 && iy<Get_NY() && pGrid->asDouble(ix,iy)==Value )
				{
					isBorder	= false;

					if( !Lock[iy][ix] )
						goStack[iStack]	|= goDir[i];
				}

				//-----------------------------------------
				if( isBorder )
				{
					switch(i)
					{
					case 0:
						Area[y+1][x  ]++;
						Area[y+1][x+1]++;
						break;

					case 1:
						Area[y  ][x+1]++;
						Area[y+1][x+1]++;
						break;

					case 2:
						Area[y  ][x  ]++;
						Area[y  ][x+1]++;
						break;

					case 3:
						Area[y  ][x  ]++;
						Area[y+1][x  ]++;
						break;
					}
				}
			}
		}

		//-------------------------------------------------
		doRecurse	= false;

		for(i=0; i<4; i++)
		{
			if( goStack[iStack] & goDir[i] )
			{
				if( doRecurse )
					goTemp			|= goDir[i];

				else
				{
					goTemp			= 0;
					doRecurse		= true;
					xStack[iStack]	= x;
					yStack[iStack]	= y;
					x				= x + xTo[i];
					y				= y + yTo[i];
				}
			}
		}

		//-------------------------------------------------
		if( doRecurse )
			goStack[iStack++]	= goTemp;

		else if( iStack > 0 )
		{
			iStack--;
			x	= xStack[iStack];
			y	= yStack[iStack];
		}
	}
	while( iStack > 0 );

	//-----------------------------------------------------
	if(goStack)
	{
		SG_Free(goStack);
		SG_Free(xStack);
		SG_Free(yStack);
	}
}

//---------------------------------------------------------
// 10 July, 2005: This function has been improved by
//    Hartmut Linke
//    e-mail: hartmut.linke@ast.iitb.fraunhofer.de
//
void CGrid_Classes_To_Shapes::Discrete_Area(int x, int y, int ID)
{
	const int	xTo[]	= { 0,  1,  0, -1 },
				yTo[]	= { 1,  0, -1,  0 };

	const int	xLock[]	= { 0,  0, -1, -1 },
				yLock[] = { 0, -1, -1,  0 };

	bool		bContinue, bStart;

	int			i, ix, iy, ix1, iy1, dir, iStart;
  
	double		xMin	= pGrid->Get_XMin(),
				yMin	= pGrid->Get_YMin();

	CSG_Shape		*pShape	= pLayer->Add_Shape();

	//-----------------------------------------------------
	pShape->Get_Record()->Set_Value(0, ID);
	pShape->Get_Record()->Set_Value(1, pGrid->asDouble(x, y));
	pShape->Get_Record()->Set_Value(2, pGrid->asDouble(x, y));

	//-----------------------------------------------------
	iStart	= 0;
	bStart	= true;

	do
	{
		pShape->Add_Point(xMin + (x - 0.5) * Get_Cellsize(), yMin + (y - 0.5) * Get_Cellsize());

		Area[y][x]	= 0;
		bContinue	= false;

		while( 1 )
		{
			// assure clockwise direction at starting point
			if( bStart )
			{
				for(i = 0; i < 4; i++)
				{
					ix	= x + xTo[i];
					iy	= y + yTo[i];

					if( ix>=0 && ix<=Get_NX() && iy>=0 && iy<=Get_NY() && Area[iy][ix]>0 )
					{
						// check, if inside situated cell (according to current direction) is locked
						ix1	= x + xLock[i];
						iy1	= y + yLock[i];

						if( ix1>=0 && ix1<=Get_NX() && iy1>=0 && iy1<=Get_NY() && Lock[iy1][ix1] == ID )
						{
							x			= ix;
							y			= iy;
							iStart		= (i + 3) % 4;
							bContinue	= true;
							bStart		= false;
							break;
						}
					}
				}
			}
			else
			{
				for(i = iStart; i < iStart+4; i++)
				{
					dir = i%4;
					ix	= x + xTo[dir];
					iy	= y + yTo[dir];

					if( ix>=0 && ix<=Get_NX() && iy>=0 && iy<=Get_NY() && Area[iy][ix]>0 )
					{
						if(i < iStart+3)
						{			  
							// check, if inside situated cell (according to current direction) is locked
							ix1 = x + xLock[dir];
							iy1 = y + yLock[dir];

							if(ix1>=0 && ix1<=Get_NX() && iy1>=0 && iy1<=Get_NY() && Lock[iy1][ix1] == ID)
							{
								x			= ix;
								y			= iy;
								iStart		= (i + 3) % 4;
								bContinue	= true;
								break;
							}
						}
						else
						{
							x			= ix;
							y			= iy;
							bContinue	= true;
							iStart		= (i + 3) % 4;
							break;
						}
					}
				}
			}

			break; 
		};	// while( 1 )
	}
	while( bContinue );

	if( pShape->Get_Point_Count(0) > 0 )
	{
		pShape->Add_Point(pShape->Get_Point(0, 0), 0);	// start point := end point...
	}
}
