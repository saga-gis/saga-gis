
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
//                  WKSP_TIN_Manager.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_TIN_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_TIN_Manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_TIN_Manager : public CWKSP_Base_Manager
{
public:
	CWKSP_TIN_Manager(void);
	virtual ~CWKSP_TIN_Manager(void);

	virtual TWKSP_Item			Get_Type		(void)		{	return( WKSP_ITEM_TIN_Manager );	}

	virtual wxString			Get_Name		(void);
	virtual wxString			Get_Description	(void);

	virtual wxMenu *			Get_Menu		(void);

	virtual bool				On_Command		(int Cmd_ID);

	class CWKSP_TIN *			Get_TIN			(int i)		{	return( (class CWKSP_TIN *)Get_Item(i) );	}
	class CWKSP_TIN *			Get_TIN			(class CSG_TIN *pTIN);

	bool						Exists			(class CSG_TIN *pTIN);
	class CWKSP_TIN *			Add				(class CSG_TIN *pTIN);
	class CSG_TIN *				Get_byFileName	(const char *File_Name);

	bool						Update			(class CSG_TIN *pTIN, class CSG_Parameters *pParameters);
	bool						Update_Views	(class CSG_TIN *pTIN);
	bool						Show			(class CSG_TIN *pTIN);
	bool						asImage			(class CSG_TIN *pTIN, class CSG_Grid *pImage);

	bool						Get_Colors		(class CSG_TIN *pTIN, class CSG_Colors *pColors);
	bool						Set_Colors		(class CSG_TIN *pTIN, class CSG_Colors *pColors);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_TIN_Manager_H
