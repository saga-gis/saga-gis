
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
bool CSG_Table::_Add_Selection(sLong iRecord)
{
	if( m_Selection.Inc_Array() )
	{
		return( _Set_Selection(iRecord, m_Selection.Get_Size() - 1) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Set_Selection(sLong Index, sLong Selected)
{
	if( Selected < m_Selection.Get_Size() )
	{
		*((sLong *)m_Selection.Get_Entry(Selected)) = Index;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Del_Selection(sLong Index)
{
	for(sLong i=0; i<m_Selection.Get_Size(); i++)
	{
		if( Index == Get_Selection_Index(i) )
		{
			sLong *Selection = (sLong *)m_Selection.Get_Array();

			for(sLong j=i+1; j<m_Selection.Get_Size(); i++, j++)
			{
				Selection[i] = Selection[j];
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
bool CSG_Table::Select(sLong iRecord, bool bInvert)
{
	if( !bInvert )
	{
		_Destroy_Selection();
	}

	CSG_Table_Record *pRecord = Get_Record(iRecord);

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
sLong CSG_Table::Del_Selection(void)
{
	sLong n = 0;

	if( Get_Selection_Count() > 0 )
	{
		for(sLong i=m_nRecords-1; i>=0; i--)
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
sLong CSG_Table::Inv_Selection(void)
{
	if( m_nRecords > 0 )
	{
		m_Selection.Set_Array(m_nRecords - m_Selection.Get_Size());

		for(sLong i=0, n=0; i<Get_Count(); i++)
		{
			CSG_Table_Record *pRecord = Get_Record(i);

			if( pRecord->is_Selected() == false )//&& n < m_Selection.Get_Size() )
			{
				pRecord->Set_Selected(true);

				*((sLong *)m_Selection.Get_Entry(n++)) = i;
			}
			else
			{
				pRecord->Set_Selected(false);
			}
		}
	}

	return( Get_Selection_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
