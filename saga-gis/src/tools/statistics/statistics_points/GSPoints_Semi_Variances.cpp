
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
//              GSPoints_Semi_Variances.cpp              //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GSPoints_Semi_Variances.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FIELD_CLASSNR = 0,
	FIELD_DISTANCE,
	FIELD_COUNT,
	FIELD_VARIANCE,
	FIELD_VARCUMUL,
	FIELD_COVARIANCE,
	FIELD_COVARCUMUL
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Semi_Variances::CGSPoints_Semi_Variances(void)
{
	Set_Name		(_TL("Variogram"));

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

	Parameters.Add_Table("",
		"RESULT"	, _TL("Sample Variogram"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int("",
		"DISTCOUNT"	, _TL("Initial Number of Distance Classes"),
		_TL(""),
		100, 1, true
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
bool CGSPoints_Semi_Variances::On_Execute(void)
{
	CSG_Shapes *pPoints = Parameters("POINTS"   )->asShapes();
	int       Attribute = Parameters("FIELD"    )->asInt();
	int           nSkip = Parameters("NSKIP"    )->asInt();
	int      nDistances = Parameters("DISTCOUNT")->asInt();
	double  maxDistance = Parameters("DISTMAX"  )->asDouble();

	if( maxDistance <= 0.0 )
	{
		maxDistance	= SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange());
	}

	double lagDistance = maxDistance / nDistances;

	double zMean = pPoints->Get_Mean(Attribute);

	//-----------------------------------------------------
	CSG_Vector Count(nDistances), Variance(nDistances), Covariance(nDistances);

	for(sLong i=0, n=0; i<pPoints->Get_Count() && Set_Progress((double)n, SG_Get_Square(pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Attribute) )
		{
			TSG_Point Pt_i = pPoint->Get_Point(0); double zi = pPoint->asDouble(Attribute);

			for(sLong j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Attribute) )
				{
					TSG_Point Pt_j = pPoint->Get_Point(0);

					int k = (int)(SG_Get_Distance(Pt_i, Pt_j) / lagDistance);

					if( k < nDistances )
					{
						double zj = pPoint->asDouble(Attribute);

						double v  = SG_Get_Square(zi - zj);
						double c  = (zi - zMean) * (zj - zMean);

						Count	  [k] ++;
						Variance  [k] += v;
						Covariance[k] += c;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Table *pTable = Parameters("RESULT")->asTable();
	pTable->Destroy();
	pTable->Fmt_Name("%s [%s: %s]", pPoints->Get_Name(), _TL("Variogram"), pPoints->Get_Field_Name(Attribute));
	pTable->Add_Field(_TL("Class"     ), SG_DATATYPE_Int   );	// FIELD_CLASSNR
	pTable->Add_Field(_TL("Distance"  ), SG_DATATYPE_Double);	// FIELD_DISTANCE
	pTable->Add_Field(_TL("Count"     ), SG_DATATYPE_Int   );	// FIELD_COUNT
	pTable->Add_Field(_TL("Variance"  ), SG_DATATYPE_Double);	// FIELD_VARIANCE
	pTable->Add_Field(_TL("Cum.Var."  ), SG_DATATYPE_Double);	// FIELD_VARCUMUL
	pTable->Add_Field(_TL("Covariance"), SG_DATATYPE_Double);	// FIELD_COVARIANCE
	pTable->Add_Field(_TL("Cum.Covar."), SG_DATATYPE_Double);	// FIELD_COVARCUMUL

	int n = 0; double v = 0., c = 0.;

	for(int i=0; i<nDistances; i++)
	{
		if( Count[i] > 0 )
		{
			n += (int)Count[i];
			v += Variance  [i];
			c += Covariance[i];

			CSG_Table_Record *pRecord = pTable->Add_Record();
			pRecord->Set_Value(FIELD_CLASSNR	, (i + 1));
			pRecord->Set_Value(FIELD_DISTANCE	, (i + 1) * lagDistance);
			pRecord->Set_Value(FIELD_COUNT		, Count[i]);
			pRecord->Set_Value(FIELD_VARIANCE	, 0.5 * Variance  [i] / Count[i]);
			pRecord->Set_Value(FIELD_VARCUMUL	, 0.5 * v / n);
			pRecord->Set_Value(FIELD_COVARIANCE	, 1.0 * Covariance[i] / Count[i]);
			pRecord->Set_Value(FIELD_COVARCUMUL	, 1.0 * c / n);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
