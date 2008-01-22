
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Points                  //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	DIF_FIELD_DISTANCE		= 0,
	DIF_FIELD_DIFFERENCE
};

//---------------------------------------------------------
enum
{
	VAR_FIELD_DISTANCE		= 0,
	VAR_FIELD_VARIANCE,
	VAR_FIELD_STDDEV,
	VAR_FIELD_CLASSVAR,
	VAR_FIELD_COUNT
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
	Set_Name	(_TL("Semi-Variogram"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

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
		NULL	, "RESULT"		, _TL("Semi-Variances"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DISTLAG"		, _TL("Distance Increment"),
		_TL(""),
		PARAMETER_TYPE_Double	, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DISTMAX"		, _TL("Maximum Distance"),
		_TL(""),
		PARAMETER_TYPE_Double	, -1.0
	);

	Parameters.Add_Value(
		NULL	, "NSKIP"		, _TL("Skip Number"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);
}

//---------------------------------------------------------
CGSPoints_Semi_Variances::~CGSPoints_Semi_Variances(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSPoints_Semi_Variances::On_Execute(void)
{
	CSG_Shapes		*pShapes;
	CSG_Table		*pTable, Table_Differences;

	//-----------------------------------------------------
	pShapes		= Parameters("POINTS")->asShapes();

	Get_Differences(
		pShapes,
		&Table_Differences,
		Parameters("FIELD")		->asInt(),
		Parameters("NSKIP")		->asInt(),
		Parameters("DISTMAX")	->asDouble()
	);

	//-----------------------------------------------------
	pTable		= Parameters("RESULT")->asTable();

	Get_Variances(
		pTable,
		&Table_Differences,
		Parameters("DISTLAG")	->asDouble()
	);

	//-----------------------------------------------------
	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]: %s"),
		pShapes->Get_Name(),
		pShapes->Get_Table().Get_Field_Name(Parameters("FIELD")->asInt()),
		_TL("Semi-Variances")
	));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGSPoints_Semi_Variances::Get_Differences(CSG_Shapes *pShapes, CSG_Table *pTable, int zField, int nSkip, double maxDist)
{
	int					iPoint, jPoint;
	double				d, dx, dy, z;
	CSG_Shape			*pPoint;
	CSG_Table_Record	*pRecord;
	TSG_Point			Pt_i, Pt_j;

	//-----------------------------------------------------
	pTable->Destroy();
	pTable->Add_Field(_TL("Distance")	, TABLE_FIELDTYPE_Double);	// DIF_FIELD_DISTANCE
	pTable->Add_Field(_TL("Difference")	, TABLE_FIELDTYPE_Double);	// DIF_FIELD_DIFFERENCE

	//-----------------------------------------------------
	for(iPoint=0; iPoint<pShapes->Get_Count()-nSkip && Set_Progress(iPoint, pShapes->Get_Count()-nSkip); iPoint+=nSkip)
	{
		pPoint	= pShapes->Get_Shape(iPoint);
		Pt_i	= pPoint->Get_Point(0);
		z		= pPoint->Get_Record()->asDouble(zField);

		for(jPoint=iPoint+nSkip; jPoint<pShapes->Get_Count(); jPoint+=nSkip)
		{
			pPoint	= pShapes->Get_Shape(jPoint);
			Pt_j	= pPoint->Get_Point(0);
			dx		= Pt_j.x - Pt_i.x;
			dy		= Pt_j.y - Pt_i.y;

			if( (d = sqrt(dx*dx + dy*dy)) < maxDist || maxDist < 0.0 )
			{
				pRecord	= pTable->Add_Record();
				pRecord->Set_Value(DIF_FIELD_DISTANCE	, d);
				pRecord->Set_Value(DIF_FIELD_DIFFERENCE	, pPoint->Get_Record()->asDouble(zField) - z);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGSPoints_Semi_Variances::Get_Variances(CSG_Table *pTab_Var, CSG_Table *pTab_Dif, double Dist_Step)
{
	int					iDif, nVar, nVarS;
	double				iDist, dz, zVar, zVarS, Dist;
	CSG_Table_Record	*pRec_Dif, *pRec_Var;

	//-----------------------------------------------------
	if( Process_Get_Okay(false) )
	{
		pTab_Var->Destroy();
		pTab_Var->Add_Field(_TL("Distance")			, TABLE_FIELDTYPE_Double);	// VAR_FIELD_DISTANCE
		pTab_Var->Add_Field(_TL("Variance")			, TABLE_FIELDTYPE_Double);	// VAR_FIELD_VARIANCE
		pTab_Var->Add_Field(_TL("Std.Deviation")	, TABLE_FIELDTYPE_Double);	// VAR_FIELD_STDDEV
		pTab_Var->Add_Field(_TL("Class Variance")	, TABLE_FIELDTYPE_Double);	// VAR_FIELD_CLASSVAR
		pTab_Var->Add_Field(_TL("Class Count")		, TABLE_FIELDTYPE_Int);		// VAR_FIELD_COUNT

		pTab_Dif->Set_Index(DIF_FIELD_DISTANCE, TABLE_INDEX_Up);

		//-----------------------------------------------------
		iDist		= 0.0;
		zVar		= 0.0;
		nVar		= 0;
		zVarS		= 0.0;
		nVarS		= 0;

		//-----------------------------------------------------
		for(iDif=0; iDif<pTab_Dif->Get_Record_Count() && Set_Progress(iDif, pTab_Dif->Get_Record_Count()); iDif++)
		{
			pRec_Dif	= pTab_Dif->Get_Record_byIndex(iDif);

			if( (Dist = pRec_Dif->asDouble(DIF_FIELD_DISTANCE)) > iDist )
			{
				if( nVar > 0 )
				{
					zVarS	+= zVar;
					nVarS	+= nVar;

					pRec_Var	= pTab_Var->Add_Record();
					pRec_Var->Set_Value(VAR_FIELD_DISTANCE	, iDist);
					pRec_Var->Set_Value(VAR_FIELD_VARIANCE	, 0.5 * zVarS / (double)nVarS);
					pRec_Var->Set_Value(VAR_FIELD_STDDEV	, sqrt(0.5 * zVarS / (double)nVarS));
					pRec_Var->Set_Value(VAR_FIELD_CLASSVAR	, 0.5 * zVar  / (double)nVar);
					pRec_Var->Set_Value(VAR_FIELD_COUNT		, nVar);
				}

				zVar		= 0.0;
				nVar		= 0;

				do	iDist	+= Dist_Step;	while( Dist > iDist );
			}

			dz		= pRec_Dif->asDouble(DIF_FIELD_DIFFERENCE);
			zVar	+= dz*dz;
			nVar++;
		}

		//-----------------------------------------------------
		pTab_Dif->Set_Index(DIF_FIELD_DISTANCE, TABLE_INDEX_None);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
