
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
//               Grid_Cluster_Analysis.cpp               //
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
#include "classify_cluster_analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cluster_Analysis::CGrid_Cluster_Analysis(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Cluster Analysis for Grids"));

	Set_Author		(SG_T("O.Conrad (c) 2001"));

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
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CLUSTER"		, _TL("Clusters"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
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

	//-----------------------------------------------------
	CSG_Parameter	*pNode	=
	Parameters.Add_Value(NULL	, "OLDVERSION", _TL("Old Version"), _TL("slower but memory saving"), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode	, "UPDATEVIEW", _TL("Update View"), _TL(""), PARAMETER_TYPE_Bool, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cluster_Analysis::On_Execute(void)
{
	if( Parameters("OLDVERSION")->asBool() )	{	return( _On_Execute() );	}

	//-----------------------------------------------------
	bool					bNormalize;
	int						iFeature, iElement, nElements;
	CSG_Cluster_Analysis	Analysis;
	CSG_Grid				*pCluster;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")		->asGridList();
	pCluster	= Parameters("CLUSTER")		->asGrid();
	bNormalize	= Parameters("NORMALISE")	->asBool();

	if( !Analysis.Create(pGrids->Get_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	pCluster->Set_NoData_Value(-1.0);

	for(iElement=0, nElements=0; iElement<Get_NCells() && Set_Progress_NCells(iElement); iElement++)
	{
		bool	bNoData		= false;

		for(iFeature=0; iFeature<pGrids->Get_Count() && !bNoData; iFeature++)
		{
			if( pGrids->asGrid(iFeature)->is_NoData(iElement) )
			{
				bNoData	= true;
			}
		}

		if( bNoData || !Analysis.Add_Element() )
		{
			pCluster->Set_Value(iElement, -1);
		}
		else
		{
			pCluster->Set_Value(iElement, 0);

			for(iFeature=0; iFeature<pGrids->Get_Count(); iFeature++)
			{
				double	d	= pGrids->asGrid(iFeature)->asDouble(iElement);

				if( bNormalize )
				{
					d	= (d - pGrids->asGrid(iFeature)->Get_ArithMean()) / pGrids->asGrid(iFeature)->Get_StdDev();
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

	for(iElement=0, nElements=0; iElement<Get_NCells(); iElement++)
	{
		Set_Progress_NCells(iElement);

		if( !pCluster->is_NoData(iElement) )
		{
			pCluster->Set_Value(iElement, Analysis.Get_Cluster(nElements++));
		}
	}

	Save_Statistics(pGrids, bNormalize, Analysis);

	Save_LUT(pCluster, Analysis.Get_nClusters());

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Cluster_Analysis::Save_Statistics(CSG_Parameter_Grid_List *pGrids, bool bNormalize, const CSG_Cluster_Analysis &Analysis)
{
	int					iCluster, iFeature;
	CSG_String			s;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;

	pTable	= Parameters("STATISTICS")->asTable();

	pTable->Destroy();
	pTable->Set_Name(_TL("Cluster Analysis"));

	pTable->Add_Field(_TL("ClusterID")	, SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Elements")	, SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Std.Dev.")	, SG_DATATYPE_Double);

	s.Printf(SG_T("\n%s:\t%ld \n%s:\t%d \n%s:\t%d \n%s:\t%f\n\n%s\t%s\t%s"),
		_TL("Number of Elements")		, Analysis.Get_nElements(),
		_TL("Number of Variables")		, Analysis.Get_nFeatures(),
		_TL("Number of Clusters")		, Analysis.Get_nClusters(),
		_TL("Value of Target Function")	, Analysis.Get_SP(),
		_TL("Cluster"), _TL("Elements"), _TL("Std.Dev.")
	);

	for(iFeature=0; iFeature<Analysis.Get_nFeatures(); iFeature++)
	{
		s	+= CSG_String::Format(SG_T("\t%s"), pGrids->asGrid(iFeature)->Get_Name());

		pTable->Add_Field(pGrids->asGrid(iFeature)->Get_Name(), SG_DATATYPE_Double);
	}

	Message_Add(s);

	for(iCluster=0; iCluster<Analysis.Get_nClusters(); iCluster++)
	{
		s.Printf(SG_T("\n%d\t%d\t%f"), iCluster, Analysis.Get_nMembers(iCluster), sqrt(Analysis.Get_Variance(iCluster)));

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, iCluster);
		pRecord->Set_Value(1, Analysis.Get_nMembers(iCluster));
		pRecord->Set_Value(2, sqrt(Analysis.Get_Variance(iCluster)));

		for(iFeature=0; iFeature<Analysis.Get_nFeatures(); iFeature++)
		{
			double	Centroid	= Analysis.Get_Centroid(iCluster, iFeature);

			if( bNormalize )
			{
				Centroid	= pGrids->asGrid(iFeature)->Get_ArithMean() + Centroid * pGrids->asGrid(iFeature)->Get_StdDev();
			}

			s	+= CSG_String::Format(SG_T("\t%f"), Centroid);

			pRecord->Set_Value(iFeature + 3, Centroid);
		}

		Message_Add(s, false);
	}
}

//---------------------------------------------------------
void CGrid_Cluster_Analysis::Save_LUT(CSG_Grid *pCluster, int nClusters)
{
	CSG_Parameters	Parms;

	if( DataObject_Get_Parameters(pCluster, Parms) && Parms("COLORS_TYPE") && Parms("LUT") )
	{
		CSG_Table_Record	*pClass;
		CSG_Table			*pLUT	= Parms("LUT")->asTable();

		for(int iCluster=0; iCluster<nClusters; iCluster++)
		{
			if( (pClass = pLUT->Get_Record(iCluster)) == NULL )
			{
				pClass	= pLUT->Add_Record();
				pClass->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
			}

			pClass->Set_Value(1, CSG_String::Format(SG_T("%s %d"), _TL("Class"), iCluster + 1));
			pClass->Set_Value(2, CSG_String::Format(SG_T("%s %d"), _TL("Class"), iCluster + 1));
			pClass->Set_Value(3, iCluster);
			pClass->Set_Value(4, iCluster);
		}

		while( pLUT->Get_Record_Count() > nClusters )
		{
			pLUT->Del_Record(pLUT->Get_Record_Count() - 1);
		}

		Parms("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pCluster, Parms);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////
//														 //
//				Deprecated Old Version					 //
//														 //
///////////////////////////////////////////////////////////
//														 //
//				slow, but safes memory !				 //
//														 //
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cluster_Analysis::_On_Execute(void)
{
	int						i, j, *nMembers, nCluster, nElements;
	double					*Variances, **Centroids, SP;
	CSG_Grid				**Grids, *pCluster;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	pCluster	= Parameters("CLUSTER")	->asGrid();
	nCluster	= Parameters("NCLUSTER")->asInt();

	if( pGrids->Get_Count() <= 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	Grids		= (CSG_Grid **)SG_Malloc(pGrids->Get_Count() * sizeof(CSG_Grid *));

	if( Parameters("NORMALISE")->asBool() )
	{
		for(i=0; i<pGrids->Get_Count(); i++)
		{
			Grids[i]	= SG_Create_Grid(pGrids->asGrid(i), SG_DATATYPE_Float);
			Grids[i]	->Assign(pGrids->asGrid(i));
			Grids[i]	->Normalise();
		}
	}
	else
	{
		for(i=0; i<pGrids->Get_Count(); i++)
		{
			Grids[i]	= pGrids->asGrid(i);
		}
	}

	pCluster->Set_NoData_Value(-1.0);
	pCluster->Assign_NoData();

	nMembers	= (int     *)SG_Malloc(nCluster * sizeof(int));
	Variances	= (double  *)SG_Malloc(nCluster * sizeof(double));
	Centroids	= (double **)SG_Malloc(nCluster * sizeof(double *));

	for(i=0; i<nCluster; i++)
	{
		Centroids[i]	= (double  *)SG_Malloc(pGrids->Get_Count() * sizeof(double));
	}

	//-------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case 0:		SP	= _MinimumDistance	(Grids, pGrids->Get_Count(), pCluster, nCluster, nMembers, Variances, Centroids, nElements = Get_NCells());	break;
	case 1:		SP	= _HillClimbing		(Grids, pGrids->Get_Count(), pCluster, nCluster, nMembers, Variances, Centroids, nElements = Get_NCells());	break;
	case 2:		SP	= _MinimumDistance	(Grids, pGrids->Get_Count(), pCluster, nCluster, nMembers, Variances, Centroids, nElements = Get_NCells());
				SP	= _HillClimbing		(Grids, pGrids->Get_Count(), pCluster, nCluster, nMembers, Variances, Centroids, nElements = Get_NCells());	break;
	}

	//-------------------------------------------------
	if( Parameters("NORMALISE")->asBool() )
	{
		for(i=0; i<pGrids->Get_Count(); i++)
		{
			delete(Grids[i]);

			for(j=0; j<nCluster; j++)
			{
				Centroids[j][i]	= pGrids->asGrid(i)->Get_StdDev() * Centroids[j][i] + pGrids->asGrid(i)->Get_ArithMean();
			}
		}
	}

	//-------------------------------------------------
	Save_LUT(pCluster, nCluster);

	//-------------------------------------------------
	int					iCluster, iFeature;
	CSG_String			s;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;

	pTable	= Parameters("STATISTICS")->asTable();

	pTable->Destroy();
	pTable->Set_Name(_TL("Cluster Analysis"));

	pTable->Add_Field(_TL("ClusterID")	, SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Elements")	, SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Std.Dev.")	, SG_DATATYPE_Double);

	s.Printf(SG_T("\n%s:\t%ld \n%s:\t%d \n%s:\t%d \n%s:\t%f\n\n%s\t%s\t%s"),
		_TL("Number of Elements")		, nElements,
		_TL("Number of Variables")		, pGrids->Get_Count(),
		_TL("Number of Clusters")		, nCluster,
		_TL("Value of Target Function")	, SP,
		_TL("Cluster"), _TL("Elements"), _TL("Std.Dev.")
	);

	for(iFeature=0; iFeature<pGrids->Get_Count(); iFeature++)
	{
		s	+= CSG_String::Format(SG_T("\t%s"), pGrids->asGrid(iFeature)->Get_Name());

		pTable->Add_Field(pGrids->asGrid(iFeature)->Get_Name(), SG_DATATYPE_Double);
	}

	Message_Add(s);

	for(iCluster=0; iCluster<nCluster; iCluster++)
	{
		s.Printf(SG_T("\n%d\t%d\t%f"), iCluster, nMembers[iCluster], sqrt(Variances[iCluster]));

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, iCluster);
		pRecord->Set_Value(1, nMembers[iCluster]);
		pRecord->Set_Value(2, sqrt(Variances[iCluster]));

		for(iFeature=0; iFeature<pGrids->Get_Count(); iFeature++)
		{
			double	Centroid	= Centroids[iCluster][iFeature];

			if( Parameters("NORMALISE")->asBool() )
			{
				Centroid	= pGrids->asGrid(iFeature)->Get_ArithMean() + Centroid * pGrids->asGrid(iFeature)->Get_StdDev();
			}

			s	+= CSG_String::Format(SG_T("\t%f"), Centroid);

			pRecord->Set_Value(iFeature + 3, Centroid);
		}

		Message_Add(s, false);
	}

	//-------------------------------------------------
	for(i=0; i<nCluster; i++)
	{
		SG_Free(Centroids[i]);
	}

	SG_Free(Centroids);
	SG_Free(Variances);
	SG_Free(nMembers);
	SG_Free(Grids);

	return( true );
}

//---------------------------------------------------------
double CGrid_Cluster_Analysis::_MinimumDistance(CSG_Grid **Grids, int nGrids, CSG_Grid *pCluster, int nCluster, int *nMembers, double *Variances, double **Centroids, int &nElements)
{
	bool	bContinue;
	int		iElement, iGrid, iCluster, nClusterElements, nShifts, minCluster, nPasses;
	double	d, Variance, minVariance, SP, SP_Last	= -1;

	//-----------------------------------------------------
	for(iElement=0, nClusterElements=0; iElement<nElements; iElement++)
	{
		for(iGrid=0, bContinue=true; iGrid<nGrids && bContinue; iGrid++)
		{
			if( Grids[iGrid]->is_NoData(iElement) )
			{
				bContinue	= false;
			}
		}

		if( bContinue )
		{
			if( pCluster->asInt(iElement) < 0 || pCluster->asInt(iElement) >= nCluster )
			{
				pCluster->Set_Value(iElement, iElement % nCluster);
			}

			nClusterElements++;
		}
		else
		{
			pCluster->Set_Value(iElement, -1);
		}
	}

	if( Parameters("UPDATEVIEW")->asBool() )
	{
		DataObject_Update(pCluster, 0, nCluster, true);
	}

	//-----------------------------------------------------
	for(nPasses=1, bContinue=true; bContinue && Process_Get_Okay(false); nPasses++)
	{
		for(iCluster=0; iCluster<nCluster; iCluster++)
		{
			Variances[iCluster]	= 0;
			nMembers [iCluster]	= 0;

			for(iGrid=0; iGrid<nGrids; iGrid++)
			{
				Centroids[iCluster][iGrid]	= 0;
			}
		}

		//-------------------------------------------------
		for(iElement=0; iElement<nElements; iElement++)
		{
			if( pCluster->asInt(iElement) >= 0 )
			{
				iCluster	= pCluster->asInt(iElement);
				nMembers[iCluster]++;

				for(iGrid=0; iGrid<nGrids; iGrid++)
				{
					Centroids[iCluster][iGrid]	+= Grids[iGrid]->asDouble(iElement);
				}
			}
		}

		//-------------------------------------------------
		for(iCluster=0; iCluster<nCluster; iCluster++)
		{
			d		= nMembers[iCluster] > 0 ? 1.0 / (double)nMembers[iCluster] : 0;

			for(iGrid=0; iGrid<nGrids; iGrid++)
			{
				Centroids[iCluster][iGrid]	*= d;
			}
		}

		//-------------------------------------------------
		SP		= 0;
		nShifts	= 0;

		for(iElement=0; iElement<nElements && bContinue; iElement++)
		{
			if( pCluster->asInt(iElement) >= 0 )
			{
				minVariance	= -1;

				for(iCluster=0; iCluster<nCluster; iCluster++)
				{
					Variance	= 0;

					for(iGrid=0; iGrid<nGrids; iGrid++)
					{
						d			= Centroids[iCluster][iGrid] - Grids[iGrid]->asDouble(iElement);
						Variance	+= d * d;
					}

					if( minVariance<0 || Variance<minVariance )
					{
						minVariance	= Variance;
						minCluster	= iCluster;
					}
				}

				if( pCluster->asInt(iElement) != minCluster )
				{
					pCluster->Set_Value(iElement, minCluster);
					nShifts++;
				}

				SP						+= minVariance;
				Variances[minCluster]	+= minVariance;
			}
		}

		//-------------------------------------------------
		if( nShifts == 0 || (SP_Last >= 0 && SP >= SP_Last) )
		{
			bContinue	= false;
		}

		Process_Set_Text(CSG_String::Format(SG_T("%s: %d >> %s %f"),
			_TL("pass")		, nPasses,
			_TL("change")	, SP_Last < 0.0 ? SP : SP_Last - SP
		));

		SP_Last		= SP;

		if( Parameters("UPDATEVIEW")->asBool() )
		{
			DataObject_Update(pCluster, 0, nCluster);	// Update_Output();
		}
	}

	nElements	= nClusterElements;

	return( SP );
}

//---------------------------------------------------------
double CGrid_Cluster_Analysis::_HillClimbing(CSG_Grid **Grids, int nGrids, CSG_Grid *pCluster, int nCluster, int *nMembers, double *Variances, double **Centroids, int &nElements)
{
	bool	bContinue;
	int		iElement, iGrid, iCluster, jCluster, kCluster, nClusterElements, noShift, nPasses;
	double	d, e, n_iK, n_jK, V, VMin, V1, V2, SP, SP_Last	= -1;

	//-----------------------------------------------------
	for(iCluster=0; iCluster<nCluster; iCluster++)
	{
		Variances[iCluster]	= 0;
		nMembers [iCluster]	= 0;

		for(iGrid=0; iGrid<nGrids; iGrid++)
		{
			Centroids[iCluster][iGrid]	= 0;
		}
	}

	//-----------------------------------------------------
	for(iElement=0, nClusterElements=0; iElement<nElements; iElement++)
	{
		for(iGrid=0, bContinue=true; iGrid<nGrids && bContinue; iGrid++)
		{
			if( Grids[iGrid]->is_NoData(iElement) )
			{
				bContinue	= false;
			}
		}

		if( bContinue )
		{
			if( pCluster->asInt(iElement) < 0 || pCluster->asInt(iElement) >= nCluster )
			{
				pCluster->Set_Value(iElement, iElement % nCluster);
			}

			nClusterElements++;

			iCluster	= pCluster->asInt(iElement);

			nMembers[iCluster]++;

			V			= 0.0;

			for(iGrid=0; iGrid<nGrids; iGrid++)
			{
				d							 = Grids[iGrid]->asDouble(iElement);
				Centroids[iCluster][iGrid]	+= d;
				V							+= d * d;
			}

			Variances[iCluster]	+= V;
		}
		else
		{
			pCluster->Set_Value(iElement, -1);
		}
	}

	//-----------------------------------------------------
	SP	= 0.0;

	for(iCluster=0; iCluster<nCluster; iCluster++)
	{
		d	= nMembers[iCluster] != 0 ? 1.0 / (double)nMembers[iCluster] : 0;
		V	= 0.0;

		for(iGrid=0; iGrid<nGrids; iGrid++)
		{
			Centroids[iCluster][iGrid]	*= d;
			e							 = Centroids[iCluster][iGrid];
			V							+= e * e;
		}

		Variances[iCluster]	-= nMembers [iCluster] * V;
		SP					+= Variances[iCluster];
	}

	if( Parameters("UPDATEVIEW")->asBool() )
	{
		DataObject_Update(pCluster, 0, nCluster, true);
	}

	//-----------------------------------------------------
	noShift		= 0;

	for(nPasses=1, bContinue=true; bContinue && Process_Get_Okay(false); nPasses++)
	{
		for(iElement=0; iElement<nElements && bContinue; iElement++)
		{
			if( pCluster->asInt(iElement) >= 0 )
			{
				if( noShift++ >= nElements )
				{
					bContinue	= false;
				}
				else
				{

					//-------------------------------------
					iCluster	= pCluster->asInt(iElement);

					if( nMembers[iCluster] > 1 )
					{
						V	= 0.0;

						for(iGrid=0; iGrid<nGrids; iGrid++)
						{
							d	= Centroids[iCluster][iGrid] - Grids[iGrid]->asDouble(iElement);
							V	+= d * d;
						}

						n_iK	= nMembers[iCluster];
						V1		= V * n_iK / (n_iK - 1.0);
						VMin	= -1.0;

						//---------------------------------
						for(jCluster=0; jCluster<nCluster; jCluster++)
						{
							if( jCluster != iCluster )
							{
								V	= 0.0;

								for(iGrid=0; iGrid<nGrids; iGrid++)
								{
									d	= Centroids[jCluster][iGrid] - Grids[iGrid]->asDouble(iElement);
									V	+= d * d;
								}

								n_jK	= nMembers[jCluster];
								V2		= V * n_jK / (n_jK + 1.0);

								if( VMin < 0 || V2 < VMin )
								{
									VMin		= V2;
									kCluster	= jCluster;
								}
							}
						}

						//---------------------------------
						if( VMin >= 0 && VMin < V1 )
						{
							noShift				= 0;
							Variances[iCluster]	-= V1;
							Variances[kCluster]	+= VMin;
							SP					= SP - V1 + VMin;
							V1					= 1.0 / (n_iK - 1.0);
							n_jK				= nMembers[kCluster];
							V2					= 1.0 / (n_jK + 1.0);

							for(iGrid=0; iGrid<nGrids; iGrid++)
							{
								d							= Grids[iGrid]->asDouble(iElement);
								Centroids[iCluster][iGrid]	= (n_iK * Centroids[iCluster][iGrid] - d) * V1;
								Centroids[kCluster][iGrid]	= (n_jK * Centroids[kCluster][iGrid] + d) * V2;
							}

							pCluster->Set_Value(iElement, kCluster);

							nMembers[iCluster]--;
							nMembers[kCluster]++;
						}
					}
				}
			}
		}

		Process_Set_Text(CSG_String::Format(SG_T("%s: %d >> %s %f"),
			_TL("pass")		, nPasses,
			_TL("change")	, SP_Last < 0.0 ? SP : SP_Last - SP
		));

		SP_Last		= SP;

		if( Parameters("UPDATEVIEW")->asBool() )
		{
			DataObject_Update(pCluster, 0, nCluster);	// Update_Output();
		}
	}

	nElements	= nClusterElements;

	return( SP );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
