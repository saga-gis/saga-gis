
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 table_selection.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Add_Selection(size_t iRecord)
{
	if( m_Selection.Inc_Array() )
	{
		return( _Set_Selection(iRecord, m_Selection.Get_Size() - 1) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Set_Selection(size_t iRecord, size_t Index)
{
	if( Index < m_Selection.Get_Size() )
	{
		*((size_t *)m_Selection.Get_Entry(Index))	= iRecord;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Del_Selection(size_t iRecord)
{
	for(size_t i=0; i<m_Selection.Get_Size(); i++)
	{
		if( iRecord == Get_Selection_Index(i) )
		{
			size_t	*Selection	= (size_t *)m_Selection.Get_Array();

			for(size_t j=i+1; j<m_Selection.Get_Size(); i++, j++)
			{
				Selection[i]	= Selection[j];
			}

			m_Selection.Dec_Array();

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Destroy_Selection(void)
{
	if( Get_Selection_Count() > 0 )
	{
		for(size_t iRecord=0; iRecord<(size_t)m_nRecords; iRecord++)
		{
			m_Records[iRecord]->Set_Selected(false);
		}

		m_Selection.Set_Array(0);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Select(int iRecord, bool bInvert)
{
	if( !bInvert )
	{
		_Destroy_Selection();
	}

	CSG_Table_Record	*pRecord	= Get_Record(iRecord);

	if( pRecord == NULL )
	{
		return( false );
	}

	if( pRecord->is_Selected() == false )
	{
		pRecord->Set_Selected(true);

		_Add_Selection(iRecord);
	}
	else // if( pRecord->is_Selected() == true )
	{
		pRecord->Set_Selected(false);

		_Del_Selection(iRecord);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Select(CSG_Table_Record *pRecord, bool bInvert)
{
	return( Select(pRecord ? pRecord->Get_Index() : -1, bInvert) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Table::Del_Selection(void)
{
	int	n	= 0;

	if( Get_Selection_Count() > 0 )
	{
		for(int i=m_nRecords-1; i>=0; i--)
		{
			if( m_Records[i]->is_Selected() )
			{
				m_Records[i]->Set_Selected(false);

				Del_Record(i);

				n++;
			}
		}

		m_Selection.Set_Array(0);
	}

	return( n );
}

//---------------------------------------------------------
int CSG_Table::Inv_Selection(void)
{
	if( Get_Record_Count() > 0 )
	{
		m_Selection.Set_Array((size_t)m_nRecords - m_Selection.Get_Size());

		for(size_t i=0, n=0; i<(size_t)Get_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= Get_Record((int)i);

			if( pRecord->is_Selected() == false )//&& n < m_Selection.Get_Size() )
			{
				pRecord->Set_Selected(true);

				*((size_t *)m_Selection.Get_Entry(n++))	= i;
			}
			else
			{
				pRecord->Set_Selected(false);
			}
		}
	}

	return( (int)Get_Selection_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
