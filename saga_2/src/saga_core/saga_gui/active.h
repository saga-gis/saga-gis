
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
//                      ACTIVE.h                         //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_H
#define _HEADER_INCLUDED__SAGA_GUI__ACTIVE_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/notebook.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CACTIVE : public wxNotebook
{
public:
	CACTIVE(wxWindow *pParent);
	virtual ~CACTIVE(void);

	void							Add_Pages				(void);

	class CACTIVE_Parameters *		Get_Parameters			(void)	{	return( m_pParameters );	}
	class CACTIVE_Description *		Get_Description			(void)	{	return( m_pDescription );	}
	class CACTIVE_Attributes *		Get_Attributes			(void)	{	return( m_pAttributes );	}
	class CACTIVE_Legend *			Get_Legend				(void)	{	return( m_pLegend );		}
	class CACTIVE_Layers *			Get_Layers				(void)	{	return( m_pLayers );		}
	class CACTIVE_HTMLExtraInfo *	Get_HTMLExtraInfo		(void)	{	return( m_pHTMLExtraInfo );	}

	bool							Set_Active				(class CWKSP_Base_Item *pItem);
	class CWKSP_Base_Item *			Get_Item				(void)	{	return( m_pItem );			}
	class CWKSP_Layer *				Get_Layer				(void)	{	return( m_pLayer );			}

	bool							Update					(class CWKSP_Base_Item *pItem, bool bSave);
	bool							Update_DataObjects		(void);
	bool							Update_Layers			(void);


private:

	class CACTIVE_Parameters		*m_pParameters;

	class CACTIVE_Description		*m_pDescription;

	class CACTIVE_Attributes		*m_pAttributes;

	class CACTIVE_Legend			*m_pLegend;

	class CACTIVE_Layers			*m_pLayers;

	class CACTIVE_HTMLExtraInfo		*m_pHTMLExtraInfo;

	class CWKSP_Base_Item			*m_pItem;

	class CWKSP_Layer				*m_pLayer;


	bool							_Add_Page				(int PageID);
	bool							_Del_Page				(int PageID);

	bool							_Set_Description		(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
extern CACTIVE						*g_pACTIVE;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_H
