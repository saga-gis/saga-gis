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
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_cluster_analysis.cpp              //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
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
#include "table_cluster_analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Cluster_Analysis::CTable_Cluster_Analysis(bool bShapes)
{
	m_bShapes	= bShapes;

	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Cluster Analysis"));

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		"Cluster Analysis for grids.\n\nReferences:\n\n"
		                                                                                                                                                                                                                  
		"Iterative Minimum Distance:\n"
		"- Forgy, E. (1965):\n"
		"  'Cluster Analysis of multivariate data: efficiency vs. interpretability of classifications',\n"
		"  Biometrics 21:768\n\n"

		"Hill-Climbing:"
		"- Rubin, J. (1967):\n"
		"  'Optimal Classification into Groups: An Approach for Solving the Taxonomy Problem',\n"
		"  J. Theoretical Biology, 15:103-144\n\n"
	));

	//-----------------------------------------------------
	if( m_bShapes )
	{
		pNode	= 
		Parameters.Add_Shapes(NULL, "INPUT" , _TL("Shapes"), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Shapes(NULL, "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}
	else
	{
		pNode	=
		Parameters.Add_Table(NULL, "INPUT"  , _TL("Table" ), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Table(NULL, "RESULT" , _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "CLUSTER"		, _TL("Cluster"),
		_TL(""),
		true
	);

	Parameters.Add_Table(
		NULL	, "STATISTICS"	, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Iterative Minimum Distance (Forgy 1965)"),
			_TL("Hill-Climbing (Rubin 1967)"),
			_TL("Combined Minimum Distance / Hillclimbing") 
		), 1
	);

	Parameters.Add_Value(
		NULL	, "NCLUSTER"	, _TL("Clusters"),
		_TL("Number of clusters"),
		PARAMETER_TYPE_Int, 10, 2, true
	);

	Parameters.Add_Value(
		NULL	, "NORMALISE"	, _TL("Normalise"),
		_TL("Automatically normalise grids by standard deviation before clustering."),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Cluster_Analysis::On_Execute(void)
{
	bool					bNormalize;
	int						iFeature, nFeatures, *Features, iElement, nElements, Cluster;
	CSG_Cluster_Analysis	Analysis;
	CSG_Table				*pTable;

	//-----------------------------------------------------
	pTable		= Parameters("RESULT"   )->asTable();
	bNormalize	= Parameters("NORMALISE")->asBool();
	Cluster		= Parameters("CLUSTER"  )->asInt();

	Features	= (int *)Parameters("FIELDS")->asPointer();
	nFeatures	=        Parameters("FIELDS")->asInt    ();

	if( !Features || nFeatures <= 0 )
	{
		Error_Set(_TL("no features in selection"));

		return( false );
	}

	if( !Analysis.Create(nFeatures) )
	{
		Error_Set(_TL("could not initialize cluster engine"));

		return( false );
	}

	if( pTable && pTable != Parameters("INPUT")->asTable() )
	{
		if( m_bShapes )
		{
			((CSG_Shapes *)pTable)->Create(*Parameters("INPUT")->asShapes());
		}
		else
		{
			pTable->Create(*Parameters("INPUT")->asTable());
		}
	}
	else
	{
		pTable	= Parameters("INPUT")->asTable();
	}

	if( Cluster < 0 )
	{
		Cluster	= pTable->Get_Field_Count();

		pTable->Add_Field(_TL("CLUSTER"), SG_DATATYPE_Int);
	}

	//-----------------------------------------------------
	for(iElement=0, nElements=0; iElement<pTable->Get_Count() && Set_Progress(iElement, pTable->Get_Count()); iElement++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iElement);

		bool	bNoData		= false;

		for(iFeature=0; iFeature<nFeatures && !bNoData; iFeature++)
		{
			if( pRecord->is_NoData(Features[iFeature]) )
			{
				bNoData	= true;
			}
		}

		if( bNoData || !Analysis.Add_Element() )
		{
			pRecord->Set_NoData(Cluster);
		}
		else
		{
			pRecord->Set_Value(Cluster, 0.0);

			for(iFeature=0; iFeature<nFeatures; iFeature++)
			{
				double	d	= pRecord->asDouble(Features[iFeature]);

				if( bNormalize )
				{
					d	= (d - pTable->Get_Mean(Features[iFeature])) / pTable->Get_StdDev(Features[iFeature]);
				}

				Analysis.Set_Feature(nElements, iFeature, d);
			}

			nElements++;
		}
	}

	if( nElements <= 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool	bResult	= Analysis.Execute(Parameters("METHOD")->asInt(), Parameters("NCLUSTER")->asInt());

	for(iElement=0, nElements=0; iElement<pTable->Get_Count(); iElement++)
	{
		Set_Progress(iElement, pTable->Get_Count());

		CSG_Table_Record	*pRecord	= pTable->Get_Record(iElement);

		if( !pRecord->is_NoData(Cluster) )
		{
			pRecord->Set_Value(Cluster, Analysis.Get_Cluster(nElements++));
		}
	}

	Save_Statistics(pTable, Features, bNormalize, Analysis);

//	Save_LUT(pCluster, Analysis.Get_nClusters());

	DataObject_Update(pTable);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Cluster_Analysis::Save_Statistics(CSG_Table *pTable, int *Features, bool bNormalize, const CSG_Cluster_Analysis &Analysis)
{
	int			iCluster, iFeature;
	CSG_String	s;
	CSG_Table	*pStatistics;

	pStatistics	= Parameters("STATISTICS")->asTable();

	pStatistics->Destroy();
	pStatistics->Set_Name(_TL("Cluster Analysis"));

	pStatistics->Add_Field(_TL("ClusterID")	, SG_DATATYPE_Int);
	pStatistics->Add_Field(_TL("Elements")	, SG_DATATYPE_Int);
	pStatistics->Add_Field(_TL("Std.Dev.")	, SG_DATATYPE_Double);

	s.Printf(SG_T("\n%s:\t%ld \n%s:\t%d \n%s:\t%d \n%s:\t%f\n\n%s\t%s\t%s"),
		_TL("Number of Elements")		, Analysis.Get_nElements(),
		_TL("Number of Variables")		, Analysis.Get_nFeatures(),
		_TL("Number of Clusters")		, Analysis.Get_nClusters(),
		_TL("Value of Target Function")	, Analysis.Get_SP(),
		_TL("Cluster"), _TL("Elements"), _TL("Std.Dev.")
	);

	for(iFeature=0; iFeature<Analysis.Get_nFeatures(); iFeature++)
	{
		s	+= CSG_String::Format(SG_T("\t%s"), pTable->Get_Field_Name(Features[iFeature]));

		pStatistics->Add_Field(pTable->Get_Field_Name(Features[iFeature]), SG_DATATYPE_Double);
	}

	Message_Add(s);

	for(iCluster=0; iCluster<Analysis.Get_nClusters(); iCluster++)
	{
		s.Printf(SG_T("\n%d\t%d\t%f"), iCluster, Analysis.Get_nMembers(iCluster), sqrt(Analysis.Get_Variance(iCluster)));

		CSG_Table_Record	*pRecord	= pStatistics->Add_Record();

		pRecord->Set_Value(0, iCluster);
		pRecord->Set_Value(1, Analysis.Get_nMembers(iCluster));
		pRecord->Set_Value(2, sqrt(Analysis.Get_Variance(iCluster)));

		for(iFeature=0; iFeature<Analysis.Get_nFeatures(); iFeature++)
		{
			double	Centroid	= Analysis.Get_Centroid(iCluster, iFeature);

			if( bNormalize )
			{
				Centroid	= pTable->Get_Mean(Features[iFeature]) + Centroid * pTable->Get_StdDev(Features[iFeature]);
			}

			s	+= CSG_String::Format(SG_T("\t%f"), Centroid);

			pRecord->Set_Value(iFeature + 3, Centroid);
		}

		Message_Add(s, false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
