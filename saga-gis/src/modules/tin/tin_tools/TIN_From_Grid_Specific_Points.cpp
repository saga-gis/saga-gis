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
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            TIN_From_Grid_Specific_Points.cpp          //
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
#include "TIN_From_Grid_Specific_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_From_Grid_Specific_Points::CTIN_From_Grid_Specific_Points(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Grid to TIN (Surface Specific Points)"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description	(_TW(
		"Creates a TIN by identifying (surface) specific points of a grid."
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "VALUES"		, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_TIN(
		NULL	, "TIN"			, _TL("TIN"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("The method used to identify surface specific points."),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Mark Highest Neighbour"),
			_TL("Opposite Neighbours"),
			_TL("Flow Direction"),
			_TL("Flow Direction (up and down)"),
			_TL("Peucker & Douglas")
		), 1
	);

	pNode	= Parameters.Add_Node(NULL, "THRESHOLDS", _TL("Thresholds"), _TL(""));

	Parameters.Add_Value(
		pNode	, "HIGH"		, _TL("Mark Highest Neighbour"),
		_TL(""),
		PARAMETER_TYPE_Int		, 4, 1, true, 4, true
	);

	Parameters.Add_Range(
		pNode	, "FLOW"		, _TL("Flow Direction"),
		_TL(""),
		0, 3, 0, true, 8, true
	);

	Parameters.Add_Value(
		pNode	, "PEUCKER"		, _TL("Peucker & Douglas"),
		_TL(""),
		PARAMETER_TYPE_Double	, 2
	);
}

//---------------------------------------------------------
CTIN_From_Grid_Specific_Points::~CTIN_From_Grid_Specific_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_From_Grid_Specific_Points::On_Execute(void)
{
	bool					bResult;
	int						x, y, i;
	CSG_TIN					*pTIN;
	CSG_Grid					*pGrid, Grid;
	CSG_Parameter_Grid_List	*pValues;
	CSG_Shape					*pPoint;
	CSG_Shapes					Points;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")->asGrid();
	Grid.Create(pGrid, SG_DATATYPE_Byte);

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	default:
		bResult	= false;
		break;

	case 0:
		bResult	= Get_MarkHighestNB	(&Grid, pGrid);
		break;

	case 1:
		bResult	= Get_OppositeNB	(&Grid, pGrid, Parameters("HIGH")->asInt());
		break;

	case 2:
		bResult	= Get_FlowDirection	(&Grid, pGrid,
			(int)Parameters("FLOW")->asRange()->Get_LoVal(),
			(int)Parameters("FLOW")->asRange()->Get_HiVal()
		);
		break;

	case 3:
		bResult	= Get_FlowDirection2(&Grid, pGrid,
			(int)Parameters("FLOW")->asRange()->Get_HiVal()
		);
		break;

	case 4:
		bResult	= Get_Peucker		(&Grid, pGrid, Parameters("PEUCKER")->asDouble());
		break;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		pValues	= Parameters("VALUES")->asGridList();

		Points.Create(SHAPE_TYPE_Point);
		Points.Add_Field(_TL("VALUE"), SG_DATATYPE_Double);

		for(i=0; i<pValues->Get_Count(); i++)
		{
			Points.Add_Field(pValues->asGrid(i)->Get_Name(), SG_DATATYPE_Double);
		}

		for(y=0; y<Get_NY() && Set_Progress(y, Get_NY()); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( Grid.asInt(x, y) != 0 )
				{
					pPoint	= Points.Add_Shape();

					pPoint->Add_Point(
						Get_XMin() + Get_Cellsize() * x,
						Get_YMin() + Get_Cellsize() * y
					);

					pPoint->Set_Value(0, pGrid->asDouble(x, y));

					for(i=0; i<pValues->Get_Count(); i++)
					{
						pPoint->Set_Value(1 + i, pValues->asGrid(i)->asDouble(x, y));
					}
				}
			}
		}

		//-------------------------------------------------
		if( Points.Get_Count() >= 3 )
		{
			pTIN	= Parameters("TIN")->asTIN();
			pTIN->Create(&Points);
			pTIN->Set_Name(pGrid->Get_Name());
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_From_Grid_Specific_Points::Get_MarkHighestNB(CSG_Grid *pResult, CSG_Grid *pGrid)	// Band & Lammers...
{
	int		i, x, y, ix, iy, xlo, ylo, xhi, yhi;
	double	lo, hi, z;
	CSG_Grid	*clo, *chi;

	clo		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);
	chi		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);

	// Pass 1: Auszaehlen...
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX()-1; x++)
        {
			lo	= hi	= pGrid->asDouble(x,y);
			xhi	= xlo	= x;
			yhi	= ylo	= y;

			for(i=0; i<4; i++)
			{
				ix	= Get_xTo(i,x);
				iy	= Get_yTo(i,y);
  
				if( is_InGrid(ix,iy) )
				{
					z	= pGrid->asDouble(ix,iy);

					if( z > hi )
					{
						hi	= z;
						xhi	= ix;
						yhi	= iy;
					}
					else if( z < lo )
					{
						lo	= z;
						xlo	= ix;
						ylo	= iy;
					}
				}
			}

			clo->Add_Value(xlo,ylo,1);
			chi->Add_Value(xhi,yhi,1);
		}
	}

	// Pass 2: Setzen...
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX()-1; x++)
		{
			if( !chi->asChar(x,y) )
			{
				if( !clo->asChar(x,y) )
					pResult->Set_Value(x,y, 2);	// Sattel
				else
					pResult->Set_Value(x,y, 1);	// Tiefenlinie
			}
			else if( !clo->asChar(x,y) )
				pResult->Set_Value(x,y, -1);	// Wasserscheide
			else
				pResult->Set_Value(x,y,  0);	// Nichts...
		}
	}

	delete(clo);
	delete(chi);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_From_Grid_Specific_Points::Get_OppositeNB(CSG_Grid *pResult, CSG_Grid *pGrid, int Threshold)
{
	int		i, x, y, ix, iy, jx, jy;
	double	z, iz, jz;
	CSG_Grid	*clo, *chi;

	clo		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);
	chi		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);

	// Pass 1: Auszaehlen...
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX()-1; x++)
        {
			z	= pGrid->asDouble(x,y);

			for(i=0; i<4; i++)
			{
				ix	= Get_xTo(i,x);
				iy	= Get_yTo(i,y);

				if( is_InGrid(ix,iy) )
				{
					jx	= Get_xFrom(i,x);
					jy	= Get_yFrom(i,y);
  
					if( is_InGrid(jx,jy) )
					{
						iz	= pGrid->asDouble(ix,iy);
						jz	= pGrid->asDouble(jx,jy);

						if( iz>z && jz>z )
							chi->Add_Value(x,y,1);

						else if( iz<z && jz<z )
							clo->Add_Value(x,y,1);
					}
				}
			}
		}
	}

	// Pass 2: Setzen...
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX()-1; x++)
		{
			if( chi->asChar(x,y) )
			{
				if( clo->asChar(x,y) )
					pResult->Set_Value(x,y, 5);					// Sattel
				else
					pResult->Set_Value(x,y, chi->asChar(x,y) );	// Tiefenlinie
			}
			else if( clo->asChar(x,y) )
				pResult->Set_Value(x,y, - clo->asChar(x,y) );	// Wasserscheide
			else
				pResult->Set_Value(x,y, 0);						// Nichts...

			pResult->Set_Value(x, y, abs(pResult->asInt(x, y)) >= Threshold ? 1 : 0);
		}
	}

	delete(clo);
	delete(chi);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_From_Grid_Specific_Points::Get_FlowDirection(CSG_Grid *pResult, CSG_Grid *pGrid, int Min, int Max)
{
	bool	bLower;
	int		x, y, i, ix, iy, xLow, yLow;
	double	z, iz, zLow;

	pResult->Assign();

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
        {
			z		= pGrid->asDouble(x,y);
			bLower	= false;

			for(i=0; i<8; i++)
			{
				ix	= Get_xTo(i,x);
				iy	= Get_yTo(i,y);
  
				if( is_InGrid(ix,iy) )
				{
					iz	= pGrid->asDouble(ix,iy);

					if(iz<z)
					{
						if(!bLower)
						{
							bLower	= true;
							zLow	= iz;
							xLow	= ix;
							yLow	= iy;
						}
						else if(iz<zLow)
						{
							zLow	= iz;
							xLow	= ix;
							yLow	= iy;
						}
					}
				}
			}

			if(bLower)
			{
				pResult->Add_Value(xLow, yLow, 1);
			}
		}
	}

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
        {
			i	= pResult->asInt(x, y);

			if( i <= Min )
			{
				pResult->Set_Value(x, y, -1);
			}
			else if( i >= Max )
			{
				pResult->Set_Value(x, y,  1);
			}
			else
			{
				pResult->Set_Value(x, y,  0);
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
bool CTIN_From_Grid_Specific_Points::Get_FlowDirection2(CSG_Grid *pResult, CSG_Grid *pGrid, int Threshold)
{
	CSG_Grid	Grid(*pGrid), Result(*pResult);

	Get_FlowDirection(pResult, &Grid, -1, Threshold);
	Grid.Invert();
	Get_FlowDirection(&Result, &Grid, -1, Threshold);

	for(long n=0; n<Get_NCells(); n++)
	{
		if( Result.asInt(n) > 0 )
		{
			pResult->Set_Value(n, 1);
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
bool CTIN_From_Grid_Specific_Points::Get_Peucker(CSG_Grid *pResult, CSG_Grid *pGrid, double Threshold)
{
	bool	wasPlus;
	int		x, y, i, ix, iy, nSgn;
	double	d, dPlus, dMinus, z, alt[8];

	for(y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(x=0; x<pGrid->Get_NX(); x++)
		{
			z	= pGrid->asDouble(x,y);

			for(i=0; i<8; i++)
			{
				ix	= pGrid->Get_System().Get_xTo(i,x);
				iy	= pGrid->Get_System().Get_yTo(i,y);

				if( pGrid->is_InGrid(ix,iy) )
					alt[i]	= pGrid->asDouble(ix,iy);
				else
					alt[i]	= z;
			}

			dPlus	= dMinus	= 0;
			nSgn	= 0;
			wasPlus	= (alt[7] - z > 0) ? true : false;

			for(i=0; i<8; i++)
			{
				d	= alt[i] - z;

				if(d>0)
				{
					dPlus	+= d;
					if(!wasPlus)
					{
						nSgn++;
						wasPlus	= true;
					}
				}
				else if(d<0)
				{
					dMinus	-= d;
					if(wasPlus)
					{
						nSgn++;
						wasPlus	= false;
					}
				}
			}

			i	= 0;
			if(!dPlus)									// Peak...
				i	=  9;
			else if(!dMinus)							// Pit
				i	= -9;
			else if(nSgn==4)							// Pass
				i	= 1;
			else if(nSgn==2)
			{
				i	= nSgn	= 0;

				if(alt[7]>z)
				{
					while(alt[i++]>z);
					do	nSgn++;	while(alt[i++]<z);
				}
				else
				{
					while(alt[i++]<z);
					do	nSgn++;	while(alt[i++]>z);
				}

				i	= 0;

				if(nSgn==4)
				{
					if(dMinus-dPlus > Threshold)		// convex break...
						i	=  2;
					else if(dPlus-dMinus > Threshold)	// concave break...
						i	= -2;
				}
				else	// lines:
				{
					if(dMinus-dPlus>0)					// Ridge
						i	=  7;
					else								// Channel
						i	= -7;
				}
			}

			pResult->Set_Value(x, y, i == 0 ? 0 : 1);
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
