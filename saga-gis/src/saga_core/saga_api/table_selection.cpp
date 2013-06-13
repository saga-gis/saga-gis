/**********************************************************
 * Version $Id$
 *********************************************************/

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
bool CSG_Table::_Destroy_Selection(void)
{
	if( m_nSelected > 0 )
	{
		for(int iRecord=0; iRecord<m_nSelected; iRecord++)
		{
			m_Records[m_Selected[iRecord]]->Set_Selected(false);
		}

		SG_FREE_SAFE(m_Selected);
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

		m_Selected	= (int *)SG_Realloc(m_Selected, (m_nSelected + 1) * sizeof(int));
		m_Selected[m_nSelected++]	= iRecord;
	}
	else
	{
		pRecord->Set_Selected(false);

		m_nSelected--;

		for(int i=0; i<m_nSelected; i++)
		{
			if( iRecord == m_Selected[i] )
			{
				for(; i<m_nSelected; i++)
				{
					m_Selected[i]	= m_Selected[i + 1];
				}
			}
		}

		m_Selected	= (int *)SG_Realloc(m_Selected, m_nSelected * sizeof(int));
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Table::Del_Selection(void)
{
	int		n	= 0;

	if( m_nSelected > 0 )
	{
		for(int i=m_nSelected-1; i>=0; i--)
		{
			if( Del_Record(m_Selected[i]) )
			{
				n++;
			}
		}

		SG_FREE_SAFE(m_Selected);
		m_nSelected	= 0;
	}

	return( n );
}

//---------------------------------------------------------
int CSG_Table::Inv_Selection(void)
{
	CSG_Table_Record	**pRecord	= m_Records + 0;

	if( Get_Record_Count() > 0 )
	{
		m_nSelected	= m_nRecords - m_nSelected;
		m_Selected	= (int *)SG_Realloc(m_Selected, m_nSelected * sizeof(int));

		for(int i=0, j=0; i<m_nRecords; i++, pRecord++)
		{
			if( (*pRecord)->is_Selected() )
			{
				(*pRecord)->Set_Selected(false);
			}
			else
			{
				(*pRecord)->Set_Selected(true);

				m_Selected[j++]	= i;
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
