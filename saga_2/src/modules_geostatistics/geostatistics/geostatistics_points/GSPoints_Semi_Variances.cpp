
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

//---------------------------------------------------------
#define DIF_FIELD_DISTANCE		0
#define DIF_FIELD_DIFFERENCE	1

#define VAR_FIELD_DISTANCE		0
#define VAR_FIELD_VARIANCE		1
#define VAR_FIELD_COUNT			2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Semi_Variances::CGSPoints_Semi_Variances(void)
{
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name	(_TL("Semivariogram"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(
		""
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		""
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "RESULT"		, _TL("Semivariances"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DISTSTEP"	, _TL("Distance Increment"),
		"",
		PARAMETER_TYPE_Double	, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DISTMAX"		, _TL("Maximum Distance"),
		"",
		PARAMETER_TYPE_Double	, -1.0
	);

	Parameters.Add_Value(
		NULL	, "NSKIP"		, _TL("Skip Number"),
		"",
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
	CShapes		*pShapes;
	CTable		*pTable, Table_Differences;

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
		Parameters("DISTSTEP")	->asDouble()
	);

	//-----------------------------------------------------
	pTable->Set_Name(CAPI_String::Format("%s [%s]: %s",
		pShapes->Get_Name(),
		pShapes->Get_Table().Get_Field_Name(Parameters("FIELD")->asInt()),
		"Semivariances")
	);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGSPoints_Semi_Variances::Get_Differences(CShapes *pShapes, CTable *pTable, int zField, int nSkip, double maxDist)
{
	int				iShape, iPart, iPoint, jShape, jPart, jPoint, nShapes;
	double			d, dx, dy, iz, z;
	CShape			*pShape_i, *pShape_j;
	CTable_Record	*pRecord;
	TGEO_Point		Pt_i, Pt_j;

	//-----------------------------------------------------
	pTable->Destroy();
	pTable->Add_Field(_TL("Distance")	, TABLE_FIELDTYPE_Double);	// DIF_FIELD_DISTANCE
	pTable->Add_Field(_TL("Difference")	, TABLE_FIELDTYPE_Double);	// DIF_FIELD_DIFFERENCE

	nShapes		= pShapes->Get_Count();

	//-----------------------------------------------------
	for(iShape=0; iShape<nShapes-1 && Set_Progress(iShape, nShapes-1); iShape+=nSkip)
	{
		pShape_i	= pShapes->Get_Shape(iShape);

		for(iPart=0; iPart<pShape_i->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape_i->Get_Point_Count(iPart); iPoint++)
			{
				Pt_i	= pShape_i->Get_Point(iPoint, iPart);
				iz		= pShape_i->Get_Record()->asDouble(zField);

				for(jShape=iShape+nSkip; jShape<nShapes; jShape+=nSkip)
				{
					pShape_j	= pShapes->Get_Shape(jShape);

					for(jPart=0; jPart<pShape_j->Get_Part_Count(); jPart++)
					{
						for(jPoint=0; jPoint<pShape_j->Get_Point_Count(jPart); jPoint++)
						{
							Pt_j	= pShape_j->Get_Point(jPoint, jPart);

							dx		= Pt_j.x - Pt_i.x;
							dy		= Pt_j.y - Pt_i.y;

							if( (d = sqrt(dx*dx + dy*dy)) < maxDist || maxDist < 0.0 )
							{
								z		= pShape_j->Get_Record()->asDouble(zField) - iz;

								pRecord	= pTable->Add_Record();

								pRecord->Set_Value(DIF_FIELD_DISTANCE	, d);
								pRecord->Set_Value(DIF_FIELD_DIFFERENCE	, z);
							}
						}
					}
				}
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
void CGSPoints_Semi_Variances::Get_Variances(CTable *pTab_Var, CTable *pTab_Dif, double Dist_Step)
{
	int				iDif, nVar;
	double			iDist, dz, zVar, Dist;
	CTable_Record	*pRec_Dif, *pRec_Var;

	//-----------------------------------------------------
	if( Process_Get_Okay(false) )
	{
		pTab_Var->Destroy();
		pTab_Var->Add_Field(_TL("Distance")		, TABLE_FIELDTYPE_Double);	// VAR_FIELD_DISTANCE
		pTab_Var->Add_Field(_TL("Semivariance")	, TABLE_FIELDTYPE_Double);	// VAR_FIELD_VARIANCE
		pTab_Var->Add_Field(_TL("Count")			, TABLE_FIELDTYPE_Int);		// VAR_FIELD_COUNT

		pTab_Dif->Set_Index(DIF_FIELD_DISTANCE, TABLE_INDEX_Up);

		//-----------------------------------------------------
		iDist		= 0.0;
		zVar		= 0.0;
		nVar		= 0;

		//-----------------------------------------------------
		for(iDif=0; iDif<pTab_Dif->Get_Record_Count() && Set_Progress(iDif, pTab_Dif->Get_Record_Count()); iDif++)
		{
			pRec_Dif	= pTab_Dif->Get_Record_byIndex(iDif);

			if( (Dist = pRec_Dif->asDouble(DIF_FIELD_DISTANCE)) > iDist )
			{
				if( nVar > 0 )
				{
					pRec_Var	= pTab_Var->Add_Record();
					pRec_Var->Set_Value(VAR_FIELD_DISTANCE	, iDist);
					pRec_Var->Set_Value(VAR_FIELD_VARIANCE	, 0.5 * zVar / (double)nVar);
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
