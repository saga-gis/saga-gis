
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
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
#include "Grid_Cluster_Analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cluster_Analysis::CGrid_Cluster_Analysis(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Cluster Analysis for Grids"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		
		"Cluster Analysis for grids.\n\nReferences:\n\n"

		"Iterative Minimum Distance:\n"
		"- Forgy, E. (1965):\n"
		"  'Cluster Analysis of multivariate data: efficiency vs. interpretability of classifications',\n"
		"  Biometrics 21:768\n\n"

		"Hill-Climbing:"
		"- Rubin, J. (1967):\n"
		"  'Optimal Classification into Groups: An Approach for Solving the Taxonomy Problem',\n"
		"  J. Theoretical Biology, 15:103-144\n\n")
	);


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid_List(
		NULL	, "INPUT"		, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Clusters"),
		"",
		PARAMETER_OUTPUT, true, GRID_TYPE_Int
	);

	Parameters.Add_Table(
		NULL	, "STATISTICS"	, _TL("Statistics"),
		"",
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// 3. General Parameters...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		"",
		CAPI_String::Format("%s|%s|%s|",
			_TL("Iterative Minimum Distance (Forgy 1965)"),
			_TL("Hill-Climbing (Rubin 1967)"),
			_TL("Combined Minimum Distance / Hillclimbing") 
		),1
	);

	Parameters.Add_Value(
		NULL	, "NCLUSTER"	, _TL("Clusters"),
		_TL("Number of clusters"),
		PARAMETER_TYPE_Int, 10, 2, true
	);

	Parameters.Add_Value(
		NULL	, "NORMALISE"	, _TL("Normalise"),
		_TL("Automatically normalise grids by standard deviation before clustering."),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "UPDATEVIEW"	, _TL("Update View"),
		_TL("Update cluster view while clustering."),
		PARAMETER_TYPE_Bool, true
	);
}

//---------------------------------------------------------
CGrid_Cluster_Analysis::~CGrid_Cluster_Analysis(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cluster_Analysis::On_Execute(void)
{
	int						i, j, nCluster;
	long					nElements;
	double					SP;
	CParameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids		= Parameters("INPUT")	->asGridList();
	nGrids		= pGrids->Get_Count();

	pCluster	= Parameters("RESULT")	->asGrid();
	nCluster	= Parameters("NCLUSTER")->asInt();

	//-----------------------------------------------------
	if( nGrids > 0 )
	{
		Grids		= (CGrid **)API_Malloc(nGrids * sizeof(CGrid *));

		if( Parameters("NORMALISE")->asBool() )
		{
			for(i=0; i<nGrids; i++)
			{
				Grids[i]	= API_Create_Grid(pGrids->asGrid(i), GRID_TYPE_Float);
				Grids[i]->Assign(pGrids->asGrid(i));
				Grids[i]->Normalise();
			}
		}
		else
		{
			for(i=0; i<nGrids; i++)
			{
				Grids[i]	= pGrids->asGrid(i);
			}
		}

		pCluster->Assign(-1);

		nMembers	= (int     *)API_Malloc(nCluster * sizeof(int));
		Variances	= (double  *)API_Malloc(nCluster * sizeof(double));
		Centroids	= (double **)API_Malloc(nCluster * sizeof(double *));

		for(i=0; i<nCluster; i++)
		{
			Centroids[i]	= (double  *)API_Malloc(nGrids * sizeof(double));
		}

		//-------------------------------------------------
		nElements	= Get_NCells();

		switch( Parameters("METHOD")->asInt() )
		{
		case 0:
			SP	= MinimumDistance	(nElements, nCluster);
			break;

		case 1:
			SP	= HillClimbing		(nElements, nCluster);
			break;

		case 2:
			SP	= MinimumDistance	(nElements, nCluster);

			nElements	= Get_NCells();	// may have been diminished because of no data values...

			SP	= HillClimbing		(nElements, nCluster);
			break;
		}

		//-------------------------------------------------
		if( Parameters("NORMALISE")->asBool() )
		{
			for(i=0; i<nGrids; i++)
			{
				delete(Grids[i]);

				Grids[i]	= pGrids->asGrid(i);
			}

			for(i=0; i<nGrids; i++)
			{
				for(j=0; j<nCluster; j++)
				{
					Centroids[j][i]	= sqrt(Grids[i]->Get_Variance()) * Centroids[j][i] + Grids[i]->Get_ArithMean();
				}
			}
		}

		Write_Result(Parameters("STATISTICS")->asTable(), nElements, nCluster, SP);

		API_Free(Grids);

		for(i=0; i<nCluster; i++)
		{
			API_Free(Centroids[i]);
		}

		API_Free(Centroids);
		API_Free(Variances);
		API_Free(nMembers);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Cluster_Analysis::Write_Result(CTable *pTable, long nElements, int nCluster, double SP)
{
	int				i, j;
	CAPI_String		s;
	CTable_Record	*pRecord;

	pTable->Destroy();
	pTable->Set_Name(_TL("Cluster Analysis"));

	pTable->Add_Field(_TL("ClusterID")	, TABLE_FIELDTYPE_Int);
	pTable->Add_Field(_TL("Elements")	, TABLE_FIELDTYPE_Int);
	pTable->Add_Field(_TL("Variance")	, TABLE_FIELDTYPE_Double);

	s.Printf(
		"\nNumber of Elements:\t%ld"
		"\nNumber of Variables:\t%d"
		"\nNumber of Clusters:\t\t%d"
		"\nValue of Target Function:\t%f"
		"\nCluster\tElements\tVariance",
		nElements, nGrids, nCluster, SP
	);

	for(j=0; j<nGrids; j++)
	{
		s.Append(CAPI_String::Format("\t%02d_%s", j + 1, Grids[j]->Get_Name()));
		pTable->Add_Field(Grids[j]->Get_Name(), TABLE_FIELDTYPE_Double);
	}

	Message_Add(s);

	for(i=0; i<nCluster; i++)
	{
		s.Printf("%d\t%d\t%f", i, nMembers[i], Variances[i]);

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, i);
		pRecord->Set_Value(1, nMembers[i]);
		pRecord->Set_Value(2, Variances[i]);

		for(j=0; j<nGrids; j++)
		{
			s.Append(CAPI_String::Format("\t%f", Centroids[i][j]));

			pRecord->Set_Value(j + 3, Centroids[i][j]);
		}

		Message_Add(s);
	}
}


///////////////////////////////////////////////////////////
//														 //
//					Minimum Distance					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// (nElements)-Array -> Bei Eingabe Startgruppierung oder 0.
// Bei Ausgabe Gruppierung: >Das ite Element ist im Cluster Cluster(i).

double CGrid_Cluster_Analysis::MinimumDistance(long &nElements, int nCluster)
{
	//-----------------------------------------------------
	// Variablen...

	bool	bContinue;
	int		iElement, iGrid, iCluster, nClusterElements, nShifts, minCluster, nPasses;
	double	d, Variance, minVariance, SP, SP_Last	= -1;


	//-----------------------------------------------------
	// Anfangspartition (Standard falls nicht vorgegeben

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
	// Hauptschleife der Iteration

	for(nPasses=1, bContinue=true; bContinue && Process_Get_Okay(false); nPasses++)
	{
		//-------------------------------------------------
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
		// Sift and Shift..

		SP		= 0;
		nShifts	= 0;

		for(iElement=0; iElement<nElements && bContinue; iElement++)
		{
			if( !(iElement % (nElements / 100)) && !Set_Progress(iElement, nElements) )
			{
				bContinue	= false;
			}

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

		Process_Set_Text(CAPI_String::Format(
			"Pass %d - Variance changed: %f", nPasses, SP_Last < 0.0 ? SP : SP_Last - SP
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
//					Hill-Climbing						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid_Cluster_Analysis::HillClimbing(long &nElements, int nCluster)
{
	//-----------------------------------------------------
	// Variablen...

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
	// Anfangspartition (Standard falls nicht vorgegeben)

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
	// Hauptschleife der Iteration

	noShift		= 0;

	for(nPasses=1, bContinue=true; bContinue && Process_Get_Okay(false); nPasses++)
	{
		//-------------------------------------------------
		for(iElement=0; iElement<nElements && bContinue; iElement++)
		{
			if( !(iElement % (nElements / 100)) && !Set_Progress(iElement, nElements) )
			{
				bContinue	= false;
			}

			if( pCluster->asInt(iElement) >= 0 )
			{
				if( noShift++ >= nElements )
				{
					bContinue	= false;
				}
				else
				{

					//---------------------------------------------
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

						//-----------------------------------------
						// Bestimme Gruppe iCluster mit evtl. groesster Verbesserung...

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


						//-----------------------------------------
						// Gruppenwechsel und Neuberechnung der Gruppencentroide...

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

		Process_Set_Text(CAPI_String::Format(
			"Pass %d - Variance changed: %f", nPasses, SP_Last < 0.0 ? SP : SP_Last - SP
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
