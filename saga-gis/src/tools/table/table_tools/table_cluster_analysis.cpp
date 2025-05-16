
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "table_cluster_analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Cluster_Analysis::CTable_Cluster_Analysis(void)
{
	Set_Name		(CSG_String::Format("%s (%s)", _TL("Cluster Analysis"), _TL("Table Fields")));

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		"K-Means cluster analysis using selected features from attributes table."
	));

	Add_Reference("Forgy, E.", "1965",
		"Cluster Analysis of multivariate data: efficiency vs. interpretability of classifications",
		"Biometrics 21:768."
	);

	Add_Reference("Rubin, J.", "1967",
		"Optimal Classification into Groups: An Approach for Solving the Taxonomy Problem",
		"J. Theoretical Biology, 15:103-144."
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"INPUT"			, _TL("Table" ),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields("INPUT",
		"FIELDS"		, _TL("Features"),
		_TL("")
	);

	Parameters.Add_Bool("INPUT",
		"NORMALISE"		, _TL("Normalize"),
		_TL(""),
		false
	);

	Parameters.Add_Table_Field("INPUT",
		"CLUSTER"		, _TL("Cluster"),
		_TL("Target field for cluster numbers. If not set a new field will be added"),
		true
	);

	Parameters.Add_Table("",
		"STATISTICS"	, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Iterative Minimum Distance (Forgy 1965)"),
			_TL("Hill-Climbing (Rubin 1967)"),
			_TL("Combined Minimum Distance / Hillclimbing") 
		), 1
	);

	Parameters.Add_Int("",
		"NCLUSTER"		, _TL("Number of Clusters"),
		_TL(""),
		10, 2, true
	);

	Parameters.Add_Table ("", "RESULT_TABLE" , _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Shapes("", "RESULT_SHAPES", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Cluster_Analysis::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("INPUT") )
	{
		if( pParameter->asDataObject() )
		{
			pParameters->Set_Enabled("RESULT_TABLE" , pParameter->asDataObject()->asShapes() == NULL);
			pParameters->Set_Enabled("RESULT_SHAPES", pParameter->asDataObject()->asShapes() != NULL);
		}
		else
		{
			pParameters->Set_Enabled("RESULT_TABLE" , false);
			pParameters->Set_Enabled("RESULT_SHAPES", false);
		}
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Cluster_Analysis::On_Execute(void)
{
	int *Features = (int *)Parameters("FIELDS")->asPointer();
	int nFeatures =        Parameters("FIELDS")->asInt    ();

	if( !Features || nFeatures <= 0 )
	{
		Error_Set(_TL("no features in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Cluster_Analysis Analysis;

	if( !Analysis.Create(nFeatures) )
	{
		Error_Set(_TL("could not initialize cluster engine"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table *pInput = Parameters("INPUT")->asTable();

	CSG_Table *pTable = Parameters(pInput->asShapes() ? "RESULT_SHAPES" : "RESULT_TABLE")->asTable();

	if( pTable && pTable != pInput )
	{
		if( pInput->asShapes() )
		{
			pTable->asShapes()->Create(*pInput->asShapes());
		}
		else
		{
			pTable->Create(*pInput->asTable());
		}

		pTable->Fmt_Name("%s (%s)", pTable->Get_Name(), Get_Name().c_str());
	}
	else
	{
		pTable = pInput;
	}

	//-----------------------------------------------------
	int Cluster = Parameters("CLUSTER")->asInt();

	if( Cluster < 0 )
	{
		Cluster	= pTable->Get_Field_Count();

		pTable->Add_Field(_TL("CLUSTER"), SG_DATATYPE_Int);
	}

	//-----------------------------------------------------
	bool bNormalize = Parameters("NORMALISE")->asBool();

	for(sLong i=0, n=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record &Record = *pTable->Get_Record(i);

		bool bNoData = false;

		for(int iFeature=0; !bNoData && iFeature<nFeatures && !bNoData; iFeature++)
		{
			bNoData = Record.is_NoData(Features[iFeature]);
		}

		if( bNoData || !Analysis.Add_Element() )
		{
			Record.Set_NoData(Cluster);
		}
		else
		{
			Record.Set_Value(Cluster, 0.);

			for(int iFeature=0; iFeature<nFeatures; iFeature++)
			{
				double d = Record.asDouble(Features[iFeature]);

				if( bNormalize && pTable->Get_StdDev(Features[iFeature]) > 0. )
				{
					d = (d - pTable->Get_Mean(Features[iFeature])) / pTable->Get_StdDev(Features[iFeature]);
				}

				Analysis.Set_Feature((int)n, iFeature, d);
			}

			n++;
		}
	}

	if( Analysis.Get_nElements() <= 1 )
	{
		Error_Set(_TL("did not find valid feature records!"));

		return( false );
	}

	//-----------------------------------------------------
	bool bResult = Analysis.Execute(Parameters("METHOD")->asInt(), Parameters("NCLUSTER")->asInt());

	for(sLong i=0, n=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record &Record = *pTable->Get_Record(i);

		if( !Record.is_NoData(Cluster) )
		{
			Record.Set_Value(Cluster, Analysis.Get_Cluster((int)n++));
		}
	}

	Save_Statistics(pTable, Features, bNormalize, Analysis);

	Set_Classification(pTable, Cluster, Analysis.Get_nClusters());

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Cluster_Analysis::Save_Statistics(CSG_Table *pTable, int *Features, bool bNormalize, const CSG_Cluster_Analysis &Analysis)
{
	CSG_String s; CSG_Table &Statistics = *Parameters("STATISTICS")->asTable();

	Statistics.Destroy();
	Statistics.Set_Name(_TL("Cluster Analysis"));
	Statistics.Add_Field("ClusterID", SG_DATATYPE_Int   );
	Statistics.Add_Field("Elements" , SG_DATATYPE_Int   );
	Statistics.Add_Field("Std.Dev." , SG_DATATYPE_Double);

	s.Printf("\n%s:\t%d \n%s:\t%d \n%s:\t%d \n%s:\t%f\n\n%s\t%s\t%s",
		_TL("Number of Elements"      ), Analysis.Get_nElements(),
		_TL("Number of Variables"     ), Analysis.Get_nFeatures(),
		_TL("Number of Clusters"      ), Analysis.Get_nClusters(),
		_TL("Value of Target Function"), Analysis.Get_SP       (),
		_TL("Cluster"), _TL("Elements"), _TL("Std.Dev.")
	);

	for(int iFeature=0; iFeature<Analysis.Get_nFeatures(); iFeature++)
	{
		s += CSG_String::Format("\t%s", pTable->Get_Field_Name(Features[iFeature]));

		Statistics.Add_Field(pTable->Get_Field_Name(Features[iFeature]), SG_DATATYPE_Double);
	}

	Message_Add(s);

	for(int iCluster=0; iCluster<Analysis.Get_nClusters(); iCluster++)
	{
		s.Printf("\n%d\t%d\t%f", iCluster, Analysis.Get_nMembers(iCluster), sqrt(Analysis.Get_Variance(iCluster)));

		CSG_Table_Record &Record = *Statistics.Add_Record();

		Record.Set_Value(0, iCluster);
		Record.Set_Value(1, Analysis.Get_nMembers(iCluster));
		Record.Set_Value(2, sqrt(Analysis.Get_Variance(iCluster)));

		for(int iFeature=0; iFeature<Analysis.Get_nFeatures(); iFeature++)
		{
			double Centroid = Analysis.Get_Centroid(iCluster, iFeature);

			if( bNormalize )
			{
				Centroid = pTable->Get_Mean(Features[iFeature]) + Centroid * pTable->Get_StdDev(Features[iFeature]);
			}

			s += CSG_String::Format("\t%f", Centroid);

			Record.Set_Value(iFeature + 3, Centroid);
		}

		Message_Add(s, false);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Cluster_Analysis::Set_Classification(CSG_Table *pTable, int Cluster, int nClusters)
{
	DataObject_Update(pTable);

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pTable, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		for(int Class=0; Class<nClusters; Class++)
		{
			CSG_Table_Record *pClass = pLUT->asTable()->Get_Record(Class);

			if( !pClass )
			{
				(pClass	= pLUT->asTable()->Add_Record())->Set_Value(0, SG_Color_Get_Random());
			}

			pClass->Set_Value(1, CSG_String::Format("Cluster %02d", 1 + Class));
			pClass->Set_Value(2, "");
			pClass->Set_Value(3, Class);
			pClass->Set_Value(4, Class);
		}

		pLUT->asTable()->Set_Count(nClusters);

		DataObject_Set_Parameter(pTable, pLUT                  ); // Lookup Table
		DataObject_Set_Parameter(pTable, "LUT_FIELD"  , Cluster); // Lookup Table Attribute
		DataObject_Set_Parameter(pTable, "COLORS_TYPE", 1      ); // Color Classification Type: Lookup Table
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
