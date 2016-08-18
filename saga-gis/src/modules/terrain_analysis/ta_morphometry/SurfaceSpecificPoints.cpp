/**********************************************************
 * Version $Id: SurfaceSpecificPoints.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               SurfaceSpecificPoints.cpp               //
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
#include "SurfaceSpecificPoints.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSurfaceSpecificPoints::CSurfaceSpecificPoints(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Surface Specific Points"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"References:\n"
		"Peucker, T.K. and Douglas, D.H., 1975:\n"
		"'Detection of surface-specific points by local parallel processing of discrete terrain elevation data',\n"
		"Computer Graphics and Image Processing, 4, 375-387\n"
	));

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Algorithm for the detection of Surface Specific Points"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Mark Highest Neighbour"),
			_TL("Opposite Neighbours"),
			_TL("Flow Direction"),
			_TL("Flow Direction (up and down)"),
			_TL("Peucker & Douglas")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "THRESHOLD"	, _TL("Threshold"),
		_TL("Threshold for Peucker & Douglas Algorithm"),
		PARAMETER_TYPE_Double	, 2
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSurfaceSpecificPoints::On_Execute(void)
{
	CSG_Grid	*pGrid, *pResult;

	pGrid		= Parameters("ELEVATION")	->asGrid();
	pResult		= Parameters("RESULT")		->asGrid();

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:
		Do_MarkHighestNB	(pGrid, pResult);
		break;

	case 1:
		Do_OppositeNB		(pGrid, pResult);
		break;

	case 2:
		Do_FlowDirection	(pGrid, pResult);
		break;

	case 3:
		Do_FlowDirection2	(pGrid, pResult);
		break;

	case 4:
		Do_PeuckerDouglas	(pGrid, pResult, Parameters("THRESHOLD")->asDouble());
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_MarkHighestNB(CSG_Grid *pGrid, CSG_Grid *pResult)	// Band & Lammers...
{
	int		i, x, y, ix, iy, xlo, ylo, xhi, yhi;

	double	lo, hi, z;

	CSG_Grid	*clo, *chi;

	clo		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);
	chi		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);

	// Pass 1: Auszaehlen...
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
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
		for(x=0; x<Get_NX(); x++)
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_OppositeNB(CSG_Grid *pGrid, CSG_Grid *pResult)
{
	int		i, x, y, ix, iy, jx, jy;

	double	z, iz, jz;

	CSG_Grid	*clo, *chi;

	clo		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);
	chi		= SG_Create_Grid(pGrid, SG_DATATYPE_Char);

	// Pass 1: Auszaehlen...
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
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
		for(x=0; x<Get_NX(); x++)
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
		}
	}

	delete(clo);
	delete(chi);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_FlowDirection(CSG_Grid *pGrid, CSG_Grid *pResult)
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_FlowDirection2(CSG_Grid *pGrid, CSG_Grid *pResult)
{
	CSG_Grid	Grid(*pGrid), Result(*pResult);

	Do_FlowDirection(&Grid, &Result);

	Grid.Invert();

	Do_FlowDirection(&Grid, pResult);

	for(sLong n=0; n<Get_NCells(); n++)
	{
		pResult->Add_Value(n, -Result.asInt(n));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_PeuckerDouglas(CSG_Grid *pGrid, CSG_Grid *pResult, double Threshold)
{
	bool	wasPlus;

	int		x, y, i, ix, iy, nSgn;

	double	d, dPlus, dMinus, z, alt[8];

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			z	= pGrid->asDouble(x,y);

			for(i=0; i<8; i++)
			{
				ix	= Get_xTo(i,x);
				iy	= Get_yTo(i,y);

				if( is_InGrid(ix,iy) )
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

			pResult->Set_Value(x,y,i);
		}
    }
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
