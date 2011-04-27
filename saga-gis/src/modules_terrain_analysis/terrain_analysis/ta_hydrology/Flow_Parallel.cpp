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
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Flow_Parallel.cpp                   //
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
#include "Flow_Parallel.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Parallel::CFlow_Parallel(void)
{
	Set_Name		(_TL("Catchment Area (Parallel)"));

	Set_Author		(SG_T("O.Conrad (c) 2001-2010, T.Grabs portions (c) 2010"));

	Set_Description	(_TW(
		"Parallel processing of cells for calculation of flow accumulation and related parameters. "
		"This set of algorithms processes a DEM downwards from the highest to the lowest cell.\n\n"

		"References:\n\n"

		"Deterministic 8\n"
		"- O'Callaghan, J.F. / Mark, D.M. (1984):\n"
		"    'The extraction of drainage networks from digital elevation data',\n"
		"    Computer Vision, Graphics and Image Processing, 28:323-344\n\n"

		"Rho 8:\n"
		"- Fairfield, J. / Leymarie, P. (1991):\n"
		"    'Drainage networks from grid digital elevation models',\n"
		"    Water Resources Research, 27:709-717\n\n"

		"Braunschweiger Reliefmodell:\n"
		"- Bauer, J. / Rohdenburg, H. / Bork, H.-R. (1985):\n"
		"    'Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse',\n"
		"    Landschaftsgenese und Landschaftsoekologie, H.10, Parameteraufbereitung fuer deterministische Gebiets-Wassermodelle,\n"
		"    Grundlagenarbeiten zu Analyse von Agrar-Oekosystemen, (Eds.: Bork, H.-R. / Rohdenburg, H.), p.1-15\n\n"

		"Deterministic Infinity:\n"
		"- Tarboton, D.G. (1997):\n"
		"    'A new method for the determination of flow directions and upslope areas in grid digital elevation models',\n"
		"    Water Ressources Research, Vol.33, No.2, p.309-319\n\n"

		"Multiple Flow Direction:\n"
		"- Freeman, G.T. (1991):\n"
		"    'Calculating catchment area with divergent flow based on a regular grid',\n"
		"    Computers and Geosciences, 17:413-22\n\n"

		"- Quinn, P.F. / Beven, K.J. / Chevallier, P. / Planchon, O. (1991):\n"
		"    'The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models',\n"
		"    Hydrological Processes, 5:59-79\n\n"
		
		"Triangular Multiple Flow Direction\n"
		"- Seibert, J. / McGlynn, B. (2007):\n"
		"    'A new triangular multiple flow direction algorithm for computing upslope areas from gridded digital elevation models',\n"
		"    Water Ressources Research, Vol. 43, W04501\n"
		"    C++ Implementation into SAGA by Thomas Grabs, Copyrights (c) 2007\n"
		"    Contact: thomas.grabs@natgeo.su.se, jan.seibert@natgeo.su.se \n"
	));


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "CASPECT"		, _TL("Catchment Aspect"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "FLWPATH"		, _TL("Flow Path Length"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Method...

	Parameters.Add_Choice(
		NULL	, "Method"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Braunschweiger Reliefmodell"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Multiple Triangular Flow Directon")
		), 5
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "DOLINEAR"	, _TL("Linear Flow"),
		_TL("Use D8 if catchment area becomes higher than specified threshold."),
		PARAMETER_TYPE_Bool
	);

	Parameters.Add_Value(
		NULL	, "LINEARTHRS"	, _TL("Linear Flow Threshold"),
		_TL("Use D8 if catchment area becomes higher than specified threshold (Cells)."),
		PARAMETER_TYPE_Double	,	500
	);

	Parameters.Add_Grid(
		NULL	, "LINEARTHRS_GRID"		, _TL("Linear Flow Threshold Grid"),
		_TL("(optional) Linear Flow Threshold Grid"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "CHDIR_GRID"			, _TL("Channel Direction"),
		_TW(	"(optional) Channel Direction Grid. Must contain direction values. "
				"For all non-missing grid cells all flow will be routed to the prescribed direction."
		),
		PARAMETER_INPUT_OPTIONAL
	);
	
	Parameters.Add_Value(
		NULL	, "CONVERGENCE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction Algorithm (Freeman 1991).\nApplies also to the Multiple Triangular Flow Directon Algorithm."),
		PARAMETER_TYPE_Double	, 1.0, 0.0, true
	);
}

//---------------------------------------------------------
CFlow_Parallel::~CFlow_Parallel(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::On_Initialize(void)
{
	pCatch_Aspect	= Parameters("CASPECT")->asGrid();
	pFlowPath		= Parameters("FLWPATH")->asGrid();

	TH_LinearFlow	= Parameters("DOLINEAR")->asBool() && pDTM
					? Parameters("LINEARTHRS")->asDouble() // (pDTM->Get_NX() * pDTM->Get_NY())
					: -1.0;

	pTH_LinearFlow  = Parameters("LINEARTHRS_GRID")		->asGrid();

	pD8_Direction   = Parameters("CHDIR_GRID")		    ->asGrid();

	MFD_Converge	= Parameters("CONVERGENCE")->asDouble();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Parallel::Calculate(void)
{
	int		x, y;

	for(y=0; y<Get_NY() && Set_Progress(y); y+=Step)
	{
		for(x=0; x<Get_NX(); x+=Step)
		{
			Init_Cell(x, y);
		}
	}

	return( Set_Flow() );
}

//---------------------------------------------------------
bool CFlow_Parallel::Calculate(int x, int y)
{
	Init_Cell(x, y);

	return( Set_Flow() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Parallel::Set_Flow(void)
{
	int		n, x, y;


	double  THRS = 0.0;
	
	int		DIR  = -1;
	
	//-----------------------------------------------------
	if( !pDTM->Get_Sorted(0, x, y, true, false) )
	{
		return( false );
	}


	//-----------------------------------------------------
	int Method	= Parameters("Method")->asInt();

	if( Method == 2 )
	{
		BRM_Init();
	}

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		pDTM->Get_Sorted(n,x,y);

		//if( !(n%(4*Get_NX())) )DataObject_Update(pFlow);

		if( pTH_LinearFlow && TH_LinearFlow > 0.0)
		{
			if( pTH_LinearFlow->is_NoData(x, y) )
			{
				THRS = 0.0;
			}
			else
			{
				THRS = pTH_LinearFlow->asDouble(x, y);
			}
		}
		else
		{
			THRS = pCatch->asDouble(x, y);
		}

		if( pD8_Direction )
		{
			if( pD8_Direction->is_NoData(x, y) )
			{
				DIR = -1;
			}
			else
			{
				DIR = pD8_Direction->asInt(x, y);
			}
		}

		if( TH_LinearFlow > 0.0 && THRS >= TH_LinearFlow || DIR > 0)
		{
			Set_D8(x, y, DIR);
		}
		else
		{
			switch( Method )
			{
			case 0:
				Set_D8(x, y);
				break;

			case 1:
				Set_Rho8(x, y);
				break;

			case 3:
				Set_DInf(x, y);
				break;

			case 4:
				Set_MFD(x, y);
				break;

			case 5:
				Set_MDInf(x, y);
				break;				

			case 2:
				Set_BRM(x, y);
				break;
			}
		}
	}

	//-----------------------------------------------------
	if( pRoute )
	{
		for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			pDTM->Get_Sorted(n, x, y, false);

			Check_Route(x, y);
		}
	}
	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
void CFlow_Parallel::Check_Route(int x, int y)
{
	bool	bSink;
	int		i, ix, iy;
	double	z;

	if( pRoute->asChar(x, y) > 0 )
	{
		z	= pDTM->asDouble(x, y);

		for(i=0, bSink=true; i<8 && bSink; i++)
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( !is_InGrid(ix, iy) || z > pDTM->asDouble(ix, iy) )
			{
				bSink	= false;
			}
		}

		//-------------------------------------------------
		if( bSink )
		{
			i	= pRoute->asChar(x, y);

			ix	= Get_xTo(i, ix);
			iy	= Get_yTo(i, iy);

			//---------------------------------------------
			while( is_InGrid(ix, iy) )
			{
				Add_Portion(x, y, ix, iy, i);

				i	= pRoute->asChar(ix, iy);

				if( i > 0 )
				{
					ix	= Get_xTo(i, ix);
					iy	= Get_yTo(i, iy);
				}
				else
				{
					i	= pDTM->Get_Gradient_NeighborDir(ix, iy);

					if( i >= 0 )
					{
						ix	= Get_xTo(i, ix);
						iy	= Get_yTo(i, iy);
					}
					else
					{
						ix	= -1;
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//					Deterministic 8						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_D8(		int x, int y, int dir)
{
	int		Direction;

	if( dir > 0.0)
	{
		Direction	= dir;
	}
	else
	{
		Direction = pDTM->Get_Gradient_NeighborDir(x, y);
	}

	if( Direction >= 0 )
	{
		Add_Fraction(x, y, Direction);
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Rho 8							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_Rho8(	int x, int y )
{
	int		i, ix, iy, iMax;

	double	z, d, dMax;

	z		= pDTM->asDouble(x, y);
	iMax	= -1;

	for(i=0; i<8; i++)
	{
		ix		= Get_xTo(i, x);
		iy		= Get_yTo(i, y);

		if( !pDTM->is_InGrid(ix, iy) )
		{
			return;
		}
		else
		{
			d		= z - pDTM->asDouble(ix, iy);

			if( i % 2 == 1 )
			{
				d		/= 1.0 + rand() / (double)RAND_MAX;
			}

			if( iMax < 0 || (iMax >= 0 && d > dMax) )
			{
				iMax	= i;
				dMax	= d;
			}
		}
	}

	if( iMax >= 0 )
	{
		Add_Fraction(x, y, iMax);
	}
}

/*void CFlow_Parallel::Set_Rho8(	int x, int y )
{
	int		Direction, ix, iy;

	double 	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	if( Aspect >= 0 )
	{
		Direction	= (int)(Aspect / M_PI_045);

		if( fmod(Aspect, M_PI_045) / M_PI_045 > rand() / (double)RAND_MAX )
		{
			Direction++;
		}

		Direction	%= 8;

		ix			= Get_xTo(Direction, x);
		iy			= Get_yTo(Direction, y);

		if( is_InGrid(ix, iy) && pDTM->asDouble(ix, iy) >= pDTM->asDouble(x, y) )
		{
			Direction	= Get_Direction_Lowest(pDTM, x, y);
		}

		Add_Fraction(x, y, Direction);
	}
}/**/


///////////////////////////////////////////////////////////
//														 //
//				Deterministic Infinity					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_DInf(	int x, int y )
{
	int		Direction, x1, y1, x2, y2;

	double	Slope, Aspect, z;

	Get_Gradient(x, y, Slope, Aspect);

	if( Aspect >= 0.0 )
	{
		Direction	= (int)(Aspect / M_PI_045);
		Aspect		=  fmod(Aspect , M_PI_045) / M_PI_045;

		z			= pDTM->asDouble(x, y);
		x1			= Get_xTo(Direction + 0, x);
		y1			= Get_yTo(Direction + 0, y);
		x2			= Get_xTo(Direction + 1, x);
		y2			= Get_yTo(Direction + 1, y);

		if( (!is_InGrid(x1, y1) || z > pDTM->asDouble(x1, y1))
		&&	(!is_InGrid(x2, y2) || z > pDTM->asDouble(x2, y2)) )
		{
			Add_Fraction(x, y,  Direction			, 1.0 - Aspect);
			Add_Fraction(x, y, (Direction + 1) % 8	,       Aspect);
		}
		else
		{
			Add_Fraction(x, y, pDTM->Get_Gradient_NeighborDir(x, y));
		}
	}
}

///////////////////////////////////////////////////////////
//														 //
//				Multiple Triangular Flow Directon		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_MDInf(	int x, int y )
{
	int		i, ii, ix, iy;

	double	z, dzSum, dz[8];
	double	hs, hr, s_facet[8], r_facet[8];
	double	valley[8], portion[8];
	double  cellsize, cellarea;
	double	nx, ny, nz, n_norm;
	bool	Dir_inGrid[8];

	z			= pDTM->asDouble(x, y);
	cellsize	= Get_Cellsize();
	cellarea	= cellsize * cellsize;

	for(i=0; i<8; i++)
	{
		ix		= Get_xTo(i, x);
		iy		= Get_yTo(i, y);
		
		Dir_inGrid[i] = true;
		if( x == 6 && y == 7 )
		{
			int k = 1;
		}

		s_facet[i] = r_facet[i] = -999.0;

		if( pDTM->is_InGrid(ix, iy) )
		{
			dz[i]		= z - pDTM->asDouble(ix, iy);
		}
		else
		{
			dz[i]		= 0.0;
			Dir_inGrid[i] = false;
		}
	}
	for(i=0; i<8; i++)
	{
		hr = hs = -999.0;
		
		if( Dir_inGrid[i] )
		{
			ii = ( i < 7)? i+1 : 0;
			
			if( Dir_inGrid[ii] )
			{
				// vb-code:  nx = (z1 * yd(ii) - z2 * yd(i)) * gridsize
				nx = ( dz[ii] * Get_yTo( i ) - dz[i] * Get_yTo( ii )) * cellsize;
				//vb-code:  ny = (z1 * xd(ii) - z2 * xd(i)) * gridsize
/*ERROR?*/		ny = ( dz[i] * Get_xTo( ii ) - dz[ii] * Get_xTo( i ) ) * cellsize;
				//vb-code:  nz = (xd(ii) * yd(i) - xd(i) * yd(ii)) * gridsize ^ 2
				nz = ( Get_xTo( i ) * Get_yTo( ii ) - Get_xTo( ii ) * Get_yTo( i )) * cellarea;
				
				n_norm = sqrt( nx*nx + ny*ny +nz*nz );
				/*
				if( nx == 0.0 )
				{
					hr = (ny >= 0.0)? 0.0 : M_PI;
				} 
				else if( nx > 0.0 )
				{
					hr = M_PI_090 - atan(ny / nx);

				} 
				else
				{
					hr = M_PI_270 - atan(ny / nx);
				}
				*/

				if( nx == 0.0 )
				{
					hr = (ny >= 0.0)? 0.0 : M_PI;
				} 
				else if( nx < 0.0 )
				{
					hr = M_PI_270 - atan(ny / nx);
				} 
				else
				{
					hr = M_PI_090 - atan(ny / nx);
				}
				//vb-code:  hs = -Tan(arccos(nz / betrag_n))
				hs = -tan( acos( nz/n_norm ) );
				
				// vb-code: If hr <= (i - 1) * PI / 4 Or hr >= i * PI / 4 Then
				//SHOULD IT BE LIKE THIS: (( hr <= i * M_PI_045 || hr >= ii * M_PI_045 )  OR AS BELOW???
				if( hr < i * M_PI_045 || hr > (i+1) * M_PI_045 )
				{
					if( dz[i] > dz[ii] )
					{
						hr = i * M_PI_045;
						hs = dz[i] / Get_Length(i);
					}
					else
					{
						hr = ii * M_PI_045;
						hs = dz[ii] / Get_Length(ii);						
					}
				}
				
			}
			else if( dz[i] > 0.0 )
			{
				hr = i * M_PI_045;
				hs = dz[i] / Get_Length(i);
			}
			
			s_facet[i] = hs;
			r_facet[i] = hr;
		}
	}
	
	dzSum		= 0.0;
	
	for(i=0; i<8; i++)
	{		
		valley[i]	= 0.0;
		ii = (i < 7)? i+1 : 0;
		
		if( s_facet[i] > 0.0 )
		{
			if( r_facet[i] > i * M_PI_045 && r_facet[i] < (i+1) * M_PI_045 )
			{
				valley[i] = s_facet[i];
			}
			else if( r_facet[i] == r_facet[ii] )
			{
				valley[i] = s_facet[i];
			}
			else if( s_facet[ii] == -999.0 && r_facet[i] == (i+1) * M_PI_045)
			{
				valley[i] = s_facet[i];
			}
			else
			{
				ii = (i > 0)? i-1 : 7;
				if( s_facet[ii] == -999.0 && r_facet[i] == i * M_PI_045 )
				{
					valley[i] = s_facet[i];
				}
			}
			
			valley[i] = pow(valley[i], MFD_Converge);
			dzSum += valley[i];
		} 
		
		portion[i] = 0.0;

	}


	if( dzSum )
	{
		for(i=0; i<8; i++)
		{
			if (i < 7)
			{
				ii = i+1;
			}
			else
			{
				ii = 0;
				if( r_facet[i] == 0.0) r_facet[i] = M_PI_360;
			}
			if( valley[i] )
			{
				valley[i] /= dzSum;
				//vb-code: portion(i) = portion(i) + valley(i) * (i * PI / 4 - r_facet(i)) / (PI / 4)
				portion[i] += valley[i] * ((i+1) * M_PI_045 - r_facet[i]) / M_PI_045;
				//vb-code: portion(ii) = portion(ii) + valley(i) * (r_facet(i) - (i - 1) * PI / 4) / (PI / 4)
				portion[ii] += valley[i] * (r_facet[i] - i * M_PI_045) / M_PI_045;
			}
		}
		for(i=0; i<8; i++)
		{
			Add_Fraction(x, y, i, portion[i] );
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//				Multiple Flow Direction					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_MFD(	int x, int y )
{
	int		i, ix, iy;

	double	z, d, dzSum, dz[8];

	z		= pDTM->asDouble(x, y);
	dzSum	= 0.0;

	for(i=0; i<8; i++)
	{
		ix		= Get_xTo(i, x);
		iy		= Get_yTo(i, y);

		if( pDTM->is_InGrid(ix, iy) )
		{
			d		= z - pDTM->asDouble(ix, iy);
		}
		else
		{
			ix		= Get_xTo(i + 4, x);
			iy		= Get_yTo(i + 4, y);

			if( pDTM->is_InGrid(ix, iy) )
			{
				d		= pDTM->asDouble(ix, iy) - z;
			}
			else
			{
				d		= 0.0;
			}
		}

		if( d > 0.0 )
		{
			//previously: dzSum	+= (dz[i]	= pow(d / Get_Length(i), MFD_Converge));
			dz[i]	= pow(d / Get_Length(i), MFD_Converge);
			dzSum	+= dz[i];
		}
		else
		{
			dz[i]	= 0.0;
		}
	}

	if( dzSum )
	{
		for(i=0; i<8; i++)
		{
			if( dz[i] )
			{
				Add_Fraction(x, y, i, dz[i] / dzSum );
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//				Braunschweiger Reliefmodell				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow_BRM.h"

#define BRM_nint(x)		((int)(x >= 0 ? x + 0.5 : x - 0.5))


//---------------------------------------------------------
void CFlow_Parallel::Set_BRM(	int x, int y )
{
	int		Dir, QBinaer,
			ix[3], iy[3],
			nexp[6];

	double	QLinks, QMitte, QRecht,
			nnei[6];

	//-----------------------------------------------------
	if( is_InGrid(x, y, 1) ) // Rand !!!
	{
		Dir	= BRM_InitRZ(x,y,ix,iy);

		if( Dir >= 0 )
		{
			if( Dir % 2 )
			{
				BRM_GetDiago(Dir,x,y,ix,iy,nnei,nexp);
				BRM_QStreuung(4,1,nnei,nexp,QBinaer,QLinks,QMitte,QRecht);
			}
			else
			{
				BRM_GetOrtho(Dir,x,y,ix,iy,nnei,nexp);
				BRM_QStreuung(6,0,nnei,nexp,QBinaer,QLinks,QMitte,QRecht);
			}

			Add_Fraction(x,y,(Dir+1)%8,BRM_BitMtrx[0][QBinaer] ? QLinks : 0);
			Add_Fraction(x,y,(Dir+0)%8,BRM_BitMtrx[1][QBinaer] ? QMitte : 0);
			Add_Fraction(x,y,(Dir+7)%8,BRM_BitMtrx[2][QBinaer] ? QRecht : 0);
		}
	}
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_Init(void)
{
	int 	i;

	double	DXT	= Get_Cellsize()/2,
			DYT	= Get_Cellsize()/2;

	//-----------------------------------------------------
	BRM_kgexp[0]	= (int)(atan2(DXT     , Get_Cellsize()) * M_RAD_TO_DEG);
	BRM_kgexp[1]	= (int)(atan2(Get_Cellsize(), DYT     ) * M_RAD_TO_DEG) + 1;
	BRM_kgexp[2]	= (int)(atan2(Get_Cellsize(),-DYT     ) * M_RAD_TO_DEG);
	BRM_kgexp[3]	= (int)(atan2(DXT     ,-Get_Cellsize()) * M_RAD_TO_DEG) + 1;

	for(i=0; i<4; i++)
		BRM_kgexp[i+4]	= BRM_kgexp[i] + 180;

	//-----------------------------------------------------
	for(i=0; i<=360; i++)
	{
		BRM_sinus[i]	= -sin(i * M_DEG_TO_RAD);
		BRM_cosin[i]	= -cos(i * M_DEG_TO_RAD);
	}

	//---BRM_idreh---------------------------------------------
	BRM_idreh[0]	= 180;
	BRM_idreh[1]	= 180 -	BRM_nint(atan2(Get_Cellsize(), Get_Cellsize()) * M_RAD_TO_DEG);
	BRM_idreh[2]	=  90;
	BRM_idreh[3]	=		BRM_nint(atan2(Get_Cellsize(), Get_Cellsize()) * M_RAD_TO_DEG);
	BRM_idreh[4]	=   0;

	for(i=1; i<4; i++)
		BRM_idreh[i+4]	= BRM_idreh[i] + 180;
}

//---------------------------------------------------------
int CFlow_Parallel::BRM_InitRZ(int x, int y, int ix[3], int iy[3])
{
	int		i, j, Dir;

	double	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	Aspect	*= M_RAD_TO_DEG;

	if( Aspect < 0 )
	{
		return( -1 );
	}

	//---Kategorisierte-Exposition-------------------------
	Dir	= 0;
	while( Aspect > BRM_kgexp[Dir] && Dir < 8 )
		Dir++;
	Dir	%=8;

    //---Finde-Die-3-ZielRasterZellen----------------------
	for(i=0; i<3; i++)	// zxy[]: 0=Recht, 1=Mitte, 2=Links
	{
		j		= (Dir + 7 + i) % 8;
		ix[2-i]	= Get_xTo(j,x);
		iy[2-i]	= Get_yTo(j,y);
	}

	return(Dir);
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_GetOrtho(int Dir, int x, int y, int ix[3], int iy[3], double nnei[6], int nexp[6])
{
	int		jx, jy, i,
			i0	= (Dir + 2) % 8,
			i4	= (Dir + 6) % 8;

	double	Slope, Aspect;

	for(i=0; i<3; i++)
	{
		jx		= ix[i];
		jy		= iy[i];

		Get_Gradient(jx, jy, Slope, Aspect);

		nnei[i]	= M_RAD_TO_DEG * Slope;
		nexp[i]	= (int)(M_RAD_TO_DEG * Aspect);
	}

	jx		= Get_xTo(i0,x);
	jy		= Get_yTo(i0,y);

	Get_Gradient(jx, jy, Slope, Aspect);

	nnei[3]	= M_RAD_TO_DEG * Slope;
	nexp[3]	= (int)(M_RAD_TO_DEG * Aspect);

	jx		= Get_xTo(i4,x);
	jy		= Get_yTo(i4,y);

	Get_Gradient(jx, jy, Slope, Aspect);

	nnei[5]	= M_RAD_TO_DEG * Slope;
	nexp[5]	= (int)(M_RAD_TO_DEG * Aspect);

	Get_Gradient(x, y, Slope, Aspect);

	nnei[4]	= M_RAD_TO_DEG * Slope;
	nexp[4]	= (int)(M_RAD_TO_DEG * Aspect);	//[jy][jx]) ????!!!!...;

	for(i=0; i<6; i++)
		if(nexp[i]<0)
			nexp[i]	= nexp[4];

	for(i=0; i<6; i++)
	{
		nexp[i]	+= BRM_idreh[Dir];

		if(nexp[i]>360)
			nexp[i]	-= 360;
	}
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_GetDiago(int Dir, int x, int y, int ix[3], int iy[3], double nnei[6], int nexp[6])
{
	int		i;

	double	Slope, Aspect;

	Get_Gradient( x   ,  y   , Slope, Aspect);
	nexp[0]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[0]	= M_RAD_TO_DEG * Slope;

	Get_Gradient(ix[0], iy[0], Slope, Aspect);
	nexp[1]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[1]	= M_RAD_TO_DEG * Slope;

	Get_Gradient(ix[2], iy[2], Slope, Aspect);
	nexp[2]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[2]	= M_RAD_TO_DEG * Slope;

	Get_Gradient(ix[1], iy[1], Slope, Aspect);
	nexp[3]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[3]	= M_RAD_TO_DEG * Slope;

	for(i=1; i<4; i++)
		if(nexp[i]<0)
			nexp[i]	= nexp[0];

	for(i=0; i<4; i++)
	{
		nexp[i]	+= BRM_idreh[Dir];

		if(nexp[i]>360)
			nexp[i]	-= 360;
	}
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_QStreuung(int i64, int g64, double nnei[6], int nexp[6], int &QBinaer, double &QLinks, double &QMitte, double &QRecht)
{
	int		i, j, ix, iy,
			ALinks, AMitte=2, ARecht;

	double	x=1, y=1, sg=0, a,
			s[6], c[6];

	ALinks	= ARecht	= 0;
	QLinks	= QRecht	= 0.0;

	for(i=0; i<i64; i++)
		sg	+= nnei[i];

	sg	= i64 / sg;

	for(i=0; i<i64; i++)
	{
		a		= sg * nnei[i];
		j		= nexp[i];
		s[i]	= a * BRM_sinus[j];
		c[i]	= a * BRM_cosin[j];
	}

    //---QLinks-ermitteln----------------------------------
	for(i=0; i<100; i++)
	{
		ix	= BRM_nint(x) - 1;
		iy	= BRM_nint(y) - 1;

		for(j=0; j<i64; j++)
		{
			a		= BRM_g[g64][j][ix][iy];
			x		+= s[j] * a;
			y		+= c[j] * a;
		}	 

		if(x<1)
		{
			ALinks	= 0;
			QLinks	= 0;
			break;
		}

		if( x>8.99 || y<1 )
		{
			ALinks	= 4;
			QLinks	= 1;			
			break;
		}

		if(y>8.95)
		{
			if(x<1.02)
			{
				ALinks	= 0;
				QLinks	= 0;
			}
			else
			{
				ALinks	= 4;
				QLinks	= (x - 1) / 8;

				if(i64==4)
				{
					if(QLinks<0.5)
						QLinks	= QLinks * (1.67 - QLinks * 1.078);
					else
						QLinks	= QLinks * 0.869 + 0.131;
				}
			}
			break;
		}
	}


    //---QRechts-ermitteln---------------------------------
	x	= 9;
	y	= 1;

	for(i=0; i<100; i++)
	{
		ix	= BRM_nint(x) - 1;
		iy	= BRM_nint(y) - 1;

		for(j=0; j<i64; j++)
		{
			a	= BRM_g[g64][j][ix][iy];
			x	+= s[j] * a;
			y	+= c[j] * a;
		}

		if(x>9)
		{
			ARecht	= 0;
			QRecht	= 0;
			break;
		}

		if( x<1.01 || y<1 )
		{
			ARecht	= 1;
			QRecht	= 1;
			break;
		}

		if(y>8.95)
		{
			if(x>8.98)
			{
				ARecht	= 0;
				QRecht	= 0;
			}
			else
			{
				ARecht	= 1;
				QRecht	= 1 - (x - 1) / 8;

				if(i64==4)					
				{
					if(QRecht<0.5)
						QRecht	= QRecht * (1.67 - QRecht * 1.078);
					else
						QRecht	= QRecht * 0.869 + 0.131;
				}
			}
			break;
		}
	}

	//---QMitte-ist-Rest-/-QBinaer'Bits-setzen-------------
	QMitte	= 1 - QLinks - QRecht;
	QBinaer	= ALinks + AMitte + ARecht;

	if(QMitte<=0.01)
	{
		a		= QLinks + QRecht;
		QLinks	/= a;
		QRecht	/= a;
		QMitte	= 0;
		QBinaer	-= 2;
	}
}
