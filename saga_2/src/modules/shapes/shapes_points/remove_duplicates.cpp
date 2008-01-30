
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Remove_Duplicates.cpp                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "remove_duplicates.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRemove_Duplicates::CRemove_Duplicates(void)
{
	Set_Name		(_TL("Remove Duplicate Points"));

	Set_Author		(_TL("(c) 2008 by O. Conrad"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "METHOD_NUM"	, _TL("Numerical Values"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("first"),
			_TL("last"),
			_TL("mean")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD_STR"	, _TL("Text"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("first"),
			_TL("last")
		), 0
	);
}

//---------------------------------------------------------
CRemove_Duplicates::~CRemove_Duplicates(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRemove_Duplicates::On_Execute(void)
{
	int					i, j;
	double				Epsilon	= 0.00001;
	CSG_Point			Point;
	CSG_Shape			*pPoint, *pDuplicate;
	CSG_Shapes			*pPoints, *pInput;
	CSG_Shapes_Search	Search;

	//-----------------------------------------------------
	pInput			= Parameters("POINTS")		->asShapes();
	pPoints			= Parameters("RESULT")		->asShapes();

	m_Method_Num	= Parameters("METHOD_NUM")	->asInt();
	m_Method_Str	= Parameters("METHOD_STR")	->asInt();

	if( pPoints == NULL )
	{
		pPoints	= pInput;
	}
	else if( pPoints != pInput )
	{
		pPoints->Assign(pInput);
	}

	//-----------------------------------------------------
	if( !pPoints->is_Valid() )
	{
		Message_Add(_TL("Invalid shapes layer."));

		return( false );
	}
	else if( !Search.Create(pPoints) )
	{
		Message_Add(_TL("Failed to initialise search engine."));

		return( false );
	}

	//-----------------------------------------------------
	pPoints->Select();

	for(i=0; i<pPoints->Get_Count() && Set_Progress(i, pPoints->Get_Count()); i++)
	{
		pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_Selected() )
		{
			Point	= pPoint->Get_Point(0);

			if( Search.Select_Radius(Point.Get_X(), Point.Get_Y(), Epsilon) > 1 )
			{
				for(j=0; j<Search.Get_Selected_Count(); j++)
				{
					pDuplicate	= Search.Get_Selected_Point(j);

					if( pDuplicate && pDuplicate != pPoint && Point == pDuplicate->Get_Point(0) )
					{
						pPoints->Select(pDuplicate, true);

						Set_Attributes(pPoint->Get_Record(), pDuplicate->Get_Record());
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pPoints->Get_Selection_Count() == 0 )
	{
		Message_Add(_TL("No duplicates found."));
	}
	else
	{
		Message_Add(CSG_String::Format(SG_T("%d %s"), pPoints->Get_Selection_Count(), _TL("duplicates have been identified.")));

		pPoints->Del_Selection();
	}

	return( true );
}

//---------------------------------------------------------
void CRemove_Duplicates::Set_Attributes(CSG_Table_Record *pTarget, CSG_Table_Record *pSource)
{
	if( m_Method_Num == 0 && m_Method_Str == 0 )
	{
		// nothing to do...
	}
	else if( m_Method_Num == 1 && m_Method_Str == 1 )
	{
		pTarget->Assign(pSource);
	}
	else
	{
		CSG_Table	*pTable	= pTarget->Get_Owner();

		for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
		{
			switch( pTable->Get_Field_Type(iField) )
			{
			case TABLE_FIELDTYPE_String:
			case TABLE_FIELDTYPE_Color:
			case TABLE_FIELDTYPE_Date:
				if( m_Method_Str == 1 )
				{
					pTarget->Set_Value(iField, pSource->asString(iField));
				}
				break;

			default:
				if( m_Method_Num == 1 )
				{
					pTarget->Set_Value(iField, pSource->asDouble(iField));
				}
				else if( m_Method_Num == 2 )
				{
					pTarget->Set_Value(iField, (pSource->asDouble(iField) + pTarget->asDouble(iField)) / 2.0);
				}
				break;
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
