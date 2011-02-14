/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     mat_tools.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double			SG_Get_Square(double x)
{
	return( x * x );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Simple_Statistics::CSG_Simple_Statistics(void)
{
	Invalidate();
}

//---------------------------------------------------------
void CSG_Simple_Statistics::Invalidate(void)
{
	m_bEvaluated	= false;
	m_nValues		= 0;
	m_Weights		= 0.0;
	m_Sum			= 0.0;
	m_Sum2			= 0.0;

	m_Minimum		= 0.0;
	m_Maximum		= 0.0;
	m_Range			= 0.0;
	m_Mean			= 0.0;
	m_Variance		= 0.0;
	m_StdDev		= 0.0;
}

//---------------------------------------------------------
void CSG_Simple_Statistics::Add_Value(double Value, double Weight)
{
	if( m_nValues == 0 )
	{
		m_Minimum	= m_Maximum	= Value;
	}
	else if( m_Minimum > Value )
	{
		m_Minimum	= Value;
	}
	else if( m_Maximum < Value )
	{
		m_Maximum	= Value;
	}

	m_nValues++;

	if( Weight > 0.0 )
	{
		m_Weights		+= Weight;
		m_Sum			+= Weight * Value;
		m_Sum2			+= Weight * Value*Value;

		m_bEvaluated	= false;
	}
}

//---------------------------------------------------------
void CSG_Simple_Statistics::_Evaluate(void)
{
	if( m_Weights > 0.0 )
	{
		m_Range			= m_Maximum - m_Minimum;
		m_Mean			= m_Sum  / m_Weights;
		m_Variance		= m_Sum2 / m_Weights - m_Mean*m_Mean;
		m_StdDev		= m_Variance > 0.0 ? sqrt(m_Variance) : 0.0;

		m_bEvaluated	= true;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Class_Statistics::CSG_Class_Statistics(void)
{
	Create();
}

//---------------------------------------------------------
CSG_Class_Statistics::~CSG_Class_Statistics(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_Class_Statistics::Create(void)
{
	m_Array.Create(sizeof(TClass), 0, SG_ARRAY_GROWTH_1);

	m_Classes	= NULL;
}

//---------------------------------------------------------
void CSG_Class_Statistics::Destroy(void)
{
	m_Array.Set_Array(0, (void **)&m_Classes);
}

//---------------------------------------------------------
void CSG_Class_Statistics::Add_Value(double Value)
{
	for(size_t i=0; i<m_Array.Get_Size(); i++)
	{
		if( m_Classes[i].Value == Value )
		{
			m_Classes[i].Count++;

			return;
		}
	}

	if( m_Array.Inc_Array((void **)&m_Classes) )
	{
		m_Classes[Get_Count() - 1].Count	= 1;
		m_Classes[Get_Count() - 1].Value	= Value;
	}
}

//---------------------------------------------------------
int CSG_Class_Statistics::Get_Majority(void)
{
	int		Index	= 0;

	for(int i=1; i<Get_Count(); i++)
	{
		if( m_Classes[i].Count > m_Classes[Index].Count )
		{
			Index	= i;
		}
	}

	return( Index );
}

bool CSG_Class_Statistics::Get_Majority(double &Value)
{
	int		Count;

	return( Get_Class(Get_Majority(), Value, Count) );
}

bool CSG_Class_Statistics::Get_Majority(double &Value, int &Count)
{
	return( Get_Class(Get_Majority(), Value, Count) );
}

//---------------------------------------------------------
int CSG_Class_Statistics::Get_Minority(void)
{
	int		Index	= 0;

	for(int i=1; i<Get_Count(); i++)
	{
		if( m_Classes[i].Count > m_Classes[Index].Count )
		{
			Index	= i;
		}
	}

	return( Index );
}

bool CSG_Class_Statistics::Get_Minority(double &Value)
{
	int		Count;

	return( Get_Class(Get_Minority(), Value, Count) );
}

bool CSG_Class_Statistics::Get_Minority(double &Value, int &Count)
{
	return( Get_Class(Get_Minority(), Value, Count) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Cluster_Analysis::CSG_Cluster_Analysis(void)
{
	m_Centroid	= NULL;
	m_Variance	= NULL;
	m_nMembers	= NULL;
	m_Cluster	= NULL;
	m_nFeatures	= 0;
	m_nClusters	= 0;
	m_Iteration	= 0;
}

//---------------------------------------------------------
CSG_Cluster_Analysis::~CSG_Cluster_Analysis(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Destroy(void)
{
	for(int i=0; i<m_nClusters; i++)
	{
		SG_Free(m_Centroid[i]);
	}

	SG_FREE_SAFE(m_Centroid);
	SG_FREE_SAFE(m_Variance);
	SG_FREE_SAFE(m_nMembers);
	SG_FREE_SAFE(m_Cluster);

	m_Features.Destroy();

	m_nFeatures	= 0;
	m_nClusters	= 0;

	m_Iteration	= 0;

	return( true );
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Create(int nFeatures)
{
	Destroy();

	if( nFeatures > 0 )
	{
		m_nFeatures	= nFeatures;

		m_Features.Create(m_nFeatures * sizeof(double), 0, SG_ARRAY_GROWTH_3);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Add_Element(void)
{
	return( m_nFeatures > 0 && m_Features.Inc_Array() );
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Set_Feature(int iElement, int iFeature, double Value)
{
	if( iElement >= 0 && iElement < Get_nElements() && iFeature >= 0 && iFeature < m_nFeatures )
	{
		((double *)m_Features.Get_Entry(iElement))[iFeature]	= Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Execute(int Method, int nClusters)
{
	if( Get_nElements() <= 1 || nClusters <= 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Iteration	= 0;

	m_nClusters	= nClusters;

	m_Cluster	= (int     *)SG_Calloc(Get_nElements()	, sizeof(int));

	m_nMembers	= (int     *)SG_Calloc(m_nClusters		, sizeof(int));
	m_Variance	= (double  *)SG_Calloc(m_nClusters		, sizeof(double));
	m_Centroid	= (double **)SG_Calloc(m_nClusters		, sizeof(double *));

	for(int iCluster=0; iCluster<m_nClusters; iCluster++)
	{
		m_Centroid[iCluster]	= (double *)SG_Calloc(m_nFeatures, sizeof(double));
	}

	//-----------------------------------------------------
	switch( Method )
	{
	case 0: default:
		return( Minimum_Distance(true) );

	case 1:
		return( Hill_Climbing(true) );

	case 2:
		return( Minimum_Distance(true) && Hill_Climbing(false) );
	}
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Minimum_Distance(bool bInitialize)
{
	bool	bContinue;
	int		iElement, iFeature, iCluster, nShifts;
	double	*Feature, SP_Last	= -1.0;

	//-----------------------------------------------------
	for(iElement=0; iElement<Get_nElements(); iElement++)
	{
		iCluster	= m_Cluster[iElement];

		if( bInitialize || iCluster < 0 || iCluster >= m_nClusters )
		{
			m_Cluster[iElement]	= iCluster = iElement % m_nClusters;
		}
	}

	//-----------------------------------------------------
	for(m_Iteration=1, bContinue=true; bContinue && SG_UI_Process_Get_Okay(); m_Iteration++)
	{
		for(iCluster=0; iCluster<m_nClusters; iCluster++)
		{
			m_Variance[iCluster]	= 0.0;
			m_nMembers[iCluster]	= 0;

			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				m_Centroid[iCluster][iFeature]	= 0.0;
			}
		}

		//-------------------------------------------------
		for(iElement=0, Feature=(double *)m_Features.Get_Array(); iElement<Get_nElements(); iElement++, Feature+=m_nFeatures)
		{
			if( (iCluster = m_Cluster[iElement]) >= 0 )
			{
				m_nMembers[iCluster]++;

				for(iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					m_Centroid[iCluster][iFeature]	+= Feature[iFeature];
				}
			}
		}

		//-------------------------------------------------
		for(iCluster=0; iCluster<m_nClusters; iCluster++)
		{
			double	d	= m_nMembers[iCluster] > 0 ? 1.0 / m_nMembers[iCluster] : 0.0;

			for(iFeature=0; iFeature<m_nFeatures; iFeature++)
			{
				m_Centroid[iCluster][iFeature]	*= d;
			}
		}

		//-------------------------------------------------
		for(iElement=0, Feature=(double *)m_Features.Get_Array(), m_SP=0.0, nShifts=0; iElement<Get_nElements() && bContinue; iElement++, Feature+=m_nFeatures)
		{
			double	minVariance	= -1.0;
			int		minCluster	= -1;

			for(iCluster=0; iCluster<m_nClusters; iCluster++)
			{
				double	iVariance	= 0.0;

				for(iFeature=0; iFeature<m_nFeatures; iFeature++)
				{
					iVariance	+= SG_Get_Square(m_Centroid[iCluster][iFeature] - Feature[iFeature]);
				}

				if( minVariance < 0.0 || iVariance < minVariance )
				{
					minVariance	= iVariance;
					minCluster	= iCluster;
				}
			}

			if( m_Cluster[iElement] != minCluster )
			{
				m_Cluster[iElement]	= minCluster;
				nShifts++;
			}

			m_SP					+= minVariance;
			m_Variance[minCluster]	+= minVariance;
		}

		//-------------------------------------------------
		if( nShifts == 0 || (SP_Last >= 0 && m_SP >= SP_Last) )
		{
			bContinue	= false;
		}

		SG_UI_Process_Set_Text(CSG_String::Format(SG_T("%s: %d >> %s %f"),
			LNG("pass")		, m_Iteration,
			LNG("change")	, m_Iteration <= 1 ? m_SP : SP_Last - m_SP
		));

		SP_Last		= m_SP;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Cluster_Analysis::Hill_Climbing(bool bInitialize)
{
	bool	bContinue;
	int		iElement, iFeature, iCluster, noShift;
	double	*Feature, Variance, SP_Last	= -1.0;

	//-----------------------------------------------------
	memset(m_Variance, 0, m_nClusters * sizeof(double));
	memset(m_nMembers, 0, m_nClusters * sizeof(int));

	for(iCluster=0; iCluster<m_nClusters; iCluster++)
	{
		memset(m_Centroid[iCluster], 0, m_nFeatures * sizeof(double));
	}

	//-----------------------------------------------------
	for(iElement=0, Feature=(double *)m_Features.Get_Array(); iElement<Get_nElements(); iElement++, Feature+=m_nFeatures)
	{
		iCluster	= m_Cluster[iElement];

		if( bInitialize || iCluster < 0 || iCluster >= m_nClusters )
		{
			m_Cluster[iElement]	= iCluster = iElement % m_nClusters;
		}

		m_nMembers[iCluster]++;

		for(iFeature=0, Variance=0.0; iFeature<m_nFeatures; iFeature++)
		{
			double	d	 = Feature[iFeature];
			m_Centroid[iCluster][iFeature]	+= d;
			Variance						+= d*d;
		}

		m_Variance[iCluster]	+= Variance;
	}

	//-----------------------------------------------------
	for(iCluster=0, m_SP=0.0; iCluster<m_nClusters; iCluster++)
	{
		double	d	= m_nMembers[iCluster] != 0 ? 1.0 / (double)m_nMembers[iCluster] : 0;

		for(iFeature=0, Variance=0.0; iFeature<m_nFeatures; iFeature++)
		{
			m_Centroid[iCluster][iFeature]	*= d;
			Variance						+= SG_Get_Square(m_Centroid[iCluster][iFeature]);
		}

		m_Variance[iCluster]	-= m_nMembers[iCluster] * Variance;
		m_SP					+= m_Variance[iCluster];
	}

	noShift		= 0;

	//-----------------------------------------------------
	for(m_Iteration=1, bContinue=true; bContinue && SG_UI_Process_Get_Okay(false); m_Iteration++)
	{
		for(iElement=0, Feature=(double *)m_Features.Get_Array(); iElement<Get_nElements() && bContinue; iElement++, Feature+=m_nFeatures)
		{
			if( (iCluster = m_Cluster[iElement]) >= 0 )
			{
				if( noShift++ >= Get_nElements() )
				{
					bContinue	= false;
				}
				else if( m_nMembers[iCluster] > 1 )
				{
					int		jCluster, kCluster;
					double	VMin, V1, V2;

					for(iFeature=0, Variance=0.0; iFeature<m_nFeatures; iFeature++)
					{
						Variance	+= SG_Get_Square(m_Centroid[iCluster][iFeature] - Feature[iFeature]);
					}

					V1		= Variance * m_nMembers[iCluster] / (m_nMembers[iCluster] - 1.0);
					VMin	= -1.0;

					//-----------------------------------------
					// Bestimme Gruppe iCluster mit evtl. groesster Verbesserung...

					for(jCluster=0; jCluster<m_nClusters; jCluster++)
					{
						if( jCluster != iCluster )
						{
							for(iFeature=0, Variance=0.0; iFeature<m_nFeatures; iFeature++)
							{
								Variance	+= SG_Get_Square(m_Centroid[jCluster][iFeature] - Feature[iFeature]);
							}

							V2		= Variance * m_nMembers[jCluster] / (m_nMembers[jCluster] + 1.0);

							if( VMin < 0.0 || V2 < VMin )
							{
								VMin		= V2;
								kCluster	= jCluster;
							}
						}
					}

					//-----------------------------------------
					// Gruppenwechsel und Neuberechnung der Gruppencentroide...

					if( VMin >= 0 && VMin < V1 )
					{
						noShift					= 0;
						m_Variance[iCluster]	-= V1;
						m_Variance[kCluster]	+= VMin;
						m_SP					= m_SP - V1 + VMin;
						V1						= 1.0 / (m_nMembers[iCluster] - 1.0);
						V2						= 1.0 / (m_nMembers[kCluster] + 1.0);

						for(iFeature=0; iFeature<m_nFeatures; iFeature++)
						{
							double	d	= Feature[iFeature];

							m_Centroid[iCluster][iFeature]	= (m_nMembers[iCluster] * m_Centroid[iCluster][iFeature] - d) * V1;
							m_Centroid[kCluster][iFeature]	= (m_nMembers[kCluster] * m_Centroid[kCluster][iFeature] + d) * V2;
						}

						m_Cluster[iElement]	= kCluster;

						m_nMembers[iCluster]--;
						m_nMembers[kCluster]++;
					}
				}
			}
		}

		SG_UI_Process_Set_Text(CSG_String::Format(SG_T("%s: %d >> %s %f"),
			LNG("pass")		, m_Iteration,
			LNG("change")	, m_Iteration <= 1 ? m_SP : SP_Last - m_SP
		));

		SP_Last		= m_SP;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
