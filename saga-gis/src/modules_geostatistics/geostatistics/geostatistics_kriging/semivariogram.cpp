/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                  semivariogram.cpp                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
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
#include "semivariogram.h"

#include "variogram_dialog.h"


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
	FIELD_MODEL
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSemiVariogram::CSemiVariogram(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Variogram (Dialog))"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

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
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "RESULT"		, _TL("Semi-Variances"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DISTMAX"		, _TL("Maximum Distance"),
		_TL(""),
		PARAMETER_TYPE_Double	, -1.0
	);

	Parameters.Add_Value(
		NULL	, "DISTCOUNT"	, _TL("Initial Number of Distance Classes"),
		_TL(""),
		PARAMETER_TYPE_Int		, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NSKIP"		, _TL("Skip Number"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters;

	pParameters	= Add_Parameters(SG_T("FORMULA"), _TL("Formula"), _TL(""));

	pParameters->Add_String(
		NULL	, "STRING"		, _TL("Formula String"),
		_TL(""),
		SG_T("a + b * x")
	);

	//-----------------------------------------------------
	m_Variances.Add_Field(SG_T("DISTANCE")	, SG_DATATYPE_Double);
	m_Variances.Add_Field(SG_T("VAR_CUM")	, SG_DATATYPE_Double);
	m_Variances.Add_Field(SG_T("VAR_CLS")	, SG_DATATYPE_Double);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSemiVariogram::On_Execute(void)
{
	bool		bResult	= false;
	int			Attribute;
	CSG_Shapes	*pPoints;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	pTable		= Parameters("RESULT")		->asTable();
	Attribute	= Parameters("ATTRIBUTE")	->asInt();

	//-----------------------------------------------------
	if( Get_Variances(pTable, pPoints, Attribute) )
	{
		m_Variogram.Set_Formula	(Get_Parameters("FORMULA")->Get_Parameter("STRING")->asString());

		if( SG_UI_Get_Window_Main() )
		{
			CVariogram_Dialog	dlg(&m_Variogram, &m_Variances);

			bResult	= dlg.ShowModal() == wxID_OK;
		}
		else
		{
			m_Variogram.Clr_Data();

			for(int i=0; i<m_Variances.Get_Count(); i++)
			{
				m_Variogram.Add_Data(m_Variances[i][0], m_Variances[i][1]);
			}

			bResult	= m_Variogram.Get_Trend();
		}

		if( bResult && m_Variogram.is_Okay() )
		{
			Get_Parameters("FORMULA")->Get_Parameter("STRING")->Set_Value(m_Variogram.Get_Formula(SG_TREND_STRING_Formula));

			Message_Add(m_Variogram.Get_Formula(), false);

			for(int i=0; i<pTable->Get_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

				pRecord->Set_Value(FIELD_MODEL, m_Variogram.Get_Value(pRecord->asDouble(FIELD_DISTANCE)));
			}
		}
	}

	m_Variogram	.Clr_Data();
	m_Variances	.Del_Records();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSemiVariogram::Get_Variances(CSG_Table *pTable, CSG_Shapes *pPoints, int Attribute)
{
	int					i, j, k, n, nDistances, nSkip;
	double				z, d, dx, dy, maxDistance, lagDistance;
	TSG_Point			Pt_i, Pt_j;
	CSG_Vector			Count, Variance;
	CSG_Table_Record	*pRecord;
	CSG_Shape			*pPoint;

	//-----------------------------------------------------
	nSkip		= Parameters("NSKIP")		->asInt();
	maxDistance	= Parameters("DISTMAX")		->asDouble();
	nDistances	= Parameters("DISTCOUNT")	->asInt();

	if( maxDistance <= 0.0 )
	{
		maxDistance	= SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange());
	}

	lagDistance	= maxDistance / nDistances;

	Count		.Create(nDistances);
	Variance	.Create(nDistances);

	//-----------------------------------------------------
	for(i=0, n=0; i<pPoints->Get_Count()-nSkip && Set_Progress(n, SG_Get_Square(pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		pPoint	= pPoints->Get_Shape(i);
		Pt_i	= pPoint->Get_Point(0);
		z		= pPoint->asDouble(Attribute);

		for(j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
		{
			pPoint	= pPoints->Get_Shape(j);
			Pt_j	= pPoint->Get_Point(0);
			dx		= Pt_j.x - Pt_i.x;
			dy		= Pt_j.y - Pt_i.y;
			d		= sqrt(dx*dx + dy*dy);
			k		= (int)(d / lagDistance);

			if( k < nDistances )
			{
				d	= pPoint->asDouble(Attribute) - z;

				Count	[k]	++;
				Variance[k]	+= d * d;
			}
		}
	}

	//-----------------------------------------------------
	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Semivariogram"), pPoints->Get_Name()));
	pTable->Add_Field(_TL("Class")		, SG_DATATYPE_Int);		// FIELD_CLASSNR
	pTable->Add_Field(_TL("Distance")	, SG_DATATYPE_Double);	// FIELD_DISTANCE
	pTable->Add_Field(_TL("Count")		, SG_DATATYPE_Int);		// FIELD_COUNT
	pTable->Add_Field(_TL("Variance")	, SG_DATATYPE_Double);	// FIELD_VARIANCE
	pTable->Add_Field(_TL("Cum.Var.")	, SG_DATATYPE_Double);	// FIELD_VARCUMUL
	pTable->Add_Field(_TL("Model")		, SG_DATATYPE_Double);	// FIELD_MODEL

	pRecord	= pTable->Add_Record();
	pRecord->Set_Value(FIELD_CLASSNR	, 0.0);
	pRecord->Set_Value(FIELD_DISTANCE	, 0.0);
	pRecord->Set_Value(FIELD_COUNT		, 0.0);
	pRecord->Set_Value(FIELD_VARIANCE	, 0.0);
	pRecord->Set_Value(FIELD_VARCUMUL	, 0.0);

	for(i=0, z=0.0, n=0; i<nDistances; i++)
	{
		if( Count[i] > 0 )
		{
			n	+= (int)Count[i];
			z	+= Variance[i];

			pRecord	= pTable->Add_Record();
			pRecord->Set_Value(FIELD_CLASSNR	, (i + 1));
			pRecord->Set_Value(FIELD_DISTANCE	, (i + 1) * lagDistance);
			pRecord->Set_Value(FIELD_COUNT		, Count[i]);
			pRecord->Set_Value(FIELD_VARIANCE	, 0.5 * Variance[i] / Count[i]);
			pRecord->Set_Value(FIELD_VARCUMUL	, 0.5 * z / n);

			pRecord	= m_Variances.Add_Record();
			pRecord->Set_Value(0				, (i + 1) * lagDistance);
			pRecord->Set_Value(1				, 0.5 * Variance[i] / Count[i]);
			pRecord->Set_Value(2				, 0.5 * z / n);
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
