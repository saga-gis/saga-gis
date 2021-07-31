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
//                  pc_ground_filter.cpp                 //
//                                                       //
//                 Copyright (C) 2021 by                 //
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
#include "pc_ground_filter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGround_Filter::CGround_Filter(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Ground Classification"));

	Set_Author		("V. Wichmann (c) 2021");

	Set_Description	(_TW(
		"The tool allows one to filter a point cloud into ground (bare earth) and "
		"non-ground points. The ground points can be used later to create a "
		"digital elevation model from the data, for example.\n"
		"The tool uses concepts as described by Vosselman (2000) and is based on the "
		"assumption that a large height difference between two nearby points is unlikely "
		"to be caused by a steep slope in the terrain. The probability that the higher point "
		"might be non-ground increases when the distance between the two points decreases. "
		"Therefore the filter defines a maximum height difference (dz_max) between two points as "
		"a function of the distance (d) between the points (dz_max(d) = d). A point is classified "
		"as terrain if there is no other point within the kernel radius to which the height difference "
		"is larger than the allowed maximum height difference at the distance between these two points.\n"
		"The approximate terrain slope (s) parameter is used to modify the filter function to match "
		"the overall slope in the study area (dz_max(d) = d * s).\n"
		"A 5% confidence interval (ci = 1.65 * sqrt(2 * stddev)) may be used to modify the "
		"filter function even further by either relaxing (dz_max(d) = d * s + ci) or amplifying "
		"(dz_max(d) = d * s - ci) the filter criterium.\n\n"
	));

	Add_Reference("Vosselman, G.", "2000",
		"Slope based filtering of laser altimetry data",
		"IAPRS, Vol. XXXIII, Part B3, Amsterdam, The Netherlands, 935-942"
	);

	//-----------------------------------------------------
	Parameters.Add_PointCloud("",
		"PC_IN"	, _TL("Point Cloud"),
		_TL("The input point cloud to classify."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud("",
		"PC_OUT", _TL("Point Cloud Classified"),
		_TL("The classified point cloud."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"RADIUS", _TL("Filter Radius"),
		_TL("The radius of the filter kernel [map units]. Must be large enough to reach ground points next to non-ground objects."),
		2.5, 0.001, true
	);

	Parameters.Add_Double("",
		"TERRAINSLOPE", _TL("Terrain Slope [%]"),
		_TL("The approximate terrain slope [%]. Used to relax the filter criterium in steeper terrain."),
		30.0, 0.0, true
	);

	Parameters.Add_Choice("",
		"FILTERMOD", _TL("Filter Modification"),
		_TL("Choose whether to apply the filter kernel without modification or to use a confidence interval to relax or amplify the height criterium."),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
			_TL("relax filter"),
			_TL("amplify filter")
		), 0
	);

	Parameters.Add_Double("",
		"STDDEV", _TL("Standard Deviation"),
		_TL("The standard deviation used to calculate a 5% confidence interval applied to the height threshold [map units]."),
		0.1, 0.0, true
	);
}

//---------------------------------------------------------
bool CGround_Filter::On_Execute(void)
{
	CSG_PointCloud	*pPC_in			= Parameters("PC_IN")->asPointCloud();
	CSG_PointCloud	*pPC_out		= Parameters("PC_OUT")->asPointCloud();
	double			dRadius			= Parameters("RADIUS")->asDouble();
	double			dTerrainSlope	= Parameters("TERRAINSLOPE")->asDouble() / 100.0;
	int				iFilterMod		= Parameters("FILTERMOD")->asInt();
	double			dStdDev			= Parameters("STDDEV")->asDouble();

	//-----------------------------------------------------
	CSG_PointCloud PC_out;

	if( !pPC_out || pPC_out == pPC_in )
	{
		pPC_out = &PC_out;
	}

	pPC_out->Create(pPC_in);
	pPC_out->Add_Field(_TL("classification"), SG_DATATYPE_Byte);
	int iFieldClass = pPC_out->Get_Field_Count() - 1;


	//-----------------------------------------------------
	CSG_KDTree_2D	Search(pPC_in);

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<pPC_in->Get_Point_Count() && Set_Progress(iPoint, pPC_in->Get_Point_Count()); iPoint++)
	{
		CSG_Array_Int	Indices;
		CSG_Vector		Distances;

		Search.Get_Nearest_Points(pPC_in->Get_X(iPoint), pPC_in->Get_Y(iPoint), 0, dRadius, Indices, Distances);

		int iClass = 2;		// ground

		for(size_t i=0; i<Indices.Get_Size(); i++)
		{
			double dMaxDz = 0.0;

			switch (iFilterMod)
			{
			default:
			case 0:		dMaxDz = Distances[i] * dTerrainSlope;								break;
			case 1:		dMaxDz = Distances[i] * dTerrainSlope + 1.65 * sqrt(2 * dStdDev);	break;
			case 2:		double dz = Distances[i] * dTerrainSlope - 1.65 * sqrt(2 * dStdDev);
						dz > 0.0 ? dMaxDz = dz : dMaxDz = 0.0;
						break;
			}

			double dz = pPC_in->Get_Z(iPoint) - pPC_in->Get_Z(Indices[i]);

			if( dz > 0.0 && dz > dMaxDz )
			{
				iClass = 1;		// non-ground
				break;
			}
		}

		//-----------------------------------------------------
		pPC_out->Add_Point(pPC_in->Get_X(iPoint), pPC_in->Get_Y(iPoint), pPC_in->Get_Z(iPoint));

		for (int j=0; j<pPC_in->Get_Attribute_Count(); j++)
		{
			switch (pPC_in->Get_Attribute_Type(j))
			{
			default:					pPC_out->Set_Attribute(j, pPC_in->Get_Attribute(iPoint, j));		break;
			case SG_DATATYPE_Date:
			case SG_DATATYPE_String:	CSG_String sAttr; pPC_in->Get_Attribute(iPoint, j, sAttr); pPC_out->Set_Attribute(j, sAttr);		break;
			}
		}

		pPC_out->Set_Value(iFieldClass, iClass);
	}


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
		pPC_out->Fmt_Name("%s_classified", pPC_in->Get_Name());
	}

	return( true );	
}


//---------------------------------------------------------
bool CGround_Filter::On_After_Execution(void)
{
	CSG_PointCloud	*pPC_out = Parameters("PC_OUT")->asPointCloud();

	if( pPC_out == NULL )
	{
		pPC_out = Parameters("PC_IN")->asPointCloud();
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT = DataObject_Get_Parameter(pPC_out, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		CSG_Table_Record	*pRecord = pLUT->asTable()->Add_Record();
		pRecord->Set_Value(0, SG_GET_RGB(80, 80, 80));
		pRecord->Set_Value(1, _TL("Undefined"));
		pRecord->Set_Value(2, _TL("LAS Class 1"));
		pRecord->Set_Value(3, 1);
		pRecord->Set_Value(4, 1);

		pRecord = pLUT->asTable()->Add_Record();
		pRecord->Set_Value(0, SG_GET_RGB(180, 120, 0));
		pRecord->Set_Value(1, _TL("Ground"));
		pRecord->Set_Value(2, _TL("LAS Class 2"));
		pRecord->Set_Value(3, 2);
		pRecord->Set_Value(4, 2);

		DataObject_Set_Parameter(pPC_out, pLUT);
		DataObject_Set_Parameter(pPC_out, "COLORS_TYPE", 1);	// lookup table
		DataObject_Set_Parameter(pPC_out, "LUT_ATTRIB", pPC_out->Get_Field_Count() - 1);
	}

	if( pPC_out == Parameters("PC_IN")->asPointCloud() )
	{
		Parameters("PC_OUT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


//---------------------------------------------------------
int CGround_Filter::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if (pParameter->Cmp_Identifier("FILTERMOD"))
	{
		pParameters->Get_Parameter("STDDEV")->Set_Enabled(pParameter->asInt() > 0);
	}

	//-----------------------------------------------------
	return (1);
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
