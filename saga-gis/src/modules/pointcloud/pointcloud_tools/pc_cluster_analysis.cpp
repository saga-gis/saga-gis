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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 pc_cluster_analysis.h                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
//                                                       //
//  This module is a port of the Grid_Cluster_Analysis   //
//  module from the Grid_Discretisation module library,  //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_cluster_analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Cluster_Analysis::CPC_Cluster_Analysis(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Cluster Analysis for Point Clouds"));

	Set_Author		(SG_T("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(		
		"Cluster Analysis for Point Clouds.\n\n"
		
		"References:\n\n"

		"This module is a port of the 'Cluster Analysis for Grids' "
		"module from the 'Imagery - Classification' module library, "
		"Copyright (C) 2003 by Olaf Conrad.\n\n"

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
	// 2. Datasets...

	CSG_Parameter *pNode = Parameters.Add_PointCloud(
		NULL	, "PC_IN"		,_TL("Point Cloud"),
		_TL("Input"),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("The attribute fields to cluster")
	);

	Parameters.Add_PointCloud(
		NULL	, "PC_OUT"		,_TL("Result"),
		_TL("Output"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL	, "STATISTICS"	, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// 3. General Parameters...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
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
		_TL("Automatically normalise attributes by standard deviation before clustering."),
		PARAMETER_TYPE_Bool, true
	);

	if (SG_UI_Get_Window_Main())
	{
		Parameters.Add_Value(
			NULL	, "UPDATEVIEW"	, _TL("Update View"),
			_TL("Update cluster view while clustering."),
			PARAMETER_TYPE_Bool, true
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Cluster_Analysis::On_Execute(void)
{
	int				nCluster;
	long			nElements;
	double			SP;
	CSG_Parameters Parms;

	m_bUpdateView = false;

	//-----------------------------------------------------
	nCluster	= Parameters("NCLUSTER")->asInt();
	pInput		= Parameters("PC_IN")	->asPointCloud();
	pResult		= Parameters("PC_OUT")	->asPointCloud();

	if( SG_UI_Get_Window_Main() )
		m_bUpdateView = Parameters("UPDATEVIEW")->asBool();

	//-------------------------------------------------
	m_Features	= (int *)Parameters("FIELDS")->asPointer();
	m_nFeatures	=        Parameters("FIELDS")->asInt    ();

	if( !m_Features || m_nFeatures <= 0 )
	{
		Error_Set(_TL("no features in selection"));

		return( false );
	}


	//-----------------------------------------------------
	pResult->Create(pInput);
	pResult->Set_Name(CSG_String::Format(SG_T("%s_cluster"), pInput->Get_Name()));
	pResult->Add_Field(SG_T("CLUSTER"), SG_DATATYPE_Int);
	DataObject_Update(pResult);

	pResult->Set_NoData_Value(-1.0);
	clustField	= pResult->Get_Field_Count() - 1;


	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing ..."));

	for( int i=0; i<m_nFeatures; i++ )
		vValues.push_back( std::vector<double>() );

	for( int i=0; i<pInput->Get_Record_Count() && SG_UI_Process_Set_Progress(i, pInput->Get_Record_Count()); i++ )
	{
		pResult->Add_Point(pInput->Get_X(i), pInput->Get_Y(i), pInput->Get_Z(i));

		for( int j=0; j<pInput->Get_Attribute_Count(); j++ )
			pResult->Set_Attribute(i, j, pInput->Get_Attribute(i, j));

		pResult->Set_NoData(i, clustField);
		
		bool bNoData = false;

		for( int j=0; j<m_nFeatures; j++)
		{
			if( pInput->is_NoData(i, m_Features[j]) )
			{
				bNoData = true;
				break;
			}
		}

		if( !bNoData )
		{
			for( int j=0; j<m_nFeatures; j++ )
			{
				if( Parameters("NORMALISE")->asBool() )
					vValues.at(j).push_back( (pInput->Get_Value(i, m_Features[j]) - pInput->Get_Mean(m_Features[j])) / pInput->Get_StdDev(m_Features[j]) );
				else
					vValues.at(j).push_back(pInput->Get_Value(i, m_Features[j]));
			}
		}
		else
		{
			for( int j=0; j<m_nFeatures; j++ )
			{
				vValues.at(j).push_back(pInput->Get_NoData_Value());
			}
		}
	}


	if( m_bUpdateView )
	{
		if( DataObject_Get_Parameters(pResult, Parms) && Parms("COLORS_TYPE") && Parms("METRIC_ATTRIB") && Parms("METRIC_COLORS") && Parms("METRIC_ZRANGE") )
		{
			Parms("COLORS_TYPE")					->Set_Value(2);			// graduated color
			Parms("METRIC_COLORS")->asColors()		->Set_Count(nCluster);
			Parms("METRIC_ATTRIB")					->Set_Value(clustField);
			Parms("METRIC_ZRANGE")->asRange()		->Set_Range(0, nCluster);
		}
		DataObject_Set_Parameters(pResult, Parms);
		DataObject_Update(pResult, SG_UI_DATAOBJECT_SHOW_LAST_MAP);
	}


	nMembers	= (int     *)SG_Malloc(nCluster * sizeof(int));
	Variances	= (double  *)SG_Malloc(nCluster * sizeof(double));
	Centroids	= (double **)SG_Malloc(nCluster * sizeof(double *));

	for( int i=0; i<nCluster; i++ )
	{
		Centroids[i]	= (double  *)SG_Malloc(m_nFeatures * sizeof(double));
	}

	//-------------------------------------------------
	nElements	= pInput->Get_Point_Count();

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

		nElements	= pInput->Get_Point_Count();	// may have been diminished because of no data values...

		SP	= HillClimbing		(nElements, nCluster);
		break;
	}

	//-------------------------------------------------
	if( Parameters("NORMALISE")->asBool() )
	{
		int		iv = 0;

		for( int i=0; i<m_nFeatures; i++ )
		{
			for( int j=0; j<nCluster; j++ )
			{
				Centroids[j][iv]	= sqrt(pInput->Get_Variance(m_Features[i])) * Centroids[j][iv] + pInput->Get_Mean(m_Features[i]);
			}

			iv++;
		}
	}

	Write_Result(Parameters("STATISTICS")->asTable(), nElements, nCluster, SP);

	//-------------------------------------------------
	if( DataObject_Get_Parameters(pResult, Parms) && Parms("COLORS_TYPE") && Parms("LUT") && Parms("LUT_ATTRIB") )
	{
		CSG_Table_Record	*pClass;
		CSG_Table			*pLUT	= Parms("LUT")->asTable();

		for( int i=0; i<nCluster; i++ )
		{
			if( (pClass = pLUT->Get_Record(i)) == NULL )
			{
				pClass	= pLUT->Add_Record();
				pClass->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
			}

			pClass->Set_Value(1, CSG_String::Format(SG_T("%s %d"), _TL("Class"), i));
			pClass->Set_Value(2, CSG_String::Format(SG_T("%s %d"), _TL("Class"), i));
			pClass->Set_Value(3, i);
			pClass->Set_Value(4, i);
		}

		while( pLUT->Get_Record_Count() > nCluster )
		{
			pLUT->Del_Record(pLUT->Get_Record_Count() - 1);
		}

		Parms("COLORS_TYPE")	->Set_Value(1);	// Color Classification Type: Lookup Table
		Parms("LUT_ATTRIB")		->Set_Value(clustField);

		DataObject_Set_Parameters(pResult, Parms);
	}

	//-------------------------------------------------
	for( int i=0; i<nCluster; i++ )
	{
		SG_Free(Centroids[i]);
	}

	SG_Free(Centroids);
	SG_Free(Variances);
	SG_Free(nMembers);

	vValues.clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPC_Cluster_Analysis::Write_Result(CSG_Table *pTable, long nElements, int nCluster, double SP)
{
	CSG_String			s;
	CSG_Table_Record	*pRecord;

	pTable->Destroy();
	s	= CSG_String::Format(_TL("Cluster Analysis PC"));
	s	+= CSG_String::Format(SG_T("_%s"), pInput->Get_Name());
	pTable->Set_Name(s);

	pTable->Add_Field(_TL("ClusterID")	, SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Elements")	, SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Variance")	, SG_DATATYPE_Double);

	s.Printf(SG_T("\n%s:\t%ld \n%s:\t%d \n%s:\t%d \n%s:\t%f"),
		_TL("Number of Elements")			, nElements,
		_TL("\nNumber of Variables")		, m_nFeatures,
		_TL("\nNumber of Clusters")			, nCluster,
		_TL("\nValue of Target Function")	, SP
	);

	s.Append(CSG_String::Format(SG_T("%s\t%s\t%s"), _TL("Cluster"), _TL("Elements"), _TL("Variance")));

	for( int j=0; j<m_nFeatures; j++ )
	{
		s.Append(CSG_String::Format(SG_T("\t%02d_%s"), j + 1, pInput->Get_Field_Name(m_Features[j])));
		pTable->Add_Field(pInput->Get_Field_Name(m_Features[j]), SG_DATATYPE_Double);
	}

	Message_Add(s);

	for( int i=0; i<nCluster; i++ )
	{
		s.Printf(SG_T("%d\t%d\t%f"), i, nMembers[i], Variances[i]);

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, i);
		pRecord->Set_Value(1, nMembers[i]);
		pRecord->Set_Value(2, Variances[i]);

		for( int j=0; j<m_nFeatures; j++ )
		{
			s.Append(CSG_String::Format(SG_T("\t%f"), Centroids[i][j]));

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

double CPC_Cluster_Analysis::MinimumDistance(long &nElements, int nCluster)
{
	//-----------------------------------------------------
	// Variablen...

	bool	bContinue;
	int		iElement, iField, iCluster, nClusterElements, nShifts, minCluster, nPasses;
	double	d, Variance, minVariance, SP, SP_Last	= -1;


	//-----------------------------------------------------
	// Anfangspartition (Standard falls nicht vorgegeben

	for( iElement=0, nClusterElements=0; iElement<nElements; iElement++ )
	{
		/*for( iField=0, bContinue=true; iField<nFields && bContinue; iField++)
		{
			if( Grids[iGrid]->is_NoData(iElement) )
			{
				bContinue	= false;
			}
		}*/
		bContinue	= true;

		if( bContinue )
		{
			if( pResult->Get_Value(iElement, clustField) < 0 || pResult->Get_Value(iElement, clustField) >= nCluster )
			{
				pResult->Set_Value(iElement, clustField, iElement % nCluster);
			}

			nClusterElements++;
		}
		else
		{
			pResult->Set_Value(iElement, clustField, -1);
		}
	}

	if( m_bUpdateView )
	{
		DataObject_Update(pResult);
	}


	//-----------------------------------------------------
	// Hauptschleife der Iteration

	for( nPasses=1, bContinue=true; bContinue && Process_Get_Okay(false); nPasses++ )
	{
		//-------------------------------------------------
		for( iCluster=0; iCluster<nCluster; iCluster++ )
		{
			Variances[iCluster]	= 0;
			nMembers [iCluster]	= 0;

			for( iField=0; iField<m_nFeatures; iField++ )
			{
				Centroids[iCluster][iField]	= 0;
			}
		}

		//-------------------------------------------------
		for( iElement=0; iElement<nElements; iElement++ )
		{
			if( pResult->Get_Value(iElement, clustField) >= 0 )
			{
				iCluster	= (int)pResult->Get_Value(iElement, clustField);
				nMembers[iCluster]++;

				for( iField=0; iField<m_nFeatures; iField++ )
				{
					Centroids[iCluster][iField]	+= vValues.at(iField).at(iElement);
				}
			}
		}

		//-------------------------------------------------
		for( iCluster=0; iCluster<nCluster; iCluster++ )
		{
			d		= nMembers[iCluster] > 0 ? 1.0 / (double)nMembers[iCluster] : 0;

			for( iField=0; iField<m_nFeatures; iField++ )
			{
				Centroids[iCluster][iField]	*= d;
			}
		}


		//-------------------------------------------------
		// Sift and Shift..

		SP		= 0;
		nShifts	= 0;

		for( iElement=0; iElement<nElements && bContinue; iElement++ )
		{
			if( !(iElement % (nElements / 100)) && !Set_Progress(iElement, nElements) )
			{
				bContinue	= false;
			}

			if( pResult->Get_Value(iElement, clustField) >= 0 )
			{
				minVariance	= -1;

				for( iCluster=0; iCluster<nCluster; iCluster++ )
				{
					Variance	= 0;

					for( iField=0; iField<m_nFeatures; iField++ )
					{
						d			= Centroids[iCluster][iField] - vValues.at(iField).at(iElement);
						Variance	+= d * d;
					}

					if( minVariance<0 || Variance<minVariance )
					{
						minVariance	= Variance;
						minCluster	= iCluster;
					}
				}

				if( pResult->Get_Value(iElement, clustField) != minCluster )
				{
					pResult->Set_Value(iElement, clustField, minCluster);
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

		if( m_bUpdateView )
		{
			DataObject_Update(pResult);
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
double CPC_Cluster_Analysis::HillClimbing(long &nElements, int nCluster)
{
	//-----------------------------------------------------
	// Variablen...

	bool	bContinue;
	int		iElement, iField, iCluster, jCluster, kCluster, nClusterElements, noShift, nPasses;
	double	d, e, n_iK, n_jK, V, VMin, V1, V2, SP, SP_Last	= -1;


	//-----------------------------------------------------

	for( iCluster=0; iCluster<nCluster; iCluster++ )
	{
		Variances[iCluster]	= 0;
		nMembers [iCluster]	= 0;

		for( iField=0; iField<m_nFeatures; iField++)
		{
			Centroids[iCluster][iField]	= 0;
		}
	}


	//-----------------------------------------------------
	// Anfangspartition (Standard falls nicht vorgegeben)

	for( iElement=0, nClusterElements=0; iElement<nElements; iElement++ )
	{
		/*for( iField=0, bContinue=true; iField<nFields && bContinue; iField++)
		{
			if( Grids[iGrid]->is_NoData(iElement) )
			{
				bContinue	= false;
			}
		}*/
		bContinue	= true;

		if( bContinue )
		{
			if( pResult->Get_Value(iElement, clustField) < 0 || pResult->Get_Value(iElement, clustField) >= nCluster )
			{
				pResult->Set_Value(iElement, clustField, iElement % nCluster);
			}

			nClusterElements++;
		
			iCluster	= (int)pResult->Get_Value(iElement, clustField);

			nMembers[iCluster]++;

			V			= 0.0;

			for( iField=0; iField<m_nFeatures; iField++)
			{
				d							 = vValues.at(iField).at(iElement);
				Centroids[iCluster][iField]	+= d;
				V							+= d * d;
			}

			Variances[iCluster]	+= V;
		}
		else
		{
			pResult->Set_Value(iElement, clustField, -1);
		}
	}


	//-----------------------------------------------------

	SP	= 0.0;

	for( iCluster=0; iCluster<nCluster; iCluster++ )
	{
		d	= nMembers[iCluster] != 0 ? 1.0 / (double)nMembers[iCluster] : 0;
		V	= 0.0;

		for( iField=0; iField<m_nFeatures; iField++)
		{
			Centroids[iCluster][iField]	*= d;
			e							 = Centroids[iCluster][iField];
			V							+= e * e;
		}

		Variances[iCluster]	-= nMembers [iCluster] * V;
		SP					+= Variances[iCluster];
	}

	if( m_bUpdateView )
	{
		DataObject_Update(pResult);
	}


	//-----------------------------------------------------
	// Hauptschleife der Iteration

	noShift		= 0;

	for( nPasses=1, bContinue=true; bContinue && Process_Get_Okay(false); nPasses++ )
	{
		//-------------------------------------------------
		for( iElement=0; iElement<nElements && bContinue; iElement++ )
		{
			if( !(iElement % (nElements / 100)) && !Set_Progress(iElement, nElements) )
			{
				bContinue	= false;
			}

			if( pResult->Get_Value(iElement, clustField) >= 0 )
			{
				if( noShift++ >= nElements )
				{
					bContinue	= false;
				}
				else
				{

					//---------------------------------------------
					iCluster	= (int)pResult->Get_Value(iElement, clustField);

					if( nMembers[iCluster] > 1 )
					{
						V	= 0.0;

						for( iField=0; iField<m_nFeatures; iField++)
						{
							d	= Centroids[iCluster][iField] - vValues.at(iField).at(iElement);
							V	+= d * d;
						}

						n_iK	= nMembers[iCluster];
						V1		= V * n_iK / (n_iK - 1.0);
						VMin	= -1.0;

						//-----------------------------------------
						// Bestimme Gruppe iCluster mit evtl. groesster Verbesserung...

						for( jCluster=0; jCluster<nCluster; jCluster++ )
						{
							if( jCluster != iCluster )
							{
								V	= 0.0;

								for( iField=0; iField<m_nFeatures; iField++)
								{
									d	= Centroids[jCluster][iField] - vValues.at(iField).at(iElement);
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

							for( iField=0; iField<m_nFeatures; iField++)
							{
								d							= vValues.at(iField).at(iElement);
								Centroids[iCluster][iField]	= (n_iK * Centroids[iCluster][iField] - d) * V1;
								Centroids[kCluster][iField]	= (n_jK * Centroids[kCluster][iField] + d) * V2;
							}

							pResult->Set_Value(iElement, clustField, kCluster);

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

		if( m_bUpdateView )
		{
			DataObject_Update(pResult);
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
