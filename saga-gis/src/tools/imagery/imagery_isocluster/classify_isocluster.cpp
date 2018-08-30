
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  imagery_isocluster                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                classify_isocluster.cpp                //
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
// Free Software Foundation, either version 3 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// see <http://www.gnu.org/licenses/>.                   //
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
#include "classify_isocluster.h"

//---------------------------------------------------------
#include "cluster_isodata.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cluster_ISODATA::CGrid_Cluster_ISODATA(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("ISODATA Clustering for Grids"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool executes the Isodata unsupervised "
		"classification - clustering algorithm. Isodata "
		"stands for Iterative Self-Organizing Data Analysis "
		"Techniques. This is a more sophisticated algorithm "
		"which allows the number of clusters to be "
		"automatically adjusted during the iteration by "
		"merging similar clusters and splitting clusters "
		"with large standard deviations. "
		"The tool is based on Christos Iosifidis' Isodata implementation. "
	));
	
	Add_Reference("http://users.ntua.gr/chiossif/Free_As_Freedom_Software/isodata.c",
		SG_T("isodata.c (Christos Iosifidis)")
	);

	Add_Reference("https://www.cs.umd.edu/~mount/Projects/ISODATA",
		SG_T("A Fast Implementation of the ISODATA Clustering Algorithm")
	);

	Add_Reference("Memarsadeghi, N., Mount, D. M., Netanyahu, N. S., Le Moigne, J.", "2007",
		"A Fast Implementation of the ISODATA Clustering Algorithm",
		"International Journal of Computational Geometry and Applications, 17, 71-103.",
		SG_T("https://www.cs.umd.edu/~mount/Projects/ISODATA/ijcga07-isodata.pdf"), SG_T("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"FEATURES"		, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CLUSTER"		, _TL("Clusters"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Table("",
		"STATISTICS"	, _TL("Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"NORMALIZE"		, _TL("Normalize"),
		_TL(""),
		false
	);

	Parameters.Add_Int("",
		"ITERATIONS"	, _TL("Maximum Number of Iterations"),
		_TL(""),
		20, 3, true
	);

	Parameters.Add_Int("",
		"CLUSTER_INI"	, _TL("Initial Number of Clusters"),
		_TL(""),
		5, 0, true
	);

	Parameters.Add_Int("",
		"CLUSTER_MAX"	, _TL("Maximum Number of Clusters"),
		_TL(""),
		16, 3, true
	);

	Parameters.Add_Int("",
		"SAMPLES_MIN"	, _TL("Minimum Number of Samples in a Cluster"),
		_TL(""),
		5, 2, true
	);

	//Parameters.Add_Double("",
	//	"DIST_MAX"	, _TL("Distance Threshold"),
	//	_TL("Clusters, which are closer than this distance to each other, are merged."),
	//	0.001, 0.0, true
	//);

	//Parameters.Add_Double("",
	//	"STDV_MAX"	, _TL("Maximum Standard Deviation within a Cluster"),
	//	_TL(""),
	//	10.0, 0.0, true
	//);

	Parameters.Add_Bool("",
		"RGB_COLORS"	, _TL("Update Colors from Features"),
		_TL("Use the first three features in list to obtain blue, green, red components for class colour in look-up table."),
		false
	)->Set_UseInCMD(false);

	Parameters.Add_Choice("",
		"INITIALIZE"	, _TL("Start Partition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("random"),
			_TL("periodical"),
			_TL("keep values") 
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Cluster_ISODATA::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return(CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter));
}

//---------------------------------------------------------
int CGrid_Cluster_ISODATA::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FEATURES") )
	{
		pParameters->Set_Enabled("RGB_COLORS", pParameter->asGridList()->Get_Grid_Count() >= 3);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cluster_ISODATA::On_Execute(void)
{
	int		iFeature;
	sLong	iCell;
	size_t	iSample, iCluster;

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pFeatures	= Parameters("FEATURES")->asGridList();

	CSG_Grid	*pCluster	= Parameters("CLUSTER")->asGrid();

	pCluster->Set_NoData_Value(0.0);

	bool	bNormalize	= Parameters("NORMALIZE")->asBool();

	//-----------------------------------------------------
	TSG_Data_Type	Data_Type;

	if( bNormalize )
	{
		Data_Type	= SG_DATATYPE_Float;
	}
	else
	{
		Data_Type	= SG_DATATYPE_Char;

		for(iFeature=0; iFeature<pFeatures->Get_Grid_Count(); iFeature++)
		{
			if( Data_Type < pFeatures->Get_Grid(iFeature)->Get_Type() )
			{
				Data_Type	= pFeatures->Get_Grid(iFeature)->Get_Type();
			}
		}

		Message_Add(CSG_String::Format("\n%s: %s", _TL("internal data type"), SG_Data_Type_Get_Name(Data_Type).c_str()), false);
	}

	//-----------------------------------------------------
	CCluster_ISODATA	Cluster(pFeatures->Get_Grid_Count(), Data_Type);

	Cluster.Set_Max_Iterations(Parameters("ITERATIONS" )->asInt   ());
	Cluster.Set_Ini_Clusters  (Parameters("CLUSTER_INI")->asInt   ());
	Cluster.Set_Max_Clusters  (Parameters("CLUSTER_MAX")->asInt   ());
	Cluster.Set_Min_Samples   (Parameters("SAMPLES_MIN")->asInt   ());
//	Cluster.Set_Max_Distance  (Parameters("DIST_MAX"   )->asDouble());
//	Cluster.Set_Max_StdDev    (Parameters("STDV_MAX"   )->asDouble());

	//-----------------------------------------------------
	for(iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
	{
		CSG_Vector	Features(pFeatures->Get_Grid_Count());

		for(iFeature=0; Features.Get_Size() && iFeature<pFeatures->Get_Grid_Count(); iFeature++)
		{
			if( pFeatures->Get_Grid(iFeature)->is_NoData(iCell) )
			{
				Features.Destroy();
			}
			else
			{
				Features[iFeature]	= pFeatures->Get_Grid(iFeature)->asDouble(iCell);

				if( bNormalize )
				{
					Features[iFeature]	= (Features[iFeature] - pFeatures->Get_Grid(iFeature)->Get_Mean()) / pFeatures->Get_Grid(iFeature)->Get_StdDev();
				}
			}
		}

		if( Features.Get_Size() )
		{
			Cluster.Add_Sample(Features);

			pCluster->Set_Value(iCell, 1.0);
		}
		else
		{
			pCluster->Set_Value(iCell, 0.0);
		}
	}

	//-----------------------------------------------------
	if( !Cluster.Run(Parameters("INITIALIZE")->asInt()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(iCell=0, iSample=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
	{
		if( pCluster->asInt(iCell) )
		{
			pCluster->Set_Value(iCell, 1 + Cluster.Get_Cluster(iSample++));
		}
	}

	//-----------------------------------------------------
	CSG_Table	&Statistics	= *Parameters("STATISTICS")->asTable();

	Statistics.Destroy();
	Statistics.Set_Name(_TL("ISODATA Cluster Statistics"));

	Statistics.Add_Field("CLUSTER" , SG_DATATYPE_Int);
	Statistics.Add_Field("ELEMENTS", SG_DATATYPE_Int);
	Statistics.Add_Field("MEANDIST", SG_DATATYPE_Double);

	for(iFeature=0; iFeature<pFeatures->Get_Grid_Count(); iFeature++)
	{
		Statistics.Add_Field(CSG_String::Format("MEAN.%s", pFeatures->Get_Grid(iFeature)->Get_Name()), SG_DATATYPE_Double);
		Statistics.Add_Field(CSG_String::Format("STDV.%s", pFeatures->Get_Grid(iFeature)->Get_Name()), SG_DATATYPE_Double);
	}

	for(iCluster=0; iCluster<Cluster.Get_Cluster_Count(); iCluster++)
	{
		CSG_Table_Record	&Record	= *Statistics.Add_Record();

		Record.Set_Value(0, iCluster + 1);
		Record.Set_Value(1, Cluster.Get_Cluster_Count (iCluster));
		Record.Set_Value(2, Cluster.Get_Cluster_StdDev(iCluster));

		for(iFeature=0; iFeature<pFeatures->Get_Grid_Count(); iFeature++)
		{
			double	Mean	= Cluster.Get_Cluster_Mean  (iCluster, iFeature);
			double	Stdv	= Cluster.Get_Cluster_StdDev(iCluster, iFeature);

			if( bNormalize )
			{
				Mean	= Mean * pFeatures->Get_Grid(iFeature)->Get_StdDev() + pFeatures->Get_Grid(iFeature)->Get_Mean();
				Stdv	= Stdv * pFeatures->Get_Grid(iFeature)->Get_StdDev();
			}

			Record.Set_Value(3 + 2 * iFeature + 0, Mean);
			Record.Set_Value(3 + 2 * iFeature + 1, Stdv);
		}
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pCluster, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		bool	bRGB	= pFeatures->Get_Grid_Count() >= 3 && Parameters("RGB_COLORS")->asBool();

		for(iCluster=0; iCluster<Statistics.Get_Count(); iCluster++)
		{
			CSG_Table_Record	*pClass	= pLUT->asTable()->Get_Record(iCluster);

			if( !pClass )
			{
				(pClass	= pLUT->asTable()->Add_Record())->Set_Value(0, SG_Color_Get_Random());
			}

			pClass->Set_Value(1, CSG_String::Format("%s %d", _TL("Cluster"), iCluster + 1));
			pClass->Set_Value(2, "");
			pClass->Set_Value(3, iCluster + 1);
			pClass->Set_Value(4, iCluster + 1);

			if( bRGB )
			{
				#define SET_COLOR_COMPONENT(c, i)	c = (int)(127 + (Statistics[iCluster].asDouble(3 + 2 * i) - pFeatures->Get_Grid(i)->Get_Mean()) * 127 / pFeatures->Get_Grid(i)->Get_StdDev()); if( c < 0 ) c = 0; else if( c > 255 ) c = 255;

				int	r; SET_COLOR_COMPONENT(r, 2);
				int	g; SET_COLOR_COMPONENT(g, 1);
				int	b; SET_COLOR_COMPONENT(b, 0);

				pClass->Set_Value(0, SG_GET_RGB(r, g, b));
			}
		}

		pLUT->asTable()->Set_Record_Count(Statistics.Get_Count());

		DataObject_Set_Parameter(pCluster, pLUT);
		DataObject_Set_Parameter(pCluster, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
