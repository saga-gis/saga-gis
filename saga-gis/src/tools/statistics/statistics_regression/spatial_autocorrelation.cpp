
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              spatial_autocorrelation.cpp              //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
#include "spatial_autocorrelation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSpatial_Autocorrelation::CSpatial_Autocorrelation(void)
{
	Set_Name		(_TL("Global Spatial Autocorrelation for Shapes"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"Calculates Moran's I and Geary's C for the selected variable of the input shapes. "
		"For geometries other than single points (i.e. multi-points, lines, polygons) the "
		"centroids are used as location. "
	));

	Add_Reference("Lloyd, C.D.", "2011",
		"Local Models for Spatial Analysis",
		"CRC Press, Taylor & Francis Group, 2nd Edition, 336p."
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"  , _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"VARIABLE", _TL("Variable"),
		_TL("")
	);

	Parameters.Add_Table("",
		"SUMMARY" , _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Set_IDW_Power(1.);
	m_Weighting.Create_Parameters(Parameters);

	m_Search.Create(&Parameters, "NODE_SEARCH");
	Parameters("SEARCH_RANGE"     )->Set_Value(1);
	Parameters("SEARCH_POINTS_ALL")->Set_Value(1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSpatial_Autocorrelation::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SHAPES") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);

		if( pParameter->asShapes() )
		{
			CSG_Rect Extent(pParameter->asShapes()->Get_Extent());

			pParameters->Set_Parameter("DW_BANDWIDTH", 0.05 * Extent.Get_Diameter());
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CSpatial_Autocorrelation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Weighting.Enable_Parameters(*pParameters);

	m_Search.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpatial_Autocorrelation::On_Execute(void)
{
	int Field; CSG_Shapes Points, *pPoints;

	pPoints = Get_Points(Field, Points);

	if( !pPoints )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Weighting.Set_Parameters(Parameters);

	if( !m_Search.Initialize(pPoints, -1) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double Xm = pPoints->Get_Mean(Field), dXidXj_Sum = 0., dXiXj2_Sum = 0., dXi2_Sum = 0., Wij_Sum = 0.; sLong N = 0;

	for(sLong i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		CSG_Shape &Pi = *pPoints->Get_Shape(i);

		if( Pi.is_NoData(Field) )
		{
			continue;
		}

		double Xi = Pi.asDouble(Field);

		dXi2_Sum += (Xi - Xm)*(Xi - Xm); N++;

		CSG_Array_sLong Index;

		if( !m_Search.Do_Use_All() && !m_Search.Get_Points(Pi.Get_Point(), Index) )
		{
			continue;
		}

		sLong nPoints = m_Search.Do_Use_All() ? pPoints->Get_Count() : Index.Get_Size();

		for(sLong j=0; j<nPoints; j++)
		{
			CSG_Shape &Pj = *pPoints->Get_Shape(m_Search.Do_Use_All() ? j : Index[j]);

			if( Pj.is_NoData(Field) )
			{
				continue;
			}

			double Dij = SG_Get_Distance(Pi.Get_Point(), Pj.Get_Point());
			double Wij = m_Weighting.Get_Weight(Dij);
			double Xj  = Pj.asDouble(Field);

			dXidXj_Sum += Wij * (Xi - Xm) * (Xj - Xm);
			dXiXj2_Sum += Wij * (Xi - Xj) * (Xi - Xj);
			Wij_Sum    += Wij;
		}
	}

	m_Search.Finalize();

	//-----------------------------------------------------
	double I = N * dXidXj_Sum / (dXi2_Sum * Wij_Sum);
	double C = (N - 1) * dXiXj2_Sum / (2*dXi2_Sum * Wij_Sum);

	Message_Fmt("\nMoran's I: %f\nGeary's C: %f", I, C);

	#define Add_Summary(p, v) { CSG_Table_Record &r = *Summary.Add_Record(); r.Set_Value(0, p); r.Set_Value(1, v); }

	CSG_Table &Summary = *Parameters("SUMMARY")->asTable();
	Summary.Destroy(); Summary.Fmt_Name("%s [%s.%s]", _TL("Spatial Autocorrelation"), pPoints->Get_Name(), pPoints->Get_Field_Name(Field));
	Summary.Add_Field("Property", SG_DATATYPE_String);
	Summary.Add_Field("Value"   , SG_DATATYPE_String);

	Add_Summary("Moran's I", I);
	Add_Summary("Geary's C", C);

	DataObject_Update(&Summary, SG_UI_DATAOBJECT_SHOW_MAP);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * CSpatial_Autocorrelation::Get_Points(int &Field, CSG_Shapes &Points)
{
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes(); Field = Parameters("VARIABLE")->asInt();

	if( pShapes->Get_Count() < 2 )
	{
		return( NULL );
	}

	if( pShapes->Get_Type() == SHAPE_TYPE_Point )
	{
		return( pShapes );
	}

	Points.Create(SHAPE_TYPE_Point); Points.Add_Field(pShapes->Get_Field_Name(Field), SG_DATATYPE_Double); Points.Set_Name(pShapes->Get_Name());

	for(sLong iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape *pShape = pShapes->Get_Shape(iShape), *pPoint = Points.Add_Shape();

		pPoint->Set_Point(pShape->Get_Centroid());

		pPoint->Set_Value(0, pShape->asDouble(Field));
	}

	Field = 0;

	return( &Points );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
