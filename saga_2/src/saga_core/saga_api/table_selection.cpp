
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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable::_Destroy_Selection(void)
{
	if( m_nSelected > 0 )
	{
		for(int iRecord=0; iRecord<m_nSelected; iRecord++)
		{
			m_Selected[iRecord]->m_bSelected	= false;
		}

		API_Free(m_Selected);
		m_Selected	= NULL;
		m_nSelected	= 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable::Select(int iRecord, bool bInvert)
{
	int				i;
	CTable_Record	*pRecord;

	if( !bInvert )
	{
		_Destroy_Selection();
	}

	if( (pRecord = Get_Record(iRecord)) != NULL )
	{
		if( pRecord->m_bSelected == false )
		{
			m_nSelected++;
			m_Selected	= (CTable_Record **)API_Realloc(m_Selected, m_nSelected * sizeof(CTable_Record *));
			m_Selected[m_nSelected - 1]	= pRecord;
			pRecord->m_bSelected		= true;
		}
		else
		{
			m_nSelected--;

			for(i=0; i<m_nSelected; i++)
			{
				if( pRecord == m_Selected[i] )
				{
					for(; i<m_nSelected; i++)
					{
						m_Selected[i]	= m_Selected[i + 1];
					}
				}
			}

			m_Selected	= (CTable_Record **)API_Realloc(m_Selected, m_nSelected * sizeof(CTable_Record *));
			pRecord->m_bSelected		= false;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CTable::Select(CTable_Record *pRecord, bool bInvert)
{
	return( Select(pRecord ? pRecord->Get_Index() : -1, bInvert) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable::Del_Selection(void)
{
	int		i, n	= 0;

	if( !is_Private() )
	{
		for(i=m_nSelected-1; i>=0; i--)
		{
			if( _Del_Record(m_Selected[i]->Get_Index()) )
			{
				n++;
			}
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
