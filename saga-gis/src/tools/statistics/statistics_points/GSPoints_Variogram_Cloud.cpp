
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   statistics_points                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSPoints_Variogram_Cloud.cpp             //
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
#include "GSPoints_Variogram_Cloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	DIF_FIELD_DISTANCE = 0,
	DIF_FIELD_DIRECTION,
	DIF_FIELD_DIFFERENCE,
	DIF_FIELD_VARIANCE,
	DIF_FIELD_SEMIVARIANCE,
	DIF_FIELD_COVARIANCE
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Variogram_Cloud::CGSPoints_Variogram_Cloud(void)
{
	Set_Name		(_TL("Variogram Cloud"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description(
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"RESULT"	, _TL("Variogram Cloud"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"DISTMAX"	, _TL("Maximum Distance"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Int("",
		"NSKIP"		, _TL("Skip Number"),
		_TL(""),
		1, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSPoints_Variogram_Cloud::On_Execute(void)
{
	CSG_Shapes *pPoints = Parameters("POINTS")->asShapes();

	double maxDistance = Parameters("DISTMAX")->asDouble();

	if( maxDistance <= 0. )
	{
		maxDistance	= SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange());
	}

	int Attribute = Parameters("FIELD")->asInt();

	double zMean = pPoints->Get_Mean(Attribute);

	//-----------------------------------------------------
	CSG_Table *pTable = Parameters("RESULT")->asTable();
	pTable->Destroy();
	pTable->Fmt_Name("%s [%s]", pPoints->Get_Name(), _TL("Variogram Cloud"));
	pTable->Add_Field(_TL("Distance"    ), SG_DATATYPE_Double);	// DIF_FIELD_DISTANCE
	pTable->Add_Field(_TL("Direction"   ), SG_DATATYPE_Double);	// DIF_FIELD_DIRECTION
	pTable->Add_Field(_TL("Difference"  ), SG_DATATYPE_Double);	// DIF_FIELD_DIFFERENCE
	pTable->Add_Field(_TL("Variance"    ), SG_DATATYPE_Double);	// DIF_FIELD_VARIANCE
	pTable->Add_Field(_TL("Semivariance"), SG_DATATYPE_Double);	// DIF_FIELD_SEMIVARIANCE
	pTable->Add_Field(_TL("Covariance"  ), SG_DATATYPE_Double);	// DIF_FIELD_COVARIANCE

	//-----------------------------------------------------
	int nSkip = Parameters("NSKIP")->asInt();

	for(sLong i=0; i<pPoints->Get_Count()-nSkip && Set_Progress(i, pPoints->Get_Count()-nSkip); i+=nSkip)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Attribute) )
		{
			TSG_Point Pt_i = pPoint->Get_Point(0); double zi = pPoint->asDouble(Attribute);

			for(sLong j=i; j<pPoints->Get_Count() && Process_Get_Okay(); j+=nSkip)
			{
				pPoint = pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Attribute) )
				{
					TSG_Point Pt_j = pPoint->Get_Point(0); double d = SG_Get_Distance(Pt_i, Pt_j);

					if( d <= maxDistance )
					{
						CSG_Table_Record *pRecord = pTable->Add_Record();

						double zj = pPoint->asDouble(Attribute);

						pRecord->Set_Value(DIF_FIELD_DISTANCE    , d);
						pRecord->Set_Value(DIF_FIELD_DIRECTION   , SG_Get_Angle_Of_Direction(Pt_i, Pt_j) * M_RAD_TO_DEG);
						pRecord->Set_Value(DIF_FIELD_DIFFERENCE  , fabs(d = zi - zj));
						pRecord->Set_Value(DIF_FIELD_VARIANCE    , d = d*d);
						pRecord->Set_Value(DIF_FIELD_SEMIVARIANCE, 0.5*d);
						pRecord->Set_Value(DIF_FIELD_COVARIANCE  , (zi - zMean) * (zj - zMean));
					}
				}
			}
		}
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
