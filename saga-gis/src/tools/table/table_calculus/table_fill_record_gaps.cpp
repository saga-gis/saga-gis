
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
	//-----------------------------------------------------
	Set_Name		(_TL("Fill Gaps in Records"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"This tool fills gaps in the table records. for the chosen "
		"attribute field it interpolates values for those records, which "
		"have no-data, using existing data from the surrounding records. "
		"If no order field is specified, simply the record index is taken "
		"as coordinate, for which the interpolation will be performed. "
		"Notice: extrapolation is not supported, i.e. only those gaps "
		"will be filled that have lower and higher values surrounding "
		"them following the record order. "
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"ORDER"		, _TL("Order"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Field"),
		_TL("")
	);

	Parameters.Add_Table("",
		"NOGAPS"	, _TL("Table without Gaps"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
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
	//-----------------------------------------------------
	m_pTable	= Parameters("NOGAPS")->asTable();

	if( m_pTable == NULL )
	{
		m_pTable	= Parameters("TABLE")->asTable();
	}
	else if( m_pTable != Parameters("TABLE")->asTable() )
	{
		m_pTable->Create(*Parameters("TABLE")->asTable());

		m_pTable->Fmt_Name("%s [%s]", Parameters("TABLE")->asTable()->Get_Name(), _TL("no gaps"));
	}

	//-----------------------------------------------------
	if( !m_pTable->is_Valid() || m_pTable->Get_Count() < 1 )
	{
		Error_Set(_TL("no records in table"));

		return( false );
	}

	//-----------------------------------------------------
	int	Field	= Parameters("FIELD")->asInt();

	if( !SG_Data_Type_is_Numeric(m_pTable->Get_Field_Type(Field)) )
	{
		Error_Fmt("%s (%s)", _TL("attribute field type has to be numeric"), Parameters("FIELD")->asString());

		return( false );
	}

	//-----------------------------------------------------
	int	Order	= Parameters("ORDER")->asInt();

	if( Order < 0 )
	{
		m_pTable->Del_Index();
	}
	else if( !m_pTable->Set_Index(Order, TABLE_INDEX_Ascending) )
	{
		Error_Fmt("%s (%s)", _TL("failed to create index"), Parameters("ORDER")->asString());

		return( false );
	}

	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<m_pTable->Get_Count() && Set_Progress(iRecord, m_pTable->Get_Count()-1); iRecord++)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

		if( pRecord->is_NoData(Field) )
		{
			CSG_Vector	Y, X;

			if( Get_Neighbours(iRecord, Order, Field, Y, X, Method == 2 ? 2 : 1) )
			{
				while( pRecord && pRecord->is_NoData(Field) )
				{
					double	y	= Order < 0 ? iRecord : pRecord->asDouble(Order);

					switch( Method )
					{
					case  0: pRecord->Set_Value(Field, Get_Nearest(Y, X, y)); break;
					default: pRecord->Set_Value(Field, Get_Linear (Y, X, y)); break;
					case  2: pRecord->Set_Value(Field, Get_Spline (Y, X, y)); break;
					}

					pRecord	= ++iRecord < m_pTable->Get_Count() ? m_pTable->Get_Record_byIndex(iRecord) : NULL;
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_pTable != Parameters("NOGAPS")->asTable() )
	{
		DataObject_Update(m_pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_Y(i)	(Order < 0 ? i : m_pTable->Get_Record_byIndex(i)->asDouble(Order)))
#define GET_X(i)	(                m_pTable->Get_Record_byIndex(i)->asDouble(Order)))

//---------------------------------------------------------
bool CTable_Fill_Record_Gaps::Get_Neighbours(int iRecord, int Order, int Field, CSG_Vector &Y, CSG_Vector &X, int Number)
{
	int		i, n;

	for(i=iRecord-1, n=0; n<Number && i>=0; i--)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(i);

		if( !pRecord->is_NoData(Field) )
		{
			Y.Add_Row(Order < 0 ? i : pRecord->asDouble(Order));
			X.Add_Row(                pRecord->asDouble(Field));

			n++;
		}
	}

	if( n < Number )
	{
		return( false );
	}

	for(i=iRecord+1, n=0; n<Number && i<m_pTable->Get_Count(); i++)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(i);

		if( !pRecord->is_NoData(Field) )
		{
			Y.Add_Row(Order < 0 ? i : pRecord->asDouble(Order));
			X.Add_Row(                pRecord->asDouble(Field));

			n++;
		}
	}

	if( n < Number )
	{
		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CTable_Fill_Record_Gaps::Get_Nearest(const double Y[2], const double X[2], double y)
{
	double	D	= Y[0] + (Y[1] - Y[0]) / 2;

	return( X[y < D ? 0 : 1] );
}

//---------------------------------------------------------
double CTable_Fill_Record_Gaps::Get_Linear(const double Y[2], const double X[2], double y)
{
	if( Y[0] < Y[1] )
	{
		double	D	= (X[1] - X[0]) / (Y[1] - Y[0]);

		return( X[0] + D * (y - Y[0]) );
	}

	return( X[0] );
}

//---------------------------------------------------------
double CTable_Fill_Record_Gaps::Get_Spline(const double Y[4], const double X[4], double y)
{
	CSG_Spline	Spline;

	Spline.Add(Y[0], X[0]);
	Spline.Add(Y[1], X[1]);
	Spline.Add(Y[2], X[2]);
	Spline.Add(Y[3], X[3]);

	return( Spline.Get_Value(y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Insert_Records::CTable_Insert_Records(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Fill Gaps in Ordered Records"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"This tool inserts records where the chosen order field "
		"has gaps expecting an increment of one. It is assumed "
		"that the order field represents integers."
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"ORDER"		, _TL("Order"),
		_TL("")
	);

	Parameters.Add_Table("",
		"NOGAPS"	, _TL("Table without Gaps"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
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
bool CTable_Insert_Records::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	if( !pTable->is_Valid() || pTable->Get_Count() < 2 )
	{
		Error_Set(_TL("not enough records in table"));

		return( false );
	}

	if( !pTable->Set_Index(m_fOrder = Parameters("ORDER")->asInt(), TABLE_INDEX_Ascending) )
	{
		Error_Set(_TL("failed to create index"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table_Record	*pA, *pB;

	m_pNoGaps	= Parameters("NOGAPS")->asTable();
	m_pNoGaps->Create(pTable);
	m_pNoGaps->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("no gaps"));
	m_pNoGaps->Add_Record(pB = pTable->Get_Record(0));

	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	for(int iRecord=1; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()-1); iRecord++)
	{
		pA		= pB;
		pB		= pTable->Get_Record(iRecord);

		int	iA	= pA->asInt(m_fOrder);
		int	iB	= pB->asInt(m_fOrder);

		if( iB - iA > 1 )
		{
			int	iStart	= m_pNoGaps->Get_Count();

			for(int i=iA+1; i<iB; i++)
			{
				m_pNoGaps->Add_Record()->Set_Value(m_fOrder, i);
			}

			for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				if( iField != m_fOrder )
				{
					switch( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(iField)) ? Method : 0 )
					{
					default: Set_Nearest(iStart, iField, pA, pB); break;
					case  1: Set_Linear (iStart, iField, pA, pB); break;
					case  2: Set_Spline (iStart, iField, pA, pB, pTable->Get_Record(iRecord - 2), pTable->Get_Record(iRecord + 1)); break;
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
bool CTable_Insert_Records::Set_Nearest(int iOffset, int iField, CSG_Table_Record *p1, CSG_Table_Record *p2)
{
	int	n	= iOffset + (m_pNoGaps->Get_Count() - iOffset) / 2;

	for(int iRecord=iOffset; iRecord<m_pNoGaps->Get_Count(); iRecord++)
	{
		m_pNoGaps->Get_Record(iRecord)->Set_Value(iField, iRecord < n ? p1->asString(iField) : p2->asString(iField));
	}

	return( true );
}

//---------------------------------------------------------
bool CTable_Insert_Records::Set_Linear(int iOffset, int iField, CSG_Table_Record *p1, CSG_Table_Record *p2)
{
	double	z1	=  p1->asDouble(iField);
	double	dz	= (p2->asDouble(iField) - z1) / abs(p2->asInt(m_fOrder) - p1->asInt(m_fOrder));

	for(int iRecord=iOffset, iX=1; iRecord<m_pNoGaps->Get_Count(); iRecord++, iX++)
	{
		m_pNoGaps->Get_Record(iRecord)->Set_Value(iField, z1 + iX * dz);
	}

	return( true );
}

//---------------------------------------------------------
bool CTable_Insert_Records::Set_Spline(int iOffset, int iField, CSG_Table_Record *p1, CSG_Table_Record *p2, CSG_Table_Record *p0, CSG_Table_Record *p3)
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
