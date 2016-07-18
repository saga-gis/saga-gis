/**********************************************************
 * Version $Id: line_crossings.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  line_crossings.cpp                   //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "line_crossings.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Crossings::CLine_Crossings(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Crossings"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES_A"		, _TL("1st Lines Layer"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "LINES_B"		, _TL("2nd Lines Layer"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "CROSSINGS"	, _TL("Crossings"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "ATTRIBUTES"	, _TL("Parent Attributes"),
		_TL("attributes inherited by parent lines layers"),
		CSG_String::Format("%s|%s|%s|",
			_TL("index"),
			_TL("attributes"),
			_TL("index and attributes")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Crossings::On_Execute(void)
{
	CSG_Shapes	*pLines_A	= Parameters("LINES_A"  )->asShapes();
	CSG_Shapes	*pLines_B	= Parameters("LINES_B"  )->asShapes();
	CSG_Shapes	*pCrossings	= Parameters("CROSSINGS")->asShapes();

	if(	!pLines_A->is_Valid() || !pLines_B->is_Valid() || pLines_A->Get_Extent().Intersects(pLines_B->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no intersection"));

		return( false );
	}

	//--------------------------------------------------------
	int	Attributes	= Parameters("ATTRIBUTES")->asInt();

	pCrossings->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s - %s]", _TL("Crossings"), pLines_A->Get_Name(), pLines_B->Get_Name()));

	if( Attributes == 0 || Attributes == 2 )
	{
		pCrossings->Add_Field("ID_A", SG_DATATYPE_Int);
		pCrossings->Add_Field("ID_B", SG_DATATYPE_Int);
	}

	if( Attributes == 1 || Attributes == 2 )
	{
		Add_Attributes(pCrossings, pLines_A);
		Add_Attributes(pCrossings, pLines_B);
	}

	//--------------------------------------------------------
	for(int aLine=0, iPair=0, nPairs=pLines_A->Get_Count()*pLines_B->Get_Count() && Process_Get_Okay(); aLine<pLines_A->Get_Count(); aLine++)
	{
		CSG_Shape_Line	*pA	= (CSG_Shape_Line *)pLines_A->Get_Shape(aLine);

		for(int bLine=0; bLine<pLines_B->Get_Count() && Set_Progress(iPair++, nPairs); bLine++)
		{
			CSG_Shape_Line	*pB	= (CSG_Shape_Line *)pLines_B->Get_Shape(bLine);

			if( pA->Intersects(pB) )
			{
				for(int aPart=0; aPart<pA->Get_Part_Count(); aPart++)
				{
					TSG_Point	A[2];	A[1]	= pA->Get_Point(0, aPart);

					for(int aPoint=1; aPoint<pA->Get_Point_Count(aPart); aPoint++)
					{
						A[0]	= A[1];	A[1]	= pA->Get_Point(aPoint, aPart);

						for(int bPart=0; bPart<pB->Get_Part_Count(); bPart++)
						{
							TSG_Point	B[2], C;	B[1]	= pB->Get_Point(0, bPart);

							for(int bPoint=1; bPoint<pB->Get_Point_Count(bPart); bPoint++)
							{
								B[0]	= B[1];	B[1]	= pB->Get_Point(bPoint, bPart);

								if( SG_Get_Crossing(C, A[0], A[1], B[0], B[1]) )
								{
									Set_Crossing(C, pA, pB, pCrossings->Add_Shape(), Attributes);
								}
							}
						}
					}
				}
			}
		}
	}

	//--------------------------------------------------------
	return( pCrossings->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Crossings::Add_Attributes(CSG_Shapes *pCrossings, CSG_Shapes *pLines)
{
	for(int i=0; i<pLines->Get_Field_Count(); i++)
	{
		pCrossings->Add_Field(pLines->Get_Field_Name(i), pLines->Get_Field_Type(i));
	}

	return( true );
}

//---------------------------------------------------------
bool CLine_Crossings::Set_Attributes(CSG_Shape *pCrossing, CSG_Shape *pLine, int &Offset)
{
	for(int i=0; i<pLine->Get_Table()->Get_Field_Count(); i++)
	{
		if( SG_Data_Type_is_Numeric(pLine->Get_Table()->Get_Field_Type(i)) )
		{
			pCrossing->Set_Value(Offset++, pLine->asDouble(i));
		}
		else
		{
			pCrossing->Set_Value(Offset++, pLine->asString(i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CLine_Crossings::Set_Crossing(const TSG_Point &Crossing, CSG_Shape *pA, CSG_Shape *pB, CSG_Shape *pCrossing, int Attributes)
{
	int	Offset	= 0;

	if( Attributes == 0 || Attributes == 2 )
	{
		pCrossing->Set_Value(Offset++, pA->Get_Index());
		pCrossing->Set_Value(Offset++, pB->Get_Index());
	}

	if( Attributes == 1 || Attributes == 2 )
	{
		Set_Attributes(pCrossing, pA, Offset);
		Set_Attributes(pCrossing, pB, Offset);
	}

	pCrossing->Set_Point(Crossing, 0);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
