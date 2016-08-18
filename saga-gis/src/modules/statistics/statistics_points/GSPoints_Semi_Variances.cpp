/**********************************************************
 * Version $Id: GSPoints_Semi_Variances.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	FIELD_CLASSNR	= 0,
	FIELD_DISTANCE,
	FIELD_COUNT,
	FIELD_VARIANCE,
	FIELD_VARCUMUL,
	FIELD_COVARIANCE,
	FIELD_COVARCUMUL
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Semi_Variances::CGSPoints_Semi_Variances(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Variogram"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description(
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "RESULT"		, _TL("Sample Variogram"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DISTCOUNT"	, _TL("Initial Number of Distance Classes"),
		_TL(""),
		PARAMETER_TYPE_Int		, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "DISTMAX"		, _TL("Maximum Distance"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "NSKIP"		, _TL("Skip Number"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSPoints_Semi_Variances::On_Execute(void)
{
	int					i, j, k, n, nDistances, nSkip, Attribute;
	double				zi, zj, zMean, v, c, maxDistance, lagDistance;
	TSG_Point			Pt_i, Pt_j;
	CSG_Vector			Count, Variance, Covariance;
	CSG_Table_Record	*pRecord;
	CSG_Table			*pTable;
	CSG_Shape			*pPoint;
	CSG_Shapes			*pPoints;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	pTable		= Parameters("RESULT")		->asTable();
	Attribute	= Parameters("FIELD")		->asInt();
	nSkip		= Parameters("NSKIP")		->asInt();
	maxDistance	= Parameters("DISTMAX")		->asDouble();
	nDistances	= Parameters("DISTCOUNT")	->asInt();

	if( maxDistance <= 0.0 )
	{
		maxDistance	= SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange());
	}

	lagDistance	= maxDistance / nDistances;

	zMean		= pPoints->Get_Mean(Attribute);

	Count		.Create(nDistances);
	Variance	.Create(nDistances);
	Covariance	.Create(nDistances);

	//-----------------------------------------------------
	for(i=0, n=0; i<pPoints->Get_Count() && Set_Progress(n, SG_Get_Square(pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Attribute) )
		{
			Pt_i	= pPoint->Get_Point(0);
			zi		= pPoint->asDouble(Attribute);

			for(j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Attribute) )
				{
					Pt_j	= pPoint->Get_Point(0);
					k		= (int)(SG_Get_Distance(Pt_i, Pt_j) / lagDistance);

					if( k < nDistances )
					{
						zj	= pPoint->asDouble(Attribute);

						v	= SG_Get_Square(zi - zj);
						c	= (zi - zMean) * (zj - zMean);

						Count	  [k]	++;
						Variance  [k]	+= v;
						Covariance[k]	+= c;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s: %s]"), pPoints->Get_Name(), _TL("Variogram"), pPoints->Get_Field_Name(Attribute)));
	pTable->Add_Field(_TL("Class")		, SG_DATATYPE_Int);		// FIELD_CLASSNR
	pTable->Add_Field(_TL("Distance")	, SG_DATATYPE_Double);	// FIELD_DISTANCE
	pTable->Add_Field(_TL("Count")		, SG_DATATYPE_Int);		// FIELD_COUNT
	pTable->Add_Field(_TL("Variance")	, SG_DATATYPE_Double);	// FIELD_VARIANCE
	pTable->Add_Field(_TL("Cum.Var.")	, SG_DATATYPE_Double);	// FIELD_VARCUMUL
	pTable->Add_Field(_TL("Covariance")	, SG_DATATYPE_Double);	// FIELD_COVARIANCE
	pTable->Add_Field(_TL("Cum.Covar.")	, SG_DATATYPE_Double);	// FIELD_COVARCUMUL

	for(i=0, v=0.0, c=0.0, n=0; i<nDistances; i++)
	{
		if( Count[i] > 0 )
		{
			n	+= (int)Count[i];
			v	+= Variance  [i];
			c	+= Covariance[i];

			pRecord	= pTable->Add_Record();
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
