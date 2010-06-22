
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
		"Module usage is different between SAGA GUI and SAGA CMD: With "
		"SAGA GUI you will get prompted to choose the attributes to use "
		"once you execute the module. With SAGA CMD you have to provide "
		"a string with the -ATTR_FIELDS parameter containing the field "
		"numbers of the attributes to use (separated by semicolon). Field "
		"numbers start with 1, e.g. -ATTR_FIELDS=\"4;5;7\".\n\n" 
		
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

	Parameters.Add_PointCloud(
		NULL	, "PC_IN"		,_TL("Point Cloud"),
		_TL("Input"),
		PARAMETER_INPUT
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

	if (!SG_UI_Get_Window_Main())
	{
		Parameters.Add_String(
            NULL	, "ATTR_FIELDS"    , _TL("Attribute Fields"),
            _TL("The numbers (starting from 1) of the fields to use for clustering, separated by semicolon, e.g. \"4;5;7\""),
            SG_T("")
        );
	}
	else
	{
		Parameters.Add_Value(
			NULL	, "UPDATEVIEW"	, _TL("Update View"),
			_TL("Update cluster view while clustering."),
			PARAMETER_TYPE_Bool, true
		);
	}
}

//---------------------------------------------------------
CPC_Cluster_Analysis::~CPC_Cluster_Analysis(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Cluster_Analysis::On_Execute(void)
{
	int						i, j, nCluster, cnt;
	long					nElements;
	double					SP;
	CSG_Parameters			Parms;

	//-----------------------------------------------------
	nCluster	= Parameters("NCLUSTER")->asInt();
	pInput		= Parameters("PC_IN")	->asPointCloud();
	pResult		= Parameters("PC_OUT")	->asPointCloud();
	pResult		->Create(pInput);
	pResult		->Set_Name(CSG_String::Format(SG_T("%s_cluster"), pInput->Get_Name()));
	pResult		->Add_Field(SG_T("CLUSTER"), SG_DATATYPE_Int);
	DataObject_Update(pResult);
	pResult		->Set_NoData_Value(- 1.0);
	clustField	= pResult->Get_Field_Count()-1;

	vFields.resize(pInput->Get_Field_Count());

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() )	// GUI
    {
		CSG_Parameters			Parms;
		CSG_String				fName, fID;

		Parms.Set_Name(_TL("Choose the attributes to use for clustering:"));

		for( int iField=0; iField<pInput->Get_Field_Count(); iField++ )
		{
			fName.Printf(SG_T("%s"), pInput->Get_Field_Name(iField));
			fID.Printf(SG_T("FIELD_%03d"), iField);
			Parms.Add_Value(NULL, fID, fName, _TL(""), PARAMETER_TYPE_Bool, false);
		}

		if( Dlg_Parameters(&Parms, _TL("")) )
		{
			for( int iField=0; iField<pInput->Get_Field_Count(); iField++ )
			{
				fID.Printf(SG_T("FIELD_%03d"), iField);

				if( Parms.Get_Parameter(fID)->asBool() )
					vFields.at(iField) = true;
			}
		}
		else
			return( false );
	}
	else		// CMD LINE
	{
		CSG_String		attrFields;
		CSG_String		token;
		int				field;

		Parameters.Add_Value(
			NULL	, "UPDATEVIEW"	, _TL("Update View"),
			_TL("Update cluster view while clustering."),
			PARAMETER_TYPE_Bool, false
		);

		attrFields		= Parameters("ATTR_FIELDS")->asString();

		wxStringTokenizer   tkz_fields(attrFields.c_str(), wxT(";"), wxTOKEN_STRTOK);

		while( tkz_fields.HasMoreTokens() )
		{
			token	= tkz_fields.GetNextToken();

			if( token.Length() == 0 )
				break;

			if( !token.asInt(field) )
			{
				SG_UI_Msg_Add_Error(_TL("Error parsing attribute fields: can't convert to number"));
				return( false );
			}

			field	-= 1;

			if( field < 0 || field > vFields.size() - 1 )
			{
				SG_UI_Msg_Add_Error(_TL("Error parsing attribute fields: field index out of range"));
				return( false );
			}
			else
				vFields.at(field)	= true;
		}
	}


	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing ..."));

	nFields		= 0;

	for( i=0; i<vFields.size(); i++ )
	{
		if( vFields.at(i) )
			nFields++;
	}

	if( nFields > 0 )
	{
		for( i=0; i<nFields; i++ )
			vValues.push_back( std::vector<double>() );

		for( i=0; i<pInput->Get_Record_Count() && SG_UI_Process_Set_Progress(i, pInput->Get_Record_Count()); i++ )
		{
			pResult->Add_Point(pInput->Get_X(i), pInput->Get_Y(i), pInput->Get_Z(i));
			
			cnt		= 0;

			for( j=0; j<pInput->Get_Field_Count(); j++ )
			{
				if( vFields.at(j) )
				{
					if( Parameters("NORMALISE")->asBool() )
						vValues.at(cnt).push_back( (pInput->Get_Value(i, j) - pInput->Get_Mean(j)) / pInput->Get_StdDev(j) );
					else
						vValues.at(cnt).push_back(pInput->Get_Value(i, j));

					cnt++;
				}

				if( j > 2 )
					pResult->Set_Value(j, pInput->Get_Value(i, j));
			}
		}
		
		pResult->Set_NoData(clustField);

		if( Parameters("UPDATEVIEW")->asBool() )
		{
			if( DataObject_Get_Parameters(pResult, Parms) && Parms("COLORS_TYPE") && Parms("COLORS_ATTRIB") && Parms("METRIC_COLORS") && Parms("METRIC_ZRANGE") )
			{
				Parms("COLORS_TYPE")					->Set_Value(2);			// graduated color
				Parms("METRIC_COLORS")->asColors()		->Set_Count(nCluster);
				Parms("COLORS_ATTRIB")					->Set_Value(clustField);
				Parms("METRIC_ZRANGE")->asRange()		->Set_Range(0, nCluster);
			}
			DataObject_Set_Parameters(pResult, Parms);
			DataObject_Update(pResult, SG_UI_DATAOBJECT_SHOW_LAST_MAP);
		}


		nMembers	= (int     *)SG_Malloc(nCluster * sizeof(int));
		Variances	= (double  *)SG_Malloc(nCluster * sizeof(double));
		Centroids	= (double **)SG_Malloc(nCluster * sizeof(double *));

		for( i=0; i<nCluster; i++ )
		{
			Centroids[i]	= (double  *)SG_Malloc(nFields * sizeof(double));
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

			for( i=0; i<vFields.size(); i++ )
			{
				if( vFields.at(i) )
				{
					for( j=0; j<nCluster; j++ )
					{
						Centroids[j][iv]	= sqrt(pInput->Get_Variance(i)) * Centroids[j][iv] + pInput->Get_Mean(i);
					}

					iv++;
				}
			}
		}

		Write_Result(Parameters("STATISTICS")->asTable(), nElements, nCluster, SP);

		//-------------------------------------------------
		if( DataObject_Get_Parameters(pResult, Parms) && Parms("COLORS_TYPE") && Parms("LUT") && Parms("COLORS_ATTRIB") )
		{
			CSG_Table_Record	*pClass;
			CSG_Table			*pLUT	= Parms("LUT")->asTable();

			for( i=0; i<nCluster; i++ )
			{
				if( (pClass = pLUT->Get_Record(i)) == NULL )
				{
					pClass	= pLUT->Add_Record();
					pClass->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
				}

				pClass->Set_Value(1, CSG_String::Format(SG_T("%s %d"), _TL("Class"), i + 1));
				pClass->Set_Value(2, CSG_String::Format(SG_T("%s %d"), _TL("Class"), i + 1));
				pClass->Set_Value(3, i);
				pClass->Set_Value(4, i + 1);
			}

			while( pLUT->Get_Record_Count() > nCluster )
			{
				pLUT->Del_Record(pLUT->Get_Record_Count() - 1);
			}

			Parms("COLORS_TYPE")	->Set_Value(1);	// Color Classification Type: Lookup Table
			Parms("COLORS_ATTRIB")	->Set_Value(clustField);

			DataObject_Set_Parameters(pResult, Parms);
		}

		//-------------------------------------------------
		for( i=0; i<nCluster; i++ )
		{
			SG_Free(Centroids[i]);
		}

		SG_Free(Centroids);
		SG_Free(Variances);
		SG_Free(nMembers);

		vFields.clear();
		vValues.clear();

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
void CPC_Cluster_Analysis::Write_Result(CSG_Table *pTable, long nElements, int nCluster, double SP)
{
	int					i, j, cntField;
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
		_TL("\nNumber of Variables")		, nFields,
		_TL("\nNumber of Clusters")			, nCluster,
		_TL("\nValue of Target Function")	, SP
	);

	s.Append(CSG_String::Format(SG_T("%s\t%s\t%s"), _TL("Cluster"), _TL("Elements"), _TL("Variance")));

	cntField	= 1;

	for( j=0; j<vFields.size(); j++ )
	{
		if( vFields.at(j) )
		{
			s.Append(CSG_String::Format(SG_T("\t%02d_%s"), cntField, pInput->Get_Field_Name(j)));
			pTable->Add_Field(pInput->Get_Field_Name(j), SG_DATATYPE_Double);
			cntField++;
		}
	}

	Message_Add(s);

	for( i=0; i<nCluster; i++ )
	{
		s.Printf(SG_T("%d\t%d\t%f"), i, nMembers[i], Variances[i]);

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, i);
		pRecord->Set_Value(1, nMembers[i]);
		pRecord->Set_Value(2, Variances[i]);

		for( j=0; j<nFields; j++ )
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

	if( Parameters("UPDATEVIEW")->asBool() )
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

			for( iField=0; iField<nFields; iField++ )
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

				for( iField=0; iField<nFields; iField++ )
				{
					Centroids[iCluster][iField]	+= vValues.at(iField).at(iElement);
				}
			}
		}

		//-------------------------------------------------
		for( iCluster=0; iCluster<nCluster; iCluster++ )
		{
			d		= nMembers[iCluster] > 0 ? 1.0 / (double)nMembers[iCluster] : 0;

			for( iField=0; iField<nFields; iField++ )
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

					for( iField=0; iField<nFields; iField++ )
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

		if( Parameters("UPDATEVIEW")->asBool() )
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

		for( iField=0; iField<nFields; iField++)
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

			for( iField=0; iField<nFields; iField++)
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

		for( iField=0; iField<nFields; iField++)
		{
			Centroids[iCluster][iField]	*= d;
			e							 = Centroids[iCluster][iField];
			V							+= e * e;
		}

		Variances[iCluster]	-= nMembers [iCluster] * V;
		SP					+= Variances[iCluster];
	}

	if( Parameters("UPDATEVIEW")->asBool() )
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

						for( iField=0; iField<nFields; iField++)
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

								for( iField=0; iField<nFields; iField++)
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

							for( iField=0; iField<nFields; iField++)
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

		if( Parameters("UPDATEVIEW")->asBool() )
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
