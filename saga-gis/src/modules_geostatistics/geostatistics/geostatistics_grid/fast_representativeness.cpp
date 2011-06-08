/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              fast_representativeness.cpp              //
//                                                       //
//                   Copyright (C) 2009                  //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@saga-gis.org                   //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "fast_representativeness.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define eps 1.0e-6


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFast_Representativeness::CFast_Representativeness(void)
{
	Set_Name		(_TL("Fast Representativeness"));

	Set_Author		(SG_T("A.Ringeler (c) 2009"));

	Set_Description	(_TW(
		"A fast representativeness algorithm. Resulting seeds might be used with 'Fast Region Growing'.\n"
		"\n"
		"References:\n"
		"Boehner, J., Selige, T., Ringeler, A. (2006): Image segmentation using representativeness analysis and region growing. "
		"In: Boehner, J., McCloy, K.R., Strobl, J. [Eds.]:  SAGA – Analysis and Modelling Applications. "
		"Goettinger Geographische Abhandlungen, Vol.115, "
		"<a href=\"http://downloads.sourceforge.net/saga-gis/gga115_03.pdf\">pdf</a>\n"
	));

	Parameters.Add_Grid(	NULL, "INPUT"		, _TL("Input"),
		_TL("Input for module calculations."),
		PARAMETER_INPUT, true, SG_DATATYPE_Float
	);

	Parameters.Add_Grid(	NULL, "RESULT"		, _TL("Output"),
		_TL("Output of module calculations."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(	NULL, "RESULT_LOD", _TL("Output Lod"),
		_TL("Output of module calculations."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(	NULL, "SEEDS"		, _TL("Output Seeds"),
		_TL("Output of module calculations."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Value(	NULL, "LOD"			, _TL("Level of Generalisation"),
		_TL(""),
		PARAMETER_TYPE_Double, 16, 1
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFast_Representativeness::On_Execute(void)
{
	double Lod;
	CSG_Grid *Lod_Grid, *Seeds_Grid;


	pOrgInput	= Parameters("INPUT")->asGrid();

	pOutput		= Parameters("RESULT")->asGrid();

	Lod_Grid	= Parameters("RESULT_LOD")->asGrid();

	Seeds_Grid	= Parameters("SEEDS")->asGrid();
	Seeds_Grid	->Set_NoData_Value(0);
	Seeds_Grid	->Assign(0.0);

	Process_Set_Text(_TL("Init Fast Representativeness"));
	
	FastRep_Initialize();

	Process_Set_Text(_TL("Fast Representativeness"));
	
	FastRep_Execute();
	
	Process_Set_Text(_TL("Clear Fast Representativeness"));
	
	FastRep_Finalize();

	Lod = Parameters("LOD")->asDouble();


	CSG_Grid * GenLevel = (CSG_Grid *) new CSG_Grid(	SG_DATATYPE_Float,
											(int)(pOrgInput->Get_NX()/Lod+1),
											(int)(pOrgInput->Get_NY()/Lod+1),
											pOrgInput->Get_Cellsize()*Lod,
											pOrgInput->Get_XMin(),
											pOrgInput->Get_YMin()
										  ) ;

	CSG_Grid * GenLevelRep = (CSG_Grid *) new CSG_Grid(	SG_DATATYPE_Float,
											(int)(pOrgInput->Get_NX()/Lod+1),
											(int)(pOrgInput->Get_NY()/Lod+1),
											pOrgInput->Get_Cellsize()*Lod,
											pOrgInput->Get_XMin(),
											pOrgInput->Get_YMin()
										  ) ;

	GenLevel->Assign(pOrgInput); 


	

	pOrgInput = GenLevel;
	pOutput   = GenLevelRep;

	Process_Set_Text(_TL("Init Generalisation"));
	
	FastRep_Initialize();

	Process_Set_Text(_TL("Generalisation"));
	
	FastRep_Execute();
	
	Process_Set_Text(_TL("Clear Generalisation"));
	
	FastRep_Finalize();
	
	smooth_rep(pOutput,GenLevel );

	Lod_Grid->Assign(GenLevel);

	Find_Local_Maxima_Minima(Lod_Grid, Seeds_Grid);
	
	return( true );
}

//---------------------------------------------------------
// Repraesentanz stuff
//---------------------------------------------------------

//---------------------------------------------------------
void CFast_Representativeness::FastRep_Local_Sum(CSG_Grid *pInput, CSG_Grid **pOutput)
{
	*pOutput	= (CSG_Grid *) new CSG_Grid(SG_DATATYPE_Float,
					pInput->Get_NX()/2,
					pInput->Get_NY()/2,
					pInput->Get_Cellsize()*2,
					pInput->Get_Cellsize()*2);
	
	for (int y = 0; y < pInput->Get_NY() - 1; y += 2)
	for (int x = 0; x < pInput->Get_NX() - 1; x += 2)
	{
		float val;

		if( pInput->is_NoData(x,y) || pInput->is_NoData(x + 1, y) || pInput->is_NoData(x, y + 1) || pInput->is_NoData(x + 1, y + 1))
		{
			(*pOutput)->Set_NoData(x/2, y/2);
		}
		else
		{
			val		=	pInput->asFloat(x, y)
					+	pInput->asFloat(x + 1, y)
					+	pInput->asFloat(x, y + 1)
					+	pInput->asFloat(x + 1, y + 1);
		
			(*pOutput)->Set_Value(x/2, y/2, val);
		}
	}
}

//---------------------------------------------------------
void CFast_Representativeness::FastRep_Execute(void)
{
	int		x, y;
	for (y = 0; y < pOutput->Get_NY()&&Set_Progress(y,pOutput->Get_NY());  y++)
	{
		for (x = 0; x < pOutput->Get_NX(); x++)
		{
			if( ! pOrgInput->is_NoData(x,y ))
			pOutput->Set_Value(x, y, FastRep_Get_Laenge(x, y));
		}
	}
}

//---------------------------------------------------------
void CFast_Representativeness::FastRep_Initialize(void)
{
	
	double logNx2,logNy2;
	int pow2y, pow2x;
	int x,y;
	long i;
	long size;

	logNx2	= log((double)pOrgInput->Get_NX())/log(2.0f);
	
	pow2x	= (int) logNx2;
	
	if (fabs(logNx2 - pow2x) > eps)
		pow2x++;
	
	logNy2	= log((double)pOrgInput->Get_NY())/log(2.0f);
	
	pow2y	= (int) logNy2;
	
	if (fabs(logNy2 - pow2y) > eps)
		pow2y++;
	
	Pow2Grid	= (CSG_Grid *) new CSG_Grid(SG_DATATYPE_Double, 1 << pow2x, 1 << pow2y);
	
	for ( y = 0; y < Pow2Grid->Get_NY();  y++)
	{
		Set_Progress(y, Pow2Grid->Get_NY());
		
		for ( x = 0; x < Pow2Grid->Get_NX(); x++)
		{
			if (y<pOrgInput->Get_NY() && x < pOrgInput->Get_NX())
				Pow2Grid->Set_Value(x, y, pOrgInput->asFloat(x, y));
			else
			{
				if(x >= pOrgInput->Get_NX()&&y<pOrgInput->Get_NY())
				Pow2Grid->Set_Value(x, y,pOrgInput->asFloat(2*pOrgInput->Get_NX()-x-1, y));

				if(y >= pOrgInput->Get_NY()&& x<pOrgInput->Get_NX())
				Pow2Grid->Set_Value(x, y, pOrgInput->asFloat(x, 2* pOrgInput->Get_NY()-y-1));

				if(y >= pOrgInput->Get_NY()&& x >= pOrgInput->Get_NX())
				{
					Pow2Grid->Set_Value(x, y, pOrgInput->asFloat(2*pOrgInput->Get_NX()-x-1, 2* pOrgInput->Get_NY()-y-1));
				}
			}
		}
	}	
	
	Pow2Grid->Standardise();

	pOutput->Assign_NoData();
		
	size = (int)(log((double)Pow2Grid->Get_NX())/log(2.0)) - 1;
		
	m_deep = size;
		
	Sum[0] = Pow2Grid;
		
	for ( i = 0; i < size - 1; i++)
	{
		FastRep_Local_Sum(Sum[i], &Sum[i + 1]);
	}
	
	QSum[0]	= (CSG_Grid *)new CSG_Grid(Pow2Grid, SG_DATATYPE_Double); // datas
	QSum[0]->Assign(Pow2Grid);  
	
	for ( i = 0; i < Pow2Grid->Get_NCells() ; i++)
	{
		if (!QSum[0]->is_NoData(i))
			QSum[0]->Set_Value(i,QSum[0]->asDouble(i)*Pow2Grid->asDouble(i));
	}
	
	for (i = 0; i < size - 1; i++)
	{
		Set_Progress(i,size - 1);
		FastRep_Local_Sum(QSum[i], &QSum[i + 1]);
	}
	
	V	=(double *) malloc((m_deep + 12) * sizeof(double));
	Z	=(int    *) malloc((m_deep + 12) * sizeof(int));
	g	=(double *) malloc((m_deep + 12) * sizeof(double));
	m	=(double *) malloc((m_deep + 12) * sizeof(double));
	
	maxRadius	= 12;
	
	rLength	=(int *)malloc((maxRadius + 12) * sizeof(int));
	
	for (i = 0; i < m_deep; i++)
	{
		g[i]	= 1.0 / (Get_Cellsize() * (1 <<(i)));
	}
	
	FastRep_Init_Radius();
}

//---------------------------------------------------------
void CFast_Representativeness::FastRep_Finalize(void)
{
	free(V);
	free(Z);
	free(rLength);
	free(g);
	free(m);
	free(x_diff);
	free(y_diff);
	delete Pow2Grid;
int		i;
	for (i = 0; i < m_deep ; i++)
		delete QSum[i];

	for (i = 1; i < m_deep ; i++)
		delete Sum[i];
}

//---------------------------------------------------------
void CFast_Representativeness::FastRep_Init_Radius(void)
{
	int		k, maxZ;
	
	long	i, j, iijj, rr, r1r1, z;
	
	maxZ		= z	= 0;
	rLength[0]	= 0;
	
	x_diff		= y_diff	= NULL;
	
	for (k = 1; k <= maxRadius; k++) 
	{
		rr		= k*k;
		r1r1	=(k - 1) * (k - 1);
		
		for (i=-k; i <= k; i++)
		{
			for (j=-k; j <= k; j++) 
			{
				iijj	= i*i + j*j;
				
				if (iijj <= rr && iijj >= r1r1) 
				{
					if (maxZ <= z)
					{
						maxZ	+= 1000;
						x_diff	=(int *)realloc(x_diff, maxZ*sizeof(int));
						y_diff	=(int *)realloc(y_diff, maxZ*sizeof(int));
					}
					
					x_diff[z]	= j;
					y_diff[z]	= i;
					
					z++;
				}
			}
		}
		rLength[k]	= z;
	}
}	


//---------------------------------------------------------
double CFast_Representativeness::FastRep_Get_Laenge(int x, int y)
{
	int		Count;
	
	double	d;
	
	V[0]	= FastRep_Get_Variance(x, y, 1, 0, Count);
	Z[0]	= Count;
int ideep;
	for (ideep = 1; ideep < m_deep; ideep++)
	{
		V[ideep]	= V[ideep - 1] + FastRep_Get_Variance(x, y, 4, ideep - 1, Count);
		Z[ideep]	= Z[ideep - 1] + Count;
	}

	for (ideep = 0; ideep < m_deep; ideep++)
	{
		V[ideep]	=sqrt(V[ideep]/ (double)(Z[ideep]+1));
	}
	
	d	= FastRep_Get_Steigung();
	
	if ( d == 0.0 )
		return ( pOutput->Get_NoData_Value());
	else
		return ( V[m_deep - 1] / d / 2.0 );

}
 
//---------------------------------------------------------
double CFast_Representativeness::FastRep_Get_Variance(int x, int y, int iRadius, int deep, int &Count)
{
	int		i, ix, iy;
	
	double	Variance;
	
	double	q	= 0;
	double	s	= 0;
	int		lnr	= 0;
	
	Variance	= 0;
	
	int size	= (1 << deep) * (1 << deep);
	
	double	z	= Sum[0]->asDouble(x, y);
	
	for (i = rLength[iRadius - 1], Count = 0; i < rLength[iRadius]; i++)
	{
		ix	= x/ (1 << deep) + x_diff[i];
		if (ix < 0)
			continue; 
		else if (ix >= Sum[deep]->Get_NX())
			continue;
		
		iy	= y/ (1 << deep) + y_diff[i];
		if (iy < 0)
			continue;
		else if (iy >= Sum[deep]->Get_NY())
			continue;
		
		if( !QSum[deep]->is_NoData(ix, iy))
		{
			Count	+=	size;
			lnr		+=	size;
	
			q	+=	QSum[deep]->asDouble(ix, iy);
			s	+=	Sum[deep]->asDouble(ix, iy);
		}
	}
	
	Variance = q + z* (-s - s +lnr*z);

	if ( Variance < 0.0 ) Variance = 0.0; 

	return ( Variance );
}

//---------------------------------------------------------
double CFast_Representativeness::FastRep_Get_Steigung()
{
	int		i;
	double	summe_mg, summe_g;
	
	m[0]		= V[0] / Get_Cellsize();

	for (i = 1; i < m_deep; i++)
	{
		m[i]	=( V[i] - V[i-1] ) / ( Get_Cellsize()* (1 << (i) ) );
	}
	
	summe_mg	=	summe_g	= 0;
	
	for (i = 0; i < m_deep; i++)
	{
		summe_mg	+= m[i] * g[i];
		summe_g		+= g[i];
	}

	return ( summe_mg / summe_g );
}

//---------------------------------------------------------
void CFast_Representativeness::smooth_rep(CSG_Grid * in, CSG_Grid * out)
{
	int x,y;
	int i,j;
	int xpos, ypos;
	double val;
	int count;

	for (y = 0; y < in->Get_NY(); y++)
		for (x = 0; x < in->Get_NX(); x++)
		{
			val = 0.0;
			count = 0;
			for (i=-3; i<= 3; i++)
			for (j=-3; j<= 3; j++)
			{
				xpos = x + j;
				ypos = y + i;

				if( in->is_InGrid(xpos,ypos))
				{
					val += in->asDouble(xpos , ypos);
					count ++;
				}
			}
		
			out ->Set_Value(x,y, val/(double) count );
		}

}

//---------------------------------------------------------
void CFast_Representativeness::Find_Local_Maxima_Minima(CSG_Grid * in, CSG_Grid * out)
{
	int x,y;
	int i,j;
	int xpos, ypos;
	
	double max, min;
	
	bool ismax ,ismin;
	bool hasmax;

	for (y = 2; y < in->Get_NY()-2; y++)
		for (x = 2; x < in->Get_NX()-2; x++)
		{
			ismax = true;
			ismin = true;
			hasmax =false;

			max =min = in->asDouble(x , y);
			
			for (i=-2; i<= 2; i++)
			for (j=-2; j<= 2; j++)
			{
				xpos = x + j;
				ypos = y + i;

				if (max < in->asDouble(xpos , ypos))
				ismax= false;

				if (min > in->asDouble(xpos , ypos))
				ismin= false;

				if(out ->asInt(xpos, ypos))
				hasmax=true;
			}
		
			if (hasmax) 
			
			{
				ismin = ismax =false; 
			}

			if( ismax||ismin )
			{
				out ->Set_Value(x,y, 1);
			}
			else
			{
				out ->Set_NoData(x,y);
			}
		}
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
