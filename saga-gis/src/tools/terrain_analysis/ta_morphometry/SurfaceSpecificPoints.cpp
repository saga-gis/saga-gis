
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
#include "SurfaceSpecificPoints.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSurfaceSpecificPoints::CSurfaceSpecificPoints(void)
{
	Set_Name		(_TL("Surface Specific Points"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Classification of raster cells representing surface-specific points. "
	));

	Add_Reference("Band, L.E.", "1986",
		"Topographic partition of watersheds with Digital Elevation Models",
		"Water Resources Research, 22.1, 15-24.",
		SG_T("https://doi.org/10.1029/WR022i001p00015"), SG_T("doi:10.1029/WR022i001p00015")
	);

	Add_Reference("Peucker, T.K. & Douglas, D.H.", "1975",
		"Detection of surface-specific points by local parallel processing of discrete terrain elevation data",
		"Computer Graphics and Image Processing, 4, 375-387.",
		SG_T("https://doi.org/10.1016/0146-664X(75)90005-2"), SG_T("doi:10.1016/0146-664X(75)90005-2")
	);

	Parameters.Add_Grid("",
		"ELEVATION", _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"   , _TL("Surface Specific Points"),
		_TL(""),
	PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Choice("",
		"METHOD"   , _TL("Method"),
		_TL("Algorithm for the detection of Surface Specific Points"),
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Flagging Neighbours of Highest Elevation"),
			_TL("Opposite Neighbours"),
			_TL("Flow Direction (down)"),
			_TL("Flow Direction (up and down)"),
			_TL("Peucker & Douglas")
		), 1
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD", _TL("Threshold"),
		_TL("Threshold for Peucker & Douglas Algorithm"),
		1.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSurfaceSpecificPoints::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("THRESHOLD", pParameter->asInt() == 4);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSurfaceSpecificPoints::On_Execute(void)
{
	CSG_Grid *pDEM    = Parameters("ELEVATION")->asGrid();
	CSG_Grid *pResult = Parameters("RESULT"   )->asGrid();

	switch( Parameters("METHOD")->asInt() )
	{
	case  0: Do_MarkHighestNB (pDEM, pResult); break;
	default: Do_OppositeNB    (pDEM, pResult); break;
	case  2: Do_FlowDirection (pDEM, pResult); break;
	case  3: Do_FlowDirection2(pDEM, pResult); break;
	case  4: Do_PeuckerDouglas(pDEM, pResult); break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_MarkHighestNB(CSG_Grid *pDEM, CSG_Grid *pResult)	// Band & Lammers...
{
	CSG_Grid clo(Get_System(), SG_DATATYPE_Char);
	CSG_Grid chi(Get_System(), SG_DATATYPE_Char);

	// Pass 1: Auszaehlen...
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
        {
			double lo, hi; lo = hi = pDEM->asDouble(x, y);
			int xlo = x, xhi = x, ylo = y, yhi = y;

			for(int i=0; i<4; i++)
			{
				int ix = Get_xTo(i,x), iy = Get_yTo(i,y);
  
				if( is_InGrid(ix, iy) )
				{
					double z = pDEM->asDouble(ix, iy);

					if( z > hi )
					{
						hi = z; xhi = ix; yhi = iy;
					}
					else if( z < lo )
					{
						lo = z; xlo	= ix; ylo = iy;
					}
				}
			}

			clo.Add_Value(xlo, ylo, 1);
			chi.Add_Value(xhi, yhi, 1);
		}
	}

	// Pass 2: Setzen...
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) ) { pResult->Set_NoData(x, y); continue; }

			if( !chi.asChar(x, y) )
			{
				if( !clo.asChar(x, y) )
					pResult->Set_Value(x, y, 2); // Sattel
				else
					pResult->Set_Value(x, y, 1); // Tiefenlinie
			}
			else if( !clo.asChar(x,y) )
				pResult->Set_Value(x, y, -1);    // Wasserscheide
			else
				pResult->Set_Value(x, y,  0);    // Nichts...
		}
	}

	DataObject_Set_Colors(pResult, 5, SG_COLORS_RED_GREY_BLUE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_OppositeNB(CSG_Grid *pDEM, CSG_Grid *pResult)
{
	CSG_Grid clo(Get_System(), SG_DATATYPE_Char);
	CSG_Grid chi(Get_System(), SG_DATATYPE_Char);

	// Pass 1: Auszaehlen...
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
        {
			double z = pDEM->asDouble(x, y);

			for(int i=0; i<4; i++)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if( is_InGrid(ix, iy) )
				{
					int jx = Get_xFrom(i,x), jy = Get_yFrom(i,y);
  
					if( is_InGrid(jx, jy) )
					{
						double iz = pDEM->asDouble(ix, iy);
						double jz = pDEM->asDouble(jx, jy);

						if( iz > z && jz > z )
							chi.Add_Value(x, y, 1);

						else if( iz < z && jz < z )
							clo.Add_Value(x, y, 1);
					}
				}
			}
		}
	}

	// Pass 2: Setzen...
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) ) { pResult->Set_NoData(x, y); continue; }

			if( chi.asChar(x, y) )
			{
				if( clo.asChar(x, y) )
					pResult->Set_Value(x, y,                5); // Sattel
				else
					pResult->Set_Value(x, y, chi.asChar(x, y)); // Tiefenlinie
			}
			else if( clo.asChar(x,y) )
				pResult->Set_Value(x, y, -clo.asChar(x, y));    // Wasserscheide
			else
				pResult->Set_Value(x, y, 0);                    // Nichts...
		}
	}

	DataObject_Set_Colors(pResult, 5, SG_COLORS_RED_GREY_BLUE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_FlowDirection(CSG_Grid *pDEM, CSG_Grid *pResult)
{
	pResult->Assign(0.);

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
        {
			if( pDEM->is_NoData(x, y) ) { pResult->Set_NoData(x, y); continue; }

			double z = pDEM->asDouble(x, y), zLow; int xLow, yLow; bool bLower = false;

			for(int i=0; i<8; i++)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);
  
				if( pDEM->is_InGrid(ix, iy) )
				{
					double iz = pDEM->asDouble(ix,iy);

					if( iz < z )
					{
						if( !bLower )
						{
							bLower = true;

							zLow = iz; xLow = ix; yLow = iy;
						}
						else if( iz < zLow )
						{
							zLow = iz; xLow = ix; yLow = iy;
						}
					}
				}
			}

			if( bLower )
			{
				pResult->Add_Value(xLow, yLow, 1);
			}
		}
	}

	DataObject_Set_Colors(pResult, 5, SG_COLORS_RED_GREY_BLUE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_FlowDirection2(CSG_Grid *pDEM, CSG_Grid *pResult)
{
	CSG_Grid Grid(*pDEM), Result(*pResult);

	Do_FlowDirection(&Grid, &Result);

	Grid.Invert();

	Do_FlowDirection(&Grid, pResult);

	#pragma omp parallel for
	for(sLong n=0; n<Get_NCells(); n++)
	{
		if( !pResult->is_NoData(n) )
		{
			pResult->Add_Value(n, -Result.asInt(n));
		}
	}

	DataObject_Set_Colors(pResult, 5, SG_COLORS_RED_GREY_BLUE, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSurfaceSpecificPoints::Do_PeuckerDouglas(CSG_Grid *pDEM, CSG_Grid *pResult)
{
	double Threshold = Parameters("THRESHOLD")->asDouble();

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) ) { pResult->Set_NoData(x, y); continue; }

			double alt[8], z = pDEM->asDouble(x, y);

			for(int i=0; i<8; i++)
			{
				int ix = Get_xTo(i,x), iy = Get_yTo(i,y);

				if( is_InGrid(ix, iy) )
					alt[i] = pDEM->asDouble(ix, iy);
				else
					alt[i] = z;
			}

			double dPlus = 0., dMinus = 0; int nSgn = 0;

			bool wasPlus = (alt[7] - z > 0.) ? true : false;
			
			for(int i=0; i<8; i++)
			{
				double d = alt[i] - z;

				if( d > 0. )
				{
					dPlus += d;

					if( !wasPlus )
					{
						nSgn++; wasPlus = true;
					}
				}
				else if( d < 0. )
				{
					dMinus -= d;

					if( wasPlus )
					{
						nSgn++; wasPlus = false;
					}
				}
			}

			int i = 0;

			if( !dPlus )        // Peak
				i =  9;
			else if( !dMinus )  // Pit
				i = -9;
			else if( nSgn == 4) // Pass
				i = 1;
			else if( nSgn == 2)
			{
				i = nSgn = 0;

				if( alt[7] > z )
				{
					while(alt[i++]>z);
					do	nSgn++;	while(alt[i++]<z);
				}
				else
				{
					while(alt[i++]<z);
					do	nSgn++;	while(alt[i++]>z);
				}

				i = 0;

				if( nSgn == 4 )
				{
					if( dMinus - dPlus > Threshold )      // convex break
						i	=  2;
					else if( dPlus - dMinus > Threshold ) // concave break
						i	= -2;
				}
				else // lines:
				{
					if( dMinus - dPlus > 0 )              // Ridge
						i	=  7;
					else                                  // Channel
						i	= -7;
				}
			}

			pResult->Set_Value(x, y, i);
		}
    }

	DataObject_Set_Colors(pResult, 5, SG_COLORS_RED_GREY_BLUE, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
