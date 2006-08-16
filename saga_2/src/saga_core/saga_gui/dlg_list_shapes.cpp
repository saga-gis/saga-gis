
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 DLG_List_Shapes.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#include <saga_api/saga_api.h>

#include "wksp_data_manager.h"
#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "dlg_list_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_List_Shapes, CDLG_List_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_List_Shapes, CDLG_List_Base)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_List_Shapes::CDLG_List_Shapes(CParameter_Shapes_List *pList, wxString Caption)
	: CDLG_List_Base(pList, Caption)
{
	m_Shape_Type	= pList->Get_Shape_Type();

	_Set_Objects();
}

//---------------------------------------------------------
CDLG_List_Shapes::~CDLG_List_Shapes(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_List_Shapes::_Set_Objects(void)
{
	int						i;
	CWKSP_Shapes_Manager	*pManager;

	//-----------------------------------------------------
	if( (pManager = g_pData->Get_Shapes()) != NULL )
	{
		for(i=0; i<m_pList->Get_Count(); i++)
		{
			m_pAdd->Append(m_pList->asDataObject(i)->Get_Name(), m_pList->asDataObject(i));
		}

		//-------------------------------------------------
		if( m_Shape_Type == SHAPE_TYPE_Undefined )
		{
			for(i=0; i<pManager->Get_Count(); i++)
			{
				_Set_Shapes((CWKSP_Shapes_Type *)pManager->Get_Item(i));
			}
		}
		else
		{
			_Set_Shapes(pManager->Get_Shapes_Type(m_Shape_Type));
		}
	}
}

//---------------------------------------------------------
void CDLG_List_Shapes::_Set_Shapes(CWKSP_Shapes_Type *pType)
{
	bool	bList;
	int		i, j;
	CShapes	*pShapes;

	if( pType )
	{
		for(i=0; i<pType->Get_Count(); i++)
		{
			pShapes	= pType->Get_Shapes(i)->Get_Shapes();

			for(j=0, bList=true; j<(int)m_pAdd->GetCount() && bList; j++)
			{
				if( pShapes == m_pAdd->GetClientData(j) )
				{
					bList	= false;
				}
			}

			if( bList )
			{
				m_pSelect->Append(pShapes->Get_Name(), pShapes);
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
