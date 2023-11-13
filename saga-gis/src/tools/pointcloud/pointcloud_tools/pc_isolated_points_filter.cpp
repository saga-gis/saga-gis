
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
		"These points can be either tagged as \"isolated\" or be removed "
		"entirely from the dataset.\n"
		"A point is assumed to be isolated as soon as the number of points in "
		"the search radius is below the specified threshold.\n"
		"If isolated points become tagged, a new attribute field \"ISOLATED\" "
		"is added that provides the number of neighbours found for isolated "
		"points (including the point itself) or zero for all other points. "
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud("",
		"PC_IN"	, _TL("Points"),
		_TL("The input point cloud to analyze."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud("",
		"PC_OUT", _TL("Filtered Points"),
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
		_TL("The maximum number of points within the search radius to consider a point as isolated. Includes the search point."),
		1, 1, true, 255, true
	);
	
	Parameters.Add_Choice("",
		"METHOD", _TL("Method"),
		_TL("Choose the filter method."),
		CSG_String::Format("%s|%s",
			_TL("remove points"),
			_TL("tag points")
		), 0
	);
}

//---------------------------------------------------------
bool CIsolated_Points_Filter::On_Execute(void)
{
	CSG_PointCloud *pPoints = Parameters("PC_IN")->asPointCloud();

	if( pPoints->Get_Count() < 1 )
	{
		Error_Set(_TL("Input points must not be empty!"));

		return( false );
	}

	if( Parameters("PC_OUT")->asPointCloud() == pPoints )
	{
		Parameters("PC_OUT")->Set_Value(DATAOBJECT_NOTSET);
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing..."));

	CSG_KDTree_3D Search(pPoints);

	double Radius = Parameters("RADIUS")->asDouble(); size_t MaxPoints = Parameters("MAX_POINTS")->asInt();

	//-----------------------------------------------------
	if( Parameters("METHOD")->asInt() == 1 ) // tagging isolated points
	{
		Process_Set_Text(_TL("Tagging..."));

		if( Parameters("PC_OUT")->asPointCloud() && Parameters("PC_OUT")->asPointCloud() != pPoints )
		{
			Parameters("PC_OUT")->asPointCloud()->Create(*pPoints); // copy constructor

			pPoints = Parameters("PC_OUT")->asPointCloud();

			pPoints->Fmt_Name("%s [%s]", pPoints->Get_Name(), _TL("tagged"));
		}

		int Tag = pPoints->Get_Attribute_Count();

		pPoints->Add_Field("ISOLATED", SG_DATATYPE_Byte);

		#pragma omp parallel for
		for(sLong i=0; i<pPoints->Get_Count(); i++)
		{
			if( !SG_OMP_Get_Thread_Num() )
			{
				Set_Progress(i * SG_OMP_Get_Max_Num_Threads() / (double)pPoints->Get_Count());
			}

			CSG_Array_sLong Indices; CSG_Vector Distances;

			Search.Get_Nearest_Points(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i), 0, Radius, Indices, Distances);

			size_t n = Indices.Get_uSize();

			pPoints->Set_Attribute(i, Tag, n > MaxPoints ? 0. : n > 255 ? 255. : (double)n);
		}
	}

	//-----------------------------------------------------
	else // remove isolated points
	{
		Process_Set_Text(_TL("Tagging..."));

		sLong nRemoved = 0, nPoints = pPoints->Get_Count(); std::vector<bool> Isolated(nPoints, false);

		#pragma omp parallel for
		for(sLong i=0; i<nPoints; i++)
		{
			if( !SG_OMP_Get_Thread_Num() )
			{
				Set_Progress(0.5 * i * SG_OMP_Get_Max_Num_Threads() / (double)nPoints);
			}

			CSG_Array_sLong Indices; CSG_Vector Distances;

			Search.Get_Nearest_Points(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i), 0, Radius, Indices, Distances);

			Isolated[i] = Indices.Get_uSize() <= MaxPoints;
		}

		Process_Set_Text(_TL("Filtering..."));

		CSG_PointCloud *pFiltered = Parameters("PC_OUT")->asPointCloud();

		if( !pFiltered || pFiltered == pPoints )
		{
			pPoints->Select(); // just in case, clear any selection!

			for(sLong i=0; i<nPoints && Set_Progress(0.5 + 0.5 * i / (double)nPoints); i++)
			{
				if( Isolated[i] )
				{
					nRemoved++;

					pPoints->Select(i, true);
				}
			}

			pPoints->Del_Selection(); // the fastest way to delete records from a point cloud
		}
		else
		{
			pFiltered->Create(pPoints);

			pFiltered->Fmt_Name("%s [%s]", pPoints->Get_Name(), _TL("filtered"));

			for(sLong i=0; i<nPoints && Set_Progress(0.5 + 0.5 * i / (double)nPoints); i++)
			{
				if( Isolated[i] )
				{
					nRemoved++;

					continue;
				}

				pFiltered->Add_Point(pPoints->Get_X(i), pPoints->Get_Y(i), pPoints->Get_Z(i));

				for(int Field=3; Field<pPoints->Get_Field_Count(); Field++)
				{
					if( pPoints->is_NoData(i, Field) )
					{
						pFiltered->Set_NoData(i, Field);
					}
					else switch( pPoints->Get_Field_Type(Field) )
					{
					default                : {
						pFiltered->Set_Value(Field, pPoints->Get_Value(i, Field));
						break; }

					case SG_DATATYPE_Date  :
					case SG_DATATYPE_String: { CSG_String Value; pPoints->Get_Value(i, Field, Value);
						pFiltered->Set_Value(Field, Value);
						break; }
					}
				}
			}
		}

		Message_Fmt("\n%s: %lld (%.2f%%)", _TL("Number of removed points"), nRemoved, 100. * nRemoved / (double)nPoints);
	}

	//-----------------------------------------------------
	if( !Parameters("PC_OUT")->asPointCloud() ) // ...do we need to update the input points?!
	{
		DataObject_Update(pPoints);
	}

	return( true );	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
