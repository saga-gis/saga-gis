/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    DistanceMatrix.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "DistanceMatrix.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDistanceMatrix::CDistanceMatrix(void)
{	
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Point Distances"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Computes distances between pairs of points."
	));

	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"), 
		_TL(""), 
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ID_POINTS"	, _TL("Identifier"),
		_TL(""),
		true
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "NEAR"		, _TL("Near Points"), 
		_TL(""), 
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ID_NEAR"		, _TL("Identifier"),
		_TL(""),
		true
	);

	Parameters.Add_Table(
		NULL	, "DISTANCES"	, _TL("Distances"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "FORMAT"		, _TL("Output Format"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("complete input times near points matrix"),
			_TL("each pair with a single record")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "MAX_DIST"	, _TL("Maximum Distance"),
		_TL("ignored if set to zero (consider all pairs)"),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDistanceMatrix::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FORMAT") )
	{
		pParameters->Get_Parameter("MAX_DIST")->Set_Enabled(pParameter->asInt() == 1);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NEAR") )
	{
		pParameters->Get_Parameter("ID_NEAR")->Set_Enabled(pParameter->asShapes() != NULL);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_ID(p, id, i)	(id < 0 ? CSG_String::Format(SG_T("%d"), i + 1) : CSG_String(p->Get_Shape(i)->asString(id)))

//---------------------------------------------------------
bool CDistanceMatrix::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS"   )->asShapes();
	int			id_Points	= Parameters("ID_POINTS")->asInt   ();
	CSG_Shapes	*pNear		= Parameters("NEAR"     )->asShapes();
	int			id_Near		= Parameters("ID_NEAR"  )->asInt   ();
	CSG_Table	*pDistances = Parameters("DISTANCES")->asTable ();
	double		max_Dist	= Parameters("MAX_DIST" )->asDouble();

	//-----------------------------------------------------
	if( pNear == NULL )
	{
		pNear	= pPoints;
		id_Near	= id_Points;
	}

	pDistances->Destroy();

	if( pPoints != pNear )
	{
		pDistances->Set_Name(CSG_String::Format(SG_T("%s [%s / %s]"), _TL("Distances"), pPoints->Get_Name(), pNear->Get_Name()));
	}
	else
	{
		pDistances->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Distances"), pPoints->Get_Name()));
	}

	//-----------------------------------------------------
	if( Parameters("FORMAT")->asInt() == 1 )
	{
		pDistances->Add_Field("ID_POINT", SG_DATATYPE_String);
		pDistances->Add_Field("ID_NEAR" , SG_DATATYPE_String);
		pDistances->Add_Field("DISTANCE", SG_DATATYPE_Double);

		for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
		{
			TSG_Point	Point	= pPoints->Get_Shape(iPoint)->Get_Point(0);

			for(int iNear=0; iNear<pNear->Get_Count(); iNear++)
			{
				if( pPoints != pNear || iPoint != iNear )
				{
					double	Distance	= SG_Get_Distance(Point, pNear->Get_Shape(iNear)->Get_Point(0));

					if( Distance <= max_Dist || max_Dist <= 0.0 )
					{
						CSG_Table_Record	*pRecord	= pDistances->Add_Record();

						pRecord->Set_Value(0, GET_ID(pPoints, id_Points, iPoint));
						pRecord->Set_Value(1, GET_ID(pNear  , id_Near  , iNear ));
						pRecord->Set_Value(2, Distance);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	else // Matrix
	{
		int	iNear;

		pDistances->Add_Field("ID_POINT", SG_DATATYPE_String);

		for(iNear=0; iNear<pNear->Get_Count(); iNear++)
		{
			pDistances->Add_Field(GET_ID(pNear, id_Near, iNear), SG_DATATYPE_Double);
		}

		for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
		{
			TSG_Point	Point	= pPoints->Get_Shape(iPoint)->Get_Point(0);

			CSG_Table_Record	*pRecord	= pDistances->Add_Record();

			pRecord->Set_Value(0, GET_ID(pPoints, id_Points, iPoint));

			for(iNear=0; iNear<pNear->Get_Count(); iNear++)
			{
				pRecord->Set_Value(1 + iNear, SG_Get_Distance(Point, pNear->Get_Shape(iNear)->Get_Point(0)));
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
