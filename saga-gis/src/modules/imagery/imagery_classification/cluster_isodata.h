
////////////////////////////////////////////////////////////
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
#ifndef HEADER_INCLUDED__cluster_isodata_H
#define HEADER_INCLUDED__cluster_isodata_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCluster_ISODATA
{
public:
	CCluster_ISODATA(void);
	CCluster_ISODATA(size_t nFeatures, TSG_Data_Type Data_Type);

	virtual ~CCluster_ISODATA(void);

	bool					Create					(size_t nFeatures, TSG_Data_Type Data_Type);
	bool					Destroy					(void);

	size_t					Get_Feature_Count		(void)	{	return( m_nFeatures );	}

	size_t					Get_Sample_Count		(void)	{	return( m_Data.Get_Size() );	}
	bool					Add_Sample				(const double *Sample);

	size_t					Get_Cluster_Count		(void)	{	return( m_nCluster );	}
	size_t					Get_Cluster_Count		(size_t iCluster)					{	return( cl_m[iCluster]           );	}
	double					Get_Cluster_StdDev		(size_t iCluster)					{	return( cl_d[iCluster]           );	}
	double					Get_Cluster_Mean		(size_t iCluster, size_t iFeature)	{	return( cl_c[iCluster][iFeature] );	}
	double					Get_Cluster_StdDev		(size_t iCluster, size_t iFeature)	{	return( cl_s[iCluster][iFeature] );	}

	size_t					Get_Cluster				(size_t iSample)	{	return( iSample < Get_Sample_Count() ? data_cl[iSample] : m_nCluster );	}

	bool					Set_Max_Iterations		(size_t Value);
	bool					Set_Max_Clusters		(size_t Value);
	bool					Set_Ini_Clusters		(size_t Value);
	bool					Set_Min_Samples			(size_t Value);
	bool					Set_Max_Distance		(double Value);
	bool					Set_Max_StdDev			(double Value);

	bool					Run						(void);


private:

	size_t					m_maxIterations, m_nFeatures, m_nCluster, m_nCluster_Ini, m_nCluster_Max, m_nSamples_Min;

	double					m_Distance, m_Distance_Max, m_StdDev_Max;

	TSG_Data_Type			m_Data_Type;

	CSG_Array				m_Data;

	CSG_Array_Int			cl_m, cl_msc, data_cl;

	CSG_Vector				cl_d, cl_ms, data_d;

	CSG_Matrix				cl_c, cl_s;


	void					_On_Construction		(void);

	bool					_Initialize				(void);

	double					_Get_Sample				(size_t iSample, size_t iFeature);

	double					_Get_Sample_Distance	(int iSample , int iCluster);
	double					_Get_Cluster_Distance	(int iCluster, int jCluster);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__cluster_isodata_H
