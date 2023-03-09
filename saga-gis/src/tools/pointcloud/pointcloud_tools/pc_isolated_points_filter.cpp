
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_isolated_points_filter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CIsolated_Points_Filter::CIsolated_Points_Filter(void)
{
	//-----------------------------------------------------
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
		"PC_OUT", _TL("Point Cloud Filtered"),
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
		CSG_String::Format(SG_T("%s|%s"),
			_TL("remove points"),
			_TL("label points")
		), 0
	);
}

//---------------------------------------------------------
bool CIsolated_Points_Filter::On_Execute(void)
{
	CSG_PointCloud	*pPC_in			= Parameters("PC_IN")->asPointCloud();
	CSG_PointCloud	*pPC_out		= Parameters("PC_OUT")->asPointCloud();
	double			dRadius			= Parameters("RADIUS")->asDouble();
	size_t			iMaxPoints		= Parameters("MAX_POINTS")->asInt();
	int				iMethod			= Parameters("METHOD")->asInt();
	
	enum
	{
		METHOD_REMOVE_PTS = 0,
		METHOD_LABEL_PTS
	};
	
	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing ..."));

	CSG_PointCloud PC_out;

	if( !pPC_out || pPC_out == pPC_in )
	{
		pPC_out = &PC_out;
	}

	pPC_out->Create(pPC_in);
	pPC_out->Add_Field(_TL("isolated"), SG_DATATYPE_Byte);
	int iFieldLabel = pPC_out->Get_Field_Count() - 1;

	//-----------------------------------------------------
	CSG_KDTree_2D		Search(pPC_in);
	std::vector<bool>	Isolated(pPC_in->Get_Count(), false);

	Set_Progress(20., 100.);


	//-----------------------------------------------------
	Process_Set_Text(_TL("Processing ..."));
		
	#pragma omp parallel for
	for(sLong iPoint=0; iPoint<pPC_in->Get_Count(); iPoint++)
	{
		if( SG_OMP_Get_Thread_Num() == 0 )
		{
			Set_Progress(20. + 55. / pPC_in->Get_Count() * iPoint * SG_OMP_Get_Max_Num_Threads(), 100.);
		}

		CSG_Array_Int	Indices;
		CSG_Vector		Distances;

		Search.Get_Nearest_Points(pPC_in->Get_X(iPoint), pPC_in->Get_Y(iPoint), 0, dRadius, Indices, Distances);

		if (Indices.Get_Size() <= iMaxPoints)
		{
			Isolated[iPoint] = true;
		}
	}
	
	Set_Progress(75., 100.);


	//-----------------------------------------------------
	Process_Set_Text(_TL("Writing ..."));

	sLong iCntIsolated = 0;

	for(sLong iPoint=0; iPoint<pPC_in->Get_Count() && Set_Progress(75. + 25. / pPC_in->Get_Count() * iPoint, 100.); iPoint++)
	{
		if( iMethod == METHOD_REMOVE_PTS && Isolated[iPoint] )
		{
			iCntIsolated++;
			continue;
		}

		pPC_out->Add_Point(pPC_in->Get_X(iPoint), pPC_in->Get_Y(iPoint), pPC_in->Get_Z(iPoint));

		for(int j=0; j<pPC_in->Get_Attribute_Count(); j++)
		{
			switch (pPC_in->Get_Attribute_Type(j))
			{
			default:					pPC_out->Set_Attribute(iPoint, j, pPC_in->Get_Attribute(iPoint, j));		break;
			case SG_DATATYPE_Date:
			case SG_DATATYPE_String:	CSG_String sAttr; pPC_in->Get_Attribute(iPoint, j, sAttr); pPC_out->Set_Attribute(iPoint, j, sAttr);		break;
			}
		}

		pPC_out->Set_Value(iFieldLabel, Isolated[iPoint]);

		if( Isolated[iPoint] )
		{
			iCntIsolated++;
		}
	}

	SG_UI_Msg_Add(_TL("Number of isolated points:") + CSG_String::Format(" %lld (%.2f%%)", iCntIsolated, iCntIsolated / (double)pPC_in->Get_Count() * 100.0), true);


	//-----------------------------------------------------
	if( pPC_out == &PC_out )
	{
		CSG_MetaData	History = pPC_in->Get_History();
		CSG_String		sName = pPC_in->Get_Name();

		pPC_in->Assign(pPC_out);

		pPC_in->Get_History() = History;
		pPC_in->Set_Name(sName);

		Parameters("PC_OUT")->Set_Value(pPC_in);
	}
	else
	{
		pPC_out->Fmt_Name("%s_filtered", pPC_in->Get_Name());
	}

	return( true );	
}


//---------------------------------------------------------
bool CIsolated_Points_Filter::On_After_Execution(void)
{
	CSG_PointCloud	*pPC_out = Parameters("PC_OUT")->asPointCloud();

	if( pPC_out == NULL )
	{
		pPC_out = Parameters("PC_IN")->asPointCloud();
	}

	if( pPC_out == Parameters("PC_IN")->asPointCloud() )
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
