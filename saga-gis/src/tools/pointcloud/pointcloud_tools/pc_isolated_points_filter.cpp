
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              pc_isolated_points_filter.cpp            //
//                                                       //
//                 Copyright (C) 2023 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_isolated_points_filter.h"

#include <vector>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CIsolated_Points_Filter::CIsolated_Points_Filter(void)
{
	Set_Name		(_TL("Isolated Points Filter"));

	Set_Author		("V. Wichmann (c) 2023");

	Set_Description	(_TW(
		"The tool allows one to detect isolated points within a point cloud. "
		"These points can be either labeled as \"isolated\" or be removed "
		"entirely from the dataset.\n"
		"A point is assumed to be isolated as soon as the number of points in "
		"the search radius is below the specified threshold.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud("",
		"PC_IN"	, _TL("Point Cloud"),
		_TL("The input point cloud to filter."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud("",
		"PC_OUT", _TL("Filtered Point Cloud"),
		_TL("The filtered point cloud."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"RADIUS", _TL("Filter Radius"),
		_TL("The search radius of the filter [map units]."),
		1.5, 0.001, true
	);

	Parameters.Add_Int("",
		"MAX_POINTS", _TL("Maximum Number of Points"),
		_TL("The maximum number of points within the search radius to consider a point as isolated [-]. Includes the search point."),
		3, 1, true
	);
	
	Parameters.Add_Choice("",
		"METHOD", _TL("Method"),
		_TL("Choose the filter method."),
		CSG_String::Format("%s|%s",
			_TL("remove points"),
			_TL("label points")
		), 0
	);
}

//---------------------------------------------------------
bool CIsolated_Points_Filter::On_Execute(void)
{
	enum
	{
		METHOD_REMOVE_PTS = 0,
		METHOD_LABEL_PTS
	};

	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing ..."));

	CSG_PointCloud *pPoints = Parameters("PC_IN")->asPointCloud();

	CSG_KDTree_3D Search(pPoints); std::vector<bool> Isolated(pPoints->Get_Count(), false);

	//-----------------------------------------------------
	Process_Set_Text(_TL("Processing ..."));

	double Radius = Parameters("RADIUS")->asDouble(); size_t MaxPoints = Parameters("MAX_POINTS")->asInt();

	Set_Progress(0.2);

	#ifndef _DEBUG
	#pragma omp parallel for
	#endif
	for(sLong i=0; i<pPoints->Get_Count(); i++)
	{
		if( SG_OMP_Get_Thread_Num() == 0 )
		{
			Set_Progress(0.2 + 0.55 / pPoints->Get_Count() * i * SG_OMP_Get_Max_Num_Threads());
		}

		CSG_Array_Int Indices; CSG_Vector Distances;

		Search.Get_Nearest_Points(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i), 0, Radius, Indices, Distances);

		if( Indices.Get_uSize() <= MaxPoints )
		{
			Isolated[i] = true;
		}
	}
	
	Set_Progress(0.75);

	//-----------------------------------------------------
	Process_Set_Text(_TL("Writing ..."));

	CSG_PointCloud *pFiltered = Parameters("PC_OUT")->asPointCloud(), Filtered;

	if( !pFiltered || pFiltered == pPoints )
	{
		pFiltered = &Filtered;
	}

	pFiltered->Create(pPoints);
	pFiltered->Add_Field(_TL("isolated"), SG_DATATYPE_Byte);

	int Label = pFiltered->Get_Field_Count() - 1;

	int Method = Parameters("METHOD")->asInt();

	sLong nIsolated = 0;

	for(sLong i=0; i<pPoints->Get_Count() && Set_Progress(0.75 + 0.25 / pPoints->Get_Count() * i); i++)
	{
		if( Method == METHOD_REMOVE_PTS && Isolated[i] )
		{
			nIsolated++;

			continue;
		}

		pFiltered->Add_Point(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i));

		for(int j=0; j<pPoints->Get_Attribute_Count(); j++)
		{
			switch( pPoints->Get_Attribute_Type(j) )
			{
			default                : {
				pFiltered->Set_Attribute(j, pPoints->Get_Attribute(i, j));
				break; }

			case SG_DATATYPE_Date  :
			case SG_DATATYPE_String: { CSG_String Value; pPoints->Get_Attribute(i, j, Value);
				pFiltered->Set_Attribute(j, Value);
				break; }
			}
		}

		pFiltered->Set_Value(Label, Isolated[i]);

		if( Isolated[i] )
		{
			nIsolated++;
		}
	}

	//-----------------------------------------------------
	SG_UI_Msg_Add(_TL("Number of isolated points:") + CSG_String::Format(" %lld (%.2f%%)", nIsolated, 100. * nIsolated / (double)pPoints->Get_Count()), true);

	if( pFiltered == &Filtered )
	{
		CSG_MetaData History = pPoints->Get_History(); CSG_String Name = pPoints->Get_Name();

		pPoints->Assign(pFiltered);

		pPoints->Get_History() = History; pPoints->Set_Name(Name);

		Parameters("PC_OUT")->Set_Value(pPoints);
	}
	else
	{
		pFiltered->Fmt_Name("%s (filtered)", pPoints->Get_Name());
	}

	return( true );	
}

//---------------------------------------------------------
bool CIsolated_Points_Filter::On_After_Execution(void)
{
	CSG_PointCloud *pFiltered = Parameters("PC_OUT")->asPointCloud();

	if( pFiltered == NULL )
	{
		pFiltered = Parameters("PC_IN")->asPointCloud();
	}

	if( pFiltered == Parameters("PC_IN")->asPointCloud() )
	{
		Parameters("PC_OUT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
