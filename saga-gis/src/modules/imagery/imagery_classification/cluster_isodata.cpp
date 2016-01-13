
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                imagery_classification                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  cluster_isodata.cpp                  //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "cluster_isodata.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "cluster_isodata.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define EPSILON	1.e-12


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCluster_ISODATA::CCluster_ISODATA(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CCluster_ISODATA::CCluster_ISODATA(size_t nFeatures, TSG_Data_Type Data_Type)
{
	_On_Construction();

	Create(nFeatures, Data_Type);
}

//---------------------------------------------------------
bool CCluster_ISODATA::Create(size_t nFeatures, TSG_Data_Type Data_Type)
{
	Destroy();

	switch( Data_Type )
	{
	case SG_DATATYPE_Bit   :
		m_Data_Type	= SG_DATATYPE_Byte;
		break;

	case SG_DATATYPE_Byte  :
	case SG_DATATYPE_Char  :
	case SG_DATATYPE_Word  :
	case SG_DATATYPE_Short :
	case SG_DATATYPE_DWord :
	case SG_DATATYPE_Int   :
	case SG_DATATYPE_ULong :
	case SG_DATATYPE_Long  :
	case SG_DATATYPE_Float :
	case SG_DATATYPE_Double:
		m_Data_Type	= Data_Type;
		break;

	default:
		return( false );
	}

	m_nFeatures	= nFeatures;

	m_Data.Create(m_nFeatures * SG_Data_Type_Get_Size(m_Data_Type), 0, SG_ARRAY_GROWTH_2);

	return( m_nFeatures > 0 );
}

//---------------------------------------------------------
CCluster_ISODATA::~CCluster_ISODATA(void)
{
	Destroy();
}

//---------------------------------------------------------
void CCluster_ISODATA::_On_Construction(void)
{
	m_nFeatures		= 0;
	m_Data_Type		= SG_DATATYPE_Float;

	m_maxIterations	= 999;	// maximum number of iterations

	m_nCluster_Max	= 16;	// maximum number of clusters
	m_nCluster_Ini	= 0;	// number of initial cluster centers

	m_nSamples_Min	= 1;	// minimum number of samples in cluster
	m_StdDev_Max	= 3.0;	// maximum standard deviation of one cluster
	m_Distance_Max	= 2.0;	// maximum distance to merge clusters
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCluster_ISODATA::Destroy(void)
{
	cl_m	.Destroy();	// m_nCluster_Max cluster number of members
	cl_d	.Destroy();	// m_nCluster_Max cluster average distance
	cl_ms	.Destroy();	// m_nCluster_Max cluster maximum standard deviation
	cl_msc	.Destroy();	// m_nCluster_Max cluster maximum standard deviation component

	cl_s	.Destroy();	// m_nCluster_Max*m_nFeatures cluster centers
	cl_c	.Destroy();	// m_nCluster_Max*m_nFeatures cluster standard deviations

	data_cl	.Destroy();	// nSamples data cluster ids
	data_d	.Destroy();	// nSamples data cluster distances

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCluster_ISODATA::Add_Sample(const double *Sample)
{
	if( m_Data.Inc_Array() )
	{
		void	*Data	= m_Data.Get_Entry(m_Data.Get_Size() - 1);

		for(size_t iFeature=0; iFeature<m_nFeatures; iFeature++)
		{
			switch( m_Data_Type )
			{
			case SG_DATATYPE_Byte  :	((BYTE   *)Data)[iFeature]	= (BYTE  )Sample[iFeature];	break;
			case SG_DATATYPE_Char  :	((char   *)Data)[iFeature]	= (char  )Sample[iFeature];	break;
			case SG_DATATYPE_Word  :	((WORD   *)Data)[iFeature]	= (WORD  )Sample[iFeature];	break;
			case SG_DATATYPE_Short :	((short  *)Data)[iFeature]	= (short )Sample[iFeature];	break;
			case SG_DATATYPE_DWord :	((DWORD  *)Data)[iFeature]	= (DWORD )Sample[iFeature];	break;
			case SG_DATATYPE_Int   :	((int    *)Data)[iFeature]	= (int   )Sample[iFeature];	break;
			case SG_DATATYPE_ULong :	((uLong  *)Data)[iFeature]	= (uLong )Sample[iFeature];	break;
			case SG_DATATYPE_Long  :	((sLong  *)Data)[iFeature]	= (sLong )Sample[iFeature];	break;
			case SG_DATATYPE_Float :	((float  *)Data)[iFeature]	= (float )Sample[iFeature];	break;
			default                :	((double *)Data)[iFeature]	= (double)Sample[iFeature];	break;
			}
		}


		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline double CCluster_ISODATA::_Get_Sample(size_t iSample, size_t iFeature)
{
	void	*Sample	= m_Data.Get_Entry(iSample);

	switch( m_Data_Type )
	{
	case SG_DATATYPE_Byte  :	return( ((BYTE   *)Sample)[iFeature] );
	case SG_DATATYPE_Char  :	return( ((char   *)Sample)[iFeature] );
	case SG_DATATYPE_Word  :	return( ((WORD   *)Sample)[iFeature] );
	case SG_DATATYPE_Short :	return( ((short  *)Sample)[iFeature] );
	case SG_DATATYPE_DWord :	return( ((DWORD  *)Sample)[iFeature] );
	case SG_DATATYPE_Int   :	return( ((int    *)Sample)[iFeature] );
	case SG_DATATYPE_ULong :	return( ((uLong  *)Sample)[iFeature] );
	case SG_DATATYPE_Long  :	return( ((sLong  *)Sample)[iFeature] );
	case SG_DATATYPE_Float :	return( ((float  *)Sample)[iFeature] );
	default                :	return( ((double *)Sample)[iFeature] );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCluster_ISODATA::Set_Max_Iterations(size_t Value)
{
	if( Value < 3 )	// maximum number of iterations must be more than 3
	{
		return( false );
	}

	m_maxIterations	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCluster_ISODATA::Set_Ini_Clusters(size_t Value)
{
	m_nCluster_Ini	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCluster_ISODATA::Set_Max_Clusters(size_t Value)
{
	if( Value < 2 )
	{
		return( false );
	}

	m_nCluster_Max	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCluster_ISODATA::Set_Min_Samples(size_t Value)
{
	if( Value < 1 )	// minimum number of samples in cluster must be more than 1
	{
		return( false );
	}

	m_nSamples_Min	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCluster_ISODATA::Set_Max_StdDev(double d)
{
	if( d < 0.0 )	// maximum standard deviation of one cluster must be a positive real
	{
		return( false );
	}

	m_StdDev_Max	= d;

	return( true );
}

//---------------------------------------------------------
bool CCluster_ISODATA::Set_Max_Distance(double d)
{
	if( d < 0.0 )	// maximum distance to merge clusters must be a positive real
	{
		return( false );
	}

	m_Distance_Max	= d;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCluster_ISODATA::_Initialize(void)
{
	//-----------------------------------------------------
	if( Get_Feature_Count() < 1 )
	{
		SG_UI_Msg_Add_Error(_TL("attributes must be more than 1"));

		return( false );
	}

	if( Get_Sample_Count() <= 1 )
	{
		SG_UI_Msg_Add_Error(_TL("samples must be more than 1"));

		return( false );
	}

	if( m_nCluster_Max < 2 || m_nCluster_Max >= Get_Sample_Count() )
	{
		SG_UI_Msg_Add_Error(_TL("maximum number of clusters must be more than 2 and less than number of samples"));

		return( false );
	}

	if( m_nCluster_Ini >= m_nCluster_Max )
	{
		SG_UI_Msg_Add_Error(_TL("maximum number of initial cluster centers must be more or equal to 0 and less than number of clusters"));

		return( false );
	}

	//-----------------------------------------------------
//	int	n	= m_nCluster_Max << 1;	// := m_nCluster_Max * 2;

	if( !data_cl.Create(Get_Sample_Count()) )	{	return( false );	}
	if( !data_d .Create(Get_Sample_Count()) )	{	return( false );	}

	if( !cl_m  .Create(m_nCluster_Max << 1) )	{	return( false );	}
	if( !cl_d  .Create(m_nCluster_Max << 1) )	{	return( false );	}
	if( !cl_ms .Create(m_nCluster_Max << 1) )	{	return( false );	}
	if( !cl_msc.Create(m_nCluster_Max << 1) )	{	return( false );	}

	if( !cl_c.Create(m_nFeatures, m_nCluster_Max << 1) )	{	return( false );	}
	if( !cl_s.Create(m_nFeatures, m_nCluster_Max << 1) )	{	return( false );	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCluster_ISODATA::Run(void)
{
	//-----------------------------------------------------
	if( !_Initialize() )
	{
		Destroy();

		return( false );
	}

	//-----------------------------------------------------
	size_t	iSample, iFeature, iCluster;
	
	//-----------------------------------------------------
	// Step 1

	m_nCluster	= m_nCluster_Ini;

	if( m_nCluster > 0 )
	{
		cl_c.Assign(0.0);

		for(iCluster=0; iCluster<m_nCluster; iCluster++)
		{
			cl_m[iCluster]	= 0;
		}

		for(iSample=0; iSample<Get_Sample_Count(); iSample++)
		{
			cl_m[iCluster = iSample % m_nCluster]	++;

			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				cl_c[iCluster][iFeature]	+= _Get_Sample(iSample, iFeature);
			}
		}

		for(iCluster=0; iCluster<m_nCluster; iCluster++)
		{
			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				cl_c[iCluster][iFeature]	/= cl_m[iCluster];
			}
		}
	}
	else // if( m_nCluster == 0 )
	{
		m_nCluster	= 1;

		for(iFeature=0; iFeature<m_nFeatures; iFeature++)
		{
			cl_c[0][iFeature]	= _Get_Sample(0, iFeature);
		}
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Text(CSG_String::Format("%s: 1", _TL("pass")));

	for(size_t Iteration=1; Iteration<=m_maxIterations && SG_UI_Process_Get_Okay(true); Iteration++)
	{
		//-------------------------------------------------
		// Step 2
		for(iCluster=0; iCluster<m_nCluster; iCluster++)
		{
			cl_m[iCluster]	= 0;
		}

		for(iSample=0; iSample<Get_Sample_Count(); iSample++)
		{
			data_d [iSample]	=  _Get_Sample_Distance(iSample, 0);
			data_cl[iSample]	= 0;

			for(iCluster=1; iCluster<m_nCluster; iCluster++)
			{
				double	Distance	= _Get_Sample_Distance(iSample, iCluster);

				if( Distance < data_d[iSample] )
				{
					data_d [iSample]	= Distance;
					data_cl[iSample]	= iCluster;
				}
			}

			cl_m[data_cl[iSample]]++;
		}

		//-------------------------------------------------
		// Step 3
		for(iCluster=0; iCluster<m_nCluster; iCluster++)
		{
			if( cl_m[iCluster] < (int)m_nSamples_Min )
			{
				for(iSample=0; iSample<Get_Sample_Count(); iSample++)
				{
					if( data_cl[iSample] == iCluster )
					{
						data_cl[iSample]	= iCluster > 0 ? iCluster - 1 : iCluster + 1;
						data_d [iSample]	= _Get_Sample_Distance(iSample, data_cl[iSample]);
					}
				}

				for(size_t jCluster=iCluster; jCluster<m_nCluster-1; jCluster++)
				{
					cl_m[jCluster]	= cl_m[jCluster + 1];
				}

				m_nCluster--;
			}
		}

		//-------------------------------------------------
		// Step 4
		cl_c.Assign(0.0);

		for(iSample=0; iSample<Get_Sample_Count(); iSample++)
		{
			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				cl_c[data_cl[iSample]][iFeature]	+= _Get_Sample(iSample, iFeature);
			}
		}

		for(iCluster=0; iCluster<m_nCluster; iCluster++)
		{
			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				cl_c[iCluster][iFeature]	/= cl_m[iCluster];
			}
		}

		//-------------------------------------------------
		// Step 5
		cl_d.Assign(0.0);

		for(iSample=0; iSample<Get_Sample_Count(); iSample++)
		{
			cl_d[data_cl[iSample]]	+= _Get_Sample_Distance(iSample, data_cl[iSample]);
		}

		for(iCluster=0; iCluster<m_nCluster; iCluster++)
		{
			cl_d[iCluster]	/= cl_m[iCluster];
		}

		//-------------------------------------------------
		// Step 6
		for(iSample=0, m_Distance=0.0; iSample<Get_Sample_Count(); iSample++)
		{
			m_Distance	+= _Get_Sample_Distance(iSample, data_cl[iSample]);
		}

		m_Distance	/= Get_Sample_Count();

		//-------------------------------------------------
		// Step 7
		if( m_nCluster <= m_nCluster_Max / 2 )	// too few clusters
		{
			//---------------------------------------------
			// Step 8
			cl_s.Assign(0.0);

			for(iSample=0; iSample<Get_Sample_Count(); iSample++)
			{
				for(iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					cl_s[data_cl[iSample]][iFeature]	+= SG_Get_Square(_Get_Sample(iSample, iFeature) - cl_c[data_cl[iSample]][iFeature]);
				}
			}

			for(iCluster=0; iCluster<m_nCluster; iCluster++)
			{
				for(iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					cl_s[iCluster][iFeature]	= sqrt(cl_s[iCluster][iFeature] / cl_m[iCluster]);
				}
			}

			//---------------------------------------------
			// Step 9
			for(iCluster=0; iCluster<m_nCluster; iCluster++)
			{
				for(iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					if( iFeature == 0 || cl_s[iCluster][iFeature] > cl_ms[iCluster] )
					{
						cl_ms [iCluster]	= cl_s[iCluster][iFeature];	// cluster maximum standard deviation
						cl_msc[iCluster]	=                iFeature ;	// cluster maximum standard deviation component
					}
				}
			}

			//---------------------------------------------
			// Step 10
			for(iCluster=0; iCluster<m_nCluster; iCluster++)
			{
				//cl_c: cluster center
				//cl_d: cluster average distance
				//cl_m: cluster number of members
				//m_StdDev_Max: maximum standard deviation of cluster
				//m_Distance: overall average distance

				if( /*cl_ms[iCluster] > m_StdDev_Max &&*/ cl_d[iCluster] > m_Distance - EPSILON && (size_t)cl_m[iCluster] > 2 * m_nSamples_Min )
				{
					for(iFeature=0; iFeature<m_nFeatures; iFeature++)
					{
						if( iFeature - cl_msc[iCluster] )
						{
							cl_c[m_nCluster][iFeature]	= cl_c[iCluster][iFeature];
						}
						else
						{
							cl_c[m_nCluster][iFeature]	= cl_c[iCluster][iFeature] + cl_ms[iCluster];
							cl_c[iCluster][iFeature]	= cl_c[iCluster][iFeature] - cl_ms[iCluster];
						}
					}

					m_nCluster	++;
				//	Iteration	--;
				}
			}
		}

		//-------------------------------------------------
		// Step 7
		else if( m_nCluster > m_nCluster_Max )	// many clusters
		{
			//---------------------------------------------
			// Step 11
			int	nPairs	= m_nCluster * (m_nCluster - 1) / 2;	// number of pairwise distances

			CSG_Vector		cl_pd(nPairs);
			CSG_Array_Int	cl_pi(nPairs), cl_pj(nPairs);

			for(iCluster=0, nPairs=0; iCluster<m_nCluster-1; iCluster++)
			{
				for(size_t jCluster=iCluster+1; jCluster<m_nCluster; jCluster++, nPairs++)
				{
					cl_pi[nPairs]	= iCluster;
					cl_pj[nPairs]	= jCluster;
					cl_pd[nPairs]	= _Get_Cluster_Distance(iCluster, jCluster);
				}
			}

			//---------------------------------------------
			// Step 12
			for(int i=0; i<nPairs-1; i++)
			{
				for(int j=i+1; j<nPairs; j++)
				{
					if( cl_pd[i] > cl_pd[j] )
					{
						{	double t = cl_pd[i]; cl_pd[i] = cl_pd[j]; cl_pd[j] = t;	}
						{	int    t = cl_pi[i]; cl_pi[i] = cl_pi[j]; cl_pi[j] = t;	}
						{	int    t = cl_pj[i]; cl_pj[i] = cl_pj[j]; cl_pj[j] = t;	}
					}
				}
			}

			//---------------------------------------------
			// Step 13: Notice: Smallest cluster pair below m_Distance_Max will be merged
			CSG_Vector	t(m_nFeatures);

			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				t[iFeature]	= cl_c[cl_pi[0]][iFeature] * cl_m[cl_pi[0]] + cl_c[cl_pj[0]][iFeature] * cl_m[cl_pj[0]];
			}

			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				cl_c[cl_pi[0]][iFeature]	= t[iFeature];
			}

			for(iCluster=cl_pj[0]; iCluster<m_nCluster-1; iCluster++)
			{
				for(iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					cl_c[iCluster][iFeature]	= cl_c[iCluster + 1][iFeature];
				}
			}
		}

		//-------------------------------------------------
		// Step 14: Additional criterion by Ch Iossif

		double	d	= 0.0;

		for(iSample=0; iSample<Get_Sample_Count(); iSample++)
		{
			d	+= _Get_Sample_Distance(iSample, data_cl[iSample]);
		}

		d	= fabs(d - m_Distance * Get_Sample_Count());

		if( d < EPSILON )
		{
			return( true );
		}

		SG_UI_Process_Set_Text(CSG_String::Format("%s: %d (%f)", _TL("pass"), Iteration + 1, d / Get_Sample_Count()));
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCluster_ISODATA::_Get_Sample_Distance(int iSample, int iCluster)
{
	double	s	= 0.0;

	for(size_t iFeature=0; iFeature<m_nFeatures; iFeature++)
	{
		s	+= SG_Get_Square(_Get_Sample(iSample, iFeature) - cl_c[iCluster][iFeature]);
	}

	return( sqrt(s) );
}

//---------------------------------------------------------
double CCluster_ISODATA::_Get_Cluster_Distance(int iCluster, int jCluster)
{
	double	s	= 0.0;

	for(size_t iFeature=0; iFeature<m_nFeatures; iFeature++)
	{
		s	+= SG_Get_Square(cl_c[jCluster][iFeature] - cl_c[iCluster][iFeature]);
	}

	return( sqrt(s) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
///* isodata v0.0.0a1 Ver.: ?? GPLv3 (c) 02/10 Ch Iossif <chiossif@yahoo.com> */
///*
//   Isodata unsupervised classification - clustering.
//
//   Name:			isodata
//   Version:		    0.0.0a1
//   Copyright:	    Ch Iossif @ 2010
//   Author:		    Christos Iosifidis
//   Date:			27/02/10 12:20
//   Modified:		27/02/10 12:20 - 16:35 - 586 lines of code !!! but bug before line 240
//                    31/03/10 12:00 - 18:43
//   Description:     This program executes the Isodata unsupervised
//                    classification - clustering algorithm. Isodata
//                    stands for Iterative Self-Organizing Data Analysis
//                    Techniques. This is a more sophisticated algorithm
//                    which allows the number of clusters to be
//                    automatically adjusted during the iteration by
//                    merging similar clusters and splitting clusters
//                    with large standard deviations.
//   [ http://fourier.eng.hmc.edu/e161/lectures/classification/node13.html ]
//
//   Usage:           >isodata parameter_file
//   Input:           Parameter filename at command line:
//
//#This is the Parameter file for isodata
//9 number of samples
//2 number of attributes
//1.0 1.0 initial attributes factors
//isodata_in.txt input data file name
//isodata_out.txt output data file name
//2 maximum number of clusters
//999 maximum number of iterations
//1 minumum number of samples in cluster
//3.0 maximum standard deviation of one cluster
//2.0 maximum distance to merge clusters
//0 number of initial cluster centers
//#on number of initial cluster centers > 0
//1 number of initial cluster centers
//1.5 1.5 cluster center
//
//                     Input data in isodata_in.txt:
// 1  1
// 2  2
// 3  3
//-1 -1
//-1  1
// 1 -1
// 2  3
// 3  2
// 0  2
//
//   Output:
//                        Output data in isodata_out.txt:
//1
//2
//2
//1
//1
//1
//2
//2
//1
//
//   License:
//
//   Copyright (C) Feb 2010 Ch Iossif <chiossif@yahoo.com>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//*/
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//
//int n;             /* number of samples */
//int m;             /* number of attributes */
//double *p;         /* m initial attributes factors */
//char fname_in[BUFSIZ];    /* input data file name */
//char fname_out[BUFSIZ];   /* output data file name */
//double **data;     /* nxm input data */
//int k;             /* maximum number of clusters */
//int iter;          /* maximum number of iterations */
///*int p;*/         /* maximum number of pairs of cluster which can be merged  */
//int th_n;          /* minimum number of samples in cluster */
//double th_s;       /* maximum standard deviation of one cluster */
//double th_c;       /* maximum distance to merge clusters */
//int l;             /* number of initial cluster centers */
//double **cl_c;     /* kxm cluster centers */
//int *cl_m;         /* m cluster number of members */
//double *cl_d;      /* k cluster average distance */
//double **cl_s;     /* kxm cluster standard deviations */
//double *cl_ms;     /* k cluster maximum standard deviation */
//int *cl_msc;       /* k cluster maximum standard deviation component */
//double *cl_pd;     /* k*(k-1)/2 pairwise distances */
//int *cl_pi;        /* k*(k-1)/2 pairwise distance cluster i */
//int *cl_pj;        /* k*(k-1)/2 pairwise distance cluster j */
//int pdn;           /* k*(k-1)/2 number of pairwise distances */
//int *data_cl;      /* n data cluster ids */
//double *data_d;    /* n data cluster distances */
//double oad, poad;  /* overall average distance */
//
//#define abs(x) (((x)<0)?-(x):(x))
//#define EPSILON 1.e-12
//
//void error(const char *);
//double distance(double *, double *);
//double sqr(double);
//double sqrt(double);
//void display_memory(const char *);
//
//int main(int argc, char **argv) {
//    FILE *fp;
//    char buff[BUFSIZ];
//    int temp_int, *tmp_int_vector;
//    double temp_double, *tmp_double_vector;
//    register int i, j, o;
//
//    if (argc==2) {
//        /* read parameter file */
//        if ((fp=fopen(argv[1],"r"))==NULL)
//            error("bad file name at command line");
//        /* read number of samples */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%d",&n)!=1)
//            error("bad string read");
//        if (n<1)
//            error("samples must be more than 1");
//        /* read number of attributes */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%d",&m)!=1)
//            error("bad string read");
//        if (m<1)
//            error("attributes must be more than 1");
//        /* read m initial attributes factors */
//        if ((p=malloc(m*sizeof(double)))==NULL)
//            error("bad memory allocation");
//        for (i=0; i<m; i++)
//            if (fscanf(fp,"%lf",&p[i])!=1)
//                error("bad factor read");
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        /* read input data file name */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%s",fname_in)!=1)
//            error("bad string read");
//        /* read output data file name */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%s",fname_out)!=1)
//            error("bad string read");
//        /* read maximum number of clusters */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%d",&k)!=1)
//            error("bad string read");
//        if (k<2 && k<n)
//            error("maximum number of clusters must be more than 2 and less than number of samples");
//        /* read maximum number of iterations */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%d",&iter)!=1)
//            error("bad string read");
//        if (iter<3)
//            error("maximum number of iterations must be more than 3");
//        /* read minimum number of samples in cluster */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%d",&th_n)!=1)
//            error("bad string read");
//        if (th_n<1)
//            error("minimum number of samples in cluster must be more than 1");
//        /* maximum standard deviation of one cluster */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%lf",&th_s)!=1)
//            error("bad string read");
//        if (th_s<0.0)
//            error("maximum standard deviation of one cluster must be a positive real");
//        /* maximum distance to merge clusters */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%lf",&th_c)!=1)
//            error("bad string read");
//        if (th_c<0.0)
//            error("maximum distance to merge clusters must be a positive real");
//        /* number of initial cluster centers */
//        do
//            if (fgets(buff,BUFSIZ,fp)!=buff)
//                error("bad read");
//        while (buff[0]=='#');
//        if (sscanf(buff,"%d",&l)!=1)
//            error("bad string read");
//        if (l<0 || l>k)
//            error("maximum number of initial cluster centers must be more or equal to 0 and less than number of clusters");
//        if ((cl_c=malloc(k*sizeof(double*)))==NULL)
//            error("bad memory allocation");
//        for (i=0; i<k; i++)
//            if ((cl_c[i]=malloc(m*sizeof(double)))==NULL)
//                error("bad memory allocation");
//        if (l) {
//            for (i=0; i<l; i++)
//                for (j=0; j<m; j++)
//                    if (fscanf(fp,"%lf",&cl_c[i][j])!=1)
//                        error("bad cluster center read");
//        }
//        fclose(fp);
//
//        /* rest of memory allocations */
//        k<<1;
//        pdn=k*(k-1)/2;
//        if ((cl_m=malloc(k*sizeof(int)))==NULL)
//            error("bad memory allocation");
//        if ((cl_d=malloc(k*sizeof(double)))==NULL)
//            error("bad memory allocation");
//        if ((cl_pd=malloc(pdn*sizeof(double)))==NULL)
//            error("bad memory allocation");
//        if ((cl_pi=malloc(pdn*sizeof(int)))==NULL)
//            error("bad memory allocation");
//        if ((cl_pj=malloc(pdn*sizeof(int)))==NULL)
//            error("bad memory allocation");
//        if ((cl_ms=malloc(k*sizeof(double)))==NULL)
//            error("bad memory allocation");
//        if ((cl_msc=malloc(k*sizeof(int)))==NULL)
//            error("bad memory allocation");
//        if ((cl_s=malloc(k*sizeof(double*)))==NULL)
//            error("bad memory allocation");
//        for (i=0; i<k; i++)
//            if ((cl_s[i]=malloc(m*sizeof(double)))==NULL)
//                error("bad memory allocation");
//        if ((tmp_double_vector=malloc(k*sizeof(double)))==NULL)
//            error("bad memory allocation");
//        if ((tmp_int_vector=malloc(k*sizeof(int)))==NULL)
//            error("bad memory allocation");
//        k>>1;
//        if ((data=malloc(n*sizeof(double*)))==NULL)
//            error("bad memory allocation");
//        for (i=0; i<n; i++)
//            if ((data[i]=malloc(m*sizeof(double)))==NULL)
//                error("bad memory allocation");
//        if ((data_cl=malloc(n*sizeof(int)))==NULL)
//            error("bad memory allocation");
//        if ((data_d=malloc(n*sizeof(double)))==NULL)
//            error("bad memory allocation");
//    } else
//        error("usage is >isodata parameter_file");
//
//    /* *** report input parameters *** */
//    /* ***
//    printf("number of samples is %d\n",n);
//    printf("number of attributes is %d\n",m);
//    printf("initial attributes factors are :");
//    for (i=0;i<m;i++)
//        printf(" %lf",p[i]);
//    printf("\ninput data file name is %s\n",fname_in);
//    printf("output data file name is %s\n",fname_out);
//    printf("maximum number of clusters is %d\n",k);
//    printf("maximum number of iterations is %d\n",iter);
//    printf("minimum number of samples in cluster is %d\n",th_n);
//    printf("maximum standard deviation of one cluster is %lf\n",th_s);
//    printf("maximum distance to merge clusters is %lf\n",th_c);
//    printf("number of initial cluster centers is %d\n",l);
//    if (l) {
//        printf("initial cluster centers are :\n");
//        for (i=0;i<l;i++) {
//            for (j=0;j<m;j++)
//                printf("%lf ",cl_c[i][j]);
//            printf("\n");
//        }
//    }
//    *** */
//    /* read data */
//    if ((fp=fopen(fname_in,"r"))==NULL)
//        error("bad input file name");
//    for (i=0; i<n; i++)
//        for (j=0; j<m; j++)
//            fscanf(fp,"%lf ",&data[i][j]);
//    fclose(fp);
//    /* *** report input data *** */
//    /* ***
//    printf("input data are:\n");
//    for (i=0; i<n; i++)
//        for (j=0; j<m; j++)
//            printf("i=%4d/%4d j=%4d/%4d - %lf\n",i+1,n,j+1,m,data[i][j]);
//    *** */
//
//    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//     * ISODATA algorithm. Isodata stands for Iterative Self-Organizing Data Analysis Techniques. *
//     * This is a more sophisticated algorithm which allows the number of clusters to be          *
//     * automatically adjusted during the iteration by merging similar clusters and splitting     *
//     * clusters with large standard deviations.                                                  *
//     *                  [ http://fourier.eng.hmc.edu/e161/lectures/classification/node13.html ]  *
//     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//
//    /* Step 1 */
//    if (!l) {
//        l=1;
//        for (j=0; j<m; j++) {
//            cl_c[0][j]=data[0][j];
//        }
//    }
//    display_memory("Step 1");
//
//    while (iter) {
//        /* Step 2 */
//        for (j=0; j<l; j++) {
//            cl_m[j]=0;
//        }
//        for (i=0; i<n; i++) {
//            data_d[i]=distance(data[i],cl_c[0]);
//            data_cl[i]=0;
//            for (j=1; j<l; j++) {
//                if ((temp_double=distance(data[i],cl_c[j]))<data_d[i]) {
//                    data_d[i]=temp_double;
//                    data_cl[i]=j;
//                }
//            }
//            cl_m[data_cl[i]]++;
//        }
//        display_memory("Step 2");
//
//        /* Step 3 */
//        for (j=0; j<l; j++) {
//            if (cl_m[j]<th_n) {
//                if (j) {
//                    for (i=0; i<n; i++) {
//                        if (data_cl[i]==j) {
//                            data_cl[i]=j-1;
//                            data_d[i]=distance(data[i],cl_c[j-1]);
////                        cl_m[j-1]++;
//                        }
//                    }
//                } else {
//                    for (i=0; i<n; i++) {
//                        if (data_cl[i]==j) {
//                            data_cl[i]=j+1;
//                            data_d[i]=distance(data[i],cl_c[j+1]);
////                        cl_m[j+1]++;
//                        }
//                    }
//                }
//                for (i=j+1; i<l; i++) {
//                    cl_m[i-1]=cl_m[i];
////                    for (o=0;o<m;o++) {
////                        cl_c[i-1][o]=cl_c[i][o];
////                    }
//                }
//                l--;
//            }
//        }
//        display_memory("Step 3");
//
//        /* Step 4 */
//        for (i=0; i<n; i++) {
//            for (j=0; j<l; j++) {
//                cl_c[j][i]=0.0;
//            }
//        }
//        for (i=0; i<n; i++) {
//            for (j=0; j<m; j++) {
//                cl_c[data_cl[i]][j]+=data[i][j];
//            }
//        }
//        for (i=0; i<l; i++) {
//            for (j=0; j<m; j++) {
//                cl_c[i][j]/=cl_m[i];
//            }
//        }
//        display_memory("Step 4");
//
//        /* Step 5 */
//        for (j=0; j<l; j++) {
//            cl_d[j]=0.0;
//        }
//        for (i=0; i<n; i++) {
//            cl_d[data_cl[i]]+=distance(data[i],cl_c[data_cl[i]]);
//        }
//        for (i=0; i<l; i++) {
//            cl_d[i]/=cl_m[i];
//        }
//        display_memory("Step 5");
//
//        /* Step 6 */
//        poad=0.0;
//        for (i=0; i<n; i++) {
//            poad+=distance(data[i],cl_c[data_cl[i]]);
//        }
//        oad=poad/n;
//        display_memory("Step 6");
//
//        /* Step 7 */
//        display_memory("Step 7");
//        if (l<=k/2) { // too few clusters
//
//            /* Step 8 */
//            for (i=0; i<n; i++) {
//                for (j=0; j<l; j++) {
//                    cl_s[j][i]=0.0;
//                }
//            }
//            for (i=0; i<n; i++) {
//                for (j=0; j<m; j++) {
//                    cl_s[data_cl[i]][j]+=sqr(data[i][j]-cl_c[data_cl[i]][j]);
//                }
//            }
//            for (i=0; i<l; i++) {
//                for (j=0; j<m; j++) {
//                    cl_s[i][j]/=cl_m[i];
//                }
//            }
//            for (i=0; i<l; i++) {
//                for (j=0; j<m; j++) {
//                    cl_s[i][j]=sqrt(cl_s[i][j]);
//                }
//            }
//            display_memory("Step 8");
//
//            /* Step 9 */
//            for (i=0; i<l; i++) {
//                for (j=0; j<m; j++) {
//                    if (j==0||cl_s[i][j]>cl_ms[i]) {
//                        cl_ms[i]=cl_s[i][j];
//                        cl_msc[i]=j;
//                    }
//                }
//            }
//            display_memory("Step 9");
//
//            /* Step 10 */
//            for (i=0; i<l; i++) {
//                /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//                /*
//                i: cluster iteration
//                l: cluster number
//                j: attribute iteration
//                m: number of attributes
//                cl_msc: cluster maximum standard deviation component
//                cl_c: cluster center
//                cl_ms: cluster maximum standard deviation
//                th_s: maximum standard deviation of cluster
//                cl_d: cluster average distance
//                cl_m: cluster number of members
//                th_n: minimum number of samples in cluster
//                oad: overall average distance
//                iter: number of iterations
//                */
//
//                if (/*cl_ms[i]>th_s ? &&*/  cl_d[i]>oad-EPSILON && cl_m[i]>2*th_n) {
//                    for (j=0; j<m; j++) {
//                        if (j-cl_msc[i]) {
//                            cl_c[l][j]=cl_c[i][j];
//                        } else {
//                            cl_c[l][j]=cl_c[i][j]+cl_ms[i];
//                            cl_c[i][j]=cl_c[i][j]-cl_ms[i];
//                        }
//                    }
//                    l++;
//                    iter++;
//                }
//                /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//            }
//            display_memory("Step 10");
//        } else if (l>k) { // many clusters
//
//            /* Step 11 */
//            for (o=i=0; i<l-1; i++) {
//                for (j=i+1; j<l; j++) {
//                    cl_pi[o]=i;
//                    cl_pj[o]=j;
//                    cl_pd[o]=distance(cl_c[i],cl_c[j]);
//                    o++;
//                }
//            }
//            pdn=o;
//            display_memory("Step 11");
//
//            /* Step 12 */
//            for (i=0; i<o-1; i++) {
//                for (j=i+1; j<o; j++) {
//                    if (cl_pd[i]>cl_pd[j]) {
//                        temp_double=cl_pd[i];
//                        cl_pd[i]=cl_pd[j];
//                        cl_pd[j]=temp_double;
//                        temp_int=cl_pi[i];
//                        cl_pi[i]=cl_pi[j];
//                        cl_pi[j]=temp_int;
//                        temp_int=cl_pj[i];
//                        cl_pj[i]=cl_pj[j];
//                        cl_pj[j]=temp_int;
//                    }
//                }
//            }
//            display_memory("Step 12");
//
//            /* Step 13 */
//            /* Notice: Smallest cluster pair below th_c will be merged */
//            for (j=0; j<m; j++) {
//                tmp_double_vector[j]=cl_c[cl_pi[0]][j]*cl_m[cl_pi[0]] +cl_c[cl_pj[0]][j]*cl_m[cl_pj[0]];
//            }
//            tmp_double_vector[j]/=(cl_m[cl_pi[0]]+cl_m[cl_pj[0]]);
////            cl_m[cl_pi[0]]=cl_m[cl_pi[0]]+cl_m[cl_pj[0]];
//            for (j=0; j<m; j++) {
//                cl_c[cl_pi[0]][j]=tmp_double_vector[j];
//            }
//            for (i=cl_pj[0]+1; i<l; i++) {
////                cl_m[i-1]=cl_m[i];
//                for (j=0; j<m; j++) {
//                    cl_c[i-1][j]=cl_c[i][j];
//                }
//            }
//            display_memory("Step 13");
//        }
//
//        /* Step 14 */
//        iter--;
//        display_memory("Step 14");
//        /* Additional criterion by Ch Iossif */
//        oad=poad;
//        poad=0.0;
//        for (i=0; i<n; i++) {
//            poad+=distance(data[i],cl_c[data_cl[i]]);
//        }
//        if (abs(oad-poad)<EPSILON)
//            break;
//    }
//    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//
//    /* save results */
//    if ((fp=fopen(fname_out,"w"))==NULL)
//        error("bad output file name");
//    for (i=0; i<n; i++)
//        fprintf(fp,"%d\n",data_cl[i]+1);
//    fclose(fp);
//
//    /* free allocated memory */
//    /*
//    k<<1;
//    for (i=0; i<n; i++)
//        free(data[i]);
//    free(data);
//    for (i=0; i<k; i++) {
//        free(cl_c[i]);
//        free(cl_s[i]);
//    }
//    free(cl_c);
//    free(cl_s);
//    free(cl_m);
//    free(cl_ms);
//    free(cl_msc);
//    free(cl_pd);
//    free(cl_pi);
//    free(cl_pj);
//    free(p);
//    free(data_cl);
//    free(data_d);
//    free(tmp_double_vector);
//    free(tmp_int_vector);
//    */
//
//    printf("\nisodata v0.0.0a1 Ver.: ?? GPLv3 (c) 02/10 Ch Iossif <chiossif@yahoo.com>\n\n");
//    return 0;
//}
//
//void error(const char *s) {
//    printf("\nisodata reports: error: %s.\n",s);
//    exit(1);
//}
//
//double distance(double *a, double *b) {
//    register int i;
//    double s;
//    for (s=i=0; i<m; i++)
//        s+=sqr(a[i]-b[i]);
//    /*
//        printf("\t\t\t\tDistance between:");
//        for (i=0;i<m;i++)
//            printf(" %lf",a[i]);
//        printf(" and");
//        for (i=0;i<m;i++)
//            printf(" %lf",b[i]);
//        printf(" is %lf\n",sqrt(s));
//    */
//    return sqrt(s);
//}
//
//double sqr(double x) {
//    return x*x;
//}
//
//double sqrt(double x) {
//    double px,a;
//    a=px=x;
//    x/=2.0;
//    while (abs(x-px)>EPSILON) {
//        px=x;
//        x=(px+a/px)/2;
//    }
//    return x;
//}
//
//void display_memory(const char *s) {
//    register int i, j;
//
//    /*
//        printf("Report at iteration %d %s:\n", iter, s);
//        printf("\t\tnumber of samples is %d\n",n);
//        printf("\t\tnumber of attributes is %d\n",m);
//        printf("\t\tinitial attributes factors are :");
//        for (i=0;i<m;i++)
//            printf(" %lf",p[i]);
//        printf("\n");
//        printf("\t\tinput data file name is <%s>\n",fname_in);
//        printf("\t\toutput data file name is %s\n",fname_out);
//        printf("\t\tmaximum number of clusters is %d\n",k);
//        printf("\t\tmaximum number of iterations is %d\n",iter);
//        printf("\t\tminimum number of samples in cluster is %d\n",th_n);
//        printf("\t\tmaximum standard deviation of one cluster is %lf\n",th_s);
//        printf("\t\tmaximum distance to merge clusters is %lf\n",th_c);
//        printf("\t\tnumber of initial cluster centers is %d\n",l);
//        if (l) {
//            printf("\t\tinitial cluster centers are :\n");
//            for (i=0;i<l;i++) {
//                printf("\t\t\t");
//                for (j=0;j<m;j++)
//                    printf("%lf ",cl_c[i][j]);
//                printf("\n");
//            }
//        }
//        printf("\t\tinput data are:\n");
//        for (i=0; i<n; i++)
//            for (j=0; j<m; j++)
//                printf("\t\t\ti=%4d/%4d j=%4d/%4d - %lf\n",i+1,n,j+1,m,data[i][j]);
//        printf("\n");
//    */
//    /*int p;*/         /* maximum number of pairs of cluster which can be merged  */
//    /*
//        printf("\t\tnumber of cluster centers is %d\n",l);
//        printf("\t\tcluster centers are:\n");
//        for (i=0; i<l; i++)
//            for (j=0; j<m; j++)
//                printf("\t\t\ti=%4d/%4d j=%4d/%4d - %lf\n",i+1,l,j+1,m,cl_c[i][j]);
//        printf("\n");
//        printf("\t\tcluster number of members are: ");
//        for (i=0; i<m; i++)
//            printf("%d ",cl_m[i]);
//        printf("\n");
//        printf("\t\tcluster average distances are: ");
//        for (i=0; i<k; i++)
//            printf("%lf ",cl_d[i]);
//        printf("\n");
//        printf("\t\tcluster standard deviations are:\n");
//        for (i=0; i<k; i++)
//            for (j=0; j<m; j++)
//                printf("\t\t\ti=%4d/%4d j=%4d/%4d - %lf\n",i+1,k,j+1,m,cl_s[i][j]);
//        printf("\n");
//        printf("\t\tcluster maximum standard deviations are: ");
//        for (i=0; i<k; i++)
//            printf("%lf ",cl_ms[i]);
//        printf("\n");
//        printf("\t\tcluster maximum standard deviation components are: ");
//        for (i=0; i<k; i++)
//            printf("%d ",cl_msc[i]);
//        printf("\n");
//        printf("\t\tcluster pairwise distances are: ");
//        for (i=0; i<k*(k-1)/2; i++)
//            printf("%lf ",cl_pd[i]);
//        printf("\n");
//        printf("\t\tcluster pairwise distance cluster i are: ");
//        for (i=0; i<k*(k-1)/2; i++)
//            printf("%d ",cl_pi[i]);
//        printf("\n");
//        printf("\t\tcluster pairwise distance cluster j are: ");
//        for (i=0; i<k*(k-1)/2; i++)
//            printf("%d ",cl_pj[i]);
//        printf("\n");
//        printf("\t\tcluster number of pairwise distances is: %d\n", pdn);
//        printf("\t\tdata cluster ids are: ");
//        for (i=0; i<n; i++)
//            printf("%d ",data_cl[i]);
//        printf("\n");
//        printf("\t\tdata cluster distances are: ");
//        for (i=0; i<n; i++)
//            printf("%lf ",data_d[i]);
//        printf("\n");
//        printf("\t\toverall average distance is: %lf\n\n",oad);
//    */
//    return;
//}
