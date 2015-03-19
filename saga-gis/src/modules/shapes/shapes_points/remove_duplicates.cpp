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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Remove Duplicate Points"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

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

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Point to Keep"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("first point"),
			_TL("last point"),
			_TL("point with minimum attribute value"),
			_TL("point with maximum attribute value")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "NUMERIC"		, _TL("Numeric Attribute Values"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("take value from the point to be kept"),
			_TL("minimum value of all duplicates"),
			_TL("maximum value of all duplicates"),
			_TL("mean value of all duplicates")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CRemove_Duplicates::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		pParameters->Get_Parameter("FIELD")->Set_Enabled(pParameter->asInt() >= 2);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRemove_Duplicates::On_Execute(void)
{
	CSG_PRQuadTree	Search;

	//-----------------------------------------------------
	m_pPoints	= Parameters("RESULT")	->asShapes();
	m_Field		= Parameters("FIELD")	->asInt();
	m_Method	= Parameters("METHOD")	->asInt();
	m_Numeric	= Parameters("NUMERIC")	->asInt();

	//-----------------------------------------------------
	if( m_pPoints == NULL )
	{
		m_pPoints	= Parameters("POINTS")->asShapes();
	}
	else if( m_pPoints != Parameters("POINTS")->asShapes() )
	{
		m_pPoints	->Assign(Parameters("POINTS")->asShapes());
	}

	//-----------------------------------------------------
	if( !m_pPoints->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	if( m_pPoints->Get_Count() <= 0 )
	{
		Error_Set(_TL("no points in layer"));

		return( false );
	}

	if( !Search.Create(m_pPoints, -1) )
	{
		Error_Set(_TL("failed to initialise search engine"));

		return( false );
	}

	//-----------------------------------------------------
	m_pPoints->Add_Field(_TL("Duplicates"), SG_DATATYPE_Int);

	m_pPoints->Select();

	for(int i=0; i<m_pPoints->Get_Count() && Set_Progress(i, m_pPoints->Get_Count()); i++)
	{
		CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

		if( !pPoint->is_Selected() )
		{
			double	Distance;

			CSG_PRQuadTree_Leaf	*pLeaf	= Search.Get_Nearest_Leaf(pPoint->Get_Point(0), Distance);

			if( Distance == 0.0 && pLeaf && pLeaf->has_Statistics() )
			{
				Set_Attributes(pPoint, (CSG_PRQuadTree_Leaf_List *)pLeaf);
			}
		}
	}

	//-----------------------------------------------------
	if( m_pPoints->Get_Selection_Count() == 0 )
	{
		Message_Add(_TL("No duplicates found."));
	}
	else
	{
		Message_Add(CSG_String::Format(SG_T("%d %s"), m_pPoints->Get_Selection_Count(), _TL("duplicates have been identified.")));

		m_pPoints->Del_Selection();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CRemove_Duplicates::Set_Attributes(CSG_Shape *pPoint, CSG_PRQuadTree_Leaf_List *pList)
{
	int			iDuplicate;
	double		dKeep;
	CSG_Shape	*pKeep;

	//-----------------------------------------------------
	for(iDuplicate=0, pKeep=NULL; iDuplicate<pList->Get_Count(); iDuplicate++)
	{
		CSG_Shape	*pDuplicate	= m_pPoints->Get_Shape((int)pList->Get_Value(iDuplicate));

		if( pDuplicate != pPoint )
		{
			m_pPoints->Select(pDuplicate->Get_Index(), true);
		}

		switch( m_Method )
		{
		case 2:	// point with minimum attribute value
			if( !pKeep || (!pDuplicate->is_NoData(m_Field) && pDuplicate->asDouble(m_Field) < dKeep) )
			{
				dKeep	= pDuplicate->asDouble(m_Field);
				pKeep	= pDuplicate;
			}
			break;

		case 3:	// point with maximum attribute value")
			if( !pKeep || (!pDuplicate->is_NoData(m_Field) && pDuplicate->asDouble(m_Field) > dKeep) )
			{
				dKeep	= pDuplicate->asDouble(m_Field);
				pKeep	= pDuplicate;
			}
			break;
		}
	}

	//-----------------------------------------------------
	if( m_Method == 1 )	// last point
	{
		pKeep	= m_pPoints->Get_Shape((int)pList->Get_Value((int)(pList->Get_Count() - 1)));
	}

	if( pKeep )
	{
		((CSG_Table_Record *)pPoint)->Assign(pKeep);
	}

	pPoint->Set_Value(m_pPoints->Get_Field_Count() - 1, (double)pList->Get_Count());

	//-----------------------------------------------------
	if( m_Numeric > 0 )
	{
		for(int iField=0; iField<m_pPoints->Get_Field_Count()-1; iField++)
		{
			if( SG_Data_Type_is_Numeric(m_pPoints->Get_Field_Type(iField)) )
			{
				CSG_Simple_Statistics	s;

				for(iDuplicate=0; iDuplicate<pList->Get_Count(); iDuplicate++)
				{
					CSG_Shape	*pDuplicate	= m_pPoints->Get_Shape((int)pList->Get_Value(iDuplicate));

					if( !pDuplicate->is_NoData(iField) )
					{
						s	+= m_pPoints->Get_Shape((int)pList->Get_Value(iDuplicate))->asDouble(iField);
					}
				}

				switch( m_Numeric )
				{
				case 1:	pPoint->Set_Value(iField, s.Get_Minimum());	break;	// minimun value
				case 2:	pPoint->Set_Value(iField, s.Get_Maximum());	break;	// maximum value
				case 3:	pPoint->Set_Value(iField, s.Get_Mean());	break;	// mean value
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
