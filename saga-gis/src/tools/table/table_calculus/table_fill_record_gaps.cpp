/**********************************************************
 * Version $Id: table_fill_record_gaps.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_fill_record_gaps.cpp              //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "table_fill_record_gaps.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Fill_Record_Gaps::CTable_Fill_Record_Gaps(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Fill Gaps in Records"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ORDER"			, _TL("Order"),
		_TL("")
	);

	pNode	= Parameters.Add_Table(
		NULL	, "NOGAPS"			, _TL("Table without Gaps"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Nearest Neighbour"),
			_TL("Linear"),
			_TL("Spline")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Fill_Record_Gaps::On_Execute(void)
{
	int			Method;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable		= Parameters("TABLE")	->asTable();
	m_pNoGaps	= Parameters("NOGAPS")	->asTable();
	m_fOrder	= Parameters("ORDER")	->asInt();
	Method		= Parameters("METHOD")	->asInt();

	if( pTable->Get_Count() == 0 || !pTable->Set_Index(m_fOrder, TABLE_INDEX_Ascending) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Table_Record	*pA, *pB;

	m_pNoGaps->Create(pTable);
	m_pNoGaps->Set_Name("%s [%s]", pTable->Get_Name(), _TL("no gaps"));
	m_pNoGaps->Add_Record(pB = pTable->Get_Record(0));

	//-----------------------------------------------------
	for(int iRecord=1; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()-1); iRecord++)
	{
		pA		= pB;
		pB		= pTable->Get_Record(iRecord);

		int	iA	= pA->asInt(m_fOrder);
		int	iB	= pB->asInt(m_fOrder);

		if( iB - iA > 1 )
		{
			int		iStart	= m_pNoGaps->Get_Count();

			for(int i=iA+1; i<iB; i++)
			{
				m_pNoGaps->Add_Record()->Set_Value(m_fOrder, i);
			}

			for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				if( iField != m_fOrder && ::SG_Data_Type_is_Numeric(pTable->Get_Field_Type(iField)) )
				{
					switch( Method )
					{
					case 0:
						Set_Nearest (iStart, iField, pA, pB);
						break;

					case 1:
						Set_Linear	(iStart, iField, pA, pB);
						break;

					case 2:
						Set_Spline	(iStart, iField, pTable->Get_Record(iRecord - 2), pA, pB, pTable->Get_Record(iRecord + 1));
						break;
					}
				}
			}
		}

		m_pNoGaps->Add_Record(pB);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Fill_Record_Gaps::Set_Nearest(int iOffset, int iField, CSG_Table_Record *p1, CSG_Table_Record *p2)
{
	double	z1	= p1->asDouble(iField);
	double	z2	= p2->asDouble(iField);

	int		n	= iOffset + (m_pNoGaps->Get_Count() - iOffset) / 2;

	for(int iRecord=iOffset; iRecord<m_pNoGaps->Get_Count(); iRecord++)
	{
		m_pNoGaps->Get_Record(iRecord)->Set_Value(iField, iOffset < n ? z1 : z2);
	}

	return( true );
}

//---------------------------------------------------------
bool CTable_Fill_Record_Gaps::Set_Linear(int iOffset, int iField, CSG_Table_Record *p1, CSG_Table_Record *p2)
{
	double	z1	= p1->asDouble(iField);
	double	dz	= (p2->asDouble(iField) - z1) / abs(p2->asInt(m_fOrder) - p1->asInt(m_fOrder));

	for(int iRecord=iOffset, iX=1; iRecord<m_pNoGaps->Get_Count(); iRecord++, iX++)
	{
		m_pNoGaps->Get_Record(iRecord)->Set_Value(iField, z1 + iX * dz);
	}

	return( true );
}

//---------------------------------------------------------
bool CTable_Fill_Record_Gaps::Set_Spline(int iOffset, int iField, CSG_Table_Record *p0, CSG_Table_Record *p1, CSG_Table_Record *p2, CSG_Table_Record *p3)
{
	CSG_Spline	Spline;

	int		i0	= p0 ? p0->asInt(m_fOrder) : p1->asInt(m_fOrder) - 1;
	int		i1	= p1->asInt(m_fOrder);
	int		i2	= p2->asInt(m_fOrder);
	int		i3	= p3 ? p3->asInt(m_fOrder) : p2->asInt(m_fOrder) + 1;

	Spline.Add(i0 - i1, p0 ? p0->asDouble(iField) : p1->asDouble(iField));
	Spline.Add(0      , p1->asDouble(iField));
	Spline.Add(i2 - i1, p2->asDouble(iField));
	Spline.Add(i3 - i1, p3 ? p3->asDouble(iField) : p2->asDouble(iField));

	if( !Spline.Create() )
	{
		return( Set_Linear(iOffset, iField, p1, p2) );
	}

	for(int iRecord=iOffset, i=1; iRecord<m_pNoGaps->Get_Count(); iRecord++, i++)
	{
		m_pNoGaps->Get_Record(iRecord)->Set_Value(iField, Spline.Get_Value(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
